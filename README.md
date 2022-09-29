# What

Temperature sensor (DHT11 / DHT22) connected to WiFi (via esp8266) for reporting temperature and relative humidity values to a webhook (in HomeAssistant in my case).

Note that this is a learning project, do not treat my code as gospel. I didn't know anything about C/C++, embedded programming, or electronics when starting this project, and I still don't.

# Components
DHT11 / DHT22
ESP8266 module (Tested with ESP-01, ESP-01S, daily driving Wemos D1 Mini)

# Build
1. Copy `src/config.h.example` to `src/config.h` and set your values in it
2. Install VS Code
3. Install the Code extension PlatformIO IDE
4. Press the ant head on the left then Build under the wanted platform
