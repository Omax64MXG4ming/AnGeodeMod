
#pragma once

#include "NeoBackend.hpp"

namespace neo::accounts {

    void login(
        const std::string& username,
        const std::string& password,
        backend::Callback callback
    );

    void registerAccount(
        const std::string& username,
        const std::string& password,
        const std::string& email,
        backend::Callback callback
    );

    void logout(
        const std::string& sessionToken,
        backend::Callback callback
    );

}
