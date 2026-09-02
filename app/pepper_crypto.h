#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PepperHashSha1,
    PepperHashSha256,
    PepperHashSha512,
} PepperHashAlgorithm;

size_t pepper_crypto_digest_size(PepperHashAlgorithm algorithm);

bool pepper_crypto_hmac(
    PepperHashAlgorithm algorithm,
    const uint8_t* key,
    size_t key_size,
    const uint8_t* message,
    size_t message_size,
    uint8_t* output,
    size_t output_size);

#ifdef __cplusplus
}
#endif
