#include "pepper_challenge.h"
#include "pepper_crypto.h"
#include "pepper_secure.h"

#define PEPPER_CHALLENGE_MAX_SIZE 1024U

bool pepper_challenge_response(
    const uint8_t* namespace_key,
    size_t namespace_key_size,
    const uint8_t* challenge,
    size_t challenge_size,
    uint8_t response[PEPPER_CHALLENGE_RESPONSE_SIZE]) {
    if(response) pepper_secure_zero(response, PEPPER_CHALLENGE_RESPONSE_SIZE);
    if(!namespace_key || !namespace_key_size || !challenge || !challenge_size ||
       challenge_size > PEPPER_CHALLENGE_MAX_SIZE || !response) {
        return false;
    }
    return pepper_crypto_hmac(
        PepperHashSha256,
        namespace_key,
        namespace_key_size,
        challenge,
        challenge_size,
        response,
        PEPPER_CHALLENGE_RESPONSE_SIZE);
}
