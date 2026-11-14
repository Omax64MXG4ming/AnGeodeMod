#include "NeoGDIntercept.hpp"
#include "../activity/NeoActivityManager.hpp"

#include <algorithm>
#include <cctype>
#include <utility>

namespace neo::intercept {

namespace {

bool isSensitiveKey(std::string key) {
    std::transform(
        key.begin(),
        key.end(),
        key.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );

    return key.find("password") != std::string::npos ||
           key.find("pass") != std::string::npos ||
           key.find("token") != std::string::npos ||
           key.find("gjp") != std::string::npos ||
           key.find("secret") != std::string::npos ||
           key.find("cookie") != std::string::npos ||
           key.find("authorization") != std::string::npos ||
           key.find("session") != std::string::npos;
}

std::string sanitizeFormBody(const std::string& body) {
    std::string result;
    size_t start = 0;

    while (start < body.size()) {
        size_t end = body.find('&', start);

        if (end == std::string::npos)
            end = body.size();

        std::string part = body.substr(start, end - start);
        size_t separator = part.find('=');

        if (separator != std::string::npos) {
            std::string key = part.substr(0, separator);
            std::string value = part.substr(separator + 1);

            if (isSensitiveKey(key))
                value = "[REDACTED]";

            result += key + "=" + value;
        } else {
            result += part;
        }

        if (end < body.size())
            result += "&";

        start = end + 1;
    }

    return result;
}

} // namespace

std::map<std::string, std::string>
NeoGDIntercept::sanitizeHeaders(
    const std::map<std::string, std::string>& headers
) {
    auto safeHeaders = headers;

    for (auto& [key, value] : safeHeaders) {
        if (isSensitiveKey(key))
            value = "[REDACTED]";
    }

    return safeHeaders;
}

std::string NeoGDIntercept::sanitizeBody(
    const std::string& body
) {
    if (body.empty())
        return {};

    size_t first = body.find_first_not_of(" \t\r\n");

    if (first != std::string::npos && body[first] == '{')
        return "[JSON body omitted for credential safety]";

    return sanitizeFormBody(body);
}

uint64_t NeoGDIntercept::requestStarted(
    const std::string& method,
    const std::string& url,
    const std::map<std::string, std::string>& headers,
    const std::string& body
) {
    NeoRequest request;

    request.source = RequestSource::GeometryDash;
    request.method = method;
    request.url = url;
    request.requestHeaders = sanitizeHeaders(headers);
    request.requestBody = sanitizeBody(body);

    return NeoActivityManager::get().begin(std::move(request));
}

void NeoGDIntercept::requestCompleted(
    uint64_t requestID,
    int responseCode,
    const std::string& responseBody
) {
    NeoActivityManager::get().complete(
        requestID,
        responseCode,
        sanitizeBody(responseBody)
    );
}

void NeoGDIntercept::requestFailed(
    uint64_t requestID,
    const std::string& error
) {
    NeoActivityManager::get().fail(requestID, error);
}

} // namespace neo::intercept
