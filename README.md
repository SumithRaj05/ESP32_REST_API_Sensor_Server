
# ESP32 REST API Sensor Server

This project uses an ESP32 to read multiple sensors and provide their data via REST API endpoints within a local network (LAN).

      +3.3V
       │
       │
       ├─────> DHT11 VCC
       │
       ├─────> LDR ──┐
       │             │
       │             R 10kΩ
       │             │
       │            GND
       │
       ├─────> PIR VCC
       │
       └─────> OLED VCC


## Sensors Used
- DHT11 → Temperature & Humidity (GPIO5)  
- LDR → Light sensor (GPIO32)  
- PIR → Motion sensor (GPIO2)  
- OLED Display → I2C (SDA: GPIO21, SCL: GPIO22)  

## Features
- `/` → Check if server is online  
- `/temp` → Get temperature and humidity (JSON)  
- `/photo` → Get light level (JSON)  
- `/motion` → Get motion status (JSON)  

## Usage
1. Connect the sensors to the ESP32.  
2. Upload the code to the ESP32.  
3. Connect ESP32 to your WiFi network.  
4. Access sensor data via the provided endpoints using a browser or API client.

## License
MIT License – free to use and modify. Keep credit to Sumith D.
