#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

WebServer server(80);
Adafruit_BME280 bme;

// Relay pins
int relays[] = {16, 15, 7, 6, 5, 4};
bool relayState[6] = {0,0,0,0,0,0};

// HTML Page
String webpage() {
  String page = "<!DOCTYPE html><html><head><title>ESP32 Control</title></head><body>";
  page += "<h1>ESP32 Home Automation</h1>";

  // Sensor Data
  page += "<h2>Environment</h2>";
  page += "Temp: " + String(bme.readTemperature()) + " °C<br>";
  page += "Humidity: " + String(bme.readHumidity()) + " %<br>";
  page += "Pressure: " + String(bme.readPressure() / 100.0F) + " hPa<br>";

  // Relay Controls
  page += "<h2>Relays</h2>";
  for (int i = 0; i < 6; i++) {
    page += "Relay " + String(i+1) + " : ";
    page += relayState[i] ? "ON" : "OFF";
    page += " <a href='/toggle?relay=" + String(i) + "'>Toggle</a><br>";
  }

  page += "</body></html>";
  return page;
}

// Toggle handler
void handleToggle() {
  if (server.hasArg("relay")) {
    int r = server.arg("relay").toInt();
    if (r >= 0 && r < 6) {
      relayState[r] = !relayState[r];
      digitalWrite(relays[r], relayState[r]);
    }
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

// Root page
void handleRoot() {
  server.send(200, "text/html", webpage());
}

void setup() {
  Serial.begin(115200);

  // Init relays
  for (int i = 0; i < 6; i++) {
    pinMode(relays[i], OUTPUT);
    digitalWrite(relays[i], LOW);
  }

  // I2C + BME280
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!bme.begin(0x76)) {
    Serial.println("BME280 not found!");
  }

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  // Web routes
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);

  server.begin();
}

void loop() {
  server.handleClient();
}
