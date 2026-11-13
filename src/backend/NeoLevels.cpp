
#include "NeoLevels.hpp"
#include "NeoEndpoints.hpp"

namespace neo::levels {

    void search(
        const std::string& query,
        int page,
        backend::Callback callback
    ) {
        backend::NeoBackend::get().post(
            endpoints::LEVEL_SEARCH,
            {
                {"query", query},
                {"page", std::to_string(page)}
            },
            std::move(callback)
        );
    }

    void download(
        int levelID,
        backend::Callback callback
    ) {
        backend::NeoBackend::get().post(
            endpoints::LEVEL_DOWNLOAD,
            {
                {"level_id", std::to_string(levelID)}
            },
            std::move(callback)
        );
    }

    void upload(
        const std::string& levelData,
        backend::Callback callback
    ) {
        backend::NeoBackend::get().post(
            endpoints::LEVEL_UPLOAD,
            {
                {"level_data", levelData}
            },
            std::move(callback)
        );
    }

}
