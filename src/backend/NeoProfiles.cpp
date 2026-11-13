
#include "NeoProfiles.hpp"
#include "NeoEndpoints.hpp"

namespace neo::profiles {

    void getUserInfo(
        int accountID,
        backend::Callback callback
    ) {
        backend::NeoBackend::get().post(
            endpoints::PROFILE_INFO,
            {
                {"account_id", std::to_string(accountID)}
            },
            std::move(callback)
        );
    }

    void searchUser(
        const std::string& username,
        backend::Callback callback
    ) {
        backend::NeoBackend::get().post(
            endpoints::PROFILE_SEARCH,
            {
                {"username", username}
            },
            std::move(callback)
        );
    }

}
