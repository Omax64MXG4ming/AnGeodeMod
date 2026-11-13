#pragma once

#include "NeoRequest.hpp"

#include <vector>
#include <mutex>
#include <cstdint>
#include <string>

namespace neo::intercept {

class NeoActivityManager {
public:
    static NeoActivityManager& get();

    uint64_t begin(NeoRequest request);

    void complete(
        uint64_t id,
        int responseCode,
        std::string responseBody
    );

    void fail(
        uint64_t id,
        std::string error
    );

    std::vector<NeoRequest> getHistory();

    void clear();

private:
    NeoActivityManager() = default;

    std::mutex m_mutex;
    std::vector<NeoRequest> m_history;

    uint64_t m_nextID = 1;

    static constexpr size_t MAX_HISTORY = 100;
};

}
