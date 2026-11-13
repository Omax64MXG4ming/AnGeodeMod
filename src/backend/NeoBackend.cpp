#include "NeoBackend.hpp"

#include <cctype>
#include <sstream>
#include <iomanip>
#include <chrono>

using namespace geode::prelude;

namespace neo::backend {

    NeoBackend& NeoBackend::get() {
        static NeoBackend instance;
        return instance;
    }

    void NeoBackend::setBaseURL(std::string url) {
        while (!url.empty() && url.back() == '/') {
            url.pop_back();
        }

        m_baseURL = std::move(url);
    }

    std::string NeoBackend::getBaseURL() const {
        return m_baseURL;
    }

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

    void NeoBackend::get(
    const std::string& endpoint,
    Callback callback
) {
    auto request = web::WebRequest();

    request.timeout(asp::Duration(20'000));
    request.userAgent("NeoBackend/1.0 GeometryDash/2.2091");

    async::spawn(
        request.get(buildURL(endpoint)),
        [callback = std::move(callback)](
            web::WebResponse response
        ) mutable {
            NeoResponse result;

            result.httpCode = response.code();
            result.success = response.ok();
            result.body = response.string().unwrapOr("");

            if (!result.success) {
                result.error =
                    "HTTP request failed: " +
                    std::to_string(result.httpCode);
            }

            if (callback) {
                callback(std::move(result));
            }
        }
    );
}

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

        postRaw(endpoint, body, std::move(callback));
    }

    void NeoBackend::postRaw(
        const std::string& endpoint,
        const std::string& body,
        Callback callback
    ) {
        auto request = web::WebRequest();

        request.timeout(std::chrono::seconds(20));
        request.userAgent("NeoBackend/1.0 GeometryDash/2.2091");

        request.header(
            "Content-Type",
            "application/x-www-form-urlencoded"
        );

        request.header(
            "Accept",
            "application/json, text/plain, */*"
        );

        request.bodyString(body);

        async::spawn(
            request.post(buildURL(endpoint)),
            [callback = std::move(callback)](
                web::WebResponse response
            ) mutable {
                NeoResponse result;

                result.httpCode = response.code();
                result.success = response.ok();
                result.body = response.string().unwrapOr("");

                if (!result.success) {
                    result.error =
                        "HTTP request failed: " +
                        std::to_string(result.httpCode);
                }

                if (callback) {
                    callback(std::move(result));
                }
            }
        );
    }

}
