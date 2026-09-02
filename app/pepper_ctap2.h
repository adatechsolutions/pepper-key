#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PEPPER_CTAP2_AAGUID_SIZE 16U

typedef enum {
    PepperCtap2MakeCredential = 0x01,
    PepperCtap2GetAssertion = 0x02,
    PepperCtap2GetInfo = 0x04,
    PepperCtap2ClientPin = 0x06,
    PepperCtap2Reset = 0x07,
    PepperCtap2GetNextAssertion = 0x08,
    PepperCtap2CredentialManagement = 0x0A,
    PepperCtap2Selection = 0x0B,
    PepperCtap2LargeBlobs = 0x0C,
    PepperCtap2Config = 0x0D,
} PepperCtap2Command;

typedef struct {
    bool usb_cbor_transport;
    bool make_credential;
    bool get_assertion;
    bool discoverable_credentials;
    bool client_pin;
    bool credential_management;
    bool credential_protection;
    bool hmac_secret;
    bool large_blobs;
} PepperCtap2Readiness;

extern const uint8_t pepper_ctap2_aaguid[PEPPER_CTAP2_AAGUID_SIZE];

const PepperCtap2Readiness* pepper_ctap2_readiness(void);
bool pepper_ctap2_command_ready(PepperCtap2Command command);

#ifdef __cplusplus
}
#endif
