#include "pepper_crypto.h"
#include "pepper_secure.h"

#include <limits.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

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

static const EVP_MD* pepper_crypto_md(PepperHashAlgorithm algorithm) {
    switch(algorithm) {
    case PepperHashSha1:
        return EVP_sha1();
    case PepperHashSha256:
        return EVP_sha256();
    case PepperHashSha512:
        return EVP_sha512();
    default:
        return NULL;
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
    const EVP_MD* digest = pepper_crypto_md(algorithm);
    const size_t expected_size = pepper_crypto_digest_size(algorithm);
    if(!digest || !key || !key_size || key_size > INT_MAX || (!message && message_size) ||
       !output || output_size < expected_size) {
        return false;
    }

    unsigned int actual_size = 0;
    if(!HMAC(
           digest,
           key,
           (int)key_size,
           message,
           message_size,
           output,
           &actual_size) ||
       actual_size != expected_size) {
        pepper_secure_zero(output, output_size);
        return false;
    }
    return true;
}
