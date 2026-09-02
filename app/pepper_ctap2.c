#include "pepper_ctap2.h"

const uint8_t pepper_ctap2_aaguid[PEPPER_CTAP2_AAGUID_SIZE] = {
    0x34,
    0xE4,
    0xA4,
    0x41,
    0x9A,
    0x60,
    0x43,
    0x61,
    0x83,
    0x0A,
    0xCF,
    0xA2,
    0x0B,
    0xFF,
    0x31,
    0x47,
};

/*
 * Fail closed until every required command, persistence rule, PIN rule, and
 * physical-approval path is implemented and accepted together. Do not set
 * usb_cbor_transport merely because a CBOR parser or GetInfo encoder exists.
 */
static const PepperCtap2Readiness readiness = {
    .usb_cbor_transport = false,
    .make_credential = false,
    .get_assertion = false,
    .discoverable_credentials = false,
    .client_pin = false,
    .credential_management = false,
    .credential_protection = false,
    .hmac_secret = false,
    .large_blobs = false,
};

const PepperCtap2Readiness* pepper_ctap2_readiness(void) {
    return &readiness;
}

bool pepper_ctap2_command_ready(PepperCtap2Command command) {
    if(!readiness.usb_cbor_transport) return false;
    switch(command) {
    case PepperCtap2MakeCredential:
        return readiness.make_credential;
    case PepperCtap2GetAssertion:
    case PepperCtap2GetNextAssertion:
        return readiness.get_assertion;
    case PepperCtap2GetInfo:
        return true;
    case PepperCtap2ClientPin:
        return readiness.client_pin;
    case PepperCtap2CredentialManagement:
        return readiness.credential_management;
    default:
        return false;
    }
}
