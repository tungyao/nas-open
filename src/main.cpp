#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <uri/UriBraces.h>


const char* ssid = "Redmi_6368";
const char* password = "123123123";
const int led = 35;


WebServer server(80);

void handleRoot() {

  server.send(200, "text/plain", "hello from esp32!");
}

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

void setup(void) {
  pinMode(35, OUTPUT);
  pinMode(27, OUTPUT);
  digitalWrite(35, 0);
  digitalWrite(27, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on(UriBraces("/a/{}"), []() {
    String user = server.pathArg(0);
    int a = user.toInt();
    server.send(200, "text/plain", "User: '" + user + "'");
    digitalWrite(a, 1);
    delay(2000);
    digitalWrite(a, 0);
    Serial.println(a);
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}
unsigned long previousMillis = 0;
unsigned long interval = 30000;
void loop(void) {
   unsigned long currentMillis = millis();
  // 定时重连
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
  server.handleClient();
  delay(2);  //allow the cpu to switch to other tasks
}