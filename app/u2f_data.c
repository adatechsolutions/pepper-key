#include <furi.h>
#include "u2f_data.h"
#include <furi_hal.h>
#include <storage/storage.h>
#include <furi_hal_random.h>
#include <flipper_format/flipper_format.h>

#define TAG "PepperKeyData"

#define U2F_CERT_FILE                       APP_ASSETS_PATH("assets/cert.der")
#define U2F_CERT_KEY_FILE                   APP_ASSETS_PATH("assets/cert_key.u2f")
#define U2F_KEY_FILE                        APP_DATA_PATH("key.u2f")
#define U2F_CNT_FILE                        APP_DATA_PATH("cnt.u2f")
#define U2F_CERT_MAX_SIZE                   1024
#define U2F_CIPHERTEXT_SIZE                 32
#define U2F_STOCK_CERT_LEGACY_ENVELOPE_SIZE 48

#define U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_FACTORY 2
#define U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE  FURI_HAL_CRYPTO_ENCLAVE_UNIQUE_KEY_SLOT

#define U2F_CERT_STOCK 0 // Stock certificate, private key is encrypted with factory key
#define U2F_CERT_USER  1 // User certificate, private key is encrypted with unique key
#define U2F_CERT_USER_UNENCRYPTED \
    2 // Unencrypted user certificate, will be encrypted after first load

#define U2F_CERT_KEY_FILE_TYPE "Flipper U2F Certificate Key File"
#define U2F_CERT_KEY_VERSION   1

#define U2F_DEVICE_KEY_FILE_TYPE "Flipper U2F Device Key File"
#define U2F_DEVICE_KEY_VERSION   2

#define U2F_COUNTER_FILE_TYPE "Flipper U2F Counter File"
#define U2F_COUNTER_VERSION   3

#define U2F_COUNTER_CONTROL_VAL 0xAA5500FF

typedef struct {
    uint32_t counter;
    uint8_t random_salt[24];
    uint32_t control;
} FURI_PACKED U2fCounterData;

_Static_assert(sizeof(U2fCounterData) == U2F_CIPHERTEXT_SIZE, "Counter record must be 32 bytes");

static void pepper_key_data_secure_zero(void* data, size_t size) {
    volatile uint8_t* cursor = data;
    while(size--)
        *cursor++ = 0;
}

static U2fDataLoadResult u2f_data_file_status(const char* path) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    const FS_Error error = storage_common_stat(storage, path, NULL);
    furi_record_close(RECORD_STORAGE);

    if(error == FSE_OK) return U2fDataLoadOk;
    if(error == FSE_NOT_EXIST) return U2fDataLoadMissing;

    FURI_LOG_E(TAG, "Unable to inspect app data: %d", error);
    return U2fDataLoadError;
}

bool u2f_data_check(bool cert_only) {
    bool state = false;
    Storage* fs_api = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(fs_api);

    do {
        if(!storage_file_open(file, U2F_CERT_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) break;
        storage_file_close(file);
        if(!storage_file_open(file, U2F_CERT_KEY_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) break;
        if(cert_only) {
            state = true;
            break;
        }
        storage_file_close(file);
        if(!storage_file_open(file, U2F_KEY_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) break;
        storage_file_close(file);
        if(!storage_file_open(file, U2F_CNT_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) break;
        state = true;
    } while(0);

    storage_file_close(file);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);

    return state;
}

bool u2f_data_cert_check(void) {
    bool state = false;
    Storage* fs_api = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(fs_api);
    uint8_t file_buf[8];

    if(storage_file_open(file, U2F_CERT_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
        do {
            // Read header to check certificate size
            size_t file_size = storage_file_size(file);
            if((file_size < 4) || (file_size > U2F_CERT_MAX_SIZE)) break;
            size_t len_cur = storage_file_read(file, file_buf, 4);
            if(len_cur != 4) break;

            if(file_buf[0] != 0x30) {
                FURI_LOG_E(TAG, "Wrong certificate header");
                break;
            }

            size_t temp_len = ((file_buf[2] << 8) | (file_buf[3])) + 4;
            if(temp_len != file_size) {
                FURI_LOG_E(TAG, "Wrong certificate length");
                break;
            }
            state = true;
        } while(0);
    }

    storage_file_close(file);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);

    return state;
}

uint32_t u2f_data_cert_load(uint8_t* cert) {
    furi_assert(cert);

    Storage* fs_api = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(fs_api);
    uint32_t file_size = 0;
    uint32_t len_cur = 0;

    if(storage_file_open(file, U2F_CERT_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
        file_size = storage_file_size(file);
        if((file_size >= 4) && (file_size <= U2F_CERT_MAX_SIZE)) {
            len_cur = storage_file_read(file, cert, file_size);
            if(len_cur != file_size) len_cur = 0;
        }
    }

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    return len_cur;
}

static bool u2f_data_cert_key_encrypt(uint8_t* cert_key) {
    furi_assert(cert_key);

    bool state = false;
    uint8_t iv[16];
    uint8_t key[U2F_CIPHERTEXT_SIZE] = {0};
    uint32_t cert_type = U2F_CERT_USER;

    FURI_LOG_I(TAG, "Encrypting user cert key");

    // Generate random IV
    furi_hal_random_fill_buf(iv, 16);

    if(!furi_hal_crypto_enclave_load_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE, iv)) {
        FURI_LOG_E(TAG, "Unable to load encryption key");
        pepper_key_data_secure_zero(key, sizeof(key));
        return false;
    }

    if(!furi_hal_crypto_encrypt(cert_key, key, sizeof(key))) {
        FURI_LOG_E(TAG, "Encryption failed");
        furi_hal_crypto_enclave_unload_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE);
        pepper_key_data_secure_zero(key, sizeof(key));
        return false;
    }
    furi_hal_crypto_enclave_unload_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* flipper_format = flipper_format_file_alloc(storage);

    if(flipper_format_file_open_always(flipper_format, U2F_CERT_KEY_FILE)) {
        do {
            if(!flipper_format_write_header_cstr(
                   flipper_format, U2F_CERT_KEY_FILE_TYPE, U2F_CERT_KEY_VERSION))
                break;
            if(!flipper_format_write_uint32(flipper_format, "Type", &cert_type, 1)) break;
            if(!flipper_format_write_hex(flipper_format, "IV", iv, 16)) break;
            if(!flipper_format_write_hex(flipper_format, "Data", key, sizeof(key))) break;
            state = true;
        } while(0);
    }

    flipper_format_free(flipper_format);
    furi_record_close(RECORD_STORAGE);

    pepper_key_data_secure_zero(key, sizeof(key));

    return state;
}

bool u2f_data_cert_key_load(uint8_t* cert_key) {
    furi_assert(cert_key);

    bool state = false;
    uint8_t iv[16];
    uint8_t key[U2F_STOCK_CERT_LEGACY_ENVELOPE_SIZE] = {0};
    uint32_t cert_type = 0;
    uint8_t key_slot = 0;
    uint32_t version = 0;
    uint32_t data_size = 0;

    memset(cert_key, 0, U2F_CIPHERTEXT_SIZE);

    FuriString* filetype;
    filetype = furi_string_alloc();

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* flipper_format = flipper_format_file_alloc(storage);

    if(flipper_format_file_open_existing(flipper_format, U2F_CERT_KEY_FILE)) {
        do {
            if(!flipper_format_read_header(flipper_format, filetype, &version)) {
                FURI_LOG_E(TAG, "Missing or incorrect header");
                break;
            }

            if(strcmp(furi_string_get_cstr(filetype), U2F_CERT_KEY_FILE_TYPE) != 0 ||
               version != U2F_CERT_KEY_VERSION) {
                FURI_LOG_E(TAG, "Type or version mismatch");
                break;
            }

            if(!flipper_format_read_uint32(flipper_format, "Type", &cert_type, 1)) {
                FURI_LOG_E(TAG, "Missing cert type");
                break;
            }

            if(cert_type == U2F_CERT_STOCK) {
                key_slot = U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_FACTORY;
                data_size = U2F_STOCK_CERT_LEGACY_ENVELOPE_SIZE;
            } else if(cert_type == U2F_CERT_USER) {
                key_slot = U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE;
                data_size = U2F_CIPHERTEXT_SIZE;
            } else if(cert_type == U2F_CERT_USER_UNENCRYPTED) {
                key_slot = 0;
                data_size = U2F_CIPHERTEXT_SIZE;
            } else {
                FURI_LOG_E(TAG, "Unknown cert type");
                break;
            }
            uint32_t stored_data_size = 0;
            if(!flipper_format_get_value_count(flipper_format, "Data", &stored_data_size) ||
               stored_data_size != data_size) {
                FURI_LOG_E(TAG, "Unexpected cert key data size");
                break;
            }
            if(key_slot != 0) {
                if(!flipper_format_read_hex(flipper_format, "IV", iv, 16)) {
                    FURI_LOG_E(TAG, "Missing IV");
                    break;
                }

                if(!flipper_format_read_hex(flipper_format, "Data", key, data_size)) {
                    FURI_LOG_E(TAG, "Missing data");
                    break;
                }

                if(!furi_hal_crypto_enclave_load_key(key_slot, iv)) {
                    FURI_LOG_E(TAG, "Unable to load encryption key");
                    break;
                }
                if(!furi_hal_crypto_decrypt(key, cert_key, U2F_CIPHERTEXT_SIZE)) {
                    memset(cert_key, 0, U2F_CIPHERTEXT_SIZE);
                    FURI_LOG_E(TAG, "Decryption failed");
                    furi_hal_crypto_enclave_unload_key(key_slot);
                    break;
                }
                furi_hal_crypto_enclave_unload_key(key_slot);
            } else {
                if(!flipper_format_read_hex(
                       flipper_format, "Data", cert_key, U2F_CIPHERTEXT_SIZE)) {
                    FURI_LOG_E(TAG, "Missing data");
                    break;
                }
            }
            state = true;
        } while(0);
    }

    flipper_format_free(flipper_format);
    furi_record_close(RECORD_STORAGE);
    furi_string_free(filetype);

    pepper_key_data_secure_zero(key, sizeof(key));

    if(state && cert_type == U2F_CERT_USER_UNENCRYPTED) {
        if(!furi_hal_crypto_enclave_ensure_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE)) {
            pepper_key_data_secure_zero(cert_key, U2F_CIPHERTEXT_SIZE);
            return false;
        }
        state = u2f_data_cert_key_encrypt(cert_key);
    }

    return state;
}

U2fDataLoadResult u2f_data_key_load(uint8_t* device_key) {
    furi_assert(device_key);

    const U2fDataLoadResult file_status = u2f_data_file_status(U2F_KEY_FILE);
    if(file_status != U2fDataLoadOk) return file_status;

    bool state = false;
    uint8_t iv[16];
    uint8_t key[U2F_CIPHERTEXT_SIZE] = {0};
    uint32_t version = 0;
    uint32_t data_size = 0;

    memset(device_key, 0, U2F_CIPHERTEXT_SIZE);

    FuriString* filetype;
    filetype = furi_string_alloc();

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* flipper_format = flipper_format_file_alloc(storage);

    if(flipper_format_file_open_existing(flipper_format, U2F_KEY_FILE)) {
        do {
            if(!flipper_format_read_header(flipper_format, filetype, &version)) {
                FURI_LOG_E(TAG, "Missing or incorrect header");
                break;
            }
            if(strcmp(furi_string_get_cstr(filetype), U2F_DEVICE_KEY_FILE_TYPE) != 0 ||
               version != U2F_DEVICE_KEY_VERSION) {
                FURI_LOG_E(TAG, "Type or version mismatch");
                break;
            }
            if(!flipper_format_read_hex(flipper_format, "IV", iv, 16)) {
                FURI_LOG_E(TAG, "Missing IV");
                break;
            }
            if(!flipper_format_get_value_count(flipper_format, "Data", &data_size) ||
               data_size != sizeof(key) ||
               !flipper_format_read_hex(flipper_format, "Data", key, sizeof(key))) {
                FURI_LOG_E(TAG, "Missing data");
                break;
            }
            if(!furi_hal_crypto_enclave_load_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE, iv)) {
                FURI_LOG_E(TAG, "Unable to load encryption key");
                break;
            }
            if(!furi_hal_crypto_decrypt(key, device_key, sizeof(key))) {
                memset(device_key, 0, U2F_CIPHERTEXT_SIZE);
                FURI_LOG_E(TAG, "Decryption failed");
                furi_hal_crypto_enclave_unload_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE);
                break;
            }
            furi_hal_crypto_enclave_unload_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE);
            state = true;
        } while(0);
    }
    flipper_format_free(flipper_format);
    furi_record_close(RECORD_STORAGE);
    furi_string_free(filetype);
    pepper_key_data_secure_zero(key, sizeof(key));
    return state ? U2fDataLoadOk : U2fDataLoadError;
}

bool u2f_data_key_create(uint8_t* device_key) {
    furi_assert(device_key);

    bool state = false;
    bool file_created = false;
    uint8_t iv[16];
    uint8_t key[U2F_CIPHERTEXT_SIZE];
    uint8_t key_encrypted[U2F_CIPHERTEXT_SIZE] = {0};

    // Generate random IV and key
    furi_hal_random_fill_buf(iv, 16);
    furi_hal_random_fill_buf(key, sizeof(key));

    if(!furi_hal_crypto_enclave_load_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE, iv)) {
        FURI_LOG_E(TAG, "Unable to load encryption key");
        pepper_key_data_secure_zero(key, sizeof(key));
        pepper_key_data_secure_zero(key_encrypted, sizeof(key_encrypted));
        return false;
    }

    if(!furi_hal_crypto_encrypt(key, key_encrypted, sizeof(key_encrypted))) {
        FURI_LOG_E(TAG, "Encryption failed");
        furi_hal_crypto_enclave_unload_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE);
        pepper_key_data_secure_zero(key, sizeof(key));
        pepper_key_data_secure_zero(key_encrypted, sizeof(key_encrypted));
        return false;
    }
    furi_hal_crypto_enclave_unload_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* flipper_format = flipper_format_file_alloc(storage);

    if(flipper_format_file_open_new(flipper_format, U2F_KEY_FILE)) {
        file_created = true;
        do {
            if(!flipper_format_write_header_cstr(
                   flipper_format, U2F_DEVICE_KEY_FILE_TYPE, U2F_DEVICE_KEY_VERSION))
                break;
            if(!flipper_format_write_hex(flipper_format, "IV", iv, 16)) break;
            if(!flipper_format_write_hex(
                   flipper_format, "Data", key_encrypted, sizeof(key_encrypted)))
                break;
            state = true;
            memcpy(device_key, key, sizeof(key));
        } while(0);
    }

    flipper_format_free(flipper_format);
    furi_record_close(RECORD_STORAGE);

    if(file_created && !state) {
        Storage* cleanup_storage = furi_record_open(RECORD_STORAGE);
        storage_common_remove(cleanup_storage, U2F_KEY_FILE);
        furi_record_close(RECORD_STORAGE);
    }

    pepper_key_data_secure_zero(key, sizeof(key));
    pepper_key_data_secure_zero(key_encrypted, sizeof(key_encrypted));

    return state;
}

bool u2f_data_key_remove_created(void) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    const FS_Error error = storage_common_remove(storage, U2F_KEY_FILE);
    furi_record_close(RECORD_STORAGE);
    return error == FSE_OK;
}

U2fDataLoadResult u2f_data_cnt_read(uint32_t* cnt_val) {
    furi_assert(cnt_val);

    const U2fDataLoadResult file_status = u2f_data_file_status(U2F_CNT_FILE);
    if(file_status != U2fDataLoadOk) return file_status;

    bool state = false;
    uint8_t iv[16];
    U2fCounterData cnt;
    uint8_t cnt_encr[U2F_CIPHERTEXT_SIZE] = {0};
    uint32_t version = 0;
    uint32_t data_size = 0;

    FuriString* filetype;
    filetype = furi_string_alloc();

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* flipper_format = flipper_format_file_alloc(storage);

    if(flipper_format_file_open_existing(flipper_format, U2F_CNT_FILE)) {
        do {
            if(!flipper_format_read_header(flipper_format, filetype, &version)) {
                FURI_LOG_E(TAG, "Missing or incorrect header");
                break;
            }
            if(strcmp(furi_string_get_cstr(filetype), U2F_COUNTER_FILE_TYPE) != 0) {
                FURI_LOG_E(TAG, "Type mismatch");
                break;
            }
            if(version != U2F_COUNTER_VERSION) {
                FURI_LOG_E(TAG, "Version mismatch");
                break;
            }
            if(!flipper_format_read_hex(flipper_format, "IV", iv, 16)) {
                FURI_LOG_E(TAG, "Missing IV");
                break;
            }
            if(!flipper_format_get_value_count(flipper_format, "Data", &data_size) ||
               data_size != sizeof(cnt_encr) ||
               !flipper_format_read_hex(flipper_format, "Data", cnt_encr, sizeof(cnt_encr))) {
                FURI_LOG_E(TAG, "Missing data");
                break;
            }
            if(!furi_hal_crypto_enclave_load_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE, iv)) {
                FURI_LOG_E(TAG, "Unable to load encryption key");
                break;
            }
            memset(&cnt, 0, sizeof(U2fCounterData));
            if(!furi_hal_crypto_decrypt(cnt_encr, (uint8_t*)&cnt, sizeof(U2fCounterData))) {
                memset(&cnt, 0, sizeof(U2fCounterData));
                FURI_LOG_E(TAG, "Decryption failed");
                furi_hal_crypto_enclave_unload_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE);
                break;
            }
            furi_hal_crypto_enclave_unload_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE);
            if(cnt.control == U2F_COUNTER_CONTROL_VAL) {
                *cnt_val = cnt.counter;
                state = true;
            }
        } while(0);
    }
    flipper_format_free(flipper_format);
    furi_record_close(RECORD_STORAGE);
    furi_string_free(filetype);
    pepper_key_data_secure_zero(&cnt, sizeof(cnt));
    pepper_key_data_secure_zero(cnt_encr, sizeof(cnt_encr));
    return state ? U2fDataLoadOk : U2fDataLoadError;
}

static bool u2f_data_cnt_store(uint32_t cnt_val, bool create_new) {
    bool state = false;
    bool file_created = false;
    uint8_t iv[16];
    U2fCounterData cnt;
    uint8_t cnt_encr[U2F_CIPHERTEXT_SIZE] = {0};

    // Generate random IV and key
    furi_hal_random_fill_buf(iv, 16);
    furi_hal_random_fill_buf(cnt.random_salt, 24);
    cnt.control = U2F_COUNTER_CONTROL_VAL;
    cnt.counter = cnt_val;

    if(!furi_hal_crypto_enclave_load_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE, iv)) {
        FURI_LOG_E(TAG, "Unable to load encryption key");
        pepper_key_data_secure_zero(&cnt, sizeof(cnt));
        return false;
    }

    if(!furi_hal_crypto_encrypt((uint8_t*)&cnt, cnt_encr, sizeof(cnt_encr))) {
        FURI_LOG_E(TAG, "Encryption failed");
        furi_hal_crypto_enclave_unload_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE);
        pepper_key_data_secure_zero(&cnt, sizeof(cnt));
        pepper_key_data_secure_zero(cnt_encr, sizeof(cnt_encr));
        return false;
    }
    furi_hal_crypto_enclave_unload_key(U2F_DATA_FILE_ENCRYPTION_KEY_SLOT_UNIQUE);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* flipper_format = flipper_format_file_alloc(storage);

    const bool opened = create_new ?
                            flipper_format_file_open_new(flipper_format, U2F_CNT_FILE) :
                            flipper_format_file_open_existing(flipper_format, U2F_CNT_FILE);
    if(opened) {
        file_created = create_new;
        do {
            if(create_new) {
                if(!flipper_format_write_header_cstr(
                       flipper_format, U2F_COUNTER_FILE_TYPE, U2F_COUNTER_VERSION))
                    break;
                if(!flipper_format_write_hex(flipper_format, "IV", iv, sizeof(iv))) break;
                if(!flipper_format_write_hex(flipper_format, "Data", cnt_encr, sizeof(cnt_encr)))
                    break;
            } else {
                FuriString* filetype = furi_string_alloc();
                uint32_t version = 0;
                uint32_t iv_size = 0;
                uint32_t data_size = 0;
                const bool valid_record =
                    flipper_format_read_header(flipper_format, filetype, &version) &&
                    strcmp(furi_string_get_cstr(filetype), U2F_COUNTER_FILE_TYPE) == 0 &&
                    version == U2F_COUNTER_VERSION &&
                    flipper_format_get_value_count(flipper_format, "IV", &iv_size) &&
                    iv_size == sizeof(iv) &&
                    flipper_format_get_value_count(flipper_format, "Data", &data_size) &&
                    data_size == sizeof(cnt_encr);
                furi_string_free(filetype);
                if(!valid_record || !flipper_format_rewind(flipper_format) ||
                   !flipper_format_update_hex(flipper_format, "IV", iv, sizeof(iv)) ||
                   !flipper_format_rewind(flipper_format) ||
                   !flipper_format_update_hex(flipper_format, "Data", cnt_encr, sizeof(cnt_encr)))
                    break;
            }
            state = true;
        } while(0);
    }

    flipper_format_free(flipper_format);
    furi_record_close(RECORD_STORAGE);

    if(file_created && !state) {
        Storage* cleanup_storage = furi_record_open(RECORD_STORAGE);
        storage_common_remove(cleanup_storage, U2F_CNT_FILE);
        furi_record_close(RECORD_STORAGE);
    }

    pepper_key_data_secure_zero(&cnt, sizeof(cnt));
    pepper_key_data_secure_zero(cnt_encr, sizeof(cnt_encr));

    return state;
}

bool u2f_data_cnt_create(uint32_t cnt_val) {
    return u2f_data_cnt_store(cnt_val, true);
}

bool u2f_data_cnt_write(uint32_t cnt_val) {
    return u2f_data_cnt_store(cnt_val, false);
}
