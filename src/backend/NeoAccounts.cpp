
#include "NeoAccounts.hpp"
#include "NeoEndpoints.hpp"

namespace neo::accounts {

    void login(
        const std::string& username,
        const std::string& password,
        backend::Callback callback
    ) {
        backend::NeoBackend::get().post(
            endpoints::ACCOUNT_LOGIN,
            {
                {"username", username},
                {"password", password}
            },
            std::move(callback)
        );
    }

    void registerAccount(
        const std::string& username,
        const std::string& password,
        const std::string& email,
        backend::Callback callback
    ) {
        backend::NeoBackend::get().post(
            endpoints::ACCOUNT_REGISTER,
            {
                {"username", username},
                {"password", password},
                {"email", email}
            },
            std::move(callback)
        );
    }

    void logout(
        const std::string& sessionToken,
        backend::Callback callback
    ) {
        backend::NeoBackend::get().post(
            endpoints::ACCOUNT_LOGOUT,
            {
                {"session_token", sessionToken}
            },
            std::move(callback)
        );
    }

}
