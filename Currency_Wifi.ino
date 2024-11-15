#include <Wire.h>
#include <SPI.h>
#include <WiFi101.h>
#include <TinyScreen.h>
#include <ArduinoJson.h>

const char* ssid = "Wifi_Name";
const char* password = "Wifi_Password";
const char* serverUrl = "IP_Address";  // Replace with actual IP address
const int serverPort = 8080;

TinyScreen display = TinyScreen(TinyScreenDefault);

#if defined(ARDUINO_ARCH_SAMD)
  #define SerialMonitorInterface SerialUSB
#else
  #define SerialMonitorInterface Serial
#endif

void setup() {
  SerialMonitorInterface.begin(9600);
  WiFi.setPins(8, 2, A3, -1);
  while (!SerialMonitorInterface);

  // Initialize TinyScreen
  Wire.begin();                    
  display.begin();
  display.setBrightness(10);
  delay(100);
  display.clearScreen();
  
  // Connect to WiFi
  SerialMonitorInterface.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    SerialMonitorInterface.print(".");
    delay(500);
  }
  SerialMonitorInterface.println("\nWiFi connected");

  // Display WiFi connection success on TinyScreen
  display.setCursor(10, 20);
  display.setFont(thinPixel7_10ptFontInfo);
  display.fontColor(TS_8b_White, TS_8b_Black);
  display.print("WiFi Connected!");
  delay(2000);  // Display message for 2 seconds
}

void loop() {
  WiFiClient client;

  // Attempt to connect to the server
  if (client.connect(serverUrl, serverPort)) {
    SerialMonitorInterface.println("Connected to server.");
    client.println("GET /exchange_rate?base_currency=USD&target_currency=EUR HTTP/1.1");
    client.print("Host: ");
    client.println(serverUrl);
    client.println("Connection: close");
    client.println();

    String payload = "";
    while (client.connected() || client.available()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        payload += line + "\n";
      }
    }
    client.stop();

    SerialMonitorInterface.println("Raw payload received:");
    SerialMonitorInterface.println(payload);

    int jsonStartIndex = payload.indexOf('{');
    if (jsonStartIndex == -1) {
      SerialMonitorInterface.println("Error: JSON not found in response.");
      return;
    }

    String jsonPayload = payload.substring(jsonStartIndex);
    SerialMonitorInterface.println("Extracted JSON:");
    SerialMonitorInterface.println(jsonPayload);

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, jsonPayload);

    if (error) {
      SerialMonitorInterface.print("JSON Parse Error: ");
      SerialMonitorInterface.println(error.c_str());
      return;
    }

    const char* base_currency = doc["base_currency"];
    const char* target_currency = doc["target_currency"];
    float rate = doc["rate"];
    const char* timestamp = doc["timestamp"];

    // Display data on TinyScreen in a similar format to the working code
    display.clearScreen();
    display.setCursor(10, 10);
    display.setFont(thinPixel7_10ptFontInfo);
    display.fontColor(TS_8b_White, TS_8b_Black);
    display.print("Base: ");
    display.print(base_currency);

    display.setCursor(10, 25);
    display.print("Target: ");
    display.print(target_currency);

    display.setCursor(10, 40);
    display.print("Rate: ");
    display.print(rate, 4);  // Display rate with 4 decimal places

    display.setCursor(10, 55);
    display.print("Time: ");
    display.print(timestamp);

    // Print data to Serial Monitor for confirmation
    SerialMonitorInterface.println("Parsed Data:");
    SerialMonitorInterface.print("Base Currency: ");
    SerialMonitorInterface.println(base_currency);
    SerialMonitorInterface.print("Target Currency: ");
    SerialMonitorInterface.println(target_currency);
    SerialMonitorInterface.print("Rate: ");
    SerialMonitorInterface.println(rate);
    SerialMonitorInterface.print("Timestamp: ");
    SerialMonitorInterface.println(timestamp);
  } else {
    SerialMonitorInterface.println("Failed to connect to server.");

    // Display connection error on TinyScreen
    display.clearScreen();
    display.setCursor(10, 20);
    display.setFont(thinPixel7_10ptFontInfo);
    display.fontColor(TS_8b_White, TS_8b_Black);
    display.print("Failed to connect");
  }

  delay(60000);  // Fetch data every 60 seconds
}

