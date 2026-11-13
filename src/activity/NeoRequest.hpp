#pragma once

#include <string>
#include <map>
#include <cstdint>
#include <chrono>

namespace neo::intercept {

enum class RequestSource {
    NeoBackend,
    GeometryDash,
    Geode
};

enum class RequestState {
    Pending,
    Completed,
    Failed
};

struct NeoRequest {
    uint64_t id = 0;

    RequestSource source = RequestSource::NeoBackend;
    RequestState state = RequestState::Pending;

    std::string method;
    std::string url;

    std::map<std::string, std::string> requestHeaders;
    std::string requestBody;

    int responseCode = 0;

    std::map<std::string, std::string> responseHeaders;
    std::string responseBody;

    std::string error;

    double durationMs = 0.0;

    std::chrono::steady_clock::time_point startedAt{};
};

}
