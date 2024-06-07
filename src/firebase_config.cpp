//
// Created by ainho on 26/05/2024.
//

#include "firebase_config.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <lvgl.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>
// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

#define DATABASE_URL "https://asistente-lectura-esp32-s3-default-rtdb.europe-west1.firebasedatabase.app/"
#define API_KEY "AIzaSyDyI6HV9yF2pW5C4Ilrmu9VVGicfL9JrtE"
#define USER_EMAIL "amm00384@red.ujaen.es"
#define USER_PASSWORD "frbs_4"
#define WIFI_SSID "vodafoneAAP8ZC"
#define WIFI_PASSWORD "mfqaX6ZXHzqzyYxe"

//#define WIFI_SSID "Xiaomi_Ainhoa"
//#define WIFI_PASSWORD "cobw4192"

//#define WIFI_SSID "telema2"
//#define WIFI_PASSWORD "teleco2015"

//#define WIFI_SSID "iPhone 12 LR"
//#define WIFI_PASSWORD "teleco2015"

// Definición de las variables de Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Definición de otras variables relacionadas con Firebase
unsigned long sendDataPrevMillis = 0;
unsigned long count = 0;
bool signupOK = false;
String path = "/libros";

// Definición de la función para configurar Firebase
void setup_firebase() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(300);
    }
    Serial.println("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    //Assign the api key (required)
    config.api_key = API_KEY;
    //Assign the user sign in credentials
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    //Assign the RTDB URL (required)
    config.database_url = DATABASE_URL;

    //Sign up
    if (Firebase.signUp(&config, &auth, "", "")){
        Serial.println("SIGN UP CORRECT");
        signupOK = true;
    }
    else{
        Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }

    //Assign the callback function for the long running token generation task
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}



DynamicJsonDocument get_book_data(const std::string& key) {
    String path = "/libros";
    if (!key.empty()) {
        path += "/" + String(key.c_str());
    }

    if (Firebase.RTDB.get(&fbdo, path.c_str())) {
        if (fbdo.dataType() == "json") {
            FirebaseJson *json = fbdo.jsonObjectPtr();
            String jsonString;
            json->toString(jsonString);

            // Print the JSON string to verify it's loading correctly
            //Serial.println("JSON string loaded from Firebase:");
            //Serial.println(jsonString);

            DynamicJsonDocument doc(1024);
            deserializeJson(doc, jsonString);

            size_t requiredSize = measureJson(doc);
            Serial.print("Tamaño requerido para el JSON: ");
            Serial.println(requiredSize);

            return doc;
        }
    }
    DynamicJsonDocument doc(1024);
    return doc;
}


void update_current_page(const std::string& key, int page) {
    // Actualizar la página actual
    String path = "/libros/" + String(key.c_str()) + "/pagina_actual";
    Firebase.RTDB.setInt(&fbdo, path.c_str(), page);

    // Actualizar la última modificación
    path = "/libros/" + String(key.c_str()) + "/ultima_modificacion";
    Firebase.RTDB.setTimestamp(&fbdo, path.c_str());
}