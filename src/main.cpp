#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WiFi.h>

#define PIN_RED 15
#define PIN_GREEN 2
#define PIN_BLUE 4

AsyncWebServer server(80);

int red, green, blue;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Format the file system if it fails
  LittleFS.begin(true);

  // Send the index.html file on root
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  // Handle POST requests to /set-color
  server.on("/setcolor", HTTP_POST, [](AsyncWebServerRequest* request) {
    if (request->hasParam("r", true) && request->hasParam("g", true) &&
        request->hasParam("b", true)) {
      int r = request->getParam("r", true)->value().toInt();
      int g = request->getParam("g", true)->value().toInt();
      int b = request->getParam("b", true)->value().toInt();

      Serial.printf("Setting RGB to: R:%d, G:%d, B:%d\n", r, g, b);

      red = r;
      green = g;
      blue = b;

      // Set the colors on the LED using PWM
      analogWrite(PIN_RED, r);
      analogWrite(PIN_GREEN, g);
      analogWrite(PIN_BLUE, b);

      request->send(200, "text/plain", "Color Updated");
    } else {
      request->send(400, "text/plain", "Missing RGB parameters");
    }
  });

  // Get the current color (on page load) as JSON
  server.on("/getcolor", HTTP_GET, [](AsyncWebServerRequest* request) {
    // Create a JSON string: {"r":255, "g":0, "b":0}
    String json = "{\"r\":" + String(red) + ",\"g\":" + String(green) +
                  ",\"b\":" + String(blue) + "}";
    request->send(200, "application/json", json);
  });

  server.begin();
}

void loop() {}