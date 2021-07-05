# Ambrogino

A quick NodeMCU ESP2866 project for 24V irrigation.

## NodeMCU pin mapping

This is how I wired mine, your wiring might be different but take this as a guideline:

|N1 |NodeMCU GPIO|NodeMCU physical|
|---|---|---|
|N1 |GPIO16 |D0 |
|N2 |GPIO5 |D1  |
|N3 |GPIO4 |D2  |
|N4 |GPIO2 |D3  |
|unused |unused |D4 |
|3V3 |DPIO |3V3 |
|GND |DPIO |GND |

## Build and flash

This project is provided with platformio project files, you can find out more at: https://platformio.org/

You should be putting your settings in the src/settings.CHANGEME file:

```
// Update these with values suitable for your network.

const char ssid[] = "YOUR_SSID"; // your network SSID (name)
const char pass[] = "YOUR_WIFI_PASSWORD";  // your network password

const char mqtt_server[] = "YOUR_MQTT_SERVER_IP";
```

After editing save it as src/settings.h

This code is based on some concepts and actual code found at: https://thenewstack.io/off-the-shelf-hacker-adding-mqtt-and-cron-to-the-lawn-sprinkler-project/