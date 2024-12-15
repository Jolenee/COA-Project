#include <Wire.h>
#include <SPI.h>
#include <WiFi101.h>
#include <TinyScreen.h>
#include <ArduinoJson.h>

const char* ssid = "Wifi_Name";          // Your Wi-Fi SSID
const char* password = "Wifi_Password";           // Your Wi-Fi Password
const char* weatherApiUrl = "api.openweathermap.org";  // OpenWeatherMap API server
const char* apiKey = "YOUR_API_KEY";         // Replace with your OpenWeatherMap API key
const char* cityName = "YOUR_CITY_NAME";     // Replace with your city name

TinyScreen display = TinyScreen(TinyScreenDefault);

#if defined(ARDUINO_ARCH_SAMD)
  #define SerialMonitorInterface SerialUSB
#else
  #define SerialMonitorInterface Serial
#endif

unsigned long lastFetchTime = 0;   // Last fetch time for weather data
const unsigned long fetchInterval = 60000;  // Fetch new data every 60 seconds

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

  // Display Wi-Fi connection success on TinyScreen
  display.setCursor(10, 20);
  display.setFont(thinPixel7_10ptFontInfo);
  display.fontColor(TS_8b_White, TS_8b_Black);
  display.print("WiFi Connected!");
  delay(2000);
}

void loop() {
  // Fetch weather data at intervals
  if (millis() - lastFetchTime > fetchInterval) {
    fetchWeather();
    lastFetchTime = millis();
  }
}

void fetchWeather() {
  WiFiClient client;

  // Construct the API request URL
  String requestUrl = String("/data/2.5/weather?q=") + cityName + "&appid=" + apiKey + "&units=metric";

  if (client.connect(weatherApiUrl, 80)) {
    SerialMonitorInterface.println("Connected to OpenWeatherMap API.");
    client.println("GET " + requestUrl + " HTTP/1.1");
    client.println("Host: " + String(weatherApiUrl));
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

    const char* weatherCondition = doc["weather"][0]["description"];
    float temperature = doc["main"]["temp"];

    // Display weather data on TinyScreen
    display.clearScreen();
    display.setCursor(10, 5);
    display.setFont(thinPixel7_10ptFontInfo);
    display.fontColor(TS_8b_White, TS_8b_Black);
    display.print(cityName);  // Display the city name at the top

    display.setCursor(10, 20);
    display.print("Temp: ");
    display.print(temperature, 1); // Show 1 decimal place
    display.print(" C");

    display.setCursor(10, 35);
    display.print("Cond: ");
    display.print(weatherCondition);

    SerialMonitorInterface.println("Weather data displayed.");
  } else {
    SerialMonitorInterface.println("Failed to connect to OpenWeatherMap API.");
    display.clearScreen();
    display.setCursor(10, 20);
    display.setFont(thinPixel7_10ptFontInfo);
    display.fontColor(TS_8b_White, TS_8b_Black);
    display.print("Failed to connect");
  }
}

