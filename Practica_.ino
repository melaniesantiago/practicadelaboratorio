// ============================================================
// Servidor CoAP en ESP32-C6 - Práctica de laboratorio (con LED)
// Recursos ofrecidos: /sensor (lectura) y /led (acción)
// Transporte: UDP, puerto 5683 (puerto oficial de CoAP)
// Librería: CoAPSimple, de hirotakaster
// Las respuestas incluyen el Token de la petición (packet.token)
// y /led enciende o apaga un LED real con los payloads ON / OFF.
// ============================================================

#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

// --- Credenciales de la red ---------------------------------
// Sustituye por los de tu red de 2.4 GHz.
const char* ssid     = "TP-Link_1148";
const char* password = "17613460";


// --- Pin del LED --------------------------------------------
// Verifica en el diagrama de pines de tu placa que este GPIO
// esté libre. Cámbialo si tu cableado usa otro pin.
#define LED_PIN 3

WiFiUDP udp;
Coap coap(udp);

// ============================================================
// CALLBACK DEL RECURSO /led
// POST con payload "ON" enciende el LED; "OFF" lo apaga.
// ============================================================
void callback_led(CoapPacket &packet, IPAddress ip, int port) {

  Serial.println("Petición CoAP recibida en /led");

  // Copiamos el payload (bytes sin carácter de fin) a un texto.
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = '\0';

  String payloadStr(p);
  payloadStr.trim();   // quita espacios o saltos de línea sobrantes
  Serial.print("Payload recibido: ");
  Serial.println(payloadStr);

  // --- Actuar sobre el LED según el payload -------------------
  const char* msg;
  if (payloadStr == "ON") {
    digitalWrite(LED_PIN, HIGH);
    msg = "Estado: OK desde ESP32 CoAP Server - LED ON";
  } else if (payloadStr == "OFF") {
    digitalWrite(LED_PIN, LOW);
    msg = "Estado: OK desde ESP32 CoAP Server - LED OFF";
  } else {
    msg = "Estado: comando no reconocido (usa ON u OFF)";
  }

  // Respuesta con la versión larga de sendResponse, que acepta
  // el Token de la petición (packet.token / packet.tokenlen).
  coap.sendResponse(ip, port, packet.messageid,
                    msg, strlen(msg),
                    COAP_CONTENT, COAP_TEXT_PLAIN,
                    packet.token, packet.tokenlen);
}

// ============================================================
// CALLBACK DEL RECURSO /sensor
// ============================================================
void callback_sensor(CoapPacket &packet, IPAddress ip, int port) {

  Serial.println("Petición CoAP recibida en /sensor");

  // Temperatura simulada: 25.4 a 30.3 °C
  float temperaturaSimulada = 25.4 + (random(0, 50) / 10.0);

  // String(float) usa 2 decimales por defecto (ej. 27.10)
  String respuesta = "Temp: " + String(temperaturaSimulada) + " C";

  coap.sendResponse(ip, port, packet.messageid,
                    respuesta.c_str(), respuesta.length(),
                    COAP_CONTENT, COAP_TEXT_PLAIN,
                    packet.token, packet.tokenlen);
}

// ============================================================
// setup(): se ejecuta UNA sola vez
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  // LED apagado al arrancar
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado a la red");
  Serial.print("Dirección IP del Servidor CoAP: ");
  Serial.println(WiFi.localIP());

  // Tabla de rutas del servidor
  coap.server(callback_led, "led");
  coap.server(callback_sensor, "sensor");

  // Abre el puerto UDP 5683
  coap.start();
}

// ============================================================
// loop(): solo escucha. Sin delays largos aquí.
// ============================================================
void loop() {
  coap.loop();
}
