
#pragma once

#include "NeoBackend.hpp"

namespace neo::social {

    void getFriends(
        int accountID,
        backend::Callback callback
    );

    void sendFriendRequest(
        int accountID,
        int targetAccountID,
        backend::Callback callback
    );

    void acceptFriendRequest(
        int requestID,
        backend::Callback callback
    );

    void getMessages(
        int accountID,
        backend::Callback callback
    );

    void sendMessage(
        int accountID,
        int targetAccountID,
        const std::string& subject,
        const std::string& message,
        backend::Callback callback
    );

}
