#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <Servo.h>

const char* ssid = "ibrar";
const char* password = "ibrarahmad";

const char* websockets_server_host = "192.168.4.1";
const uint16_t websockets_server_port = 8888;

const int servoPin = 16; // Pin for the servo

Servo servo;
bool rotateServo = false;       // Flag to indicate servo rotation
unsigned long rotationStartTime; // Time when rotation started

using namespace websockets;
WebsocketsClient client;

void setup() {
  Serial.begin(115200);
  servo.attach(servoPin);

  Serial.setDebugOutput(true);
  Serial.println();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  while (!client.connect(websockets_server_host, websockets_server_port, "/")) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Socket Connected!");

  // Set up the callback function for receiving messages
  client.onMessage([](WebsocketsMessage msg) {
    Serial.println("Got Message: " + msg.data());

    // Check if the message is "Button pressed!"
    if (msg.data() == "Button pressed!") {
      Serial.println("Rotating servo...");
      rotateServo = true;
      rotationStartTime = millis(); // Record the start time of rotation
    }

    // Send echo message
    client.send("Echo: " + msg.data());
  });
}

void loop() {
  // Check if the servo rotation flag is set
  if (rotateServo) {
    // Check if 10 seconds have passed since rotation started
    if (millis() - rotationStartTime >= 10000) {
      Serial.println("Returning servo to initial position...");
      servo.write(0); // Set the servo to the initial position
      rotateServo = false; // Reset the flag
    } else {
      // Rotate the servo to the desired position
      servo.write(90);
    }
  }

  client.poll();

  delay(100);
}
