
#include "NeoActivityManager.hpp"

#include <algorithm>
#include <chrono>
#include <utility>

namespace neo::intercept {

NeoActivityManager& NeoActivityManager::get() {
    static NeoActivityManager instance;
    return instance;
}

// ==========================================
// NOTIFY LISTENERS
// ==========================================

void NeoActivityManager::notifyListeners() {
    std::vector<Listener> listeners;

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        for (const auto& [id, listener] : m_listeners) {
            if (listener) {
                listeners.push_back(listener);
            }
        }
    }

    // Never execute external callbacks while holding m_mutex.

    for (const auto& listener : listeners) {
        try {
            listener();
        }
        catch (...) {
            // One listener must not interrupt the others.
        }
    }
}

// ==========================================
// LISTENER MANAGEMENT
// ==========================================

uint64_t NeoActivityManager::addListener(Listener listener) {
    if (!listener) {
        return 0;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    const uint64_t id = m_nextListenerID++;

    m_listeners.emplace(id, std::move(listener));

    return id;
}

void NeoActivityManager::removeListener(uint64_t listenerID) {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_listeners.erase(listenerID);
}

// ==========================================
// BEGIN REQUEST
// ==========================================

uint64_t NeoActivityManager::begin(NeoRequest request) {
    uint64_t id;

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        request.id = m_nextID++;
        request.state = RequestState::Pending;

        request.startedAt =
            std::chrono::steady_clock::now();

        request.durationMs = 0.0;

        id = request.id;

        m_history.push_back(std::move(request));

        while (m_history.size() > MAX_HISTORY) {
            m_history.erase(m_history.begin());
        }
    }

    notifyListeners();

    return id;
}

// ==========================================
// COMPLETE REQUEST
// ==========================================

void NeoActivityManager::complete(
    uint64_t id,
    int responseCode,
    std::string responseBody
) {
    bool updated = false;

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = std::find_if(
            m_history.begin(),
            m_history.end(),
            [id](const NeoRequest& request) {
                return request.id == id;
            }
        );

        if (it != m_history.end()) {
            it->responseCode = responseCode;
            it->responseBody = std::move(responseBody);

            it->state = RequestState::Completed;

            const auto elapsed =
                std::chrono::steady_clock::now() - it->startedAt;

            it->durationMs =
                std::chrono::duration<double, std::milli>(
                    elapsed
                ).count();

            updated = true;
        }
    }

    if (updated) {
        notifyListeners();
    }
}

// ==========================================
// FAILED REQUEST
// ==========================================

void NeoActivityManager::fail(
    uint64_t id,
    std::string error
) {
    bool updated = false;

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = std::find_if(
            m_history.begin(),
            m_history.end(),
            [id](const NeoRequest& request) {
                return request.id == id;
            }
        );

        if (it != m_history.end()) {
            it->error = std::move(error);
            it->state = RequestState::Failed;

            const auto elapsed =
                std::chrono::steady_clock::now() - it->startedAt;

            it->durationMs =
                std::chrono::duration<double, std::milli>(
                    elapsed
                ).count();

            updated = true;
        }
    }

    if (updated) {
        notifyListeners();
    }
}

// ==========================================
// GET HISTORY
// ==========================================

std::vector<NeoRequest> NeoActivityManager::getHistory() {
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_history;
}

// ==========================================
// CLEAR HISTORY
// ==========================================

void NeoActivityManager::clear() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_history.clear();
    }

    notifyListeners();
}

}
