
#pragma once

namespace neo::endpoints {

    // ACCOUNTS
    inline constexpr char ACCOUNT_LOGIN[] =
        "/api/v1/accounts/login";

    inline constexpr char ACCOUNT_REGISTER[] =
        "/api/v1/accounts/register";

    inline constexpr char ACCOUNT_LOGOUT[] =
        "/api/v1/accounts/logout";

    // PROFILES
    inline constexpr char PROFILE_INFO[] =
        "/api/v1/profiles/info";

    inline constexpr char PROFILE_SEARCH[] =
        "/api/v1/profiles/search";

    // LEVELS
    inline constexpr char LEVEL_SEARCH[] =
        "/api/v1/levels/search";

    inline constexpr char LEVEL_DOWNLOAD[] =
        "/api/v1/levels/download";

    inline constexpr char LEVEL_UPLOAD[] =
        "/api/v1/levels/upload";

    // SOCIAL
    inline constexpr char FRIENDS_LIST[] =
        "/api/v1/social/friends";

    inline constexpr char FRIEND_REQUEST[] =
        "/api/v1/social/friend-request";

    inline constexpr char FRIEND_ACCEPT[] =
        "/api/v1/social/friend-accept";

    inline constexpr char MESSAGES_LIST[] =
        "/api/v1/social/messages";

    inline constexpr char MESSAGE_SEND[] =
        "/api/v1/social/message-send";

}
