//
// Created by ainho on 26/05/2024.
//

#ifndef INTERFAZ_GRAFICA_MENU_FIREBASE_CONFIG_H
#define INTERFAZ_GRAFICA_MENU_FIREBASE_CONFIG_H

#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>
#include <string>

// Declaración de las variables de Firebase
extern FirebaseData fbdo;
extern FirebaseAuth auth;
extern FirebaseConfig config;

extern const char* DATABASE_URL;
extern const char* API_KEY;
extern const char* USER_EMAIL;
extern const char* USER_PASSWORD;
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// Declaración de otras variables relacionadas con Firebase
extern unsigned long sendDataPrevMillis;
extern unsigned long count;
extern bool signupOK;
extern bool libraryLoaded;
extern String path;

// Declaración de la función para configurar Firebase
void setup_firebase();
DynamicJsonDocument get_book_data(const std::string& key = "");
void update_current_page(const std::string& key, int page);

#endif //INTERFAZ_GRAFICA_MENU_FIREBASE_CONFIG_H
