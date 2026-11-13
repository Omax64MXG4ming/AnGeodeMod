#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/utils/async.hpp>

#include <string>
#include <map>
#include <functional>

namespace neo::backend {

    struct NeoResponse {
        bool success = false;
        int httpCode = 0;

        std::string body;
        std::string error;
    };

    using Callback = std::function<void(NeoResponse)>;

    class NeoBackend {
    public:
        static NeoBackend& get();

        void setBaseURL(std::string url);
        std::string getBaseURL() const;

        void get(
            const std::string& endpoint,
            Callback callback
        );

        void post(
            const std::string& endpoint,
            const std::map<std::string, std::string>& fields,
            Callback callback
        );

        void postRaw(
            const std::string& endpoint,
            const std::string& body,
            Callback callback
        );

        static std::string encode(
            const std::string& value
        );

    private:
        NeoBackend() = default;

        std::string buildURL(
            const std::string& endpoint
        ) const;

        std::string m_baseURL =
            "https://neops.x10.mx";

    };

}
