
#include <WiFi.h>
#include <PubSubClient.h>

// iPhone hotspot credentials
const char* ssid = "iPhone";
const char* password = "";

// Raspberry Pi MQTT broker
const char* mqtt_server = "172.20.10.2";
const int mqtt_port = 1883;
const char* mqtt_topic = "iot/brook/sound";

// Sound sensor
const int soundPin = A0;

WiFiClient espClient;
PubSubClient client(espClient);

void connectWiFi() {
  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");

    String clientId = "NanoESP32-" + WiFi.macAddress();
    clientId.replace(":", "");

    if (client.connect(clientId.c_str())) {
      Serial.println("connected!");
    } else {
      Serial.print("Failed, state: ");
      Serial.println(client.state());
      delay(3000);
    }
  }
}

int readSoundLevel() {
  int minValue = 4095;
  int maxValue = 0;

  unsigned long startTime = millis();

  while (millis() - startTime < 50) {
    int value = analogRead(soundPin);

    if (value < minValue) minValue = value;
    if (value > maxValue) maxValue = value;
  }

  return maxValue - minValue;
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  connectWiFi();

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!client.connected()) {
    reconnectMQTT();
  }

  client.loop();

  int soundLevel = readSoundLevel();

  char message[16];
  snprintf(message, sizeof(message), "%d", soundLevel);

  if (client.publish(mqtt_topic, message)) {
    Serial.print("Sound level sent: ");
    Serial.println(message);
  } else {
    Serial.println("MQTT publish failed");
  }

  delay(1000);
}
