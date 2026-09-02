#include "pepper_otp.h"
#include "pepper_secure.h"

#include <stdio.h>

static uint32_t pepper_otp_modulus(uint8_t digits) {
    static const uint32_t modulus[] = {1000000U, 10000000U, 100000000U};
    return (digits >= 6 && digits <= 8) ? modulus[digits - 6] : 0;
}

bool pepper_hotp_generate(
    PepperHashAlgorithm algorithm,
    const uint8_t* secret,
    size_t secret_size,
    uint64_t counter,
    uint8_t digits,
    uint32_t* code) {
    if(code) *code = 0;
    const uint32_t modulus = pepper_otp_modulus(digits);
    const size_t digest_size = pepper_crypto_digest_size(algorithm);
    if(!secret || secret_size < 10 || secret_size > 64 || !code || !modulus || digest_size > 64 ||
       digest_size < 20) {
        return false;
    }

    uint8_t moving_factor[8];
    for(size_t index = 0; index < sizeof(moving_factor); index++) {
        moving_factor[sizeof(moving_factor) - 1 - index] = (uint8_t)(counter >> (index * 8));
    }

    uint8_t digest[64] = {0};
    if(!pepper_crypto_hmac(
           algorithm,
           secret,
           secret_size,
           moving_factor,
           sizeof(moving_factor),
           digest,
           sizeof(digest))) {
        pepper_secure_zero(moving_factor, sizeof(moving_factor));
        return false;
    }

    const uint8_t offset = digest[digest_size - 1] & 0x0f;
    if((size_t)offset + 4 > digest_size) {
        pepper_secure_zero(digest, sizeof(digest));
        pepper_secure_zero(moving_factor, sizeof(moving_factor));
        return false;
    }
    const uint32_t binary = ((uint32_t)(digest[offset] & 0x7f) << 24) |
                            ((uint32_t)digest[offset + 1] << 16) |
                            ((uint32_t)digest[offset + 2] << 8) | digest[offset + 3];
    *code = binary % modulus;

    pepper_secure_zero(digest, sizeof(digest));
    pepper_secure_zero(moving_factor, sizeof(moving_factor));
    return true;
}

bool pepper_totp_generate(
    PepperHashAlgorithm algorithm,
    const uint8_t* secret,
    size_t secret_size,
    uint64_t unix_time,
    uint64_t epoch,
    uint32_t period,
    uint8_t digits,
    uint32_t* code,
    uint32_t* seconds_remaining) {
    if(code) *code = 0;
    if(seconds_remaining) *seconds_remaining = 0;
    if(period < 15 || period > 120 || unix_time < epoch || !seconds_remaining) return false;
    const uint64_t elapsed = unix_time - epoch;
    const uint64_t counter = elapsed / period;
    const uint32_t into_period = (uint32_t)(elapsed % period);
    if(!pepper_hotp_generate(algorithm, secret, secret_size, counter, digits, code)) return false;
    *seconds_remaining = period - into_period;
    return true;
}

bool pepper_otp_format(uint32_t code, uint8_t digits, char* output, size_t output_size) {
    if(!output || digits < 6 || digits > 8 || output_size < (size_t)digits + 1) return false;
    output[0] = '\0';
    const uint32_t modulus = pepper_otp_modulus(digits);
    if(code >= modulus) return false;
    const bool success = snprintf(output, output_size, "%0*lu", digits, (unsigned long)code) ==
                         digits;
    if(!success) output[0] = '\0';
    return success;
}
