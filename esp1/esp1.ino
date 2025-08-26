#include <Keypad.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <WebServer.h>

// Wifi credentials
#define SSID "beast"
#define PASSWORD "12345abcde"
#define SERVO_PIN 23
#define BATTERY_PIN 34

// Define the size of the keypad (4x4 matrix)
const byte ROWS = 4;  // Number of rows
const byte COLS = 4;  // Number of columns

// Keymap for the keypad
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// ESP-32
byte rowPins[ROWS] = { 12, 13, 14, 15 };
byte colPins[COLS] = { 18, 19, 21, 22 };

/*The variables*/
WebServer server(80);  // server at port 80
String pass = "0000";
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
Servo servo;
String inp;

/*Setup function*/
void setup_wifi() {

  Serial.println("Setting Up WIFI...");
  Serial.print("Connecting to WIFI");

  // Connect to Wi-Fi
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print the IP address assigned so that we can use it in the website
  Serial.println();
  Serial.print("Connected to Wi-Fi. IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("The Port is 80");

  Serial.println("WIFI setup successful.");
}

/*Handling functions*/
void handle_setting_pin() {
  if (server.hasArg("pin")) {
    String pin = server.arg("pin");
    pass = pin;
    Serial.print("New Pin setup: ");
    Serial.println(pass);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(200, "text/plain", "done!");
  }
}

void handle_getting_voltage() {
  uint32_t vin = analogRead(BATTERY_PIN);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(200, "text/plain", String(vin * (3.3 / 4095.0) * (2)));
}

void handle_keypress(char c) {
  if (inp.length() == 4 || inp.length() == 3) {
    if (inp.length() == 3) {
      inp.concat(c);
      Serial.print("Entered: ");
      Serial.println(c);
    }
    if (pass == inp) {
      Serial.println("Correct password!");
      servo.write(90);
      delay(5000);  // open for five seconds
      servo.write(0);
    } else {
      Serial.println("Incorrect Password!");
    }
    inp = "";
  } else {
    inp.concat(c);
    Serial.print("Entered: ");
    Serial.println(c);
  }
}

void setup() {
  // Start Serial communication
  Serial.begin(115200);

  servo.attach(SERVO_PIN);
  servo.write(90);

  setup_wifi();

  server.on("/set_pin", HTTP_GET, handle_setting_pin);
  server.on("/vin",  HTTP_GET, handle_getting_voltage);

  // Start the server
  server.begin();
  servo.write(0);
}

void loop() {
  // Continuously check for incoming client requests
  server.handleClient();
  char key = keypad.getKey();
  if (key) {
    handle_keypress(key);
  }
}