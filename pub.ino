#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const int trig_pin = 12;  // transmitter
const int echo_pin = 14;  // receiver
const char *ssid = "microtest";
const char *password = "froggy12";
const char *mqtt_broker = "broker.emqx.io";
const char *topic_ultrasonic = "esp8266/ultrasonic-topic";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;


WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 2000;


void setup() {
  Serial.begin(9600);
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);


  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi network");

  client.setServer(mqtt_broker, mqtt_port);


  while (!client.connected()) {
    String client_id = "ultrasonic-client1-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  // Публикация
  client.subscribe(topic_ultrasonic);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastPublishTime >= publishInterval) {
    float distance = readDistance();
    // Отправка данных на брокер MQTT
    client.publish(topic_ultrasonic, String(distance).c_str());

    Serial.print("Published distance: ");
    Serial.println(distance);

    lastPublishTime = currentMillis;
  }
}

float readDistance() {
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  unsigned long duration = pulseIn(echo_pin, HIGH);
  float distance_cm = duration * 0.0343 / 2.0;

  Serial.print("Distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

  return distance_cm;
}
