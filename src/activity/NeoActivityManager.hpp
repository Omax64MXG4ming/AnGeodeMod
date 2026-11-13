
#pragma once

#include "NeoRequest.hpp"

#include <vector>
#include <mutex>
#include <cstdint>
#include <string>
#include <functional>
#include <unordered_map>

namespace neo::intercept {

class NeoActivityManager {
public:
    using Listener = std::function<void()>;

    static NeoActivityManager& get();

    // Request management
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

    // History
    std::vector<NeoRequest> getHistory();

    void clear();

    // Live events
    uint64_t addListener(Listener listener);
    void removeListener(uint64_t listenerID);

private:
    NeoActivityManager() = default;

    void notifyListeners();

    std::mutex m_mutex;

    std::vector<NeoRequest> m_history;

    std::unordered_map<uint64_t, Listener> m_listeners;

    uint64_t m_nextID = 1;
    uint64_t m_nextListenerID = 1;

    static constexpr size_t MAX_HISTORY = 100;
};

}
