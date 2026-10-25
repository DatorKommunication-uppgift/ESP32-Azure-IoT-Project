import os
import json
import paho.mqtt.client as mqtt
from azure.iot.device import IoTHubDeviceClient, Message

AZURE_CONNECTION_STRING = os.environ["AZURE_DEVICE_CONNECTION_STRING"]

MQTT_BROKER = "127.0.0.1"
MQTT_PORT = 1883

MQTT_TOPICS = [
    ("iot/brook/sound", 0),
    ("iot/brook/temperature", 0)
]

azure_client = IoTHubDeviceClient.create_from_connection_string(
    AZURE_CONNECTION_STRING
)

print("Connecting to Azure IoT Hub...")
azure_client.connect()
print("Connected to Azure IoT Hub!")


def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to local MQTT broker!")
        client.subscribe(MQTT_TOPICS)
        print("Subscribed to both sensors")
    else:
        print("MQTT connection failed:", rc)


def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode().strip()

        if msg.topic == "iot/brook/sound":
            # Existing sound sensor publishes raw ADC values
            telemetry = {
                "deviceId": "esp32-sound-01",
                "value": int(payload),
                "unit": "ADC"
            }

        elif msg.topic == "iot/brook/temperature":
            # Temperature sensor publishes JSON
            telemetry = json.loads(payload)

            if not all(k in telemetry for k in ("deviceId", "value", "unit")):
                raise ValueError("Missing telemetry fields")

        else:
            return

        message = Message(json.dumps(telemetry))
        message.content_type = "application/json"
        message.content_encoding = "utf-8"

        azure_client.send_message(message)

        print("Sent to Azure:", telemetry)

    except Exception as error:
        print("Error forwarding measurement:", error)


mqtt_client = mqtt.Client()
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

try:
    mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
    print("Waiting for sensor measurements...")
    mqtt_client.loop_forever()

except KeyboardInterrupt:
    print("Stopping gateway...")

finally:
    mqtt_client.disconnect()
    azure_client.shutdown()
