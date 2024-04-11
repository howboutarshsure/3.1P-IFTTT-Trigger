#include <WiFiNINA.h>
#include <BH1750.h>
#include <Wire.h>

// WiFi credentials
const char* ssid     = "MINEZ 2G";
const char* password = "69699696";
WiFiClient client;

// IFTTT Webhooks information
const String IFTTTKey = "dKXDCVns8E_mUn3pcYFJ_n";
const String eventStart = "Light_HIGH";  // Event name when sunlight starts
const String eventStop = "Light_LOW";    // Event name when sunlight stops

// BH1750 light sensor
BH1750 lightMeter;

// Sunlight threshold in lux
const float sunlightThreshold = 50.0; // Adjust based on your environment

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  // Initialize WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  if (WiFi.begin(ssid, password) != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi. Please check your credentials");
    while (true);
  }

  Serial.println("Connected to WiFi");
  
  // Initialize the BH1750 sensor
  Wire.begin();
  lightMeter.begin();
  
}

void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  
  static bool sunlightDetected = false;

  if (lux >= sunlightThreshold && !sunlightDetected) {
    // Sunlight starts
    Serial.println("Sunlight detected");
    sendNotification(eventStart);
    sunlightDetected = true;
  } else if (lux < sunlightThreshold && sunlightDetected) {
    // Sunlight stops
    Serial.println("Sunlight no longer detected");
    sendNotification(eventStop);
    sunlightDetected = false;
  }

  // Reading interval
  delay(5000); // Adjust as needed
}

void sendNotification(String event) {
  String url = "/trigger/" + event + "/with/key/" + IFTTTKey;
  
  if (client.connect("maker.ifttt.com", 80)) {
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: maker.ifttt.com");
    client.println("Connection: close");
    client.println(); // End of request
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break; // Headers received, body should follow
      }
    }
    Serial.println("Notification sent");
  } else {
    Serial.println("Failed to connect to IFTTT");
  }
  
  client.stop();
}
