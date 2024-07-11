#include "firebase_config.h"
#include <WiFi.h>
#include <ArduinoJson.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

#define DATABASE_URL "https://asistente-lectura-esp32-s3-default-rtdb.europe-west1.firebasedatabase.app/"
#define API_KEY "AIzaSyDyI6HV9yF2pW5C4Ilrmu9VVGicfL9JrtE"
#define USER_EMAIL "amm00384@red.ujaen.es"
#define USER_PASSWORD "frbs_4"

//#define USER_EMAIL "ainhoamiromata@gmail.com"
//#define USER_PASSWORD "noafirebase"

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
    Serial.print("Conectando a la Red Wi-Fi ");  // Imprime un mensaje de conexión a Wifi
    while (WiFi.status() != WL_CONNECTED) {  // Bucle que espera a que la conexión sea exitosa
        Serial.print(".");
        delay(300);
    }
    Serial.println("Conectado con la IP: ");  // Imprime un mensaje de conexión exitosa y la IP asignada
    Serial.println(WiFi.localIP());
    Serial.println();

    // Imprime la versión del cliente Firebase
    Serial.printf("Cliente Firebase v%s\n\n", FIREBASE_CLIENT_VERSION);

    // Asigna la clave API a la configuración
    config.api_key = API_KEY;

    // Asignar las credenciales de inicio de sesión del usuario
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    // Asigna la URL de la base de datos a la configuración
    config.database_url = DATABASE_URL;

    // Registrarse
    if (Firebase.signUp(&config, &auth, USER_EMAIL, USER_PASSWORD)) {  // Intenta registrarse con Firebase
        Serial.println("REGISTRO CORRECTO");  // Imprime mensaje de registro exitoso
        signupOK = true;
    } else {
        Serial.printf("%s\n", config.signer.signupError.message.c_str());  // Imprime mensaje de error si falla el registro
    }

    //Asignar la función de devolución de llamada para la tarea de generación de token de larga duración
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    Firebase.begin(&config, &auth);  // Inicia la comunicación con Firebase
    Firebase.reconnectWiFi(true);  // Habilita la reconexión automática a Wifi
}



// Función para obtener los datos de un libro
DynamicJsonDocument get_book_data(const std::string & key) {
    // Define el path inicial del JSON en la base de datos Firebase
    String path = "/libros";
    // Si se proporciona una clave, se añade al path para apuntar a un libro específico
    if (!key.empty()) {
        path += "/" + String(key.c_str());
    }

    // Intenta obtener los datos del libro de la base de datos Firebase
    if (Firebase.RTDB.get(&fbdo, path.c_str())) {
        // Comprueba si los datos obtenidos son de tipo JSON
        if (fbdo.dataType() == "json") {
            // Obtiene un puntero al objeto JSON
            FirebaseJson *json = fbdo.jsonObjectPtr();
            // Crea una cadena de texto para almacenar el JSON
            String jsonString;
            // Convierte el objeto JSON a una cadena de texto
            json->toString(jsonString);

            // Crea un documento JSON para almacenar los datos
            DynamicJsonDocument doc(1024);
            // Deserializa la cadena de texto JSON en el documento JSON
            deserializeJson(doc, jsonString);

            //size_t requiredSize = measureJson(doc); // Calcular el tamaño requerido para el JSON
            //Serial.print("Tamaño requerido para el JSON: ");
            //Serial.println(requiredSize);

            // Devuelve el documento JSON con los datos del libro
            return doc;
        }
    }
    // Si ocurre algún error, devuelve un documento JSON vacío
    DynamicJsonDocument doc(1024);
    return doc;
}


// Función para actualizar la página actual de un libro
void update_current_page(const std::string & key, int page) {
    // Construye la ruta al campo "pagina_actual" del libro en la base de datos Firebase
    // La clave del libro se pasa como argumento a la función
    String path = "/libros/" + String(key.c_str()) + "/pagina_actual";
    // Actualiza el valor de "pagina_actual" en la base de datos con el número de página proporcionado
    Firebase.RTDB.setInt(&fbdo, path.c_str(), page);

    // Construye la ruta al campo "ultima_modificacion" del libro en la base de datos Firebase
    path = "/libros/" + String(key.c_str()) + "/ultima_modificacion";
    // Actualiza el valor de "ultima_modificacion" en la base de datos con la marca de tiempo actual
    Firebase.RTDB.setTimestamp(&fbdo, path.c_str());
}