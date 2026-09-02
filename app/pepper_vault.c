#include "pepper_vault.h"
#include "pepper_crypto.h"
#include "pepper_secure.h"

#include <string.h>

bool pepper_vault_namespace_valid(PepperVaultNamespace namespace_id) {
    return namespace_id >= PepperVaultFido2 && namespace_id <= PepperVaultRecovery;
}

bool pepper_vault_header_valid(
    const PepperVaultHeader* header,
    PepperVaultNamespace expected_namespace,
    size_t maximum_payload_size) {
    if(!header || !pepper_vault_namespace_valid(expected_namespace)) return false;
    if(header->magic != PEPPER_VAULT_MAGIC || header->version != PEPPER_VAULT_VERSION)
        return false;
    if(header->namespace_id != (uint8_t)expected_namespace) return false;
    if(header->flags != 0 || header->generation == 0) return false;
    if(header->payload_size == 0 || header->payload_size > maximum_payload_size) return false;
    return true;
}

bool pepper_vault_derive_namespace_key(
    const uint8_t* root_key,
    size_t root_key_size,
    PepperVaultNamespace namespace_id,
    uint8_t output[PEPPER_VAULT_KEY_SIZE]) {
    if(output) pepper_secure_zero(output, PEPPER_VAULT_KEY_SIZE);
    if(!root_key || !root_key_size || !output || !pepper_vault_namespace_valid(namespace_id)) {
        return false;
    }

    static const uint8_t prefix[] = "PepperKey/v1/vault/";
    static const char* names[] = {NULL, "fido2", "oath", "challenge", "smart-card", "recovery"};
    uint8_t context[48] = {0};
    const size_t prefix_size = sizeof(prefix) - 1;
    const char* name = names[namespace_id];
    const size_t name_size = strlen(name);
    if(prefix_size + name_size + 1 > sizeof(context)) return false;

    memcpy(context, prefix, prefix_size);
    memcpy(context + prefix_size, name, name_size);
    context[prefix_size + name_size] = (uint8_t)namespace_id;
    return pepper_crypto_hmac(
        PepperHashSha256,
        root_key,
        root_key_size,
        context,
        prefix_size + name_size + 1,
        output,
        PEPPER_VAULT_KEY_SIZE);
}
