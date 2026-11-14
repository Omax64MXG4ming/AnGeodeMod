#pragma once

#include <string>
#include <map>
#include <cstdint>

namespace neo::intercept {

class NeoGDIntercept {
public:
    // Registra el inicio de una solicitud de Geometry Dash.
    static uint64_t requestStarted(
        const std::string& method,
        const std::string& url,
        const std::map<std::string, std::string>& headers = {},
        const std::string& body = ""
    );

    // Registra la respuesta recibida.
    static void requestCompleted(
        uint64_t requestID,
        int responseCode,
        const std::string& responseBody = ""
    );

    // Registra un error de transporte.
    static void requestFailed(
        uint64_t requestID,
        const std::string& error
    );

private:
    static std::map<std::string, std::string> sanitizeHeaders(
        const std::map<std::string, std::string>& headers
    );

    static std::string sanitizeBody(const std::string& body);
};

}
