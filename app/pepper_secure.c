#include "pepper_secure.h"

void pepper_secure_zero(void* data, size_t size) {
    volatile uint8_t* cursor = data;
    while(size--) {
        *cursor++ = 0;
    }
}

bool pepper_constant_time_equal(const uint8_t* left, const uint8_t* right, size_t size) {
    uint8_t difference = 0;
    for(size_t index = 0; index < size; index++) {
        difference |= left[index] ^ right[index];
    }
    return difference == 0;
}
