#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WiFi.h>

#define PIN_LED 2

AsyncWebServer server(80);

void setup() {
  pinMode(PIN_LED, OUTPUT);
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
  server.on("/set", HTTP_POST, [](AsyncWebServerRequest* request) {
    if (request->hasParam("status", true)) {
      const String status = request->getParam("status", true)->value();
      bool isOn = (status == "ON");

      digitalWrite(PIN_LED, isOn ? HIGH : LOW);

      request->send(200, "application/json",
                    "{\"status\":\"" + status +
                        "\", \"message\":\"LED turned " + status + "\"}");
    } else {
      request->send(400, "text/plain", "Missing status parameter");
    }
  });

  // Get the current LED status (on page load) as JSON
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest* request) {
    // Create a JSON string: {"status":true}
    String json = "{\"status\":" +
                  String(digitalRead(PIN_LED) == HIGH ? "true" : "false") + "}";
    request->send(200, "application/json", json);
  });

  server.begin();
}

void loop() {}