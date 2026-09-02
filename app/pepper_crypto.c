#include "pepper_crypto.h"
#include "pepper_secure.h"

#include <mbedtls/md.h>

size_t pepper_crypto_digest_size(PepperHashAlgorithm algorithm) {
    switch(algorithm) {
    case PepperHashSha1:
        return 20;
    case PepperHashSha256:
        return 32;
    case PepperHashSha512:
        return 64;
    default:
        return 0;
    }
}

static mbedtls_md_type_t pepper_crypto_md_type(PepperHashAlgorithm algorithm) {
    switch(algorithm) {
    case PepperHashSha1:
        return MBEDTLS_MD_SHA1;
    case PepperHashSha256:
        return MBEDTLS_MD_SHA256;
    case PepperHashSha512:
        return MBEDTLS_MD_SHA512;
    default:
        return MBEDTLS_MD_NONE;
    }
}

bool pepper_crypto_hmac(
    PepperHashAlgorithm algorithm,
    const uint8_t* key,
    size_t key_size,
    const uint8_t* message,
    size_t message_size,
    uint8_t* output,
    size_t output_size) {
    const size_t digest_size = pepper_crypto_digest_size(algorithm);
    if(!key || !key_size || (!message && message_size) || !output || output_size < digest_size) {
        return false;
    }

    const mbedtls_md_info_t* info = mbedtls_md_info_from_type(pepper_crypto_md_type(algorithm));
    if(!info) return false;

    bool success = false;
    mbedtls_md_context_t context;
    mbedtls_md_init(&context);
    if(mbedtls_md_setup(&context, info, 1) != 0) goto cleanup;
    if(mbedtls_md_hmac_starts(&context, key, key_size) != 0) goto cleanup;
    if(message_size && mbedtls_md_hmac_update(&context, message, message_size) != 0) goto cleanup;
    if(mbedtls_md_hmac_finish(&context, output) != 0) goto cleanup;
    success = true;

cleanup:
    mbedtls_md_free(&context);
    if(!success) pepper_secure_zero(output, output_size);
    return success;
}
