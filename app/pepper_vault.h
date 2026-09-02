#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PEPPER_VAULT_MAGIC    0x50564C54UL
#define PEPPER_VAULT_VERSION  1U
#define PEPPER_VAULT_KEY_SIZE 32U

typedef enum {
    PepperVaultFido2 = 1,
    PepperVaultOath = 2,
    PepperVaultChallenge = 3,
    PepperVaultSmartCard = 4,
    PepperVaultRecovery = 5,
} PepperVaultNamespace;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint8_t namespace_id;
    uint8_t flags;
    uint32_t generation;
    uint32_t payload_size;
    uint8_t salt[16];
} PepperVaultHeader;

_Static_assert(sizeof(PepperVaultHeader) == 32, "Pepper vault header must remain stable");

bool pepper_vault_namespace_valid(PepperVaultNamespace namespace_id);

bool pepper_vault_header_valid(
    const PepperVaultHeader* header,
    PepperVaultNamespace expected_namespace,
    size_t maximum_payload_size);

bool pepper_vault_derive_namespace_key(
    const uint8_t* root_key,
    size_t root_key_size,
    PepperVaultNamespace namespace_id,
    uint8_t output[PEPPER_VAULT_KEY_SIZE]);

#ifdef __cplusplus
}
#endif
