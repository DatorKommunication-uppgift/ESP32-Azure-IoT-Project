import os
import json
import paho.mqtt.client as mqtt

BROKER = os.getenv("MQTT_BROKER", "127.0.0.1")
PORT = 1883

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker!")
        client.subscribe("iot/brook/#")
    else:
        print("Connection failed:", rc)

def on_message(client, userdata, msg):
    payload = msg.payload.decode().strip()

    if msg.topic == "iot/brook/sound":
        print(f"Sound: {payload} ADC")

    elif msg.topic == "iot/brook/temperature":
        try:
            data = json.loads(payload)
            print(f"Temperature: {data['value']} °C")
        except (ValueError, KeyError, TypeError):
            print("Invalid temperature message:", payload)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(BROKER, PORT, 60)

print("Waiting for sensor measurements...")
client.loop_forever()