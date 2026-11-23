#pragma once

// Copia este archivo a config.h y completa los datos de tu reloj.
// Mantén config.h en tu equipo. Contiene las credenciales del reloj y de la red.
constexpr char WIFI_SSID[] = "NOMBRE_DE_TU_RED";
constexpr char WIFI_PASSWORD[] = "CLAVE_DE_TU_RED";
constexpr char DEVICE_ID[] = "ID_DE_TU_RELOJ";
constexpr char DEVICE_TOKEN[] = "TOKEN_DE_TU_RELOJ";
constexpr char SERVER_URL[] = "https://pulso-ruta-production.up.railway.app/api/lecturas";

// Ajusta los GPIO al cableado de tu montaje.
constexpr int SDA_PIN = 4;
constexpr int SCL_PIN = 5;
constexpr int BUZZER_PIN = 6;
