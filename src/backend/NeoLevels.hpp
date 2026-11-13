
#pragma once

#include "NeoBackend.hpp"

namespace neo::levels {

    void search(
        const std::string& query,
        int page,
        backend::Callback callback
    );

    void download(
        int levelID,
        backend::Callback callback
    );

    void upload(
        const std::string& levelData,
        backend::Callback callback
    );

}
