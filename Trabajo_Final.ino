#include <WiFi.h>
#include <WebServer.h>
#include "src/secrets.h"
#include "DHT.h"

// Pines
#define Led_H 10
#define Led_T 8
#define Led_B 7
#define DHTPin 4

// Sensor DHT
#define DHTTYPE DHT22
DHT dht(DHTPin, DHTTYPE);

// Servidor HTTP
WebServer server(80);

// Página web prueba
String getPage(float h, float t) {
  String html = "<html><head>";
  html += "<meta http-equiv='refresh' content='2'>";  // refresca cada 2s automáticamente
  html += "</head><body style='font-family:Arial;'>";
  
  html += "<h1>ESP32S3 Test</h1>";
  html += "<p><b>Temperatura:</b> " + String(t) + " °C</p>";
  html += "<p><b>Humedad:</b> " + String(h) + " %</p>";
  html += "<p><a href='/led_on'>Encender LEDs</a></p>";
  html += "<p><a href='/led_off'>Apagar LEDs</a></p>";
  
  html += "</body></html>";
  return html;
}

void handleRoot() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("⚠ Error leyendo el DHT22");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.println(" °C");

    Serial.print("Humedad: ");
    Serial.print(hum);
    Serial.println(" %");
  }
  server.send(200, "text/html", getPage(hum, temp));
}

void handleLedOn() {
  digitalWrite(Led_T, HIGH);
  digitalWrite(Led_H, HIGH);
  digitalWrite(Led_B, HIGH);
  server.send(200, "text/plain", "LED T encendido!");
}

void handleLedOff() {
  digitalWrite(Led_T, LOW);
  digitalWrite(Led_H, LOW);
  digitalWrite(Led_B, LOW);
  server.send(200, "text/plain", "LED T apagado!");
}

void setup() {
  Serial.begin(115200);

  // Leds
  pinMode(Led_H, OUTPUT);
  pinMode(Led_T, OUTPUT);
  pinMode(Led_B, OUTPUT);

  digitalWrite(Led_H, LOW);
  digitalWrite(Led_T, LOW);
  digitalWrite(Led_B, LOW);

  // DHT
  dht.begin();
  delay(2000); //Necesario para que le de tiempo al sensor a iniciarse

  // WIFI
  Serial.print("Conectando a ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Rutas servidor web
  server.on("/", handleRoot);
  server.on("/led_on", handleLedOn);
  server.on("/led_off", handleLedOff);

  // Inicio servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}
