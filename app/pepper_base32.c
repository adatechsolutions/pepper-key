#include "pepper_base32.h"
#include "pepper_secure.h"

static int8_t pepper_base32_value(char character) {
    if(character >= 'A' && character <= 'Z') return (int8_t)(character - 'A');
    if(character >= 'a' && character <= 'z') return (int8_t)(character - 'a');
    if(character >= '2' && character <= '7') return (int8_t)(character - '2' + 26);
    return -1;
}

PepperBase32Result pepper_base32_decode(
    const char* encoded,
    uint8_t* output,
    size_t output_capacity,
    size_t* output_size) {
    if(output_size) *output_size = 0;
    if(!encoded || !output || !output_size) return PepperBase32InvalidCharacter;

    uint32_t accumulator = 0;
    uint8_t bits = 0;
    size_t written = 0;
    bool padding_started = false;
    size_t symbols = 0;
    size_t padding = 0;

    for(const char* cursor = encoded; *cursor; cursor++) {
        const char character = *cursor;
        if(character == ' ' || character == '-') continue;
        if(character == '=') {
            padding_started = true;
            padding++;
            continue;
        }
        if(padding_started) {
            pepper_secure_zero(output, written);
            return PepperBase32InvalidPadding;
        }

        const int8_t value = pepper_base32_value(character);
        if(value < 0) {
            pepper_secure_zero(output, written);
            return PepperBase32InvalidCharacter;
        }
        symbols++;
        accumulator = (accumulator << 5) | (uint8_t)value;
        bits += 5;
        while(bits >= 8) {
            bits -= 8;
            if(written >= output_capacity) {
                pepper_secure_zero(output, written);
                return PepperBase32OutputTooSmall;
            }
            output[written++] = (uint8_t)(accumulator >> bits);
            if(bits)
                accumulator &= (1U << bits) - 1U;
            else
                accumulator = 0;
        }
    }

    if(!symbols) return PepperBase32InvalidCharacter;
    const size_t remainder = symbols % 8;
    const bool valid_remainder = remainder == 0 || remainder == 2 || remainder == 4 ||
                                 remainder == 5 || remainder == 7;
    if(!valid_remainder) {
        pepper_secure_zero(output, written);
        return PepperBase32NonCanonicalBits;
    }
    if(bits && accumulator != 0) {
        pepper_secure_zero(output, written);
        return PepperBase32NonCanonicalBits;
    }

    if(padding) {
        static const uint8_t expected_padding[8] = {0, 0, 6, 0, 4, 3, 0, 1};
        if(padding != expected_padding[remainder]) {
            pepper_secure_zero(output, written);
            return PepperBase32InvalidPadding;
        }
        if(((symbols + padding) % 8) != 0) {
            pepper_secure_zero(output, written);
            return PepperBase32InvalidPadding;
        }
    }

    *output_size = written;
    return PepperBase32Ok;
}
