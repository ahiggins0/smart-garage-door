#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>

#define LED_BUILTIN 2 //LED Pin
#define GPIO_4 4 // activate pin
#define GPIO_5 5 // magnetic status pin
#define HTTP_REST_PORT 8080

// Wi-Fi connection parameters
const char * wifi_ssid = "";
const char * wifi_password = "";

ESP8266WebServer server(HTTP_REST_PORT);

// Set your Static IP address
IPAddress local_IP(192, 168, 0, 15);
// Set your Gateway IP address
IPAddress gateway(192, 168, 0, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(192, 168, 0, 1);   //optional
IPAddress secondaryDNS(192, 168, 0, 200); //optional

void setup(void) {
  Serial.begin(9600);
  Serial.println("Booting...");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(GPIO_4, OUTPUT);
  pinMode(GPIO_5, INPUT_PULLUP);
  connectToWiFi();
  setUpOverTheAirProgramming();
  restServerRouting();
  server.onNotFound(handleNotFound);
  server.begin();
}

void connectToWiFi() {
  Serial.printf("Connecting to '%s'\n", wifi_ssid);

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    Serial.print("Connected. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Connection Failed!");
  }
}

void setUpOverTheAirProgramming() {

  // Change OTA port. 
  // Default: 8266
  // ArduinoOTA.setPort(8266);

  // Change the name of how it is going to 
  // show up in Arduino IDE.
  // Default: esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // Re-programming passowrd. 
  // No password by default.
  ArduinoOTA.setPassword("1137");

  ArduinoOTA.begin();
}

void triggerGarageDoor() {
  server.send(200, "text/json", "{\"trigger\": \"true\"}");

  digitalWrite(GPIO_4, HIGH);
  //digitalWrite(GPIO_5, HIGH);
  delay(1000); 
   digitalWrite(GPIO_4, LOW);
  //digitalWrite(GPIO_5, LOW);
}

void garageStatus() {
  if (digitalRead(GPIO_5) == 1) {
    server.send(200, F("text/json"), F("{\"status\": \"closed\"}"));
  }
  else {
    server.send(200, F("text/json"), F("{\"status\": \"open\"}"));
  }
}

// Define routing
void restServerRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, F("text/html"),
            F("Welcome to the REST Web Server"));
    });
    server.on(F("/trigger"), HTTP_GET, triggerGarageDoor);
    server.on(F("/status"), HTTP_GET, garageStatus);
}

// Manage not found URL
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


void loop(void) {
  // Give processing time for ArduinoOTA
  ArduinoOTA.handle();

  server.handleClient();
}
