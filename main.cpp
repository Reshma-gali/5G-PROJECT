#include "network_elements.h"
#include "testcases.h"
#include <iostream>

int main() {
    int passed = 0;
    int failed = 0;

    // Running test cases
    if (testRegistrationRequestHandling()) passed++; else failed++;
    if (testAuthenticationRequestAndResponse()) passed++; else failed++;
    if (testAuthenticationChallengeReceptionAndResponse()) passed++; else failed++;
    if (testAuthenticationVerification()) passed++; else failed++;
    if (testErrorHandlingInAuthentication()) passed++; else failed++;
    if (testRegistrationAndAuthenticationIntegration()) passed++; else failed++;

    std::cout << "Test Summary: " << passed << " passed, " << failed << " failed.\n";

    return 0;
}
