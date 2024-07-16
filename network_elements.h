#ifndef NETWORK_ELEMENTS_H
#define NETWORK_ELEMENTS_H

#include <iostream>
#include <string>

// Basic message logging function
void logMessage(const std::string& from, const std::string& to, const std::string& message);

// User Equipment
class UE {
public:
    std::string id;
    std::string authId;
    UE(const std::string& ue_id, const std::string& auth_id);
    void sendRegistrationRequest();
    void receiveAuthenticationChallenge(const std::string& challenge);
    void sendAuthenticationResponse(const std::string& response);
};

// Radio Access Network
class RAN {
public:
    void forwardRegistrationRequest(const std::string& request);
    void forwardAuthenticationResponse(const std::string& response);
};

// Access and Mobility Management Function
class AMF {
public:
    void receiveRegistrationRequest(const std::string& request);
    void requestAuthentication(const std::string& ueId);
    void receiveAuthenticationResponse(const std::string& response);
    void verifyAuthentication(const std::string& response);
};

// Authentication Server Function
class AUSF {
public:
    void authenticateRequest(const std::string& ueId);
    void sendAuthenticationChallenge(const std::string& challenge);
};

#endif // NETWORK_ELEMENTS_H
