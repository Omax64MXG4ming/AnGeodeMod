
#include "NeoSocial.hpp"
#include "NeoEndpoints.hpp"

namespace neo::social {

    void getFriends(
        int accountID,
        backend::Callback callback
    ) {
        backend::NeoBackend::get().post(
            endpoints::FRIENDS_LIST,
            {
                {"account_id", std::to_string(accountID)}
            },
            std::move(callback)
        );
    }

    void sendFriendRequest(
        int accountID,
        int targetAccountID,
        backend::Callback callback
    ) {
        backend::NeoBackend::get().post(
            endpoints::FRIEND_REQUEST,
            {
                {"account_id", std::to_string(accountID)},
                {"target_account_id", std::to_string(targetAccountID)}
            },
            std::move(callback)
        );
    }

    void acceptFriendRequest(
        int requestID,
        backend::Callback callback
    ) {
        backend::NeoBackend::get().post(
            endpoints::FRIEND_ACCEPT,
            {
                {"request_id", std::to_string(requestID)}
            },
            std::move(callback)
        );
    }

    void getMessages(
        int accountID,
        backend::Callback callback
    ) {
        backend::NeoBackend::get().post(
            endpoints::MESSAGES_LIST,
            {
                {"account_id", std::to_string(accountID)}
            },
            std::move(callback)
        );
    }

    void sendMessage(
        int accountID,
        int targetAccountID,
        const std::string& subject,
        const std::string& message,
        backend::Callback callback
    ) {
        backend::NeoBackend::get().post(
            endpoints::MESSAGE_SEND,
            {
                {"account_id", std::to_string(accountID)},
                {"target_account_id", std::to_string(targetAccountID)},
                {"subject", subject},
                {"message", message}
            },
            std::move(callback)
        );
    }

}
