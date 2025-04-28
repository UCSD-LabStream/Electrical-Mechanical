#include <WiFi.h>
#include <AsyncMqttClient.h>
#include <ArduinoJson.h>
#include <Stepper.h>
#include "esp_wpa2.h"

// WiFi credentials
const char* ssid = "UCSD-PROTECTED";
//const char* ssid = "nj";

// If using WPA enterprise
const char* username = "yij036";
const char* password = "6265222296aA";


// MQTT broker config
const char* mqtt_server = "labstream.ucsd.edu";
const int   mqtt_port   = 1883;  // Using raw TCP port
const char* mqtt_path   = "/mqtt";

// Topics: image_motor_H, image_motor_V, filter_motor_H, filter_motor_V
/*
const char* mqtt_topic_1  = "stepper/labstream/motors/control1";
const char* mqtt_topic_2  = "stepper/labstream/motors/control2";
const char* mqtt_topic_3  = "stepper/labstream/motors/control3";
const char* mqtt_topic_4  = "stepper/labstream/motors/control4";
*/

const char* mqtt_topic_1  = "image_motor_H";
const char* mqtt_topic_2  = "stepper/labstream/motors/image_motor_V/control";
const char* mqtt_topic_3  = "stepper/labstream/motors/filter_motor_H/control";
const char* mqtt_topic_4  = "stepper/labstream/motors/filter_motor_V/control";

// Stepper motors config
#define STEPS_PER_REV 2048

// Motor 1 Pins
#define M1_IN1 12
#define M1_IN2 13
#define M1_IN3 14
#define M1_IN4 15

// Motor 2 Pins
#define M2_IN1 16
#define M2_IN2 17
#define M2_IN3 18
#define M2_IN4 19

// Motor 3 Pins
#define M3_IN1 21
#define M3_IN2 22
#define M3_IN3 23
#define M3_IN4 25

// Motor 4 Pins
#define M4_IN1 26
#define M4_IN2 27
#define M4_IN3 32
#define M4_IN4 33

// Global variables
Stepper stepper1(STEPS_PER_REV, M1_IN1, M1_IN3, M1_IN2, M1_IN4);
Stepper stepper2(STEPS_PER_REV, M2_IN1, M2_IN3, M2_IN2, M2_IN4);
Stepper stepper3(STEPS_PER_REV, M3_IN1, M3_IN3, M3_IN2, M3_IN4);
Stepper stepper4(STEPS_PER_REV, M4_IN1, M4_IN3, M4_IN2, M4_IN4);

int selectedMotor = 1;  // Default to motor 1
String currentCommand = "";
WiFiClient net;
AsyncMqttClient mqttClient;

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)username, strlen(username));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)username, strlen(username));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)password, strlen(password));
  esp_wifi_sta_wpa2_ent_enable();

  WiFi.begin(ssid);


  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi");
  }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  
  mqttClient.subscribe(mqtt_topic_1, 0);
  mqttClient.subscribe(mqtt_topic_2, 0);
  mqttClient.subscribe(mqtt_topic_3, 0);
  mqttClient.subscribe(mqtt_topic_4, 0);

  Serial.println("Subscribed to all motor control topics");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT");
  Serial.print("Reason: ");
  Serial.println(static_cast<int>(reason));
  mqttClient.connect();
  Serial.println("Reconnecting to MQTT...");
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties props,
  size_t len, size_t index, size_t total) {

  String msg;
  for (size_t i = 0; i < len; i++) msg += (char)payload[i];
    Serial.printf("Message arrived [%s]: %s\n", topic, msg.c_str());

  // Set motor based on topic
  String topicStr(topic);
  if (topicStr == mqtt_topic_1) selectedMotor = 1;
  else if (topicStr == mqtt_topic_2) selectedMotor = 2;
  else if (topicStr == mqtt_topic_3) selectedMotor = 3;
  else if (topicStr == mqtt_topic_4) selectedMotor = 4;

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, msg);
  if (!error) {
    if (doc.containsKey("command")) {
      currentCommand = doc["command"].as<String>();
    }
  } else {
    // fallback for simple messages
    if (msg == "cw" || msg == "ccw" || msg == "stop") {
      currentCommand = msg;
    }
  }
}

void stopMotor(int motorNum) {
  switch(motorNum) {
    case 1:
      digitalWrite(M1_IN1, LOW); digitalWrite(M1_IN2, LOW);
      digitalWrite(M1_IN3, LOW); digitalWrite(M1_IN4, LOW);
      break;
    case 2:
      digitalWrite(M2_IN1, LOW); digitalWrite(M2_IN2, LOW);
      digitalWrite(M2_IN3, LOW); digitalWrite(M2_IN4, LOW);
      break;
    case 3:
      digitalWrite(M3_IN1, LOW); digitalWrite(M3_IN2, LOW);
      digitalWrite(M3_IN3, LOW); digitalWrite(M3_IN4, LOW);
      break;
    case 4:
      digitalWrite(M4_IN1, LOW); digitalWrite(M4_IN2, LOW);
      digitalWrite(M4_IN3, LOW); digitalWrite(M4_IN4, LOW);
      break;
  }
}

void testBrokerConnection() {
  WiFiClient testClient;
  Serial.print("Testing connection to ");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.print(mqtt_port);
  Serial.println("...");
  
  if (testClient.connect(mqtt_server, mqtt_port)) {
    Serial.println("Successfully connected to broker port");
    testClient.stop();
  } else {
    Serial.println("Failed to connect to broker port");
    Serial.println("Possible causes:");
    Serial.println("- Wrong port number");
    Serial.println("- Firewall blocking connection");
    Serial.println("- Broker not running");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Motor 1
  pinMode(M1_IN1, OUTPUT); pinMode(M1_IN2, OUTPUT);
  pinMode(M1_IN3, OUTPUT); pinMode(M1_IN4, OUTPUT);
  stepper1.setSpeed(10);

  // Motor 2
  pinMode(M2_IN1, OUTPUT); pinMode(M2_IN2, OUTPUT);
  pinMode(M2_IN3, OUTPUT); pinMode(M2_IN4, OUTPUT);
  stepper2.setSpeed(10);

  // Motor 3
  pinMode(M3_IN1, OUTPUT); pinMode(M3_IN2, OUTPUT);
  pinMode(M3_IN3, OUTPUT); pinMode(M3_IN4, OUTPUT);
  stepper3.setSpeed(10);

  // Motor 4
  pinMode(M4_IN1, OUTPUT); pinMode(M4_IN2, OUTPUT);
  pinMode(M4_IN3, OUTPUT); pinMode(M4_IN4, OUTPUT);
  stepper4.setSpeed(10);
  
  // Connect WiFi and MQTT
  setup_wifi();

  testBrokerConnection();
  
  // MQTT callbacks
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onMessage(onMqttMessage);
  
  // Set server and connect
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.connect();
  
}

void loop() {
  
  Stepper* motor;
  if (selectedMotor == 1) motor = &stepper1;
  else if (selectedMotor == 2) motor = &stepper2;
  else if (selectedMotor == 3) motor = &stepper3;
  else if (selectedMotor == 4) motor = &stepper4;
  else motor = &stepper1;  // default safety


  if(currentCommand == "cw") {
    motor->step(1);
  } 
  else if(currentCommand == "ccw") {
    motor->step(-1);
  }
  else if(currentCommand == "stop") {
    stopMotor(selectedMotor);
    currentCommand = "";
  }
  
  delay(3);
}