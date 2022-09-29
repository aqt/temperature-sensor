#include "metrics-client.h"

MetricsClient::MetricsClient(const char *host, uint16_t port, const char *path) {
    this->host = host;
    this->path = path;
    this->port = port;
}

bool MetricsClient::publish(const char *payload) {
    uint32_t contentLength = strlen(payload);

    if (!client.connect(host, port)) {
        return false;
    }

    // // <!-- Debug
    // Serial.printf("POST %s HTTP/1.1\r\n", path);
    // Serial.printf("Host: %s:%hu\r\n", host, port);
    // Serial.print("Content-Type: application/json\r\n");
    // Serial.printf("Content-Length: %u", contentLength);
    // Serial.print("\r\n\r\n");
    // Serial.print(payload);
    // // Debug -->

    client.printf("POST %s HTTP/1.1\r\n", path);
    // client.printf("Authorization: Basic %s\r\n", auth);
    client.printf("Host: %s:%hu\r\n", host, port);
    client.print("Content-Type: application/json\r\n");
    client.printf("Content-Length: %u", contentLength);
    client.print("\r\n\r\n");

    client.print(payload);

    // Wait for, but disregard, response
    while (client.available()) {
        client.read();
    }

    client.stop();

    return true;
}
