#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN D2
#define DHTTYPE DHT11

// Wi-Fi settings
const char* ssid = "iPhone";
const char* password = "";

// Raspberry Pi MQTT broker
const char* mqtt_server = "172.20.10.2";
const int mqtt_port = 1883;

// Second ESP32
const char* deviceId = "esp32-temperature-01";
const char* mqtt_topic = "iot/brook/temperature";

DHT dht(DHTPIN, DHTTYPE);
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.println("Connecting to MQTT...");

    if (mqtt.connect(deviceId)) {
      Serial.println("MQTT connected!");
    } else {
      Serial.print("MQTT connection failed: ");
      Serial.println(mqtt.state());
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();
  Serial.println("DHT11 temperature sensor ready");

  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi connected!");

  mqtt.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!mqtt.connected()) {
    connectMQTT();
  }

  mqtt.loop();

  delay(2000);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT11, check wiring");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C   Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // JSON telemetry
  String payload = "{";
  payload += "\"deviceId\":\"esp32-temperature-01\",";
  payload += "\"value\":" + String(temperature, 1) + ",";
  payload += "\"unit\":\"C\"";
  payload += "}";

  if (mqtt.publish(mqtt_topic, payload.c_str())) {
    Serial.println("Sent to MQTT: " + payload);
  } else {
    Serial.println("MQTT publish failed!");
  }
}