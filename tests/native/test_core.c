#include "pepper_base32.h"
#include "pepper_challenge.h"
#include "pepper_ctap2.h"
#include "pepper_otp.h"
#include "pepper_secure.h"
#include "pepper_vault.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void check_base32_vector(const char* encoded, const char* expected) {
    uint8_t output[32] = {0};
    size_t output_size = 0;
    assert(pepper_base32_decode(encoded, output, sizeof(output), &output_size) == PepperBase32Ok);
    assert(output_size == strlen(expected));
    assert(memcmp(output, expected, output_size) == 0);
}

static void test_base32(void) {
    check_base32_vector("MY======", "f");
    check_base32_vector("MZXQ====", "fo");
    check_base32_vector("MZXW6===", "foo");
    check_base32_vector("MZXW6YQ=", "foob");
    check_base32_vector("MZXW6YTB", "fooba");
    check_base32_vector("MZXW6YTBOI======", "foobar");

    uint8_t output[32] = {0};
    size_t output_size = 0;
    static const uint8_t expected[] = {'H', 'e', 'l', 'l', 'o', '!', 0xDE, 0xAD, 0xBE, 0xEF};

    assert(
        pepper_base32_decode("JBSW Y3DP-EHPK3PXP", output, sizeof(output), &output_size) ==
        PepperBase32Ok);
    assert(output_size == sizeof(expected));
    assert(pepper_constant_time_equal(output, expected, sizeof(expected)));

    assert(
        pepper_base32_decode("MZXW6===", output, sizeof(output), &output_size) == PepperBase32Ok);
    assert(output_size == 3);
    assert(memcmp(output, "foo", 3) == 0);
    assert(
        pepper_base32_decode("MZXW6======", output, sizeof(output), &output_size) ==
        PepperBase32InvalidPadding);
    assert(
        pepper_base32_decode("MZXW6=AA", output, sizeof(output), &output_size) ==
        PepperBase32InvalidPadding);
    assert(
        pepper_base32_decode("A", output, sizeof(output), &output_size) ==
        PepperBase32NonCanonicalBits);
    assert(
        pepper_base32_decode("MZXW6YQ==", output, sizeof(output), &output_size) ==
        PepperBase32InvalidPadding);
}

static void test_hotp(void) {
    static const uint8_t secret[] = "12345678901234567890";
    static const uint32_t expected[] = {
        755224, 287082, 359152, 969429, 338314, 254676, 287922, 162583, 399871, 520489};

    for(uint64_t counter = 0; counter < 10; counter++) {
        uint32_t code = 0;
        assert(pepper_hotp_generate(
            PepperHashSha1, secret, sizeof(secret) - 1, counter, 6, &code));
        assert(code == expected[counter]);
    }
    uint32_t code = 0;
    assert(!pepper_hotp_generate(PepperHashSha1, secret, sizeof(secret) - 1, 0, 5, &code));
    assert(!pepper_hotp_generate(PepperHashSha1, secret, 9, 0, 6, &code));
}

static void test_totp(void) {
    static const uint8_t sha1_secret[] = "12345678901234567890";
    static const uint8_t sha256_secret[] = "12345678901234567890123456789012";
    static const uint8_t sha512_secret[] =
        "1234567890123456789012345678901234567890123456789012345678901234";
    uint32_t code = 0;
    uint32_t remaining = 0;

    assert(pepper_totp_generate(
        PepperHashSha1,
        sha1_secret,
        sizeof(sha1_secret) - 1,
        59,
        0,
        30,
        8,
        &code,
        &remaining));
    assert(code == 94287082 && remaining == 1);
    assert(pepper_totp_generate(
        PepperHashSha256,
        sha256_secret,
        sizeof(sha256_secret) - 1,
        59,
        0,
        30,
        8,
        &code,
        &remaining));
    assert(code == 46119246);
    assert(pepper_totp_generate(
        PepperHashSha512,
        sha512_secret,
        sizeof(sha512_secret) - 1,
        59,
        0,
        30,
        8,
        &code,
        &remaining));
    assert(code == 90693936);

    char formatted[9];
    assert(pepper_otp_format(42, 6, formatted, sizeof(formatted)));
    assert(strcmp(formatted, "000042") == 0);
    assert(!pepper_otp_format(1000000, 6, formatted, sizeof(formatted)));
    assert(formatted[0] == '\0');
    assert(!pepper_totp_generate(
        PepperHashSha1, sha1_secret, sizeof(sha1_secret) - 1, 59, 0, 5, 8, &code, &remaining));
}

static void test_isolation_and_challenge(void) {
    uint8_t root[32];
    memset(root, 0xA5, sizeof(root));
    uint8_t fido_key[32] = {0};
    uint8_t oath_key[32] = {0};
    uint8_t challenge_key[32] = {0};
    uint8_t response_a[32] = {0};
    uint8_t response_b[32] = {0};

    assert(pepper_vault_derive_namespace_key(root, sizeof(root), PepperVaultFido2, fido_key));
    assert(pepper_vault_derive_namespace_key(root, sizeof(root), PepperVaultOath, oath_key));
    assert(pepper_vault_derive_namespace_key(
        root, sizeof(root), PepperVaultChallenge, challenge_key));
    assert(!pepper_constant_time_equal(fido_key, oath_key, sizeof(fido_key)));
    assert(!pepper_constant_time_equal(oath_key, challenge_key, sizeof(oath_key)));

    static const uint8_t challenge_a[] = "pepper-key-test-a";
    static const uint8_t challenge_b[] = "pepper-key-test-b";
    assert(pepper_challenge_response(
        challenge_key,
        sizeof(challenge_key),
        challenge_a,
        sizeof(challenge_a) - 1,
        response_a));
    assert(pepper_challenge_response(
        challenge_key,
        sizeof(challenge_key),
        challenge_b,
        sizeof(challenge_b) - 1,
        response_b));
    assert(!pepper_constant_time_equal(response_a, response_b, sizeof(response_a)));

    memset(response_a, 0xA5, sizeof(response_a));
    assert(!pepper_challenge_response(
        challenge_key, sizeof(challenge_key), challenge_a, 0, response_a));
    uint8_t zero[32] = {0};
    assert(pepper_constant_time_equal(response_a, zero, sizeof(response_a)));

    uint8_t invalid_derived[32];
    memset(invalid_derived, 0xA5, sizeof(invalid_derived));
    assert(!pepper_vault_derive_namespace_key(root, sizeof(root), 0, invalid_derived));
    assert(pepper_constant_time_equal(invalid_derived, zero, sizeof(invalid_derived)));

    PepperVaultHeader header = {
        .magic = PEPPER_VAULT_MAGIC,
        .version = PEPPER_VAULT_VERSION,
        .namespace_id = PepperVaultOath,
        .flags = 0,
        .generation = 1,
        .payload_size = 128,
    };
    assert(pepper_vault_header_valid(&header, PepperVaultOath, 512));
    assert(!pepper_vault_header_valid(&header, PepperVaultFido2, 512));
}

static void test_ctap2_fail_closed_gate(void) {
    const PepperCtap2Readiness* readiness = pepper_ctap2_readiness();
    assert(readiness && !readiness->usb_cbor_transport);
    assert(!readiness->make_credential);
    assert(!readiness->get_assertion);
    assert(!readiness->discoverable_credentials);
    assert(!readiness->client_pin);
    assert(!readiness->credential_management);
    assert(!readiness->credential_protection);
    assert(!readiness->hmac_secret);
    assert(!readiness->large_blobs);
    assert(!pepper_ctap2_command_ready(PepperCtap2GetInfo));
    uint8_t aggregate = 0;
    for(size_t index = 0; index < PEPPER_CTAP2_AAGUID_SIZE; index++) {
        aggregate |= pepper_ctap2_aaguid[index];
    }
    assert(aggregate != 0);
}

int main(void) {
    test_base32();
    test_hotp();
    test_totp();
    test_isolation_and_challenge();
    test_ctap2_fail_closed_gate();
    puts("PepperKey portable core tests passed.");
    return 0;
}
