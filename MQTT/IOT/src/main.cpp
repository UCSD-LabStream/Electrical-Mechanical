#include <WiFi.h>
#include <PubSubClient.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "esp_wpa2.h"
#include <Stepper.h>

// WiFi credentials
const char* ssid = "nj";
const char* password = "6265222296aA";

// MQTT broker config
const char* mqtt_server = "labstream.ucsd.edu";
const int mqtt_port = 9001;
const char* mqtt_path = "/mqtt";
const char* mqtt_topic = "stepper/labstream/motors/control";

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

// Global variables
WiFiClient wifiClient;
WebSocketsClient webSocketClient;
PubSubClient mqttClient(wifiClient);  // Use WiFiClient for MQTT connection

// Stepper motors initialization
Stepper stepper1(STEPS_PER_REV, M1_IN1, M1_IN3, M1_IN2, M1_IN4);
Stepper stepper2(STEPS_PER_REV, M2_IN1, M2_IN3, M2_IN2, M2_IN4);

int selectedMotor = 1;  // Default to motor 1
String currentCommand = "";

// Forward declaration of the WebSocket event function
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length);

// Function to connect to WiFi
void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

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

// MQTT callback functions
void onMqttConnect() {
  Serial.println("Connected to MQTT broker");
  mqttClient.subscribe(mqtt_topic);
  Serial.println("Subscribed to topic");
}

void onMqttDisconnect() {
  Serial.println("Disconnected from MQTT");
}

void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.printf("Message arrived [%s]: %s\n", topic, msg.c_str());

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, msg);
  if (!error) {
    if (doc.containsKey("motor")) {
      selectedMotor = doc["motor"];
    }
    if (doc.containsKey("command")) {
      currentCommand = doc["command"].as<String>();
    }
  } else {
    if (msg == "cw" || msg == "ccw" || msg == "stop") {
      currentCommand = msg;
    }
  }
}

// Setup WebSocket for MQTT
void setupWebSocket() {
  webSocketClient.begin(mqtt_server, mqtt_port, mqtt_path);  // Use WebSocket for MQTT
  webSocketClient.onEvent(webSocketEvent);
}

// WebSocket event callback
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket Disconnected");
      Serial.print("Reason: ");
      Serial.println((char*)payload);  // Logs the reason for the disconnect
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket Connected");
      break;
    case WStype_TEXT:
      Serial.println("WebSocket Text Message");
      // Handle incoming text data from WebSocket
      break;
    case WStype_PONG:
      Serial.println("WebSocket Pong");
      break;
    case WStype_ERROR:
      Serial.println("WebSocket Error");
      break;
  }
}


// Connect to MQTT broker via WebSocket
void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32_Client")) {
      Serial.println("connected");
      mqttClient.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize motors
  pinMode(M1_IN1, OUTPUT);
  pinMode(M1_IN2, OUTPUT);
  pinMode(M1_IN3, OUTPUT);
  pinMode(M1_IN4, OUTPUT);
  stepper1.setSpeed(10);

  pinMode(M2_IN1, OUTPUT);
  pinMode(M2_IN2, OUTPUT);
  pinMode(M2_IN3, OUTPUT);
  pinMode(M2_IN4, OUTPUT);
  stepper2.setSpeed(10);

  // Connect to WiFi
  setup_wifi();

  // Setup WebSocket and MQTT
  setupWebSocket();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(onMqttMessage);
}

void loop() {
  // MQTT loop for message processing
  mqttClient.loop();
  webSocketClient.loop();  // Ensure WebSocket connection is active

  Stepper* motor = (selectedMotor == 2) ? &stepper2 : &stepper1;

  if (currentCommand == "cw") {
    motor->step(1);
  } else if (currentCommand == "ccw") {
    motor->step(-1);
  } else if (currentCommand == "stop") {
    digitalWrite(M1_IN1, LOW); digitalWrite(M1_IN2, LOW);
    digitalWrite(M1_IN3, LOW); digitalWrite(M1_IN4, LOW);
    digitalWrite(M2_IN1, LOW); digitalWrite(M2_IN2, LOW);
    digitalWrite(M2_IN3, LOW); digitalWrite(M2_IN4, LOW);
    currentCommand = "";
  }

  delay(3);  // Delay for smoother motor control
}
