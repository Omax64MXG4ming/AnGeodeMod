#include "NeoGDHttpClient.hpp"

#include "NeoGDIntercept.hpp"

using namespace cocos2d::extension;

namespace neo::intercept {

void NeoGDHttpClient::send(CCHttpRequest* request) {
    if (request) {
        const char* method = "UNKNOWN";

        switch (request->getRequestType()) {
            case CCHttpRequest::kHttpGet:
                method = "GET";
                break;

            case CCHttpRequest::kHttpPost:
                method = "POST";
                break;

            case CCHttpRequest::kHttpPut:
                method = "PUT";
                break;

            case CCHttpRequest::kHttpDelete:
                method = "DELETE";
                break;

            default:
                break;
        }

        const char* rawURL = request->getUrl();

        std::string url = rawURL ? rawURL : "";

        // No guardar cuerpos POST sin sanitización.
        NeoGDIntercept::requestStarted(
            method,
            url,
            {},
            ""
        );
    }

    CCHttpClient::send(request);
}

}
