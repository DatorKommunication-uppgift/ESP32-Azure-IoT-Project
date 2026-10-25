# ESP32 MQTT to Azure IoT Hub

## Project Overview

This project demonstrates an IoT system where two ESP32 microcontrollers collect sensor measurements and transmit them over Wi-Fi using MQTT.

A Raspberry Pi runs a local Mosquitto MQTT broker and a Python gateway that forwards sensor telemetry to Microsoft Azure IoT Hub.

Two consumer applications allow users to monitor measurements locally and through Azure.

## System Architecture

```text
ESP32 1 (Sound Sensor) ──────┐
                            │
                            ▼
ESP32 2 (DHT11 Temperature) ─┤
                            │
                            ▼
                 Raspberry Pi
                 Mosquitto MQTT
                            │
                            ▼
                 Python Azure Gateway
                            │
                            ▼
                    Azure IoT Hub
                            │
                            ▼
                    Azure Consumer
```

The local MQTT consumer subscribes directly to the Raspberry Pi's Mosquitto broker.

## Hardware

- 2 × Arduino Nano ESP32
- 1 × Sound sensor
- 1 × DHT11 temperature and humidity sensor
- 1 × Raspberry Pi
- Breadboard and jumper wires
- USB power supplies
- Wi-Fi network

## Technologies

- Arduino C++
- Python
- MQTT
- Mosquitto
- Raspberry Pi OS
- Azure IoT Hub
- Azure CLI
- PubSubClient
- DHT sensor library
- Paho MQTT

## Project Structure

```text
ESP32-Azure-IoT-Project/
├── sound/
│   └── [sound sensor Arduino sketch]
├── temperature-sensor/
│   └── [temperature Arduino sketch]
├── azure_gateway.py
├── mqtt_consumer.py
├── azure_consumer.py
└── README.md
```

Replace the bracketed sketch names with the actual filenames in the repository.

## Sensor Configuration

### ESP32 1 — Sound Sensor

- Device ID: `esp32-sound-01`
- MQTT topic: `iot/brook/sound`
- Measurement: Raw sound sensor amplitude
- Unit: ADC (not calibrated decibels)

The current firmware publishes numeric MQTT payloads. The gateway converts these into JSON before forwarding them to Azure.

### ESP32 2 — Temperature Sensor

- Device ID: `esp32-temperature-01`
- MQTT topic: `iot/brook/temperature`
- Sensor: DHT11
- Measurement: Temperature in Celsius

Example MQTT message:

```json
{
  "deviceId": "esp32-temperature-01",
  "value": 24.8,
  "unit": "C"
}
```

The DHT11 also measures humidity, which is displayed in the Arduino Serial Monitor.

## Setup and Installation

### 1. Configure the ESP32 Devices

Open each Arduino sketch in Arduino IDE.

Install the required libraries:

- PubSubClient
- DHT sensor library
- Adafruit Unified Sensor (if required)

Select **Arduino Nano ESP32**, configure the Wi-Fi credentials and MQTT broker IP address, and upload the sketches to their respective boards.

Both ESP32 devices must connect to the same network as the Raspberry Pi MQTT broker.

### 2. Start the MQTT Broker

On the Raspberry Pi, install Mosquitto and its client tools if they are not already installed:

```bash
sudo apt update
sudo apt install mosquitto mosquitto-clients
```

Start Mosquitto:

```bash
sudo systemctl enable --now mosquitto
```

Configure the broker to accept connections from the ESP32 devices on the local network. Use appropriate authentication and network restrictions for the deployment.

### 3. Install Python Dependencies

On the Raspberry Pi:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install azure-iot-device==2.14.0 paho-mqtt==1.6.1
```

### 4. Start the Azure Gateway

Register a gateway device in Azure IoT Hub and obtain its device connection string.

Configure the connection string securely in the Raspberry Pi terminal:

```bash
read -rsp "Azure connection string: " AZURE_DEVICE_CONNECTION_STRING
echo
export AZURE_DEVICE_CONNECTION_STRING
```

Then run:

```bash
python azure_gateway.py
```

The gateway subscribes to:

- `iot/brook/sound`
- `iot/brook/temperature`

It forwards both sensors' measurements to Azure IoT Hub through the registered gateway device, `raspberrypi-gateway`.

### 5. Start the Local MQTT Consumer

Install the compatible MQTT Python library:

```bash
pip install paho-mqtt==1.6.1
```

When running the consumer on the Raspberry Pi:

```bash
python mqtt_consumer.py
```

When running it on another computer, configure the broker address first.

For Windows PowerShell:

```powershell
$env:MQTT_BROKER = "YOUR_RASPBERRY_PI_IP"
python mqtt_consumer.py
```

The consumer displays measurements from both sensors.

### 6. Start the Azure Consumer

Install Azure CLI and the Azure IoT extension:

```bash
az extension add --name azure-iot
```

Sign in:

```bash
az login --use-device-code
```

Start the consumer:

```bash
python azure_consumer.py
```

Alternatively, monitor Azure telemetry directly:

```bash
az iot hub monitor-events --hub-name brook-nackademin-iothub --device-id raspberrypi-gateway
```

The Python consumer wraps this Azure CLI command. Its Azure CLI executable path may need adjustment depending on the computer.

## Example Azure Telemetry

Sound sensor:

```json
{
  "deviceId": "esp32-sound-01",
  "value": 27,
  "unit": "ADC"
}
```

Temperature sensor:

```json
{
  "deviceId": "esp32-temperature-01",
  "value": 24.8,
  "unit": "C"
}
```

## Testing and Verification

The system was tested with both ESP32 devices operating simultaneously.

Verified functionality:

- Both ESP32 devices publish measurements to the Raspberry Pi's MQTT broker.
- The local MQTT subscriber receives data from both sensors.
- The Python gateway forwards both sensors' measurements to Azure IoT Hub.
- Azure CLI receives telemetry containing both device IDs.
- The Azure consumer Python script has been tested receiving sound telemetry.

## Security

Do not commit real Wi-Fi passwords, Azure connection strings, access keys, or other secrets to this public repository.

Use environment variables or local configuration files excluded through `.gitignore`. If credentials were accidentally committed, rotate them before publishing.

## Author

IoT coursework project — Software Development in Embedded Systems and IoT.
