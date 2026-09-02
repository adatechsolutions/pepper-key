#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void pepper_secure_zero(void* data, size_t size);
bool pepper_constant_time_equal(const uint8_t* left, const uint8_t* right, size_t size);

#ifdef __cplusplus
}
#endif
