// Wifi
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "wifiConfig.h"

String loginIndex, serverIndex;
WebServer server(80);

// SD Card - Adalogger
//#include "FS.h"
#include "SD.h"

// RTC
#include "RTClib.h"

RTC_PCF8523 rtc;

int rLED = 33;
int yLED = 27;
int gLED = 12;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  delay(1000);

  // SD Card
  setupSD();

  // Webserver

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  loadHTML();

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */

  server.on("/", HTTP_GET, []() {
    Serial.println("Index");
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    Serial.println("serverIndex");
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  server.begin();

  // RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  // The following line can be uncommented if the time needs to be reset.
  //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  rtc.start();
  logEvent("System Initialisation...");

  // Traffic light LEDs
  pinMode(rLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(gLED, OUTPUT);
}

void loop() {
  server.handleClient();

  trafficLights();
}

void setupSD() {
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("SD Started");
  //  delay(1000);
}

void logEvent(String dataToLog) {
  /*
     Log entries to a file on an SD card.
  */
  // Get the updated/current time
  DateTime rightNow = rtc.now();

  // Open the log file
  File logFile = SD.open("/logEvents.csv", FILE_APPEND);
  if (!logFile) {
    Serial.print("Couldn't create log file");
    abort();
  }

  // Log the event with the date, time and data
  logFile.print(rightNow.year(), DEC);
  logFile.print(",");
  logFile.print(rightNow.month(), DEC);
  logFile.print(",");
  logFile.print(rightNow.day(), DEC);
  logFile.print(",");
  logFile.print(rightNow.hour(), DEC);
  logFile.print(",");
  logFile.print(rightNow.minute(), DEC);
  logFile.print(",");
  logFile.print(rightNow.second(), DEC);
  logFile.print(",");
  logFile.print(dataToLog);

  // End the line with a return character.
  logFile.println();
  logFile.close();
  Serial.print("Event Logged: ");
  Serial.print(rightNow.year(), DEC);
  Serial.print(",");
  Serial.print(rightNow.month(), DEC);
  Serial.print(",");
  Serial.print(rightNow.day(), DEC);
  Serial.print(",");
  Serial.print(rightNow.hour(), DEC);
  Serial.print(",");
  Serial.print(rightNow.minute(), DEC);
  Serial.print(",");
  Serial.print(rightNow.second(), DEC);
  Serial.print(",");
  Serial.println(dataToLog);
}

String readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);
  char c;
  String tempHTML = "";

  File file = fs.open(path);
  if (!file) {
    Serial.print("Failed to open file for reading: ");
    Serial.println(path);
    return "";
  }

  while (file.available()) {
    c = file.read();
    tempHTML += c;
  }
  file.close();
  return tempHTML;
}

void loadHTML() {
  serverIndex = readFile(SD, "/serverIndex.html");
  loginIndex = readFile(SD, "/loginIndex.html");
}

void trafficLights(){ // Cycle through the colours
  digitalWrite(rLED, HIGH);
  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);
  delay(200);
  digitalWrite(rLED, LOW);
  digitalWrite(yLED, HIGH);
  delay(200);
  digitalWrite(yLED, LOW);
  digitalWrite(gLED, HIGH);
  delay(200);
}
