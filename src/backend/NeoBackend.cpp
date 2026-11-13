
#include "NeoBackend.hpp"
#include "../activity/NeoActivityManager.hpp"

#include <cctype>
#include <sstream>
#include <iomanip>
#include <utility>
#include <algorithm>

using namespace geode::prelude;

namespace neo::backend {

    // ==========================================
    // SECURITY: BODY REDACTION
    // ==========================================

    namespace {

        std::string decodeFormKey(const std::string& value) {
            std::string result;

            for (size_t i = 0; i < value.size(); ++i) {
                if (value[i] == '%' && i + 2 < value.size()) {
                    try {
                        int decoded = std::stoi(
                            value.substr(i + 1, 2),
                            nullptr,
                            16
                        );

                        result += static_cast<char>(decoded);
                        i += 2;
                    }
                    catch (...) {
                        result += value[i];
                    }
                }
                else if (value[i] == '+') {
                    result += ' ';
                }
                else {
                    result += value[i];
                }
            }

            return result;
        }

        bool isSensitiveKey(std::string key) {
            std::transform(
                key.begin(),
                key.end(),
                key.begin(),
                [](unsigned char c) {
                    return static_cast<char>(std::tolower(c));
                }
            );

            return key.find("password") != std::string::npos
                || key.find("passwd") != std::string::npos
                || key.find("token") != std::string::npos
                || key.find("secret") != std::string::npos
                || key.find("cookie") != std::string::npos
                || key.find("authorization") != std::string::npos
                || key.find("session") != std::string::npos;
        }

        std::string redactFormBody(const std::string& body) {
            std::string output;
            size_t position = 0;

            while (position < body.size()) {
                size_t end = body.find('&', position);

                if (end == std::string::npos) {
                    end = body.size();
                }

                std::string pair = body.substr(
                    position,
                    end - position
                );

                size_t equal = pair.find('=');

                if (equal != std::string::npos) {
                    std::string key = pair.substr(0, equal);

                    if (isSensitiveKey(decodeFormKey(key))) {
                        pair = key + "=[REDACTED]";
                    }
                }

                if (!output.empty()) {
                    output += '&';
                }

                output += pair;
                position = end + 1;
            }

            return output;
        }

        void recordCompletion(
            uint64_t requestID,
            int code,
            const std::string& body
        ) {
            neo::intercept::NeoActivityManager::get().complete(
                requestID,
                code,
                body
            );
        }
    }

    // ==========================================
    // SINGLETON
    // ==========================================

    NeoBackend& NeoBackend::get() {
        static NeoBackend instance;
        return instance;
    }

    // ==========================================
    // CONFIGURATION
    // ==========================================

    void NeoBackend::setBaseURL(std::string url) {
        while (!url.empty() && url.back() == '/') {
            url.pop_back();
        }

        m_baseURL = std::move(url);
    }

    std::string NeoBackend::getBaseURL() const {
        return m_baseURL;
    }

    // ==========================================
    // URL BUILDER
    // ==========================================

    std::string NeoBackend::buildURL(
        const std::string& endpoint
    ) const {
        if (endpoint.empty()) {
            return m_baseURL;
        }

        if (endpoint.find("://") != std::string::npos) {
            return endpoint;
        }

        if (endpoint.front() == '/') {
            return m_baseURL + endpoint;
        }

        return m_baseURL + "/" + endpoint;
    }

    // ==========================================
    // URL ENCODING
    // ==========================================

    std::string NeoBackend::encode(
        const std::string& value
    ) {
        std::ostringstream output;

        output << std::uppercase << std::hex;

        for (unsigned char c : value) {
            if (
                std::isalnum(c) ||
                c == '-' ||
                c == '_' ||
                c == '.' ||
                c == '~'
            ) {
                output << static_cast<char>(c);
            }
            else {
                output << '%'
                       << std::setw(2)
                       << std::setfill('0')
                       << static_cast<int>(c);
            }
        }

        return output.str();
    }

    // ==========================================
    // GET REQUEST
    // ==========================================

    void NeoBackend::get(
        const std::string& endpoint,
        Callback callback
    ) {
        const std::string url = buildURL(endpoint);

        auto request = web::WebRequest();

        request.timeout(asp::Duration(20, 0));

        request.userAgent(
            "NeoBackend/1.0 GeometryDash/2.2091"
        );

        request.header(
            "Accept",
            "application/json, text/plain, */*"
        );

        // Register request in activity manager.

        neo::intercept::NeoRequest record;

        record.source =
            neo::intercept::RequestSource::NeoBackend;

        record.method = "GET";
        record.url = url;

        record.requestHeaders = {
            {"User-Agent", "NeoBackend/1.0 GeometryDash/2.2091"},
            {"Accept", "application/json, text/plain, */*"}
        };

        const uint64_t requestID =
            neo::intercept::NeoActivityManager::get().begin(
                std::move(record)
            );

        async::spawn(
            request.get(url),

            [
                callback = std::move(callback),
                requestID
            ](web::WebResponse response) mutable {

                NeoResponse result;

                result.httpCode = response.code();
                result.success = response.ok();

                result.body =
                    response.string().unwrapOr("");

                if (!result.success) {
                    result.error =
                        "HTTP request failed: " +
                        std::to_string(result.httpCode);
                }

                recordCompletion(
                    requestID,
                    result.httpCode,
                    result.body
                );

                if (callback) {
                    callback(std::move(result));
                }
            }
        );
    }

    // ==========================================
    // POST REQUEST
    // ==========================================

    void NeoBackend::post(
        const std::string& endpoint,
        const std::map<std::string, std::string>& fields,
        Callback callback
    ) {
        std::string body;

        for (const auto& [key, value] : fields) {
            if (!body.empty()) {
                body += "&";
            }

            body += encode(key);
            body += "=";
            body += encode(value);
        }

        postRaw(
            endpoint,
            body,
            std::move(callback)
        );
    }

    // ==========================================
    // RAW POST REQUEST
    // ==========================================

    void NeoBackend::postRaw(
        const std::string& endpoint,
        const std::string& body,
        Callback callback
    ) {
        const std::string url = buildURL(endpoint);

        auto request = web::WebRequest();

        request.timeout(asp::Duration(20, 0));

        request.userAgent(
            "NeoBackend/1.0 GeometryDash/2.2091"
        );

        request.header(
            "Content-Type",
            "application/x-www-form-urlencoded"
        );

        request.header(
            "Accept",
            "application/json, text/plain, */*"
        );

        request.bodyString(body);

        // Register request in activity manager.

        neo::intercept::NeoRequest record;

        record.source =
            neo::intercept::RequestSource::NeoBackend;

        record.method = "POST";
        record.url = url;

        record.requestHeaders = {
            {"User-Agent", "NeoBackend/1.0 GeometryDash/2.2091"},
            {"Content-Type", "application/x-www-form-urlencoded"},
            {"Accept", "application/json, text/plain, */*"}
        };

        // Store a redacted copy, never the original credentials.

        record.requestBody = redactFormBody(body);

        const uint64_t requestID =
            neo::intercept::NeoActivityManager::get().begin(
                std::move(record)
            );

        async::spawn(
            request.post(url),

            [
                callback = std::move(callback),
                requestID
            ](web::WebResponse response) mutable {

                NeoResponse result;

                result.httpCode = response.code();
                result.success = response.ok();

                result.body =
                    response.string().unwrapOr("");

                if (!result.success) {
                    result.error =
                        "HTTP request failed: " +
                        std::to_string(result.httpCode);
                }

                recordCompletion(
                    requestID,
                    result.httpCode,
                    result.body
                );

                if (callback) {
                    callback(std::move(result));
                }
            }
        );
    }

}
