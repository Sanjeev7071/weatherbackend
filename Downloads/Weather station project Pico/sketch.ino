#include <WiFi.h>
#include "DHT.h"
#include <HTTPClient.h>

// ----- DHT22 -----
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ----- WiFi -----
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ----- ThingSpeak -----
const char* server = "http://api.thingspeak.com/update";
String apiKey = "RZ1AOVMUF2HP07W4";

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Start DHT22
  dht.begin();
  Serial.println("DHT22 sensor started.");

  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("Failed to read from DHT22!");
    delay(2000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" °C | Humidity: ");
  Serial.print(hum);
  Serial.println(" %");

  // Send to ThingSpeak
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(temp) + "&field2=" + String(hum);

    http.begin(url);
    int httpCode = http.GET();

    if(httpCode > 0) {
      Serial.print("ThingSpeak response code: ");
      Serial.println(httpCode);
    } else {
      Serial.print("Error sending to ThingSpeak: ");
      Serial.println(http.errorToString(httpCode));
    }

    http.end();
  } else {
    Serial.println("WiFi not connected!");
  }

  delay(15000); // ThingSpeak allows 1 update every 15 sec
}

