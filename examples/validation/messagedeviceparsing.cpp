#include <cstring> // For strstr, strlen, etc.
#include <iostream>

bool isMessageMeantForDevice(const char* buffer, const char* device) {
    const char* MESSAGE_HEADERS_nxtdev = ":nxtdev-"; // Ensure this is defined correctly
    const char* nxtdevLocation = strstr(buffer, MESSAGE_HEADERS_nxtdev);

    if (nxtdevLocation != nullptr) {
        // Move past the nxtdev header to the start of the device name
        const char* startOfInterest = nxtdevLocation + strlen(MESSAGE_HEADERS_nxtdev);
        // Find the end of the device name, assuming it ends at the start of the next section, which we'll say is '-mesg:'
        // Adjust this based on your actual message format
        const char* endOfInterest = strstr(startOfInterest, "-mesg:");

        if (endOfInterest != nullptr) {
            // Compare the device name directly using strncmp to ensure we don't read past the end of the device name
            size_t deviceNameLength = endOfInterest - startOfInterest;

            // Check if the lengths match and if the device names are equal up to the found length
            if (strlen(device) == deviceNameLength && strncmp(startOfInterest, device, deviceNameLength) == 0) {
                return true; // Device matches
            }
        }
    }

    return false; // Device does not match or 'nxtdev' not found
}

int main() {
    // Example usage
    const char* buffer = "fart:nxtdev-client-mesg:heartbeatbend";
    const char* device = "client";

    if (isMessageMeantForDevice(buffer, device)) {
        std::cout << "Message is meant for device: " << device << std::endl;
    } else {
        std::cout << "Message is not meant for device: " << device << std::endl;
    }

    return 0;
}
