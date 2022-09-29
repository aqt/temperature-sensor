#ifndef REPORTING_H
#define REPORTING_H

#include <Arduino.h>
// #include <WiFiClientSecure.h>
#include <WiFiClient.h>

class MetricsClient {
   public:
    MetricsClient(const char *host, const uint16_t port, const char *path);
    bool publish(const char *payload);

   private:
    WiFiClient client;
    const char *host;
    uint16_t port;
    const char *path;
};

#endif
