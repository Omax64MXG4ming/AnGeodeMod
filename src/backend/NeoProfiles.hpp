
#pragma once

#include "NeoBackend.hpp"

namespace neo::profiles {

    void getUserInfo(
        int accountID,
        backend::Callback callback
    );

    void searchUser(
        const std::string& username,
        backend::Callback callback
    );

}
