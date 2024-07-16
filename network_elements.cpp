#include "network_elements.h"

void logMessage(const std::string& from, const std::string& to, const std::string& message) {
    std::cout << from << " to " << to << ": " << message << std::endl;
}

UE::UE(const std::string& ue_id, const std::string& auth_id) : id(ue_id), authId(auth_id) {}

void UE::sendRegistrationRequest() {
    logMessage("UE", "RAN", "Registration Request");
    // Implementation to send registration request
}

void UE::receiveAuthenticationChallenge(const std::string& challenge) {
    logMessage("AMF", "UE", "Authentication Challenge: " + challenge);
    // UE processes the challenge
    sendAuthenticationResponse("Auth Response from UE");
}

void UE::sendAuthenticationResponse(const std::string& response) {
    logMessage("UE", "AMF", response);
    // Implementation to send authentication response
}

void RAN::forwardRegistrationRequest(const std::string& request) {
    logMessage("RAN", "AMF", request);
    // Forward to AMF
}

void RAN::forwardAuthenticationResponse(const std::string& response) {
    logMessage("RAN", "AMF", response);
    // Forward response to AMF
}

void AMF::receiveRegistrationRequest(const std::string& request) {
    logMessage("RAN", "AMF", request);
    // AMF processes the registration request
    requestAuthentication("UE_1");
}

void AMF::requestAuthentication(const std::string& ueId) {
    logMessage("AMF", "AUSF", "Authentication Request for " + ueId);
    // Request authentication from AUSF
}

void AMF::receiveAuthenticationResponse(const std::string& response) {
    logMessage("UE", "AMF", response);
    // Process the authentication response
    verifyAuthentication(response);
}

void AMF::verifyAuthentication(const std::string& response) {
    logMessage("AMF", "RAN/UE", "Authentication Success");
    // Verification logic
}

void AUSF::authenticateRequest(const std::string& ueId) {
    logMessage("AUSF", "AMF", "Authentication Request for " + ueId);
    // Authenticate the request
    sendAuthenticationChallenge("Challenge for UE");
}

void AUSF::sendAuthenticationChallenge(const std::string& challenge) {
    logMessage("AUSF", "AMF", "Authentication Challenge: " + challenge);
    // Send challenge to AMF
}
