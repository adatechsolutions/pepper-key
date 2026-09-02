#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PEPPER_CHALLENGE_RESPONSE_SIZE 32U

bool pepper_challenge_response(
    const uint8_t* namespace_key,
    size_t namespace_key_size,
    const uint8_t* challenge,
    size_t challenge_size,
    uint8_t response[PEPPER_CHALLENGE_RESPONSE_SIZE]);

#ifdef __cplusplus
}
#endif
