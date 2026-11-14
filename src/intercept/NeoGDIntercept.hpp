#pragma once

#include <string>
#include <map>
#include <cstdint>

namespace neo::intercept {

class NeoGDIntercept {
public:
    static uint64_t requestStarted(
        const std::string& method,
        const std::string& url,
        const std::map<std::string, std::string>& headers = {},
        const std::string& body = ""
    );

    static void requestCompleted(
        uint64_t requestID,
        int responseCode,
        const std::string& responseBody = ""
    );

    static void requestFailed(
        uint64_t requestID,
        const std::string& error
    );

private:
    static std::map<std::string, std::string> sanitizeHeaders(
        const std::map<std::string, std::string>& headers
    );

    static std::string sanitizeBody(const std::string& body);
};

}
