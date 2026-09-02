#pragma once

#include "pepper_crypto.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool pepper_hotp_generate(
    PepperHashAlgorithm algorithm,
    const uint8_t* secret,
    size_t secret_size,
    uint64_t counter,
    uint8_t digits,
    uint32_t* code);

bool pepper_totp_generate(
    PepperHashAlgorithm algorithm,
    const uint8_t* secret,
    size_t secret_size,
    uint64_t unix_time,
    uint64_t epoch,
    uint32_t period,
    uint8_t digits,
    uint32_t* code,
    uint32_t* seconds_remaining);

bool pepper_otp_format(uint32_t code, uint8_t digits, char* output, size_t output_size);

#ifdef __cplusplus
}
#endif
