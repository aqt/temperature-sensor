#include "dht.h"

// Please note this implementation lacks error handling

uint32_t DHT::waitForDigitalRead(uint8_t pin, int nextState, uint32_t timeout) {
    uint32_t start = micros();

    // TODO: Watchdog on esp8266 trips before timeout
    while (digitalRead(pin) != nextState) {
        if (micros() - start > timeout) {
            return 0;
        }
    }

    return micros() - start;
}

// Requests bytes from sensor
uint8_t* DHT::sample(uint8_t pin) {
    static const uint32_t timeoutMicros = 100e3;

    static uint8_t bytes[5] = {0};

    // Request signal: low >=18ms, high 20-40us
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delay(20);
    digitalWrite(pin, HIGH);
    delayMicroseconds(30);

    // Start of response signal: low 80us, high 80us
    pinMode(pin, INPUT);
    waitForDigitalRead(pin, HIGH, timeoutMicros);
    waitForDigitalRead(pin, LOW, timeoutMicros);

    // Response data: 5 bytes a 8 bits (MSB).
    // Bit timings: [low 54us, high 26us=0 or 70us=1]
    // Ends with low 54us
    // How the bytes are interpreted differs between the sensors
    for (uint8_t byteIndex = 0; byteIndex < 5; byteIndex++) {
        uint8_t byte = 0;

        for (uint8_t bitIndex = 0; bitIndex < 8; bitIndex++) {
            waitForDigitalRead(pin, HIGH, timeoutMicros);

            uint32_t width = waitForDigitalRead(pin, LOW, timeoutMicros);

            // Check bit with tolerance of +-10us (I've observed +6)
            if (width >= 60 && width <= 80) {
                // Bit is 1
                byte |= 1 << (7 - bitIndex);
            } else if (width >= 14 && width <= 34) {
                // Bit is 0. No-op since it's 0 by default
            }
        }

        bytes[byteIndex] = byte;
    }

    delayMicroseconds(54);

    return bytes;
}

// Sample and parse data from DHT11
DHTResult DHT::read11(uint8_t pin) {
    uint8_t* bytes = this->sample(pin);

    DHTResult data;

    uint16_t checksum = bytes[0] + bytes[1] + bytes[2] + bytes[3];

    if (checksum != bytes[4]) {
        data.error = true;
        return data;
    }

    // Interpreting the bytes couldn't be easier, they are simply in order
    data.humidityIntegral = bytes[0];
    data.humidityDecimal = bytes[1];
    data.temperatureIntegral = bytes[2];
    data.temperatureDecimal = bytes[3];

    return data;
}

// Sample and parse data from DHT22
DHTResult DHT::read22(uint8_t pin) {
    uint8_t* bytes = this->sample(pin);

    DHTResult data;

    uint8_t checksum = bytes[0] + bytes[1] + bytes[2] + bytes[3];

    if (checksum != bytes[4]) {
        data.error = true;
        return data;
    }

    // First two bytes is an unsigned 16 bit integer for humidity times 10
    uint16_t humidity = (bytes[0] << 8) | bytes[1];

    // Next two bytes is a signed 16 bit integer for temperature times 10
    // Negative numbers represented by sign and magnitude, NOT two's complement,
    // so special care needs to be taken of the high bit
    uint16_t temperature = ((bytes[2] & 0x7F) << 8) | bytes[3];

    if (bytes[2] & 0x80) {
        temperature *= -1;
    }

    data.humidityIntegral = humidity / 10;
    data.humidityDecimal = humidity % 10;
    data.temperatureIntegral = temperature / 10;
    data.temperatureDecimal = abs(temperature) % 10;

    return data;
}
