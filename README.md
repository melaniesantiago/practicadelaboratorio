# Implementación de un Servidor CoAP en ESP32-C6 con Arduino IDE

**Reporte de práctica de laboratorio**

| | |
| :--- | :--- |
| **Institución** | Instituto Tecnológico Superior del Occidente del Estado de Hidalgo |
| **División** | Ingeniería en Tecnologías de la Información y Comunicaciones |
| **Materia** | Internet de las cosas |
| **Docente** | Prof. Saúl Isaí Soto Ortiz |
| **Fecha de entrega** | 18 de noviembre de 2026 |

## Integrantes

| Nombre | Actividad realizada |
| :--- | :--- |
| Freyra Wendy Martínez Martínez | [COMPLETAR: actividad] |
| Carol Mera Ibarra | [COMPLETAR: actividad] |
| [COMPLETAR: nombre] | [COMPLETAR: actividad] |
| [COMPLETAR: nombre] | [COMPLETAR: actividad] |

## Evidencia en video

Video del equipo realizando la práctica: [COMPLETAR: enlace al video]

---

## Índice

1. Resumen
2. Introducción
3. Objetivo
4. Marco teórico
5. Materiales y herramientas
6. Desarrollo
7. Resultados
8. Análisis de resultados
9. Preguntas de análisis
10. Reto opcional
11. Conclusiones
12. Estructura del repositorio
13. Referencias

---

## 1. Resumen

En esta práctica se configuró una tarjeta ESP32-C6 como servidor CoAP (Constrained Application Protocol) utilizando Arduino IDE y la librería CoAPSimple. El servidor expone dos recursos: `/sensor`, que devuelve una temperatura simulada mediante el método `GET`, y `/led`, que recibe los comandos `ON` y `OFF` mediante el método `POST` para controlar un LED. La comunicación se validó con un cliente escrito en Python con la librería `aiocoap`. Se obtuvieron respuestas `2.05 Content` en ambos recursos y una respuesta `4.04 Not Found` ante una ruta no registrada. Durante el desarrollo se identificó que las respuestas del servidor debían incluir el Token de la petición para que el cliente las aceptara.

## 2. Introducción

CoAP (RFC 7252) es un protocolo de transferencia web diseñado para dispositivos con recursos limitados. Conserva el modelo de la web, basado en recursos con URI y métodos `GET`, `POST`, `PUT` y `DELETE`, pero opera sobre UDP en lugar de TCP. Esto elimina el establecimiento de conexión y reduce la sobrecarga de cada mensaje, lo que lo hace adecuado para arquitecturas de Internet de las Cosas (IoT).

## 3. Objetivo

Comprender el funcionamiento del protocolo CoAP en arquitecturas IoT, configurar la placa ESP32-C6 como servidor CoAP en el entorno de Arduino IDE y realizar pruebas de comunicación mediante peticiones desde un cliente externo.

## 4. Marco teórico

### 4.1 CoAP frente a HTTP

| Aspecto | HTTP sobre TCP | CoAP sobre UDP |
| :--- | :--- | :--- |
| Conexión previa | Sí (handshake TCP) | No |
| Formato | Texto, cabeceras extensas | Binario, cabecera fija de 4 bytes |
| Fiabilidad | Garantizada por TCP | Opcional, mediante mensajes CON con ACK y reintento |
| Consumo | Mayor | Menor |

### 4.2 Estructura de un mensaje CoAP

Todo mensaje inicia con una cabecera fija de 4 bytes que contiene la versión, el tipo de mensaje, la longitud del Token, el código y el Message ID. Después pueden seguir el Token (que empareja petición y respuesta), las opciones (por ejemplo, Uri-Path) y el payload, separado por el marcador `0xFF`.

### 4.3 Tipos de mensaje

- **CON (Confirmable):** exige una confirmación ACK.
- **NON (No confirmable):** se envía sin esperar confirmación.
- **ACK:** confirma la recepción de un mensaje CON y puede incluir la respuesta.
- **RST:** indica que el mensaje no pudo procesarse.

### 4.4 Códigos de respuesta utilizados

- **2.05 Content:** respuesta correcta con contenido.
- **4.04 Not Found:** la ruta solicitada no existe en el servidor.

## 5. Materiales y herramientas

**Hardware**

- 1 tarjeta ESP32-C6 (ESP32C6 Dev Module)
- 1 cable USB de datos
- 1 LED, 1 resistencia de 220 Ω y cables Dupont
- 1 computadora y una red Wi-Fi de 2.4 GHz compartida entre la placa y la computadora

**Software**

- Arduino IDE 2.3.7 con soporte ESP32 de Espressif
- Librería CoAPSimple (hirotakaster)
- Python 3.13 con `aiocoap` 0.4.17
- Visual Studio Code
- Wireshark

## 6. Desarrollo

### 6.1 Configuración del entorno

1. Se instaló el soporte ESP32 en el Administrador de Tarjetas de Arduino IDE.
2. Se instaló la librería CoAPSimple de hirotakaster.
3. Se seleccionó la placa **ESP32C6 Dev Module** y el puerto serie correspondiente.
4. Se configuró el Monitor Serie a 115200 baudios.

### 6.2 Servidor CoAP

El servidor se implementó en `Servidor_CoAP_ESP32_LED.ino`. En `setup()` se conecta la placa a la red Wi-Fi, se registran los recursos con `coap.server(callback, "ruta")` y se inicia el servidor con `coap.start()` en el puerto UDP 5683. En `loop()` solo se invoca `coap.loop()`.

- `callback_sensor()`: genera una temperatura simulada entre 25.4 y 30.3 °C y la devuelve como texto plano.
- `callback_led()`: lee el payload; con `ON` enciende el LED y con `OFF` lo apaga, y responde con el estado.

Las credenciales Wi-Fi del código publicado se dejaron como `TU_SSID_WIFI` y `TU_PASSWORD_WIFI`. Deben sustituirse por las de la red propia antes de cargar el programa.

### 6.3 Conexión del LED

```
GPIO [COMPLETAR: pin utilizado] -> resistencia 220 ohm -> LED (+) -> LED (-) -> GND
```

El pin se define en el código con `#define LED_PIN`.

### 6.4 Cliente Python

El cliente `cliente_coap_completo.py` utiliza `aiocoap` para ejecutar las pruebas P1 a P4 en una sola corrida: cinco peticiones `GET /sensor`, un `POST /led` con `ON`, una pausa de 2 segundos, un `POST /led` con `OFF` y un `GET /temperatura`. Cada petición tiene un límite de 10 segundos.

Para reproducirlo:

```
pip install aiocoap
python cliente_coap_completo.py
```

Antes de ejecutarlo, se debe modificar `IP_ESP32` con la dirección que muestra el Monitor Serie de la placa.

## 7. Resultados

### 7.1 Conexión del servidor

El Monitor Serie mostró la conexión a la red Wi-Fi y la dirección IP asignada a la placa.

![Monitor Serie con la IP del servidor](evidencias/monitor_serie_ip.png)

### 7.2 Resumen de pruebas

| Prueba | Petición | Resultado esperado | Resultado obtenido | Evidencia |
| :--- | :--- | :--- | :--- | :--- |
| P1 | `GET /sensor` | 2.05 con `Temp: XX.X C` | 2.05 Content con `Temp: 27.20 C` | `evidencias/P1_sensor.png` |
| P2 | `POST /led` con `ON` | Respuesta `Estado: OK...` y payload en el Monitor Serie | 2.05 Content con `Estado: OK desde ESP32 CoAP Server - LED ON`; el LED enciende. Con `OFF` se apaga | `evidencias/P2_led.png` |
| P3 | `GET /temperatura` | 4.04 Not Found | `TimeoutError` en el cliente `aiocoap`; la placa responde 4.04 (ver sección 8.2) | `evidencias/P3_temperatura.png` |
| P4 | `GET /sensor` x5 | Cinco temperaturas entre 25.4 y 30.3 °C | Ver tabla 7.3 | `evidencias/P4_sensor_x5.png` |

![P1 y P4: lecturas de /sensor](evidencias/P1_sensor.png)

![P2: control del LED, cliente y Monitor Serie](evidencias/P2_led.png)

![P3: ruta inexistente](evidencias/P3_temperatura.png)

### 7.3 Prueba P4: cinco lecturas de `/sensor`

| Lectura | Temperatura |
| :---: | :--- |
| 1 | 30.00 °C |
| 2 | 29.20 °C |
| 3 | 26.40 °C |
| 4 | 30.10 °C |
| 5 | 27.20 °C |

Todos los valores se encuentran dentro del rango esperado (25.4 a 30.3 °C).

### 7.4 Observación sobre el formato de la temperatura

La guía indica una salida como `Temp: 27.1 C`, pero la temperatura se imprime con dos decimales (`27.20`). Esto se debe a que `String(float)` de Arduino utiliza dos decimales por defecto.

## 8. Análisis de resultados

### 8.1 Problema del Token en las respuestas

Con la versión corta de `coap.sendResponse(ip, port, messageid, "texto")`, la placa recibía la petición (el Monitor Serie mostraba `Petición CoAP recibida en /led`), pero el cliente `aiocoap` terminaba en `TimeoutError`. La hipótesis es que esa versión de la función no copia el Token de la petición, y `aiocoap` empareja petición y respuesta por Token. El problema se resolvió utilizando la versión larga de la función, que acepta el Token:

```cpp
coap.sendResponse(ip, port, packet.messageid,
                  msg, strlen(msg),
                  COAP_CONTENT, COAP_TEXT_PLAIN,
                  packet.token, packet.tokenlen);
```

Tras el cambio, `/sensor` y `/led` respondieron `2.05 Content`.

### 8.2 Análisis de la prueba P3

El cliente `aiocoap` terminó en `TimeoutError` al solicitar `/temperatura`. Para conocer la respuesta real de la placa se utilizó `prueba_404.py`, que envía la petición mediante un socket UDP y muestra los bytes recibidos. La respuesta fue:

```
60 84 12 35 c2 ff ff
```

| Bytes | Interpretación |
| :--- | :--- |
| `60` | Versión 1, tipo ACK, longitud de Token 0 |
| `84` | Código 4.04 (Not Found) |
| `12 35` | Message ID, idéntico al de la petición |
| `c2 ff ff` | Opción Content-Format, sin payload |

La petición se envió con Token `0xAB` y la respuesta llegó con longitud de Token 0. Por lo tanto, el servidor sí responde 4.04 como establece la práctica, pero esa respuesta no incluye el Token de la petición. Esto es consistente con que `aiocoap` la descarte y se produzca el timeout. Esta relación causal es la explicación más razonable, pero no se comprobó de forma directa.

### 8.3 Captura con Wireshark

Se capturó el tráfico de la interfaz Wi-Fi con el filtro `coap` durante la prueba P1.

![Captura de Wireshark de GET /sensor](evidencias/wireshark_P1.png)

| Campo | Valor observado |
| :--- | :--- |
| Tipo de mensaje de la petición | [COMPLETAR: CON o NON] |
| Message ID de la petición | [COMPLETAR] |
| Message ID de la respuesta | [COMPLETAR] |
| Tipo y código de la respuesta | [COMPLETAR: por ejemplo ACK, 2.05 Content] |
| Payload de la respuesta | [COMPLETAR: por ejemplo `Temp: 27.20 C`] |
| Tamaño del mensaje CoAP de la respuesta | [COMPLETAR: bytes de la capa CoAP] |

## 9. Preguntas de análisis

**1. ¿Por qué CoAP puede permitirse usar UDP, si UDP no garantiza la entrega? ¿Qué mecanismo compensa esa falta y en qué casos ni siquiera se usa?**

CoAP puede usar UDP porque incorpora su propio mecanismo de fiabilidad, opcional y mucho más ligero que el de TCP. Cuando un mensaje se envía como Confirmable (CON), el receptor debe responder con un ACK; si el ACK no llega, el emisor retransmite el mismo mensaje con el mismo Message ID, y el receptor usa ese identificador para detectar duplicados. Este mecanismo no se usa en los mensajes No confirmables (NON), que se envían y se olvidan, pensados para datos donde perder una lectura no es grave, como lecturas periódicas de un sensor. De esta forma se evita el handshake, el control de flujo y el estado de conexión de TCP, que son costosos para un dispositivo con poca memoria y batería.

**2. En tu captura de Wireshark, ¿la respuesta de `/sensor` fue un ACK independiente o viajó "a cuestas" en el mismo mensaje que el payload? ¿Cómo lo sabes?**

Por la forma en que está escrito el servidor, la respuesta viaja "a cuestas" (*piggyback*): cada callback llama una sola vez a `sendResponse`, que envía un único mensaje de tipo ACK con el código 2.05 y el payload juntos, sin un ACK vacío previo. Esto es coherente con lo observado en P3, donde la respuesta fue un solo datagrama `60 84 ...` (ACK con código 4.04). En Wireshark se reconoce porque la petición recibe una única respuesta de tipo ACK con código 2.05 y payload, y no aparece un ACK vacío (código 0.00) seguido de otro mensaje. [COMPLETAR: confirmar con la captura de la sección 8.3.]

**3. ¿Qué campo te permitió saber que la respuesta correspondía a tu petición y no a la de un compañero? Relaciónalo con la sección 3.2.**

Dos campos: el Message ID y el Token. El Message ID de la respuesta debe coincidir con el de la petición (en la prueba de P3 la petición usó `12 35` y la respuesta devolvió `12 35`), y el Token empareja la respuesta con la petición específica del cliente. Además, la respuesta se envía a la dirección IP y puerto del cliente que preguntó, así que la respuesta a un compañero no llega a otro equipo. En esta práctica se observó la importancia del Token: aunque el Message ID coincidía, `aiocoap` descartó la respuesta del 4.04 porque no traía el Token de su petición, y por eso ocurrió el timeout.

**4. Una petición HTTP/1.1 mínima ocupa varios cientos de bytes con sus cabeceras. Calcula cuántos bytes ocupa tu mensaje CoAP de respuesta en la captura y compara.**

La respuesta de `/sensor` se compone de la cabecera fija de 4 bytes, el Token (de 0 a 8 bytes), las opciones si las hay, el marcador `0xFF` (1 byte) y el payload. El payload `Temp: 27.20 C` ocupa 13 bytes, por lo que, sin Token ni opciones, el mensaje mínimo es de 4 + 1 + 13 = 18 bytes; con Token y alguna opción serían unos pocos bytes más. Medido en la captura, el mensaje CoAP ocupa [COMPLETAR: bytes de la capa CoAP en Wireshark]. Una respuesta HTTP/1.1 equivalente incluye línea de estado y varias cabeceras en texto (como `Content-Type` y `Content-Length`) que, según la guía de la práctica, suman varios cientos de bytes, además del handshake TCP previo. El mensaje CoAP es entonces de un orden de magnitud menor.

**5. Si este servidor controlara una bomba de riego con batería, ¿usarías mensajes CON o NON para reportar la humedad cada minuto? Justifica el costo en energía de tu elección.**

Para reportar la humedad cada minuto usaría mensajes NON. Cada minuto llega una lectura nueva, de modo que perder una no es grave. Con CON, el dispositivo tendría que esperar el ACK y, si no llega, retransmitir, lo que mantiene la radio encendida más tiempo y consume más batería, y la radio es normalmente la parte que más energía gasta. Con NON, la placa envía un datagrama y puede volver a dormir de inmediato. En cambio, para el comando que enciende o apaga la bomba usaría CON, porque ese mensaje no puede perderse: si la orden de apagado no llega, la bomba podría quedar encendida.

**6. ¿Qué riesgo de seguridad implica un servidor CoAP sin DTLS dentro de una red compartida, y quién podría explotarlo en el laboratorio?**

Sin DTLS no hay cifrado ni autenticación: el tráfico viaja en texto plano y el servidor responde a cualquier cliente que conozca su IP. Cualquier persona conectada a la misma red, por ejemplo otro equipo del laboratorio, podría leer los mensajes con Wireshark y ver las lecturas del sensor, o enviar un `POST /led` para encender y apagar el LED sin permiso. En esta práctica se comprobó la falta de autenticación: el script `prueba_404.py` envió una petición con un socket UDP simple y la placa la procesó sin pedir credenciales. También sería posible suplantar o alterar mensajes, por no existir verificación de integridad.

## 10. Reto opcional

**Reto 1: control de un LED físico mediante CoAP.** Se conectó un LED a la placa y se modificó `callback_led()` para comparar el payload: con `ON` se ejecuta `digitalWrite(LED_PIN, HIGH)` y con `OFF`, `digitalWrite(LED_PIN, LOW)`. La respuesta del servidor indica el estado aplicado. El pin se configura como salida en `setup()` con `pinMode`.

![LED encendido mediante POST /led](evidencias/led_encendido.jpg)

## 11. Conclusiones

- CoAP sobre UDP permite que un dispositivo pequeño ofrezca recursos con nombre (`/sensor`, `/led`) sin mantener una conexión: cada intercambio consistió en una petición y una respuesta independientes, y bastó una función por recurso en la ESP32-C6.
- La librería CoAPSimple facilita el registro de rutas mediante callbacks, pero la respuesta debe incluir el Token de la petición para que un cliente como `aiocoap` la acepte. Con la versión corta de `sendResponse` el cliente terminaba en timeout aunque la placa recibía la petición; con la versión larga el servidor respondió `2.05 Content` en ambos recursos.
- La prueba de una ruta inexistente mostró que el servidor responde `4.04 Not Found`, aunque esa respuesta de la librería no incluye el Token. Inspeccionar los bytes con un socket UDP permitió ver qué enviaba realmente la placa cuando el cliente solo mostraba un timeout.
- La fiabilidad en CoAP se decide por mensaje: CON cuando el dato no puede perderse y NON cuando la siguiente lectura llega pronto.
- Sin DTLS no hay cifrado ni autenticación, por lo que cualquier equipo de la misma red puede leer el tráfico o controlar el LED.
- El control de un LED físico mediante `POST /led` mostró cómo un recurso CoAP puede actuar sobre el mundo físico y cómo el cliente no necesita saber cómo está implementado.

## 12. Estructura del repositorio

| Archivo | Contenido |
| :--- | :--- |
| `README.md` | Este reporte |
| `Servidor_CoAP_ESP32_LED.ino` | Código del servidor para la ESP32-C6 |
| `cliente_coap_completo.py` | Cliente Python para las pruebas P1 a P4 y el control del LED |
| `prueba_404.py` | Script de socket UDP para inspeccionar la respuesta 4.04 |
| `evidencias/` | Capturas de pantalla, fotografías y video de las pruebas |

En Arduino IDE, el archivo `.ino` debe estar dentro de una carpeta con el mismo nombre.

## 13. Referencias

- Shelby, Z., Hartke, K. y Bormann, C. (2014). *The Constrained Application Protocol (CoAP)*, RFC 7252. IETF.
- Librería CoAPSimple, de hirotakaster, disponible en el Administrador de Librerías de Arduino IDE.
- Soto Ortiz, S. I. *Práctica de Laboratorio: Implementación de un Servidor CoAP en ESP32 con Arduino IDE*. Instituto Tecnológico Superior del Occidente del Estado de Hidalgo.