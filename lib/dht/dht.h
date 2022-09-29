#ifndef DHT11_H
#define DHT11_H

#include <Arduino.h>

struct DHTResult {
    bool error = false;
    uint8_t humidityIntegral = 0;
    uint8_t humidityDecimal = 0;
    int8_t temperatureIntegral = 0;
    uint8_t temperatureDecimal = 0;

    bool operator==(DHTResult const& rhs) const {
        return this->error == rhs.error &&
               this->humidityIntegral == rhs.humidityIntegral &&
               this->temperatureIntegral == rhs.temperatureIntegral &&
               this->humidityDecimal == rhs.humidityDecimal &&
               this->temperatureDecimal == rhs.temperatureDecimal;
    }
};

class DHT {
   public:
    DHTResult read(uint8_t pin);
    DHTResult read11(uint8_t pin);
    DHTResult read22(uint8_t pin);

   private:
    byte* sample(uint8_t pin);
    uint32_t waitForDigitalRead(uint8_t pin, int nextState, uint32_t timeout);
    uint32_t lastRead;
};

#endif
