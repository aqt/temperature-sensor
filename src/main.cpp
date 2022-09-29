#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "config.h"
#include "dht.cpp"
#include "metrics-client.h"

DHT dht;
DHTResult prevResult;

MetricsClient client = MetricsClient(SERVER_HOST, SERVER_PORT, METRICS_ENDPOINT);

bool use11 = 0;

void sample() {
    DHTResult result;

    if (use11) {
        result = dht.read11(PIN_DHT_DATA);
    } else {
        result = dht.read22(PIN_DHT_DATA);
    }

    // WiFi.begin() crashes when pin mode of GPIO 2 is INPUT...? Can't repro in
    // the Arduino IDE so I was unable to decode the exception.
    // Now decoding is set up in pio but I haven't taken the time to revisit
    pinMode(PIN_DHT_DATA, OUTPUT);

    if (prevResult == result) {
        Serial.print("Sample dupe");

        if (result.error) {
            Serial.println(" (error)");
        } else {
            Serial.println();
        }

        return;
    }

    if (result.error) {
        Serial.println("Sample invalid");
        prevResult = result;
        return;
    }

    Serial.printf("[%lu] New metrics:   ", millis());
    Serial.print("T:");
    Serial.print(result.temperatureIntegral);
    Serial.print(".");
    Serial.print(result.temperatureDecimal);
    Serial.print("c   H:");
    Serial.print(result.humidityIntegral);
    Serial.print(".");
    Serial.print(result.humidityDecimal);
    Serial.println("%");

    return;

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    uint16_t iterations = 45e3 / 300;  // = maxMicros / delay
    uint16_t i = 0;
    while (WiFi.status() != WL_CONNECTED) {
        if (i++ >= iterations) {
            return;
        }

        delay(300);
    }

    // 27 == '{"t":-000.000,"h":000.000}'.length + 1
    int bufferLength = 27;
    char json[bufferLength];
    snprintf(json, bufferLength, "{\"t\":%d.%d,\"h\":%d.%d}",
             result.temperatureIntegral, result.temperatureDecimal,
             result.humidityIntegral, result.humidityDecimal);

    bool success = client.publish(json);

    WiFi.disconnect();

    if (success) {
        prevResult = result;
    }
}

void setup() {
    Serial.begin(9600);

    delay(10e3);

    // Let sensor have time to initialize, not sure if necessary
    delay(2e3);

    DHTResult result;

    // Throw away first reading (inaccurate)
    if (use11) {
        result = dht.read11(PIN_DHT_DATA);
    } else {
        result = dht.read22(PIN_DHT_DATA);
    }

    pinMode(PIN_DHT_DATA, OUTPUT);

    // Wait between reads
    delay(2e3);
}

void loop() {
    sample();
    delay(60e3 * 15);
}
