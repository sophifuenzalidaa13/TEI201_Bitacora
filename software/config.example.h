#pragma once

constexpr char WIFI_SSID[] = "TU_WIFI";
constexpr char WIFI_PASSWORD[] = "TU_CLAVE_WIFI";
constexpr char SERVER_URL[] = "https://TU-APP.up.railway.app/api/lecturas";
constexpr char DEVICE_ID[] = "reloj-01";
constexpr char DEVICE_TOKEN[] = "PEGA_EL_TOKEN_DEL_PANEL";

// Pines del cardiaco.ino adjunto. Cambia SDA/SCL solo si tu montaje usa otros.
constexpr int SDA_PIN = 4;
constexpr int SCL_PIN = 5;
constexpr int BUZZER_PIN = 6;
