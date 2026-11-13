#include "NeoActivityManager.hpp"

#include <algorithm>
#include <chrono>
#include <utility>

namespace neo::intercept {

NeoActivityManager& NeoActivityManager::get() {
    static NeoActivityManager instance;
    return instance;
}

uint64_t NeoActivityManager::begin(NeoRequest request) {
    std::lock_guard<std::mutex> lock(m_mutex);

    request.id = m_nextID++;
    request.state = RequestState::Pending;

    request.startedAt =
        std::chrono::steady_clock::now();

    request.durationMs = 0.0;

    const uint64_t id = request.id;

    m_history.push_back(std::move(request));

    while (m_history.size() > MAX_HISTORY) {
        m_history.erase(m_history.begin());
    }

    return id;
}

void NeoActivityManager::complete(
    uint64_t id,
    int responseCode,
    std::string responseBody
) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = std::find_if(
        m_history.begin(),
        m_history.end(),
        [id](const NeoRequest& request) {
            return request.id == id;
        }
    );

    if (it == m_history.end()) {
        return;
    }

    it->responseCode = responseCode;
    it->responseBody = std::move(responseBody);
    it->state = RequestState::Completed;

    const auto elapsed =
        std::chrono::steady_clock::now() - it->startedAt;

    it->durationMs =
        std::chrono::duration<double, std::milli>(
            elapsed
        ).count();
}

void NeoActivityManager::fail(
    uint64_t id,
    std::string error
) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = std::find_if(
        m_history.begin(),
        m_history.end(),
        [id](const NeoRequest& request) {
            return request.id == id;
        }
    );

    if (it == m_history.end()) {
        return;
    }

    it->error = std::move(error);
    it->state = RequestState::Failed;

    const auto elapsed =
        std::chrono::steady_clock::now() - it->startedAt;

    it->durationMs =
        std::chrono::duration<double, std::milli>(
            elapsed
        ).count();
}

std::vector<NeoRequest> NeoActivityManager::getHistory() {
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_history;
}

void NeoActivityManager::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_history.clear();
}

}
