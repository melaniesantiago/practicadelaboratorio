# cliente_coap_completo.py - Cliente CoAP con aiocoap
# Hace las pruebas P1 a P4 y enciende/apaga el LED.
# Uso: python cliente_coap_completo.py
import asyncio
from aiocoap import Context, Message, GET, POST

# Cambia esta IP por la que muestre el Monitor Serie de tu ESP32.
IP_ESP32 = "192.168.0.111"


async def pedir(contexto, metodo, ruta, payload=b""):
    msg = Message(code=metodo, uri=f"coap://{IP_ESP32}/{ruta}", payload=payload)
    try:
        resp = await asyncio.wait_for(contexto.request(msg).response, timeout=10)
        print(f"/{ruta} -> {resp.code} | {resp.payload.decode()}")
    except Exception as e:
        print(f"/{ruta} -> fallo: {type(e).__name__}: {e}")


async def main():
    contexto = await Context.create_client_context()

    print("--- P1 y P4: GET /sensor (5 veces) ---")
    for _ in range(5):
        await pedir(contexto, GET, "sensor")

    print("--- P2: POST /led (encender, esperar 2 s, apagar) ---")
    await pedir(contexto, POST, "led", b"ON")
    await asyncio.sleep(2)
    await pedir(contexto, POST, "led", b"OFF")

    print("--- P3: ruta inexistente (se espera 4.04 Not Found) ---")
    await pedir(contexto, GET, "temperatura")

    await contexto.shutdown()


asyncio.run(main())