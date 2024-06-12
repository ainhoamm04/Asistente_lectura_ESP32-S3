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
//#define WIFI_SSID "vodafoneAAP8ZC"
//#define WIFI_PASSWORD "mfqaX6ZXHzqzyYxe"

#define WIFI_SSID "Xiaomi_Ainhoa"
#define WIFI_PASSWORD "cobw4192"

//#define WIFI_SSID "telema2"
//#define WIFI_PASSWORD "teleco2015"

//#define WIFI_SSID "iPhone 12 LR"
//#define WIFI_PASSWORD "teleco2015"

// Definición de las variables de Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Definición de otras variables relacionadas con Firebase
unsigned long count = 0;
bool signupOK = false;
String path = "/libros";

// Definición de la función para configurar Firebase
void setup_firebase() {
    // Inicializar la conexión Wi-Fi
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

    // Asignar la clave de la API (requerido)
    config.api_key = API_KEY;

    // Asignar las credenciales de inicio de sesión del usuario
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    // Asignar la URL de la base de datos RTDB (requerido)
    config.database_url = DATABASE_URL;

    // Registrarse
    if (Firebase.signUp(&config, &auth, USER_EMAIL, USER_PASSWORD)){
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


// Función para obtener los datos de un libro
DynamicJsonDocument get_book_data(const std::string & key) {
    // Obtener el path
    String path = "/libros";
    if (!key.empty()) {
        path += "/" + String(key.c_str());
    }

    // Obtener los datos del libro
    if (Firebase.RTDB.get(&fbdo, path.c_str())) {
        if (fbdo.dataType() == "json") {
            FirebaseJson *json = fbdo.jsonObjectPtr(); // Obtener el puntero al objeto JSON
            String jsonString; // Crear un string para almacenar el JSON
            json->toString(jsonString); // Convertir el JSON a string

            //Serial.println("JSON string loaded from Firebase:");
            //Serial.println(jsonString);

            DynamicJsonDocument doc(1024); // Crear un documento JSON para almacenar los datos
            deserializeJson(doc, jsonString); // Deserializar el JSON en el documento para poder acceder a los datos

            //size_t requiredSize = measureJson(doc); // Calcular el tamaño requerido para el JSON
            //Serial.print("Tamaño requerido para el JSON: ");
            //Serial.println(requiredSize);

            return doc;
        }
    }
    DynamicJsonDocument doc(1024); // Crear un documento JSON vacío para que la función siga ejecutandose en caso de error
    return doc;
}


// Función para actualizar la página actual de un libro
void update_current_page(const std::string & key, int page) {
    // Actualizar la página actual
    String path = "/libros/" + String(key.c_str()) + "/pagina_actual";
    Firebase.RTDB.setInt(&fbdo, path.c_str(), page);

    // Actualizar la última modificación (timestamp
    path = "/libros/" + String(key.c_str()) + "/ultima_modificacion";
    Firebase.RTDB.setTimestamp(&fbdo, path.c_str());
}