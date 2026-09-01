#include "u2f.h"
#include "u2f_data.h"

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_random.h>

#include <mbedtls/sha256.h>
#include <mbedtls/md.h>
#include <mbedtls/ecdsa.h>

#define TAG "PepperKey"

#define WORKER_TAG TAG "Worker"

#define MCHECK(expr) furi_check((expr) == 0)

#define U2F_CMD_REGISTER     0x01
#define U2F_CMD_AUTHENTICATE 0x02
#define U2F_CMD_VERSION      0x03

typedef enum {
    U2fCheckOnly = 0x07, // "check-only" - only check key handle, don't send auth response
    U2fEnforce =
        0x03, // "enforce-user-presence-and-sign" - send auth response only if user is present
    U2fDontEnforce =
        0x08, // "dont-enforce-user-presence-and-sign" - send auth response even if user is missing
} U2fAuthMode;

#define U2F_HASH_SIZE      32
#define U2F_NONCE_SIZE     32
#define U2F_CHALLENGE_SIZE 32
#define U2F_APP_ID_SIZE    32

#define U2F_EC_KEY_SIZE    32
#define U2F_EC_BIGNUM_SIZE 32
#define U2F_EC_POINT_SIZE  65

typedef struct {
    uint8_t format;
    uint8_t xy[64];
} FURI_PACKED U2fPubKey;
_Static_assert(sizeof(U2fPubKey) == U2F_EC_POINT_SIZE, "U2fPubKey size mismatch");

typedef struct {
    uint8_t len;
    uint8_t hash[U2F_HASH_SIZE];
    uint8_t nonce[U2F_NONCE_SIZE];
} FURI_PACKED U2fKeyHandle;

typedef struct {
    uint8_t cla;
    uint8_t ins;
    uint8_t p1;
    uint8_t p2;
    uint8_t len[3];
    uint8_t challenge[U2F_CHALLENGE_SIZE];
    uint8_t app_id[U2F_APP_ID_SIZE];
} FURI_PACKED U2fRegisterReq;

typedef struct {
    uint8_t reserved;
    U2fPubKey pub_key;
    U2fKeyHandle key_handle;
    uint8_t cert[];
} FURI_PACKED U2fRegisterResp;

typedef struct {
    uint8_t cla;
    uint8_t ins;
    uint8_t p1;
    uint8_t p2;
    uint8_t len[3];
    uint8_t challenge[U2F_CHALLENGE_SIZE];
    uint8_t app_id[U2F_APP_ID_SIZE];
    U2fKeyHandle key_handle;
} FURI_PACKED U2fAuthReq;

typedef struct {
    uint8_t user_present;
    uint32_t counter;
    uint8_t signature[];
} FURI_PACKED U2fAuthResp;

static const uint8_t ver_str[] = {"U2F_V2"};

static const uint8_t state_no_error[] = {0x90, 0x00};
static const uint8_t state_not_supported[] = {0x6D, 0x00};
static const uint8_t state_user_missing[] = {0x69, 0x85};
static const uint8_t state_wrong_data[] = {0x6A, 0x80};

struct U2fData {
    uint8_t device_key[U2F_EC_KEY_SIZE];
    uint8_t cert_key[U2F_EC_KEY_SIZE];
    uint8_t pending_app_id[U2F_APP_ID_SIZE];
    uint8_t approved_app_id[U2F_APP_ID_SIZE];
    uint32_t counter;
    bool ready;
    bool user_present;
    bool pending_request;
    U2fNotifyEvent pending_event;
    FuriMutex* state_mutex;
    U2fEvtCallback callback;
    void* context;
    mbedtls_ecp_group group;
};

static void pepper_key_secure_zero(void* data, size_t size) {
    volatile uint8_t* cursor = data;
    while(size--)
        *cursor++ = 0;
}

static bool
    pepper_key_constant_time_equal(const uint8_t* left, const uint8_t* right, size_t size) {
    uint8_t difference = 0;
    for(size_t index = 0; index < size; index++) {
        difference |= left[index] ^ right[index];
    }
    return difference == 0;
}

static void u2f_clear_pending_request_locked(U2fData* U2F) {
    U2F->pending_request = false;
    U2F->user_present = false;
    pepper_key_secure_zero(U2F->pending_app_id, sizeof(U2F->pending_app_id));
    pepper_key_secure_zero(U2F->approved_app_id, sizeof(U2F->approved_app_id));
}

static bool
    u2f_begin_request(U2fData* U2F, U2fNotifyEvent event, const uint8_t app_id[U2F_APP_ID_SIZE]) {
    furi_check(furi_mutex_acquire(U2F->state_mutex, FuriWaitForever) == FuriStatusOk);
    if(!U2F->pending_request) {
        memcpy(U2F->pending_app_id, app_id, U2F_APP_ID_SIZE);
        U2F->pending_event = event;
        U2F->pending_request = true;
    }
    const bool matches =
        (U2F->pending_event == event) &&
        pepper_key_constant_time_equal(U2F->pending_app_id, app_id, U2F_APP_ID_SIZE);
    furi_check(furi_mutex_release(U2F->state_mutex) == FuriStatusOk);
    return matches;
}

static bool u2f_consume_user_presence(
    U2fData* U2F,
    U2fNotifyEvent event,
    const uint8_t app_id[U2F_APP_ID_SIZE]) {
    furi_check(furi_mutex_acquire(U2F->state_mutex, FuriWaitForever) == FuriStatusOk);
    const bool request_matches =
        U2F->pending_request && (U2F->pending_event == event) &&
        pepper_key_constant_time_equal(U2F->pending_app_id, app_id, U2F_APP_ID_SIZE);
    const bool approved =
        request_matches && U2F->user_present &&
        pepper_key_constant_time_equal(U2F->approved_app_id, app_id, U2F_APP_ID_SIZE);
    if(request_matches && U2F->user_present) {
        U2F->user_present = false;
        pepper_key_secure_zero(U2F->approved_app_id, sizeof(U2F->approved_app_id));
    }
    if(approved) u2f_clear_pending_request_locked(U2F);
    furi_check(furi_mutex_release(U2F->state_mutex) == FuriStatusOk);
    return approved;
}

static int u2f_uecc_random_cb(void* context, uint8_t* dest, unsigned size) {
    UNUSED(context);
    furi_hal_random_fill_buf(dest, size);
    return 0;
}

U2fData* u2f_alloc(void) {
    U2fData* instance = malloc(sizeof(U2fData));
    memset(instance, 0, sizeof(U2fData));
    mbedtls_ecp_group_init(&instance->group);
    instance->state_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    return instance;
}

void u2f_free(U2fData* U2F) {
    furi_assert(U2F);
    mbedtls_ecp_group_free(&U2F->group);
    pepper_key_secure_zero(U2F->device_key, sizeof(U2F->device_key));
    pepper_key_secure_zero(U2F->cert_key, sizeof(U2F->cert_key));
    pepper_key_secure_zero(U2F->pending_app_id, sizeof(U2F->pending_app_id));
    pepper_key_secure_zero(U2F->approved_app_id, sizeof(U2F->approved_app_id));
    furi_mutex_free(U2F->state_mutex);
    free(U2F);
}

bool u2f_init(U2fData* U2F) {
    furi_assert(U2F);

    if(u2f_data_cert_check() == false) {
        FURI_LOG_E(TAG, "Certificate load error");
        return false;
    }
    if(u2f_data_cert_key_load(U2F->cert_key) == false) {
        FURI_LOG_E(TAG, "Certificate key load error");
        return false;
    }
    const U2fDataLoadResult key_result = u2f_data_key_load(U2F->device_key);
    const U2fDataLoadResult counter_result = u2f_data_cnt_read(&U2F->counter);

    if(key_result == U2fDataLoadMissing && counter_result == U2fDataLoadMissing) {
        FURI_LOG_I(TAG, "Provisioning new PepperKey identity");
        if(!furi_hal_crypto_enclave_ensure_key(FURI_HAL_CRYPTO_ENCLAVE_UNIQUE_KEY_SLOT)) {
            FURI_LOG_E(TAG, "Unique encryption key unavailable");
            return false;
        }
        if(!u2f_data_key_create(U2F->device_key)) {
            FURI_LOG_E(TAG, "Key create failed");
            return false;
        }
        U2F->counter = 0;
        if(!u2f_data_cnt_create(U2F->counter)) {
            FURI_LOG_E(TAG, "Counter create failed; removing new key");
            if(!u2f_data_key_remove_created()) {
                FURI_LOG_E(TAG, "Unable to roll back incomplete provisioning");
            }
            pepper_key_secure_zero(U2F->device_key, sizeof(U2F->device_key));
            return false;
        }
    } else if(key_result != U2fDataLoadOk || counter_result != U2fDataLoadOk) {
        FURI_LOG_E(TAG, "Authenticator state is incomplete or invalid");
        pepper_key_secure_zero(U2F->device_key, sizeof(U2F->device_key));
        U2F->counter = 0;
        return false;
    }

    MCHECK(mbedtls_ecp_group_load(&U2F->group, MBEDTLS_ECP_DP_SECP256R1));

    U2F->ready = true;
    return true;
}

void u2f_set_event_callback(U2fData* U2F, U2fEvtCallback callback, void* context) {
    furi_assert(U2F);
    furi_assert(callback);
    U2F->callback = callback;
    U2F->context = context;
}

void u2f_confirm_user_present(U2fData* U2F, const uint8_t app_id[U2F_APP_ID_SIZE]) {
    furi_assert(U2F);
    furi_assert(app_id);
    furi_check(furi_mutex_acquire(U2F->state_mutex, FuriWaitForever) == FuriStatusOk);
    if(U2F->pending_request &&
       pepper_key_constant_time_equal(U2F->pending_app_id, app_id, U2F_APP_ID_SIZE)) {
        memcpy(U2F->approved_app_id, app_id, U2F_APP_ID_SIZE);
        U2F->user_present = true;
    }
    furi_check(furi_mutex_release(U2F->state_mutex) == FuriStatusOk);
}

bool u2f_get_pending_app_id(const U2fData* U2F, uint8_t app_id[U2F_APP_ID_SIZE]) {
    furi_assert(U2F);
    furi_assert(app_id);
    furi_check(furi_mutex_acquire(U2F->state_mutex, FuriWaitForever) == FuriStatusOk);
    const bool pending = U2F->pending_request;
    if(pending) memcpy(app_id, U2F->pending_app_id, U2F_APP_ID_SIZE);
    furi_check(furi_mutex_release(U2F->state_mutex) == FuriStatusOk);
    return pending;
}

void u2f_cancel_pending_request(U2fData* U2F, const uint8_t app_id[U2F_APP_ID_SIZE]) {
    furi_assert(U2F);
    furi_assert(app_id);
    furi_check(furi_mutex_acquire(U2F->state_mutex, FuriWaitForever) == FuriStatusOk);
    if(U2F->pending_request &&
       pepper_key_constant_time_equal(U2F->pending_app_id, app_id, U2F_APP_ID_SIZE)) {
        u2f_clear_pending_request_locked(U2F);
    }
    furi_check(furi_mutex_release(U2F->state_mutex) == FuriStatusOk);
}

static uint8_t u2f_der_encode_int(uint8_t* der, uint8_t* val, uint8_t val_len) {
    der[0] = 0x02; // Integer

    uint8_t len = 2;
    // Omit leading zeros
    while((val_len > 1) && (val[0] == 0)) {
        ++val;
        --val_len;
    }

    // Check if integer is negative
    if(val[0] > 0x7f) der[len++] = 0;

    memcpy(der + len, val, val_len);
    len += val_len;

    der[1] = len - 2;
    return len;
}

static uint8_t u2f_der_encode_signature(uint8_t* der, uint8_t* sig) {
    der[0] = 0x30;

    uint8_t len = 2;
    len += u2f_der_encode_int(der + len, sig, U2F_HASH_SIZE);
    len += u2f_der_encode_int(der + len, sig + U2F_HASH_SIZE, U2F_HASH_SIZE);

    der[1] = len - 2;
    return len;
}

static void
    u2f_ecc_sign(mbedtls_ecp_group* grp, const uint8_t* key, uint8_t* hash, uint8_t* signature) {
    mbedtls_mpi r, s, d;

    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);
    mbedtls_mpi_init(&d);

    MCHECK(mbedtls_mpi_read_binary(&d, key, U2F_EC_KEY_SIZE));
    MCHECK(mbedtls_ecdsa_sign(grp, &r, &s, &d, hash, U2F_HASH_SIZE, u2f_uecc_random_cb, NULL));
    MCHECK(mbedtls_mpi_write_binary(&r, signature, U2F_EC_BIGNUM_SIZE));
    MCHECK(mbedtls_mpi_write_binary(&s, signature + U2F_EC_BIGNUM_SIZE, U2F_EC_BIGNUM_SIZE));

    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_mpi_free(&d);
}

static void u2f_ecc_compute_public_key(
    mbedtls_ecp_group* grp,
    const uint8_t* private_key,
    U2fPubKey* public_key) {
    mbedtls_ecp_point Q;
    mbedtls_mpi d;
    size_t olen;

    mbedtls_ecp_point_init(&Q);
    mbedtls_mpi_init(&d);

    MCHECK(mbedtls_mpi_read_binary(&d, private_key, U2F_EC_KEY_SIZE));
    MCHECK(mbedtls_ecp_mul(grp, &Q, &d, &grp->G, u2f_uecc_random_cb, NULL));
    MCHECK(mbedtls_ecp_check_privkey(grp, &d));

    MCHECK(mbedtls_ecp_point_write_binary(
        grp, &Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &olen, (unsigned char*)public_key, sizeof(U2fPubKey)));

    mbedtls_ecp_point_free(&Q);
    mbedtls_mpi_free(&d);
}

///////////////////////////////////////////

static uint16_t u2f_register(U2fData* U2F, uint8_t* buf) {
    U2fRegisterReq* req = (U2fRegisterReq*)buf;
    U2fRegisterResp* resp = (U2fRegisterResp*)buf;
    U2fKeyHandle handle;
    uint8_t private[U2F_EC_KEY_SIZE];
    U2fPubKey pub_key;
    uint8_t hash[U2F_HASH_SIZE];
    uint8_t signature[U2F_EC_BIGNUM_SIZE * 2];

    if(u2f_data_check(false) == false) {
        U2F->ready = false;
        if(U2F->callback != NULL) U2F->callback(U2fNotifyError, U2F->context);
        memcpy(&buf[0], state_not_supported, 2);
        return 2;
    }

    if(!u2f_begin_request(U2F, U2fNotifyRegister, req->app_id)) {
        memcpy(&buf[0], state_user_missing, 2);
        return 2;
    }
    if(U2F->callback != NULL) U2F->callback(U2fNotifyRegister, U2F->context);
    if(!u2f_consume_user_presence(U2F, U2fNotifyRegister, req->app_id)) {
        memcpy(&buf[0], state_user_missing, 2);
        return 2;
    }

    handle.len = U2F_HASH_SIZE * 2;

    // Generate random nonce
    furi_hal_random_fill_buf(handle.nonce, 32);

    {
        mbedtls_md_context_t hmac_ctx;
        mbedtls_md_init(&hmac_ctx);
        MCHECK(mbedtls_md_setup(&hmac_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1));
        MCHECK(mbedtls_md_hmac_starts(&hmac_ctx, U2F->device_key, sizeof(U2F->device_key)));

        // Generate private key
        MCHECK(mbedtls_md_hmac_update(&hmac_ctx, req->app_id, sizeof(req->app_id)));
        MCHECK(mbedtls_md_hmac_update(&hmac_ctx, handle.nonce, sizeof(handle.nonce)));
        MCHECK(mbedtls_md_hmac_finish(&hmac_ctx, private));

        MCHECK(mbedtls_md_hmac_reset(&hmac_ctx));

        // Generate private key handle
        MCHECK(mbedtls_md_hmac_update(&hmac_ctx, private, sizeof(private)));
        MCHECK(mbedtls_md_hmac_update(&hmac_ctx, req->app_id, sizeof(req->app_id)));
        MCHECK(mbedtls_md_hmac_finish(&hmac_ctx, handle.hash));

        mbedtls_md_free(&hmac_ctx);
    }

    // Generate public key
    u2f_ecc_compute_public_key(&U2F->group, private, &pub_key);

    // Generate signature
    {
        uint8_t reserved_byte = 0;

        mbedtls_sha256_context sha_ctx;

        mbedtls_sha256_init(&sha_ctx);
        mbedtls_sha256_starts(&sha_ctx, 0);

        mbedtls_sha256_update(&sha_ctx, &reserved_byte, 1);
        mbedtls_sha256_update(&sha_ctx, req->app_id, sizeof(req->app_id));
        mbedtls_sha256_update(&sha_ctx, req->challenge, sizeof(req->challenge));
        mbedtls_sha256_update(&sha_ctx, handle.hash, handle.len);
        mbedtls_sha256_update(&sha_ctx, (uint8_t*)&pub_key, sizeof(U2fPubKey));

        mbedtls_sha256_finish(&sha_ctx, hash);
        mbedtls_sha256_free(&sha_ctx);
    }

    // Sign hash
    u2f_ecc_sign(&U2F->group, U2F->cert_key, hash, signature);

    // Encode response message
    resp->reserved = 0x05;
    memcpy(&(resp->pub_key), &pub_key, sizeof(U2fPubKey));
    memcpy(&(resp->key_handle), &handle, sizeof(U2fKeyHandle));
    uint32_t cert_len = u2f_data_cert_load(resp->cert);
    if(cert_len == 0) {
        pepper_key_secure_zero(private, sizeof(private));
        pepper_key_secure_zero(hash, sizeof(hash));
        pepper_key_secure_zero(signature, sizeof(signature));
        if(U2F->callback != NULL) U2F->callback(U2fNotifyError, U2F->context);
        memcpy(&buf[0], state_not_supported, 2);
        return 2;
    }
    uint8_t signature_len = u2f_der_encode_signature(resp->cert + cert_len, signature);
    memcpy(resp->cert + cert_len + signature_len, state_no_error, 2);

    pepper_key_secure_zero(private, sizeof(private));
    pepper_key_secure_zero(hash, sizeof(hash));
    pepper_key_secure_zero(signature, sizeof(signature));
    if(U2F->callback != NULL) U2F->callback(U2fNotifyRegisterSuccess, U2F->context);

    return sizeof(U2fRegisterResp) + cert_len + signature_len + 2;
}

static inline uint32_t u2f_to_big_endian(uint32_t a) {
    return __builtin_bswap32(a);
}

static uint16_t u2f_authenticate(U2fData* U2F, uint8_t* buf) {
    U2fAuthReq* req = (U2fAuthReq*)buf;
    U2fAuthResp* resp = (U2fAuthResp*)buf;
    uint8_t priv_key[U2F_EC_KEY_SIZE];
    uint8_t mac_control[32];
    const uint8_t flags = 1;
    uint8_t hash[U2F_HASH_SIZE];
    uint8_t signature[U2F_HASH_SIZE * 2];
    uint32_t be_u2f_counter;

    if(u2f_data_check(false) == false) {
        U2F->ready = false;
        if(U2F->callback != NULL) U2F->callback(U2fNotifyError, U2F->context);
        memcpy(&buf[0], state_not_supported, 2);
        return 2;
    }

    // Validate the key handle before showing an approval prompt.
    {
        mbedtls_md_context_t hmac_ctx;
        mbedtls_md_init(&hmac_ctx);
        MCHECK(mbedtls_md_setup(&hmac_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1));
        MCHECK(mbedtls_md_hmac_starts(&hmac_ctx, U2F->device_key, sizeof(U2F->device_key)));

        // Recover private key
        MCHECK(mbedtls_md_hmac_update(&hmac_ctx, req->app_id, sizeof(req->app_id)));
        MCHECK(mbedtls_md_hmac_update(
            &hmac_ctx, req->key_handle.nonce, sizeof(req->key_handle.nonce)));
        MCHECK(mbedtls_md_hmac_finish(&hmac_ctx, priv_key));

        MCHECK(mbedtls_md_hmac_reset(&hmac_ctx));

        // Generate and verify private key handle
        MCHECK(mbedtls_md_hmac_update(&hmac_ctx, priv_key, sizeof(priv_key)));
        MCHECK(mbedtls_md_hmac_update(&hmac_ctx, req->app_id, sizeof(req->app_id)));
        MCHECK(mbedtls_md_hmac_finish(&hmac_ctx, mac_control));

        mbedtls_md_free(&hmac_ctx);
    }

    if(!pepper_key_constant_time_equal(req->key_handle.hash, mac_control, sizeof(mac_control))) {
        FURI_LOG_W(TAG, "Wrong handle!");
        pepper_key_secure_zero(priv_key, sizeof(priv_key));
        pepper_key_secure_zero(mac_control, sizeof(mac_control));
        memcpy(&buf[0], state_wrong_data, 2);
        return 2;
    }

    if(req->p1 == U2fCheckOnly) { // Check-only: don't need to send full response
        pepper_key_secure_zero(priv_key, sizeof(priv_key));
        pepper_key_secure_zero(mac_control, sizeof(mac_control));
        memcpy(&buf[0], state_user_missing, 2);
        return 2;
    }
    if((req->p1 != U2fEnforce) && (req->p1 != U2fDontEnforce)) {
        pepper_key_secure_zero(priv_key, sizeof(priv_key));
        pepper_key_secure_zero(mac_control, sizeof(mac_control));
        memcpy(&buf[0], state_wrong_data, 2);
        return 2;
    }

    // PepperKey requires explicit presence even if a client requests the legacy
    // U2F "don't enforce" mode.
    if(!u2f_begin_request(U2F, U2fNotifyAuth, req->app_id)) {
        pepper_key_secure_zero(priv_key, sizeof(priv_key));
        pepper_key_secure_zero(mac_control, sizeof(mac_control));
        memcpy(&buf[0], state_user_missing, 2);
        return 2;
    }
    if(U2F->callback != NULL) U2F->callback(U2fNotifyAuth, U2F->context);
    if(!u2f_consume_user_presence(U2F, U2fNotifyAuth, req->app_id)) {
        pepper_key_secure_zero(priv_key, sizeof(priv_key));
        pepper_key_secure_zero(mac_control, sizeof(mac_control));
        memcpy(&buf[0], state_user_missing, 2);
        return 2;
    }

    if(U2F->counter == UINT32_MAX) {
        pepper_key_secure_zero(priv_key, sizeof(priv_key));
        pepper_key_secure_zero(mac_control, sizeof(mac_control));
        if(U2F->callback != NULL) U2F->callback(U2fNotifyError, U2F->context);
        memcpy(&buf[0], state_not_supported, 2);
        return 2;
    }

    // Persist before responding: skipping a counter is safe, reusing one is not.
    const uint32_t next_counter = U2F->counter + 1;
    if(!u2f_data_cnt_write(next_counter)) {
        pepper_key_secure_zero(priv_key, sizeof(priv_key));
        pepper_key_secure_zero(mac_control, sizeof(mac_control));
        if(U2F->callback != NULL) U2F->callback(U2fNotifyError, U2F->context);
        memcpy(&buf[0], state_not_supported, 2);
        return 2;
    }
    U2F->counter = next_counter;
    be_u2f_counter = u2f_to_big_endian(U2F->counter);

    // Generate the signed response hash.
    {
        mbedtls_sha256_context sha_ctx;

        mbedtls_sha256_init(&sha_ctx);
        mbedtls_sha256_starts(&sha_ctx, 0);

        mbedtls_sha256_update(&sha_ctx, req->app_id, sizeof(req->app_id));
        mbedtls_sha256_update(&sha_ctx, &flags, 1);
        mbedtls_sha256_update(&sha_ctx, (uint8_t*)&(be_u2f_counter), sizeof(be_u2f_counter));
        mbedtls_sha256_update(&sha_ctx, req->challenge, sizeof(req->challenge));

        mbedtls_sha256_finish(&sha_ctx, hash);
        mbedtls_sha256_free(&sha_ctx);
    }

    // Sign hash
    u2f_ecc_sign(&U2F->group, priv_key, hash, signature);

    resp->user_present = flags;
    resp->counter = be_u2f_counter;
    uint8_t signature_len = u2f_der_encode_signature(resp->signature, signature);
    memcpy(resp->signature + signature_len, state_no_error, 2);

    pepper_key_secure_zero(priv_key, sizeof(priv_key));
    pepper_key_secure_zero(mac_control, sizeof(mac_control));
    pepper_key_secure_zero(hash, sizeof(hash));
    pepper_key_secure_zero(signature, sizeof(signature));

    if(U2F->callback != NULL) U2F->callback(U2fNotifyAuthSuccess, U2F->context);

    return sizeof(U2fAuthResp) + signature_len + 2;
}

uint16_t u2f_msg_parse(U2fData* U2F, uint8_t* buf, uint16_t len) {
    furi_assert(U2F);
    furi_assert(buf);
    if(!U2F->ready) return 0;
    if(len < 2) return 0;
    if(buf[0] != 0x00) {
        memcpy(&buf[0], state_not_supported, 2);
        return 2;
    }
    if(buf[1] == U2F_CMD_REGISTER) { // Register request
        if(len < sizeof(U2fRegisterReq)) {
            memcpy(&buf[0], state_wrong_data, 2);
            return 2;
        }
        return u2f_register(U2F, buf);

    } else if(buf[1] == U2F_CMD_AUTHENTICATE) { // Authenticate request
        if(len < sizeof(U2fAuthReq)) {
            memcpy(&buf[0], state_wrong_data, 2);
            return 2;
        }
        U2fAuthReq* req = (U2fAuthReq*)buf;
        if(req->key_handle.len != (U2F_HASH_SIZE + U2F_NONCE_SIZE)) {
            memcpy(&buf[0], state_wrong_data, 2);
            return 2;
        }
        return u2f_authenticate(U2F, buf);

    } else if(buf[1] == U2F_CMD_VERSION) { // Get U2F version string
        memcpy(&buf[0], ver_str, 6);
        memcpy(&buf[6], state_no_error, 2);
        return 8;
    } else {
        memcpy(&buf[0], state_not_supported, 2);
        return 2;
    }
}

void u2f_wink(U2fData* U2F) {
    if(U2F->callback != NULL) U2F->callback(U2fNotifyWink, U2F->context);
}

void u2f_set_state(U2fData* U2F, uint8_t state) {
    if(state == 0) {
        if(U2F->callback != NULL) U2F->callback(U2fNotifyDisconnect, U2F->context);
    } else {
        if(U2F->callback != NULL) U2F->callback(U2fNotifyConnect, U2F->context);
    }
    furi_check(furi_mutex_acquire(U2F->state_mutex, FuriWaitForever) == FuriStatusOk);
    u2f_clear_pending_request_locked(U2F);
    furi_check(furi_mutex_release(U2F->state_mutex) == FuriStatusOk);
}
