#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCHttpClient.hpp>

namespace neo::intercept {

class $modify(NeoGDHttpClient, cocos2d::extension::CCHttpClient) {
    void send(cocos2d::extension::CCHttpRequest* request);
};

}
