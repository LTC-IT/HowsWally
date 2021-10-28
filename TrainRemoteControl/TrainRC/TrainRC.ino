/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <PowerFunctions.h>   //Power Functions Library

#define FORMAT_SPIFFS_IF_FAILED true

// Wifi & Webserver
#include "WiFi.h"
#include "SPIFFS.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "wifiConfig.h"
AsyncWebServer server(80);

const char* http_username = "admin1";
const char* http_password = "admin1";

// RTC
#include "RTClib.h"

RTC_PCF8523 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


// EINK
#include "Adafruit_ThinkInk.h"

#define EPD_CS      15
#define EPD_DC      33
#define SRAM_CS     32
#define EPD_RESET   -1 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    -1 // can set to -1 to not use a pin (will wait a fixed delay)

// 2.13" Monochrome displays with 250x122 pixels and SSD1675 chipset
ThinkInk_213_Mono_B72 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);



//IR Channels
#define CH1 0x0
#define CH2 0x1
#define CH3 0x2
#define CH4 0x3

//IR Transmission
#define IR_TRANS_IN   21  //IR Trans PIN
#define IR_DEBUG_OFF  0  //IR Debug Mode Off
#define IR_DEBUG_ON   1  //IR Debug Mode On

//Call PowerFunctions parameters
PowerFunctions pf(IR_TRANS_IN, CH1, IR_DEBUG_ON);


// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


// Single and dual motor control is defined
void step(uint8_t output, uint8_t pwm, uint16_t time) {
  pf.combo_pwm(output, pwm);
  pf.single_pwm(output, pwm);
}
// Single increment for speed is defined
void increment(uint8_t output) {
  pf.single_increment(output);
}
// Single decrement for speed is defined
void decrement(uint8_t output) {
  pf.single_decrement(output);
}


void setup() {
  // Connect to Wi-Fi network with SSID and password.
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  delay(1000);
  pinMode(21, OUTPUT);

  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    // Follow instructions in README and install
    // https://github.com/me-no-dev/arduino-esp32fs-plugin
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println();
  Serial.print("Connected to the Internet");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/speed=7", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    logEvent("Speed 7");
    step(RED, PWM_FWD7, 0); // The step function is run and speed is set to 7
    request->send(SPIFFS, "/dashboard.html", "text/html");
  });
  server.on("/speed=6", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    logEvent("Speed 6");
    step(RED, PWM_FWD6, 0); // The step function is run and speed is set to 7
    request->send(SPIFFS, "/dashboard.html", "text/html");
  });
  server.on("/speed=5", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    logEvent("Speed 5");
    step(RED, PWM_FWD5, 0); // The step function is run and speed is set to 7
    request->send(SPIFFS, "/dashboard.html", "text/html");
  });
  server.on("/speed=4", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    logEvent("Speed 4");
    step(RED, PWM_FWD4, 0); // The step function is run and speed is set to 7
    request->send(SPIFFS, "/dashboard.html", "text/html");
  });
  server.on("/speed=3", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    logEvent("Speed 3");
    step(RED, PWM_FWD3, 0); // The step function is run and speed is set to 7
    request->send(SPIFFS, "/dashboard.html", "text/html");
  });
  server.on("/speed=2", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    logEvent("Speed 2");
    step(RED, PWM_FWD2, 0); // The step function is run and speed is set to 7
    request->send(SPIFFS, "/dashboard.html", "text/html");
  });
  server.on("/speed=1", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    logEvent("Speed 1");
    step(RED, PWM_FWD1, 0); // The step function is run and speed is set to 7
    request->send(SPIFFS, "/dashboard.html", "text/html");
  });
  server.on("/brake", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    Serial.println("Brake");
    step(RED, PWM_BRK, 0);
    request->send(SPIFFS, "/dashboard.html", "text/html");
  });
  server.on("/speed=-1", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    logEvent("Speed -1");
    step(RED, PWM_REV1, 0); // The step function is run and speed is set to 7
    request->send(SPIFFS, "/dashboard.html", "text/html");
  });
  server.on("/speed=-2", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    logEvent("Speed -2");
    step(RED, PWM_REV2, 0); // The step function is run and speed is set to 7
    request->send(SPIFFS, "/dashboard.html", "text/html");
  });
  server.on("/speed=-3", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    logEvent("Speed -3");
    step(RED, PWM_REV3, 0); // The step function is run and speed is set to 7
    request->send(SPIFFS, "/dashboard.html", "text/html");
  });
  server.on("/speed=-4", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    logEvent("Speed -4");
    step(RED, PWM_REV4, 0); // The step function is run and speed is set to 7
    request->send(SPIFFS, "/dashboard.html", "text/html");
  });
  server.on("/speed=-5", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    logEvent("Speed -5");
    step(RED, PWM_REV5, 0); // The step function is run and speed is set to 7
    request->send(SPIFFS, "/dashboard.html", "text/html");
  });
  server.on("/speed=-6", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    logEvent("Speed 6");
    step(RED, PWM_REV6, 0); // The step function is run and speed is set to 7
    request->send(SPIFFS, "/dashboard.html", "text/html");
  });
  server.on("/speed=-7", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    logEvent("Speed -7");
    step(RED, PWM_REV7, 0); // The step function is run and speed is set to 7
    request->send(SPIFFS, "/dashboard.html", "text/html");
  });

  server.begin();

  // RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    //    abort();
  }

  // The following line can be uncommented if the time needs to be reset.
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  rtc.start();

  //EINK
  display.begin(THINKINK_MONO);
  display.clearBuffer();

  logEvent("System Initialisation...");
}

void loop() {

  //wifiserver();

}

void logEvent(String dataToLog) {
  /*
     Log entries to a file stored in SPIFFS partition on the ESP32.
  */
  // Get the updated/current time
  DateTime rightNow = rtc.now();
  char csvReadableDate[25];
  sprintf(csvReadableDate, "%02d,%02d,%02d,%02d,%02d,%02d,",  rightNow.year(), rightNow.month(), rightNow.day(), rightNow.hour(), rightNow.minute(), rightNow.second());

  String logTemp = csvReadableDate + dataToLog + "\n"; // Add the data to log onto the end of the date/time

  const char * logEntry = logTemp.c_str(); //convert the logtemp to a char * variable

  //Add the log entry to the end of logevents.csv
  appendFile(SPIFFS, "/logEvents.csv", logEntry);

  // Output the logEvents - FOR DEBUG ONLY. Comment out to avoid spamming the serial monitor.
  //  readFile(SPIFFS, "/logEvents.csv");

  Serial.print("\nEvent Logged: ");
  Serial.println(logEntry);
}

//SPIFFS File Functions
void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("- failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("- message appended");
  } else {
    Serial.println("- append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2) {
  Serial.printf("Renaming file %s to %s\r\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("- file renamed");
  } else {
    Serial.println("- rename failed");
  }
}

void deleteFile(fs::FS &fs, const char * path) {
  Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path)) {
    Serial.println("- file deleted");
  } else {
    Serial.println("- delete failed");
  }
}
