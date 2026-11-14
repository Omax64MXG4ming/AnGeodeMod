#include "NeoGDHttpClient.hpp"
#include "NeoGDIntercept.hpp"

using namespace cocos2d::extension;

namespace neo::intercept {

class $modify(NeoGDHttpClient, cocos2d::extension::CCHttpClient) {
    void send(cocos2d::extension::CCHttpRequest* request);
};

}
