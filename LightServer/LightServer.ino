// Load Wi-Fi library
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>   

// Replace with your network credentials
const char* ssid = "HuyenLinhKhu";
const char* password =  "1:9:7:3:5@5";

// Set web server port number to 80
WebServer server(80);

// Dht11 config
#define DHTPIN 15            // Chân dữ liệu của DHT11 kết nối với GPIO4 của ESP8266
#define DHTTYPE DHT11       // Loại DHT được sử dụng
DHT dht(DHTPIN, DHTTYPE);

// Auxiliar variables to store the current output state
String output23State = "off";
String output18State = "off";

// Assign output variables to GPIO pins
const int output23 = 23;
const int output18 = 18;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

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

void setup() {
  Serial.begin(115200);
  
  // Initialize the output variables as outputs
  pinMode(output23, OUTPUT);
  pinMode(output18, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output23, LOW);
  digitalWrite(output18, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Dht setup
  dht.begin();
  Serial.println("Connect to dht11");

  server.enableCORS();

  server.on("/dht11", []() {
    float temp = dht.readTemperature();
    float humi = dht.readHumidity();
    if (isnan(temp) || isnan(humi)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    Serial.printf("Nhiet do %s - Do am %s\r\n", String(temp, 1).c_str(), String(humi, 1).c_str());
    String tempStr = String(temp, 0);
    String humiStr = String(humi, 0);
    String res = String(tempStr + " " + humiStr);
    server.send(200, "text/plain", res);
  });
  
  server.on("/led/2/on", []() {
    if (output23State == "off") {
      Serial.println("LED 23 on");
      output23State = "on";
      digitalWrite(output23, HIGH);
      server.send(200, "text/plain", "Sucessfully!");
    }
    else {
      server.send(304, "text/plain", "No change!");
    }
  });

  server.on("/led/2/off", []() {
    if (output23State == "on"){
      Serial.println("LED 23 off");
      output23State = "off";
      digitalWrite(output23, LOW);
      server.send(200, "text/plain", "Sucessfully!");
    }
    else {
      server.send(304, "text/plain", "No change!");
    }
  });

  server.on("/led/1/on", []() {
    if (output18State == "off") {
      Serial.println("LED 18 on");
      output18State = "on";
      digitalWrite(output18, HIGH);
      server.send(200, "text/plain", "Sucessfully!");
    }
    else {
      server.send(304, "text/plain", "No change!");
    }
  });

  server.on("/led/1/off", []() {
    if (output18State == "on"){
      Serial.println("LED 18 off");
      output18State = "off";
      digitalWrite(output18, LOW);
      server.send(200, "text/plain", "Sucessfully!");
    }
    else {
      server.send(304, "text/plain", "No change!");
    }
  });

  server.on("/led/2/status", []() {
    server.send(200, "text/plain", output23State);
  });

  server.on("/led/1/status", []() {
    server.send(200, "text/plain", output18State);
  });

  server.onNotFound(handleNotFound);
  server.begin();
}

void loop(void) {
  server.handleClient();
}
