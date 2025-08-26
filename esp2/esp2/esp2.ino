#include <WiFi.h>
#include <WebServer.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <string.h>
// #include <WebSocketsServer.h>
#include <Adafruit_AHT10.h>
#include <stdatomic.h>

// Wifi credentials
#define SSID "beast"
#define PASSWORD "12345abcde"

// #define SSID "smartfarm"
// #define PASSWORD "12345678"

// Define hardware type, number of devices, and SPI pins
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4  // Number of LED matrix modules

// ESP32 SPI Pins (Use hardware SPI)
#define CS_PIN 5     // Chip Select
#define DATA_PIN 23  // MOSI (Data)
#define CLK_PIN 18   // SCLK (Clock)

// The temperature sensor uses 21 and 22
#define RES_PIN1 21
#define RES_PIN2 22

#define HUMIDIFIER_PIN 12

volatile long HUMIDIFIER_THRESHOLD = 80;
volatile long TEMPR_THRESHOLD_MIN = 13;
volatile long TEMPR_THRESHOLD_MAX = 20;

#define FAN_PIN 14

#define BIOGAS_SENSOR 34

/*The variables*/
WebServer server(80);  // server at port 80
MD_Parola display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

Adafruit_AHT10 aht;
sensors_event_t humidity, temp;
int biogas_val = 0;
volatile int sval = 0;  // state(1 says 'can' print else cannot)

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
  // Serial.println();
  Serial.print("Connected to Wi-Fi. IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("The Port is 80");

  // Serial.println("WIFI setup successful.");
}

/*Utility functions*/
// simply print something
void println(const char *str, uint16_t display_for) {
  display.displayReset();
  display.displayClear();
  display.displayText(str, PA_LEFT, 200, 20, PA_PRINT);

  while (!display.displayAnimate()) { vTaskDelay(10); }
  display.displayReset();
  delay(display_for);
}

// print anything that scrolls num_of_times
void printlnScroll(const char *str, uint32_t num_of_times) {
  display.displayReset();
  display.displayClear();
  display.displayScroll(str, PA_LEFT, PA_SCROLL_LEFT, 100);

  for (uint32_t i = 0; i < num_of_times; i++) {
    while (!display.displayAnimate()) { vTaskDelay(10); }
    display.displayReset();
  }
}

// Sending any data to the browser
/*Handling functions*/
void handle_print_line() {
  if (server.hasArg("str")) {
    String line = server.arg("str");
    String time = server.arg("time");
    println(line.c_str(), strtoul(time.c_str(), NULL, 10));  // by default
    server.send(200, "text/plain", " ");
  }
}

void handle_print_scrollline() {
  if (server.hasArg("str")) {
    String line = server.arg("str");
    String times = server.arg("times");
    printlnScroll(line.c_str(), strtoul(times.c_str(), NULL, 10));
    server.send(200, "text/plain", " ");
  }
}

void send_status(const char* msg) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(200, "text/plain", msg);
}

void handle_get_tempr() {
  send_status(String(temp.temperature).c_str());
}

void handle_get_humidity() {
  send_status(String(humidity.relative_humidity).c_str());
}

void handle_get_bio_lvl() {
  send_status(String(map(biogas_val, 0, 1023, 0, 100)).c_str());
}

void handle_change_humidifier_threshold() {
  if (server.hasArg("val"))
  {
    String val = server.arg("val");
    HUMIDIFIER_THRESHOLD = strtol(val.c_str(), NULL, 10);
    send_status("done!");
  }
}

void handle_set_fan_upper_lim() {
  if (server.hasArg("val"))
  {
    String val = server.arg("val");
    TEMPR_THRESHOLD_MAX = strtol(val.c_str(), NULL, 10);
    send_status("done!");
  }
}

void handle_set_fan_lower_lim() {
  if (server.hasArg("val"))
  {
    String val = server.arg("val");
    TEMPR_THRESHOLD_MIN = strtol(val.c_str(), NULL, 10);
    send_status("done!");
  }
}

void setPin(void *params) {
  while (1) {
    if (humidity.relative_humidity <= HUMIDIFIER_THRESHOLD) {
      // Serial.println("HUMIDITY THRESHOLD PASSED...");
      digitalWrite(HUMIDIFIER_PIN, HIGH);
    } else {
      digitalWrite(HUMIDIFIER_PIN, LOW);
    }
    if (temp.temperature >= TEMPR_THRESHOLD_MAX) {
      digitalWrite(FAN_PIN, HIGH);
    } else{
      digitalWrite(FAN_PIN, LOW);
    }
    vTaskDelay(500);
  }
}

void readState(void *params) {
  while (1) {
    biogas_val = analogRead(BIOGAS_SENSOR);
    biogas_val = map(biogas_val, 0, 1023, 0, 100);

    aht.getEvent(&humidity, &temp);
  }
}

void displayState(void *params) {
  while (1) {
    println(("T:" + String((int32_t)(temp.temperature)) + "C").c_str(), 1000);
    println(("H:" + String((int32_t)(humidity.relative_humidity)) + "%").c_str(), 1000);
    println(("G:" + String((int32_t)(biogas_val)) + "%").c_str(), 1000);
    vTaskDelay(2000);
  }
}

void setup() {
  // Start Serial communication
  Serial.begin(115200);

  setup_wifi();

  server.on("/println", HTTP_GET, handle_print_line);

  server.on("/printsln", HTTP_GET, handle_print_scrollline);

  server.on("/temp", HTTP_GET, handle_get_tempr);

  server.on("/hmd", HTTP_GET, handle_get_humidity);

  server.on("/blvl", HTTP_GET, handle_get_bio_lvl);

  server.on("/hth", HTTP_GET, handle_change_humidifier_threshold);

  server.on("/foM", HTTP_GET,handle_set_fan_upper_lim );

  server.on("/fom", HTTP_GET, handle_set_fan_lower_lim);

  // Start the server
  server.begin();

  // setup the display
  display.begin();
  display.setIntensity(5);
  display.displayReset();

  pinMode(HUMIDIFIER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  aht.begin();

  delay(5000);

  xTaskCreatePinnedToCore(readState, "READER", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(displayState, "PRINTER", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(setPin, "SETTER", 1024, NULL, 1, NULL, 1);
}

void loop() {
  server.handleClient();

  // Serial.print("Temperature: ");
  // Serial.print(temp.temperature);
  // Serial.println(" +-0.3C");
  // Serial.print("Humidity...: ");
  // Serial.print(humidity.relative_humidity);
  // Serial.println(" +-2%");
  // Serial.print("BioGas Level...: ");
  // Serial.print(biogas_val);
  // Serial.println("%");
}
