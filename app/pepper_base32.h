#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PepperBase32Ok,
    PepperBase32InvalidCharacter,
    PepperBase32InvalidPadding,
    PepperBase32NonCanonicalBits,
    PepperBase32OutputTooSmall,
} PepperBase32Result;

PepperBase32Result pepper_base32_decode(
    const char* encoded,
    uint8_t* output,
    size_t output_capacity,
    size_t* output_size);

#ifdef __cplusplus
}
#endif
