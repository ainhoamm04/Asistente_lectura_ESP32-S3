#include <Arduino.h>
#include <ArduinoNvs.h>
#include "display.h"
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "FT6336U.h"
#include "sd_card.h"
#include "camera.h"
#include "camera_ui.h"

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
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

#include <ArduinoJson.h> // Include the ArduinoJson library
#include <FirebaseJson.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>
// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>


//-------------------------DECLARACIÓN DE FUNCIONES------------------------------------
// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
unsigned long count = 0;
bool signupOK = false;
String path = "/libros";

Display screen;

lv_obj_t *scr_principal;

void tab_function(void);
void back_to_main_menu(lv_event_t * e);

// Enumeración para los diferentes estilos de títulos
typedef enum {
    TITLE_STYLE_PURPLE,
    TITLE_STYLE_BLUE,
    TITLE_STYLE_ORANGE,
    TITLE_STYLE_GREEN
} title_style_t;
void general_title(lv_obj_t * parent, const char * titulo, title_style_t style);

// Enumeración para los diferentes estilos de botones
typedef enum {
    BUTTON_STYLE_PURPLE,
    BUTTON_STYLE_BLUE,
    BUTTON_STYLE_BLUE_LARGE,
    BUTTON_STYLE_ORANGE,
    BUTTON_STYLE_GREEN
} button_style_t;
void create_button(lv_obj_t * parent, lv_obj_t * label, button_style_t style, lv_event_cb_t event_cb, lv_coord_t pos_x, lv_coord_t pos_y);

static void tab1_content(lv_obj_t * parent);
void create_second_screen_tab1(lv_obj_t *padre);
void go_to_screen2_tab1(lv_event_t * e);

void tab2_content(lv_obj_t * parent);
void book_button_event_handler(lv_event_t * e);
void create_second_screen_tab2(lv_obj_t *padre, const String& bookTitle, const String& author, int totalPage, int currentPage);
void go_to_screen2_tab2(lv_event_t * e, const char* title);

void tab3_content(lv_obj_t * parent);
void create_second_screen_tab3(lv_obj_t *padre);
static void go_to_screen2_tab3(lv_event_t * e);
void initialize_and_load_camera();

static void tab4_content(lv_obj_t * parent);
static void draw_event_cb(lv_event_t * e);
int reto_pag_mes = 300;


void prueba();

//-------------------------------SETUP------------------------------------
void setup() {
    Serial.begin(115200);



    //--------------------Configuración y conexión Firebase---------------------
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

    /* Assign the api key (required) */
    config.api_key = API_KEY;
    /* Assign the user sign in credentials */
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    /* Assign the RTDB URL (required) */
    config.database_url = DATABASE_URL;

    /* Sign up */
    if (Firebase.signUp(&config, &auth, "", "")){
        Serial.println("SIGN UP CORRECT");
        signupOK = true;
    }
    else{
        Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    //prueba();

    NVS.begin();

    sdcard_init();
    camera_init();
    screen.init();

    tab_function();
}

bool libraryLoaded = false;

//-------------------------------LOOP------------------------------------
void loop() {
    screen.routine(); /* let the GUI do its work */
    delay(5);

    //Firebase.ready();

    /*
    if (!libraryLoaded && Firebase.ready()) {
        String title;
        libraryLoaded = true;
        Serial.println("\n\nHOLAAAAAA\n\n");

        if (Firebase.RTDB.getString(&fbdo, "/libros/9788416588435/titulo")) {
            Serial.println("HAS ENTRADO EN EL PATH DE LA TAB2");

            if (fbdo.dataType() == "string") {
                title = fbdo.stringData();
                Serial.println(title);
                Serial.println("DEBERIA DECIRTE EL TITULO DEL LIBRO");
            } else {
                Serial.println(fbdo.errorReason());
                Serial.println(fbdo.dataType());
                Serial.println("HA HABIDO UN ERROR, NO TE VOY A MOSTRAR EL LIBRO");
            }
        }
    }*/
}




void prueba(){
    FirebaseJson json;       // or constructor with contents e.g. FirebaseJson json("{\"a\":true}");
    FirebaseJsonArray arr;   // or constructor with contents e.g. FirebaseJsonArray arr("[1,2,true,\"test\"]");
    FirebaseJsonData result; // object that keeps the deserializing result

    // To set content
    //json.setJsonData("{\"libros\":true}");

    //arr.setJsonArrayData("[1,2,3]");

    // To add data to json

    // Set data to json


// Set data to json
;

// Set data to json
json.set("a", "que tal"); // Set "que tal" at path "a"
json.set("b", 123); // Set 123 at path "b"
json.set("c", "c"); // Set "c" at path "c"

Firebase.RTDB.set(&fbdo, "/a/b/keyb", "new value b");
Firebase.RTDB.set(&fbdo, "/a/d/keyd", "new value d");

String keydValue;
if (Firebase.RTDB.getString(&fbdo, "/a/d/keyd")) {
    if (fbdo.dataType() == "string") {
        keydValue = fbdo.stringData();
        Serial.println(keydValue);
    } else {
        Serial.println("Error, no es un string");
    }
} else {
    Serial.println("Error al obtener datos de Firebase");
    Serial.println(fbdo.errorReason());
}

    // To add value to array
    arr.add("hello").add("test").add(99); // or arr.add("hello", "test", 99);

    // To add json into array
    FirebaseJson json2("{\"d\":888,\"e\":false}");
    arr.add(json2);

    // To set the value at index
    arr.set("[0]", 555); // or arr.set(0, 555);

    // To set the value at specific path
    arr.set("/[8]/i", 111);
    arr.set("/[8]/j", 222);
    arr.set("/[8]/k", "hi");

    // To add/set array into json
    json.set("x/y/z", arr);

    // To serialize json to serial
    json.toString(Serial, true /* prettify option */);

    // To serialize array to string
    String str;
    arr.toString(str, true /* prettify option */);

    Serial.println("\n---------");
    Serial.println(str);

    // To remove value from array at index or path
    arr.remove("[6]/d" /* path */);
    arr.remove(7 /* index */);
    Serial.println("\n---------");
    Serial.println(arr.raw()); // print raw string

    // To remove value from json
    json.remove("x/y/z/[6]");
    Serial.println("\n---------");
    Serial.println(json.raw()); // print raw string

    // To get the value from json (deserializing)
    json.get(result /* FirebaseJsonData */, "a/b/c" /* key or path */);

    // To check the deserialized result and get its type and value
    if (result.success)
    {
        Serial.println("\n---------");
        if (result.type == "string") /* or result.typeNum == FirebaseJson::JSON_STRING */
            Serial.println(result.to<String>().c_str());
        else if (result.type == "int") /* or result.typeNum == FirebaseJson::JSON_INT */
            Serial.println(result.to<int>());
        else if (result.type == "float") /* or result.typeNum == FirebaseJson::JSON_FLOAT */
            Serial.println(result.to<float>());
        else if (result.type == "double") /* or result.typeNum == FirebaseJson::JSON_DOUBLE */
            Serial.println(result.to<double>());
        else if (result.type == "bool") /* or result.typeNum == FirebaseJson::JSON_BOOL */
            Serial.println(result.to<bool>());
        else if (result.type == "object") /* or result.typeNum == FirebaseJson::JSON_OBJECT */
            Serial.println(result.to<String>().c_str());
        else if (result.type == "array") /* or result.typeNum == FirebaseJson::JSON_ARRAY */
            Serial.println(result.to<String>().c_str());
        else if (result.type == "null") /* or result.typeNum == FirebaseJson::JSON_NULL */
            Serial.println(result.to<String>().c_str());
    }

    // To get the json object from deserializing result
    json.get(result /* FirebaseJsonData */, "x/y/z/[7]" /* key or path */);
    if (result.success)
    {

        if (result.type == "object") /* or result.typeNum == FirebaseJson::JSON_OBJECT */
        {
            // Use result.get or result.getJSON instead of result.to
            FirebaseJson json3;
            result.get<FirebaseJson /* type e.g. FirebaseJson or FirebaseJsonArray */>(json3 /* object that used to store value */);
            // or result.getJSON(json3);
            Serial.println("\n---------");
            json3.toString(Serial, true); // serialize contents to serial

            // To iterate all values in Json object
            size_t count = json3.iteratorBegin();
            Serial.println("\n---------");
            for (size_t i = 0; i < count; i++)
            {
                FirebaseJson::IteratorValue value = json3.valueAt(i);
                Serial.printf("Name: %s, Value: %s, Type: %s\n", value.key.c_str(), value.value.c_str(), value.type == FirebaseJson::JSON_OBJECT ? "object" : "array");
            }

            Serial.println();
            json3.iteratorEnd(); // required for free the used memory in iteration (node data collection)
        }
    }

    // To get the json array object from deserializing result
    json.get(result /* FirebaseJsonData */, "x/y/z" /* key or path */);
    if (result.success)
    {

        if (result.type == "array") /* or result.typeNum == FirebaseJson::JSON_ARRAY */
        {
            // Use result.get or result.getJSON instead of result.to
            FirebaseJsonArray arr2;
            result.get<FirebaseJsonArray /* type e.g. FirebaseJson or FirebaseJsonArray */>(arr2 /* object that used to store value */);
            // or result.getArray(arr2);
            Serial.println("\n---------");
            arr2.toString(Serial, true); // serialize contents to serial

            // To iterate all values in Json array object
            Serial.println("\n---------");
            FirebaseJsonData result2;
            for (size_t i = 0; i < arr2.size(); i++)
            {
                arr2.get(result2, i);
                if (result2.type == "string" /* result2.typeNum == FirebaseJson::JSON_STRING */)
                    Serial.printf("Array index %d, String Val: %s\n", i, result2.to<String>().c_str());
                else if (result2.type == "int" /* result2.typeNum == FirebaseJson::JSON_INT */)
                    Serial.printf("Array index %d, Int Val: %d\n", i, result2.to<int>());
                else if (result2.type == "float" /* result2.typeNum == FirebaseJson::JSON_FLOAT */)
                    Serial.printf("Array index %d, Float Val: %f\n", i, result2.to<float>());
                else if (result2.type == "double" /* result2.typeNum == FirebaseJson::JSON_DOUBLE */)
                    Serial.printf("Array index %d, Double Val: %f\n", i, result2.to<double>());
                else if (result2.type == "bool" /* result2.typeNum == FirebaseJson::JSON_BOOL */)
                    Serial.printf("Array index %d, Bool Val: %d\n", i, result2.to<bool>());
                else if (result2.type == "object" /* result2.typeNum == FirebaseJson::JSON_OBJECT */)
                    Serial.printf("Array index %d, Object Val: %s\n", i, result2.to<String>().c_str());
                else if (result2.type == "array" /* result2.typeNum == FirebaseJson::JSON_ARRAY */)
                    Serial.printf("Array index %d, Array Val: %s\n", i, result2.to<String>().c_str());
                else if (result2.type == "null" /* result2.typeNum == FirebaseJson::JSON_NULL */)
                    Serial.printf("Array index %d, Null Val: %s\n", i, result2.to<String>().c_str());
            }

            // Or use the same method as iterate the object
            /*
            size_t count = arr2.iteratorBegin();
            Serial.println("\n---------");
            for (size_t i = 0; i < count; i++)
            {
                FirebaseJson::IteratorValue value = arr2.valueAt(i);
                Serial.printf("Name: %s, Value: %s, Type: %s\n", value.key.c_str(), value.value.c_str(), value.type == FirebaseJson::JSON_OBJECT ? "object" : "array");
            }
            */
        }
    }
}



/*
void prueba(){
    if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
    {
        sendDataPrevMillis = millis();

        FirebaseJson json;
        json.setDoubleDigits(3);
        json.add("value", count);

        Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, "/test/json", &json) ? "ok" : fbdo.errorReason().c_str());

        Serial.printf("Get json... %s\n", Firebase.RTDB.getJSON(&fbdo, "/test/json") ? fbdo.to<FirebaseJson>().raw() : fbdo.errorReason().c_str());

        FirebaseJson jVal;
        Serial.printf("Get json ref... %s\n", Firebase.RTDB.getJSON(&fbdo, "/test/json", &jVal) ? jVal.raw() : fbdo.errorReason().c_str());

        FirebaseJsonArray arr;
        arr.setFloatDigits(2);
        arr.setDoubleDigits(4);
        arr.add("a", "b", "c", true, 45, (float)6.1432, 123.45692789);

        Serial.printf("Set array... %s\n", Firebase.RTDB.setArray(&fbdo, "/test/array", &arr) ? "ok" : fbdo.errorReason().c_str());

        Serial.printf("Get array... %s\n", Firebase.RTDB.getArray(&fbdo, "/test/array") ? fbdo.to<FirebaseJsonArray>().raw() : fbdo.errorReason().c_str());

        Serial.printf("Push json... %s\n", Firebase.RTDB.pushJSON(&fbdo, "/test/push", &json) ? "ok" : fbdo.errorReason().c_str());

        json.set("value", count + 0.29745);
        Serial.printf("Update json... %s\n\n", Firebase.RTDB.updateNode(&fbdo, "/test/push/" + fbdo.pushName(), &json) ? "ok" : fbdo.errorReason().c_str());

        count++;
    }
}*/


//-------------------------DEFINICIÓN DE FUNCIONES------------------------------------

void tab_function(void)
{
    //Create a Tab view object
    lv_obj_t * tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 35);

    //Add 4 tabs (the tabs are page (lv_page) and can be scrolled
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "\xF3\xB0\x8B\x9C");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "\xF3\xB1\x89\x9F");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "\xF3\xB0\x81\xB2");
    lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "\xF3\xB0\x84\xA8");

    lv_obj_t * tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_text_font(tab_btns, &bigger_symbols, 0);

    lv_obj_set_style_radius(tab1, 4,0);
    lv_obj_set_style_bg_opa(tab1, LV_OPA_30, 0);
    lv_obj_set_style_bg_color(tab1, lv_palette_lighten(LV_PALETTE_PURPLE, 1), 0);
    lv_obj_set_style_border_width(tab1, 4, 0);
    lv_obj_set_style_border_color(tab1, lv_color_make(155, 4, 207), 0);

    lv_obj_set_style_radius(tab2, 4,0);
    lv_obj_set_style_bg_opa(tab2, LV_OPA_30, 0);
    lv_obj_set_style_bg_color(tab2, lv_color_hex(0x6CE0FF), 0);
    lv_obj_set_style_border_width(tab2, 4, 0);
    lv_obj_set_style_border_color(tab2, lv_color_make(10, 154, 254), 0);

    lv_obj_set_style_radius(tab3, 4,0);
    lv_obj_set_style_bg_opa(tab3, LV_OPA_60, 0);
    lv_obj_set_style_bg_color(tab3, lv_palette_lighten(LV_PALETTE_ORANGE, 1), 0);
    lv_obj_set_style_border_width(tab3, 4, 0);
    lv_obj_set_style_border_color(tab3, lv_color_make(255, 104, 0), 0);

    lv_obj_set_style_radius(tab4, 4,0);
    lv_obj_set_style_bg_opa(tab4, LV_OPA_30, 0);
    lv_obj_set_style_bg_color(tab4, lv_palette_lighten(LV_PALETTE_LIGHT_GREEN, 1), 0);
    lv_obj_set_style_border_width(tab4, 4, 0);
    lv_obj_set_style_border_color(tab4, lv_color_make(30, 217, 0), 0);

    tab1_content(tab1);
    tab2_content(tab2);
    tab3_content(tab3);
    tab4_content(tab4);
}


//---------------------------------Funciones generales------------------------------------------
void back_to_main_menu(lv_event_t * e) {
    lv_obj_t * current_screen = lv_obj_get_parent(lv_event_get_target(e)); // Obtener la pantalla actual (secundaria)
    lv_scr_load(scr_principal); // Obtener la pantalla principal (donde están las tabs)
    lv_obj_del(current_screen); // Eliminar la pantalla secundaria
}


// Función para crear un título con estilo de color específico
void general_title(lv_obj_t * parent, const char * titulo, title_style_t style) {
    static lv_style_t style_title; // Estilo para el título
    static lv_style_t style_title_purple, style_title_blue, style_title_orange, style_title_green;

    // Inicializa el estilo si no se ha hecho antes
    static bool styles_initialized = false;
    if (!styles_initialized) {
        // Estilo morado
        lv_style_init(&style_title_purple);
        lv_style_set_border_color(&style_title_purple, lv_color_hex(0xA800E7)); // Borde morado
        lv_style_set_text_color(&style_title_purple, lv_color_hex(0xA800E7)); // Texto morado

        // Estilo azul
        lv_style_init(&style_title_blue);
        lv_style_set_border_color(&style_title_blue, lv_color_hex(0x00AED9)); // Borde azul
        lv_style_set_text_color(&style_title_blue, lv_color_hex(0x00AED9)); // Texto azul

        // Estilo naranja
        lv_style_init(&style_title_orange);
        lv_style_set_border_color(&style_title_orange, lv_color_hex(0xFC8900)); // Borde naranja
        lv_style_set_text_color(&style_title_orange, lv_color_hex(0xFC8900 )); // Texto naranja

        // Estilo verde
        lv_style_init(&style_title_green);
        lv_style_set_border_color(&style_title_green, lv_color_hex(0x08BB00)); // Borde verde
        lv_style_set_text_color(&style_title_green, lv_color_hex(0x08BB00)); // Texto verde

        // Configuraciones comunes del estilo
        lv_style_set_radius(&style_title, 5);
        lv_style_set_bg_opa(&style_title, LV_OPA_COVER);
        lv_style_set_border_width(&style_title, 2);
        lv_style_set_pad_all(&style_title, 5);
        lv_style_set_text_letter_space(&style_title, 2);
        lv_style_set_text_line_space(&style_title, 10);

        styles_initialized = true;
    }

    // Crea una etiqueta y aplica el estilo
    lv_obj_t * label = lv_label_create(parent); // Crea una etiqueta en la pantalla activa
    lv_label_set_text(label, titulo); // Establece el texto de la etiqueta
    lv_obj_remove_style_all(label); // Elimina estilos previos

    lv_obj_set_style_text_font(label, &ubuntu_bold_16, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 5);

    // Configura el estilo del título basado en el estilo seleccionado
    switch (style) {
        case TITLE_STYLE_PURPLE:
            lv_obj_add_style(label, &style_title_purple, 0);
            break;
        case TITLE_STYLE_BLUE:
            lv_obj_add_style(label, &style_title_blue, 0);
            break;
        case TITLE_STYLE_ORANGE:
            lv_obj_add_style(label, &style_title_orange, 0);
            break;
        case TITLE_STYLE_GREEN:
            lv_obj_add_style(label, &style_title_green, 0);
            break;
    }

    lv_obj_add_style(label, &style_title, 0); // Aplica el nuevo estilo
}


// Función para crear un botón con estilo de color específico
void create_button(lv_obj_t * parent, lv_obj_t * label, button_style_t style, lv_event_cb_t event_cb, lv_coord_t pos_x, lv_coord_t pos_y) {
    // Estilos para los botones
    static lv_style_t style_purple, style_blue, style_orange, style_green;
    static lv_style_t style_btn_pressed;
    static lv_style_t style_btn;
    static lv_style_t style_btn_large;

    // Inicializa los estilos si no se han inicializado antes
    static bool styles_initialized = false;
    if (!styles_initialized) {
        // Estilo morado
        lv_style_init(&style_purple);
        lv_style_set_bg_color(&style_purple, lv_color_hex(0xC446FF)); // Fondo morado
        lv_style_set_border_color(&style_purple, lv_color_hex(0x620090)); // Borde morado

        // Estilo azul
        lv_style_init(&style_blue);
        lv_style_set_bg_color(&style_blue, lv_color_hex(0x15BFE9)); // Fondo azul
        lv_style_set_border_color(&style_blue, lv_color_hex(0x007C9A)); // Borde azul

        // Estilo naranja
        lv_style_init(&style_orange);
        lv_style_set_bg_color(&style_orange, lv_color_hex(0xFFA82A )); // Fondo naranja
        lv_style_set_border_color(&style_orange, lv_color_hex(0xD17C00)); // Borde naranja

        // Estilo verde
        lv_style_init(&style_green);
        lv_style_set_bg_color(&style_green, lv_color_hex(0x1AC412 )); // Fondo verde
        lv_style_set_border_color(&style_green, lv_color_hex(0x046100)); // Borde verde

        // Estilo para el botón presionado
        lv_style_init(&style_btn_pressed);
        lv_style_set_translate_y(&style_btn_pressed, 5);

        lv_style_init(&style_btn);
        lv_style_set_radius(&style_btn, 10);
        lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
        lv_style_set_border_width(&style_btn, 2);
        lv_style_set_border_opa(&style_btn, LV_OPA_50);

        styles_initialized = true;
    }

    // Crea el botón y aplica el estilo correspondiente
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_remove_style_all(btn); // Elimina estilos previos

    switch (style) {
        case BUTTON_STYLE_PURPLE:
            lv_obj_add_style(btn, &style_purple, 0);
            lv_obj_set_size(btn, 50, 50);
            break;
        case BUTTON_STYLE_BLUE:
            lv_obj_add_style(btn, &style_blue, 0);
            lv_obj_set_size(btn, 50, 50);
            break;
        case BUTTON_STYLE_BLUE_LARGE:
            lv_obj_add_style(btn, &style_blue, 0);
            lv_obj_set_size(btn, 210, 40);
            break;
        case BUTTON_STYLE_ORANGE:
            lv_obj_add_style(btn, &style_orange, 0);
            lv_obj_set_size(btn, 50, 50);
            break;
        case BUTTON_STYLE_GREEN:
            lv_obj_add_style(btn, &style_green, 0);
            lv_obj_set_size(btn, 50, 50);
            break;
    }

    lv_obj_add_style(btn, &style_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_style(btn, &style_btn, 0); // Aplica el nuevo estilo

    // Aplica la etiqueta al botón
    lv_obj_set_parent(label, btn); // Establece el botón como el nuevo padre de la etiqueta
    lv_obj_center(label); // Centra la etiqueta en el botón

    // Configura el tamaño y la posición del botón
    lv_obj_set_pos(btn, pos_x, pos_y); // Posición del botón modificable

    // Añade la lógica para el evento de presionado
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);
}



//--------------------------------------PESTAÑA 1---------------------------------------------------
static void tab1_content(lv_obj_t * parent)
{
    general_title(parent, "¡BIENVENIDO!", TITLE_STYLE_PURPLE);

    lv_obj_t * symbol = lv_label_create(parent);
    lv_label_set_text(symbol, "\xF3\xB0\xB3\xBD");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(parent, symbol, BUTTON_STYLE_PURPLE, go_to_screen2_tab1, 75, 180);

    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, "Yo seré tu asistente\n" "\t\t\t\t de lectura :)" "\n\n¿Quieres saber cómo \n\t\t\t\t\t\tfunciono?\n");
    lv_obj_set_style_text_font(label, &ubuntu_regular_16, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_pos(label, 28, 60);
}

void create_second_screen_tab1(lv_obj_t *padre) {
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xE9AAFF), 0);
    lv_scr_load(screen2);

    // Definir los textos de las etiquetas y sus posiciones Y en un array
    const char *textos[] = {
            "¡HOLA! " "\xEE\xAD\x94",
            "Soy una herramienta para que \n  puedas registrar de manera \n      interactiva tus lecturas \n      a través de mi cámara",
            "---------------------------------------",
            "\xF3\xB1\x89\x9F",
            " Aquí podrás almacenar los \n  libros que estés leyendo",
            "\xF3\xB0\x81\xB2",
            " Aquí podrás registrar tus \n           nuevos libros. \nPara ello deberás mostrar \n    a la cámara el código \n      de barras del libro",
            "\xF3\xB0\x84\xA8",
            "Aquí podrás comprobar tu \n    avance con la lectura"
    };
    const int posicionesY[] = {20, 60, 150, 180, 210, 270, 300, 420, 450};

    // Crear y configurar las etiquetas en un bucle
    for(int i = 0; i < sizeof(textos) / sizeof(textos[0]); i++) {
        lv_obj_t *label = lv_label_create(screen2);
        lv_label_set_text(label, textos[i]);
        lv_obj_set_style_text_font(label, &ubuntu_regular_16, 0);
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, posicionesY[i]);

        // Aplicar la fuente más grande a las etiquetas con símbolos
        if(i == 0 || i == 3 || i == 5 || i == 7) {
            lv_obj_set_style_text_font(label, &bigger_symbols, 0);
        }
    }

    lv_obj_t * symbol = lv_label_create(screen2);
    lv_label_set_text(symbol, "\xF3\xB0\xA9\x88");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(screen2, symbol, BUTTON_STYLE_PURPLE, back_to_main_menu, 95, 510);

    lv_obj_t * space = lv_label_create(screen2);
    lv_label_set_text(space, "\n\n\n");
    lv_obj_align(space, LV_ALIGN_TOP_MID, 0, 510);
}

// Manejador de eventos para el botón que cambia a la pantalla secundaria de tab1
void go_to_screen2_tab1(lv_event_t * e) {
    lv_obj_t * main_screen = lv_scr_act(); // Obtén la pantalla principal (donde están las tabs)
    scr_principal = main_screen;
    create_second_screen_tab1(main_screen);
}



//--------------------------------------PESTAÑA 2---------------------------------------------------
String title;
/*
void tab2_content(lv_obj_t * parent) {
    general_title(parent, "MIS LIBROS", TITLE_STYLE_BLUE);

    // Create a list
    lv_obj_t * list = lv_list_create(parent);
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES - 50); // Adjust the height of the list to the height of the screen minus 50
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 50); // Align the list

    static lv_style_t style_blue;
    lv_style_init(&style_blue);
    lv_style_set_bg_color(&style_blue, lv_color_hex(0x6CE0FF)); // Blue background
    lv_style_set_border_color(&style_blue, lv_color_make(10, 154, 254)); // Blue border
    lv_style_set_radius(&style_blue, 1);

    lv_obj_add_style(list, &style_blue , LV_STATE_DEFAULT);

    Serial.println("HAS ENTRADO EN LA TAB2");

    //prueba();

    // Get the data from the database
        if (!libraryLoaded && Firebase.ready()) {
        String title;
        libraryLoaded = true;
        Serial.println("\n\nHOLAAAAAA\n\n");

        if (Firebase.RTDB.getString(&fbdo, "/libros/9788416588435/titulo")) {
            Serial.println("HAS ENTRADO EN EL PATH DE LA TAB2");

            if (fbdo.dataType() == "string") {
                title = fbdo.stringData();
                Serial.println(title);
                Serial.println("DEBERIA DECIRTE EL TITULO DEL LIBRO");
            } else {
                Serial.println(fbdo.errorReason());
                Serial.println(fbdo.dataType());
                Serial.println("HA HABIDO UN ERROR, NO TE VOY A MOSTRAR EL LIBRO");
            }
        }
    } else {
            // There was an error in the request, you can access the error message
            Serial.println("Failed to retrieve data.");
            Serial.println("Error reason: " + String(fbdo.errorReason()));
        }
}*/

/*
void tab2_content(lv_obj_t * parent) {
    general_title(parent, "MIS LIBROS", TITLE_STYLE_BLUE);

    // Create a list
    lv_obj_t * list = lv_list_create(parent);
    int num_books = 5;
    int list_height = num_books * 40; // Ajustar la altura de la lista en función del número de libros
    lv_obj_set_size(list, LV_HOR_RES, list_height);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 65); // Align the list

    static lv_style_t style_blue;
    lv_style_init(&style_blue);
    lv_style_set_bg_color(&style_blue, lv_color_hex(0x6CE0FF)); // Blue background
    lv_style_set_border_color(&style_blue, lv_color_make(10, 154, 254)); // Blue border
    lv_style_set_radius(&style_blue, 1);

    lv_obj_add_style(list, &style_blue , LV_STATE_DEFAULT);


    String title1;
    if (Firebase.RTDB.getString(&fbdo, "/libros/9788416588435/titulo")) {
        if (fbdo.dataType() == "string") {
            title1 = fbdo.stringData();
            Serial.println("\n\n" + title1 + "\n\n");
        } else {
            Serial.println("Error, no es un string");
        }
    } else {
        Serial.println("Error al obtener datos de Firebase");
        Serial.println(fbdo.errorReason());
    }
    // Añadir un botón a la lista para cada libro
    lv_obj_t * btn1 = lv_list_add_btn(list, "\xEE\xB7\xA2", title1.c_str());
    lv_obj_set_style_text_font(btn1, &ubuntu_regular_16, 0);
    // Configurar el botón
    lv_obj_set_style_text_font(btn1, &ubuntu_regular_16, 0);
    lv_obj_set_style_text_color(btn1, lv_color_black(), 0);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0x6CE0FF), 0);


    String title2;
    if (Firebase.RTDB.getString(&fbdo, "/libros/9788467539677/titulo")) {
        if (fbdo.dataType() == "string") {
            title2 = fbdo.stringData();
            Serial.println("\n\n" + title2 + "\n\n");
        } else {
            Serial.println("Error, no es un string");
        }
    } else {
        Serial.println("Error al obtener datos de Firebase");
        Serial.println(fbdo.errorReason());
    }
    // Añadir un botón a la lista para cada libro
    lv_obj_t * btn2 = lv_list_add_btn(list, "\xEE\xB7\xA2", title2.c_str());
    lv_obj_set_style_text_font(btn2, &ubuntu_regular_16, 0);
    // Configurar el botón
    lv_obj_set_style_text_font(btn2, &ubuntu_regular_16, 0);
    lv_obj_set_style_text_color(btn2, lv_color_black(), 0);
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0x6CE0FF), 0);



    String title3;
    if (Firebase.RTDB.getString(&fbdo, "/libros/9788467539684/titulo")) {
        if (fbdo.dataType() == "string") {
            title3 = fbdo.stringData();
            Serial.println("\n\n" + title3 + "\n\n");
        } else {
            Serial.println("Error, no es un string");
        }
    } else {
        Serial.println("Error al obtener datos de Firebase");
        Serial.println(fbdo.errorReason());
    }
    // Añadir un botón a la lista para cada libro
    lv_obj_t * btn3 = lv_list_add_btn(list, "\xEE\xB7\xA2", title3.c_str());
    lv_obj_set_style_text_font(btn3, &ubuntu_regular_16, 0);
    // Configurar el botón
    lv_obj_set_style_text_font(btn3, &ubuntu_regular_16, 0);
    lv_obj_set_style_text_color(btn3, lv_color_black(), 0);
    lv_obj_set_style_bg_color(btn3, lv_color_hex(0x6CE0FF), 0);



    String title4;
    if (Firebase.RTDB.getString(&fbdo, "/libros/9788467539691/titulo")) {
        if (fbdo.dataType() == "string") {
            title4 = fbdo.stringData();
            Serial.println("\n\n" + title4 + "\n\n");
        } else {
            Serial.println("Error, no es un string");
        }
    } else {
        Serial.println("Error al obtener datos de Firebase");
        Serial.println(fbdo.errorReason());
    }
    // Añadir un botón a la lista para cada libro
    lv_obj_t * btn4 = lv_list_add_btn(list, "\xEE\xB7\xA2", title4.c_str());
    lv_obj_set_style_text_font(btn4, &ubuntu_regular_16, 0);
    // Configurar el botón
    lv_obj_set_style_text_font(btn4, &ubuntu_regular_16, 0);
    lv_obj_set_style_text_color(btn4, lv_color_black(), 0);
    lv_obj_set_style_bg_color(btn4, lv_color_hex(0x6CE0FF), 0);



    String title5;
    if (Firebase.RTDB.getString(&fbdo, "/libros/9788467539707/titulo")) {
        if (fbdo.dataType() == "string") {
            title5 = fbdo.stringData();
            Serial.println("\n\n" + title5 + "\n\n");
        } else {
            Serial.println("Error, no es un string");
        }
    } else {
        Serial.println("Error al obtener datos de Firebase");
        Serial.println(fbdo.errorReason());
    }
    // Añadir un botón a la lista para cada libro
    lv_obj_t * btn5 = lv_list_add_btn(list, "\xEE\xB7\xA2", title5.c_str());
    lv_obj_set_style_text_font(btn5, &ubuntu_regular_16, 0);
    // Configurar el botón
    lv_obj_set_style_text_font(btn5, &ubuntu_regular_16, 0);
    lv_obj_set_style_text_color(btn5, lv_color_black(), 0);
    lv_obj_set_style_bg_color(btn5, lv_color_hex(0x6CE0FF), 0);

}*/

/*
void tab2_content(lv_obj_t * parent) {
    general_title(parent, "MIS LIBROS", TITLE_STYLE_BLUE);

    // Create a list
    lv_obj_t * list = lv_list_create(parent);
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES - 50); // Adjust the height of the list to the height of the screen minus 50
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 50); // Align the list

    static lv_style_t style_blue;
    lv_style_init(&style_blue);
    lv_style_set_bg_color(&style_blue, lv_color_hex(0x6CE0FF)); // Blue background
    lv_style_set_border_color(&style_blue, lv_color_make(10, 154, 254)); // Blue border
    lv_style_set_radius(&style_blue, 1);

    lv_obj_add_style(list, &style_blue , LV_STATE_DEFAULT);

    Serial.println("HAS ENTRADO EN LA TAB2");

    // Get the data from the database
    if (!libraryLoaded && Firebase.ready()) {
        libraryLoaded = true;
        Serial.println("\n\nHOLAAAAAA\n\n");

        if (Firebase.RTDB.get(&fbdo, "/libros")) {
            if (fbdo.dataType() == "json") {
                FirebaseJson* json = fbdo.jsonObjectPtr(); // Get the JSON object
                String jsonString;
                json->toString(jsonString); // Convert the FirebaseJson to a String

                DynamicJsonDocument doc(1024);
                deserializeJson(doc, jsonString); // Parse the JSON string

                for(JsonPair kv : doc.as<JsonObject>()) {
                    String title = kv.value()["titulo"].as<String>();
                    Serial.println(title);

                    // Add a button to the list for each book
                    lv_obj_t * btn = lv_list_add_btn(list, "\xEE\xB7\xA2", title.c_str());
                    lv_obj_set_style_text_font(btn, &ubuntu_regular_16, 0);
                    // Configure the button
                    lv_obj_set_style_text_font(btn, &ubuntu_regular_16, 0);
                    lv_obj_set_style_text_color(btn, lv_color_black(), 0);
                    lv_obj_set_style_bg_color(btn, lv_color_hex(0x6CE0FF), 0);
                }
            } else {
                Serial.println(fbdo.errorReason());
                Serial.println(fbdo.dataType());
                Serial.println("HA HABIDO UN ERROR, NO TE VOY A MOSTRAR EL LIBRO");
            }
        } else {
            Serial.println("Failed to retrieve data.");
            Serial.println("Error reason: " + String(fbdo.errorReason()));
        }
    }
}*/

/*
void tab2_content(lv_obj_t * parent) {
    // Crea un título en la pantalla con el texto "MIS LIBROS" y un estilo de color azul
    general_title(parent, "MIS LIBROS", TITLE_STYLE_BLUE);

    // Crea una lista en la pantalla
    lv_obj_t * list = lv_list_create(parent);
    int num_books = 5;
    int list_height = num_books * 40; // Ajustar la altura de la lista en función del número de libros
    lv_obj_set_size(list, LV_HOR_RES, list_height);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 65); // Align the list

    // Inicializa un estilo con color de fondo azul, borde azul y radio 1
    static lv_style_t style_blue;
    lv_style_init(&style_blue);
    lv_style_set_bg_color(&style_blue, lv_color_hex(0x6CE0FF));
    lv_style_set_border_color(&style_blue, lv_color_make(10, 154, 254));
    lv_style_set_radius(&style_blue, 1);

    // Aplica el estilo a la lista
    lv_obj_add_style(list, &style_blue , LV_STATE_DEFAULT);

    // Imprime un mensaje en la consola
    Serial.println("HAS ENTRADO EN LA TAB2");

    // Si la biblioteca Firebase está lista y no se ha cargado aún
    if (!libraryLoaded && Firebase.ready()) {
        // Marca la biblioteca como cargada
        libraryLoaded = true;
        // Imprime un mensaje en la consola
        Serial.println("\n\nHOLAAAAAA\n\n");

        // Intenta obtener los datos de la base de datos en Firebase
        if (Firebase.RTDB.get(&fbdo, "/libros")) {
            // Si los datos obtenidos son de tipo JSON
            if (fbdo.dataType() == "json") {
                // Obtiene el objeto JSON
                FirebaseJson* json = fbdo.jsonObjectPtr();
                // Convierte el objeto JSON a una cadena
                String jsonString;
                json->toString(jsonString);

                // Crea un documento JSON y parsea la cadena JSON
                DynamicJsonDocument doc(1024);
                deserializeJson(doc, jsonString);

                // Itera sobre cada par clave-valor en el objeto JSON
                for(JsonPair kv : doc.as<JsonObject>()) {
                    // Obtiene el título del libro
                    String title = kv.value()["titulo"].as<String>();
                    // Imprime el título en la consola
                    Serial.println(title);

                    // Añade un botón a la lista para cada libro
                    lv_obj_t * btn = lv_list_add_btn(list, "\xEE\xB7\xA2", title.c_str());
                    // Configura el botón con un estilo de texto y color de fondo
                    lv_obj_set_style_text_font(btn, &ubuntu_regular_16, 0);
                    lv_obj_set_style_text_color(btn, lv_color_black(), 0);
                    lv_obj_set_style_bg_color(btn, lv_color_hex(0x6CE0FF), 0);
                }
            } else {
                // Si los datos obtenidos no son de tipo JSON, imprime un mensaje de error en la consola
                Serial.println(fbdo.errorReason());
                Serial.println(fbdo.dataType());
                Serial.println("HA HABIDO UN ERROR, NO TE VOY A MOSTRAR EL LIBRO");
            }
        } else {
            // Si falla al obtener los datos, imprime un mensaje de error en la consola
            Serial.println("Failed to retrieve data.");
            Serial.println("Error reason: " + String(fbdo.errorReason()));
        }
    }
}*/


/*
void tab2_content(lv_obj_t * parent) {
    general_title(parent, "MIS LIBROS", TITLE_STYLE_BLUE);


    //lv_obj_t * symbol = lv_label_create(parent);
    //lv_label_set_text(symbol, "\xF3\xB1\x81\xAF");
    //lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);
    //create_button(parent, symbol, BUTTON_STYLE_BLUE, go_to_screen2_tab2, 75, 50);


    // Contar el número de libros que no son "LIBRO NO ENCONTRADO"
    int num_books = 0;
    for(int i = 0; i < sizeof(book_array)/sizeof(Book); i++) {
        if (book_array[i].title != "LIBRO NO ENCONTRADO") {
            num_books++;
        }
    }

    // Crear una lista
    lv_obj_t * list = lv_list_create(parent);
    int list_height = num_books * 40; // Ajustar la altura de la lista en función del número de libros
    lv_obj_set_size(list, LV_HOR_RES, list_height);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 50); // Alinear la lista

    static lv_style_t style_blue;
    lv_style_init(&style_blue);
    lv_style_set_bg_color(&style_blue, lv_color_hex(0x6CE0FF)); // Fondo azul
    lv_style_set_border_color(&style_blue, lv_color_make(10, 154, 254)); // Borde azul
    lv_style_set_radius(&style_blue, 1);

    lv_obj_add_style(list, &style_blue , LV_STATE_DEFAULT);

    for(int i = 0; i < sizeof(book_array)/sizeof(Book); i++) {
        // Si el título del libro es "Libro no encontrado", no lo añade a la lista
        if (book_array[i].title == "LIBRO NO ENCONTRADO") {
            continue;
        }

        // Añadir un botón a la lista para cada libro
        lv_obj_t * btn = lv_list_add_btn(list, "\xEE\xB7\xA2", book_array[i].title.c_str());
        lv_obj_set_style_text_font(btn, &ubuntu_regular_16, 0);

        // Configurar el botón
        lv_obj_set_style_text_font(btn, &ubuntu_regular_16, 0);
        lv_obj_set_style_text_color(btn, lv_color_black(), 0);

        lv_obj_set_style_bg_color(btn, lv_color_hex(0x6CE0FF), 0);
    }
}*/

//std::vector<std::string> keys; // Vector para almacenar las claves

void tab2_content(lv_obj_t * parent) {
    general_title(parent, "MIS LIBROS", TITLE_STYLE_BLUE);

    // Crea una lista en la pantalla
    lv_obj_t * list = lv_list_create(parent);

    static lv_style_t style_blue;
    lv_style_init(&style_blue);
    lv_style_set_bg_color(&style_blue, lv_color_hex(0xCBECFF));
    lv_style_set_border_width(&style_blue, 0);
    lv_style_set_border_color(&style_blue, lv_color_make(10, 154, 254));
    lv_style_set_radius(&style_blue, 1);

    lv_obj_add_style(list, &style_blue , LV_STATE_DEFAULT);

    if (!libraryLoaded && Firebase.ready()) {
        libraryLoaded = true;

        if (Firebase.RTDB.get(&fbdo, "/libros")) {
            if (fbdo.dataType() == "json") {
                FirebaseJson* json = fbdo.jsonObjectPtr();
                String jsonString;
                json->toString(jsonString);

                DynamicJsonDocument doc(1024);
                deserializeJson(doc, jsonString);

                // Obtiene el número total de libros antes de entrar en el bucle
                int num_books = doc.as<JsonObject>().size();
                int list_height = num_books * 40;
                lv_obj_set_size(list, 230, list_height);
                lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 65);

                for(JsonPair kv : doc.as<JsonObject>()) {
                    String key = kv.key().c_str();
                    char* keyCopy = new char[key.length() + 1]; // Crear un nuevo array de caracteres en la memoria dinámica
                    strcpy(keyCopy, key.c_str()); // Copiar la clave en el nuevo array de caracteres
                    //keys.push_back(std::string(key.c_str())); // Convert Arduino String to std::string

                    String title = kv.value()["titulo"].as<String>();

                    lv_obj_t * btn = lv_list_add_btn(list, "\xEE\xB7\xA2", title.c_str());
                    lv_obj_set_style_text_font(btn, &ubuntu_regular_16, 0);
                    lv_obj_set_style_text_color(btn, lv_color_black(), 0);
                    lv_obj_set_style_bg_color(btn, lv_color_hex(0xCBECFF), 0);
                    lv_obj_add_event_cb(btn, book_button_event_handler, LV_EVENT_CLICKED, keyCopy); // Use the char* version of the key
                }
            } else {
                Serial.println(fbdo.errorReason());
                Serial.println(fbdo.dataType());
                Serial.println("HA HABIDO UN ERROR, NO TE VOY A MOSTRAR EL LIBRO");
            }
        } else {
            Serial.println("Failed to retrieve data.");
            Serial.println("Error reason: " + String(fbdo.errorReason()));
        }
    }
}


/*
void book_button_event_handler(lv_event_t * e) {
    // Get the button that was clicked
    //lv_obj_t * btn = lv_event_get_target(e);

    // Get the key of the book from the button
    String key = String((char*)lv_event_get_user_data(e));

    // Search the database for the details of the book with the given key
    String path = "/libros/" + key;
    Serial.println("\nAttempting to retrieve data from path: " + path);
    if (Firebase.RTDB.get(&fbdo, path.c_str())) {
        if (fbdo.dataType() == "json") {
            Serial.println("\n"+fbdo.dataType()+"\n");
            DynamicJsonDocument doc(1024);

            String jsonData = fbdo.jsonData().stringValue;

            char jsonChar[jsonData.length() + 1];
            jsonData.toCharArray(jsonChar, sizeof(jsonChar));
            DeserializationError err = deserializeJson(doc, jsonChar);

            if (err) {
                Serial.print(F("deserializeJson() failed with code "));
                Serial.println(err.c_str());
                return;
            }

            String bookTitle = doc["titulo"];
            String author = doc["autor"];
            int totalPage = doc["paginas_total"];
            int currentPage = doc["pagina_actual"];

            // Print the details to the serial console
            Serial.println("Titulo: " + bookTitle);
            Serial.println("Autor: " + author);
            Serial.println("Paginas totales: " + String(totalPage));
            Serial.println("Pagina actual: " + String(currentPage));
        } else {
            Serial.println("\nFailed to retrieve data. Data type is not JSON\n");
            Serial.println("Failed to retrieve data from path: " + path);
            Serial.println("Error reason: " + String(fbdo.errorReason()));
        }
    } else {
        Serial.println("\nFailed to retrieve data from Firebase\n");
    }
}*/

/*
void book_button_event_handler(lv_event_t * e) {
    // Obtener la clave del libro desde el botón
    const char* key = (const char*)lv_event_get_user_data(e);

    // Construir la ruta específica de los datos del libro
    String path = "/libros/" + String(key);
    Serial.println("\nIntentando recuperar datos desde la ruta: " + path);

    if (Firebase.RTDB.get(&fbdo, path.c_str())) {
        if (fbdo.dataType() == "json") {
            Serial.println("\nTipo de datos: JSON\n");
            DynamicJsonDocument doc(1024);

            String jsonData = fbdo.jsonData().stringValue;

            // Imprimir los datos recuperados de Firebase
            Serial.println("Datos recuperados de Firebase: " + jsonData);

            if (jsonData.length() > 0) { // Comprobar si los datos no están vacíos
                char jsonChar[jsonData.length() + 1];
                jsonData.toCharArray(jsonChar, sizeof(jsonChar));
                DeserializationError err = deserializeJson(doc, jsonChar);

                if (err) {
                    Serial.print(F("deserializeJson() falló con el código "));
                    Serial.println(err.c_str());
                    return;
                }

                // Guardar todo el objeto del libro para acceder a él más tarde
                JsonObject bookObject = doc.as<JsonObject>();

                // Extraer e imprimir datos específicos del libro
                String bookTitle = bookObject["titulo"];
                String author = bookObject["autor"];
                int totalPage = bookObject["paginas_total"];
                int currentPage = bookObject["pagina_actual"];

                // Aquí puedes continuar con el resto de tu código...
            } else {
                Serial.println("No hay datos disponibles en la ruta dada.");
            }
        } else {
            Serial.println("Los datos recuperados no son de tipo JSON.");
        }
    } else {
        Serial.println("Falló al recuperar datos de Firebase.");
    }
}*/


void book_button_event_handler(lv_event_t * e) {
    // Obtener la clave del libro desde el evento
    std::string key = std::string(static_cast<char*>(lv_event_get_user_data(e)));

    // Construir la ruta específica de los datos del libro
    String path = "/libros/" + String(key.c_str());
    Serial.println("\nIntentando recuperar datos desde la ruta: " + path);

    if (Firebase.RTDB.get(&fbdo, path.c_str())) {
        if (fbdo.dataType() == "json") {
            Serial.println("\nTipo de datos: JSON\n");
            DynamicJsonDocument doc(1024);

            String jsonData = fbdo.jsonData().stringValue;

            // Imprimir los datos recuperados de Firebase
            Serial.println("Datos recuperados de Firebase: " + jsonData);

            if (jsonData.length() > 0) { // Comprobar si los datos no están vacíos
                char jsonChar[jsonData.length() + 1];
                jsonData.toCharArray(jsonChar, sizeof(jsonChar));
                DeserializationError err = deserializeJson(doc, jsonChar);

                if (err) {
                    Serial.print(F("deserializeJson() falló con el código "));
                    Serial.println(err.c_str());
                    return;
                }

                // Guardar todo el objeto del libro para acceder a él más tarde
                JsonObject bookObject = doc.as<JsonObject>();

                // Extraer e imprimir datos específicos del libro
                String bookTitle = bookObject["titulo"];
                String author = bookObject["autor"];
                int totalPage = bookObject["paginas_total"];
                int currentPage = bookObject["pagina_actual"];

                // Aquí puedes continuar con el resto de tu código...
            } else {
                Serial.println("No hay datos disponibles en la ruta dada.");
            }
        } else {
            Serial.println("Los datos recuperados no son de tipo JSON.");
        }
    } else {
        Serial.println("Falló al recuperar datos de Firebase.");
    }
}


// Manejador de eventos para los botones de la lista de libros
/*
void book_button_event_handler(lv_event_t * e) {
    // Get the button that was clicked
    lv_obj_t * btn = lv_event_get_target(e);

    // Get the key of the book from the button
    String key = (char*)lv_event_get_user_data(e);

    // Search the database for the details of the book with the given key
    String path = "/libros/" + key;
    if (Firebase.RTDB.get(&fbdo, path.c_str())) {
        if (fbdo.dataType() == "json") {
            DynamicJsonDocument doc(1024);
            //deserializeJson(doc, fbdo.jsonData());

            //String jsonData = fbdo.jsonData();
            char jsonChar[jsonData.length() + 1];
            jsonData.toCharArray(jsonChar, sizeof(jsonChar));
            deserializeJson(doc, jsonChar);

            String bookTitle = doc["titulo"];
            String author = doc["autor"];
            int totalPage = doc["paginas_total"];
            int currentPage = doc["pagina_actual"];
            create_second_screen_tab2(parent, bookTitle, author, totalPage, currentPage);
        }
    }
}*/




// Manejador de eventos para el botón que cambia a la pantalla secundaria de tab2
void go_to_screen2_tab2(lv_event_t * e, const char* title) {
    // Load the book information from Firebase
    String path = "/libros/" + String(title);

    if (Firebase.RTDB.get(&fbdo, path.c_str())) {
        FirebaseJson* json = fbdo.jsonObjectPtr();
        FirebaseJsonData jsonData;

        json->get(jsonData, "/autor");
        String author = jsonData.stringValue;

        json->get(jsonData, "/pagina_total");
        int totalPage = jsonData.intValue;

        json->get(jsonData, "/pagina_actual");
        int currentPage = jsonData.intValue;

        create_second_screen_tab2(lv_event_get_current_target(e), title, author.c_str(), totalPage, currentPage);
    } else {
        // Handle the error
    }
}

void create_second_screen_tab2(lv_obj_t *padre, const String& bookTitle, const String& author, int totalPage, int currentPage) {
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xCBECFF), 0);
    lv_scr_load(screen2);

    // Muestra la información del libro
    String bookData = "Titulo: " + bookTitle + "\nAutor: " + author + "\nPagina Total: " + String(totalPage) + "\nPagina Actual: " + String(currentPage);
    lv_obj_t * label = lv_label_create(screen2);
    lv_label_set_text(label, bookData.c_str());
    lv_obj_set_style_text_font(label, &ubuntu_regular_16, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    // Crea el botón para volver a la pantalla principal
    lv_obj_t * symbol = lv_label_create(screen2);
    lv_label_set_text(symbol, "\xF3\xB0\xA9\x88");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);
    create_button(screen2, symbol, BUTTON_STYLE_PURPLE, back_to_main_menu, 95, 510);

    lv_obj_t * space = lv_label_create(screen2);
    lv_label_set_text(space, "\n\n\n");
    lv_obj_align(space, LV_ALIGN_TOP_MID, 0, 510);
}



//--------------------------------------PESTAÑA 3---------------------------------------------------
void tab3_content(lv_obj_t * parent) {
    general_title(parent, "ESCANEAR LIBRO", TITLE_STYLE_ORANGE);

    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, "¿Deseas agregar un nuevo \n     libro a tu biblioteca?");
    lv_obj_set_style_text_font(label, &ubuntu_regular_16, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 50);

    lv_obj_t * label2 = lv_label_create(parent);
    lv_label_set_text(label2, "  Pincha en el botón para \nabrir la cámara y escanear \n               el libro");
    lv_obj_set_style_text_font(label2, &ubuntu_regular_16, 0);
    lv_obj_align(label2, LV_ALIGN_TOP_MID, 0, 110);

    lv_obj_t * symbol = lv_label_create(parent);
    lv_label_set_text(symbol, "\xF3\xB0\x84\x84");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(parent, symbol, BUTTON_STYLE_ORANGE, go_to_screen2_tab3, 75, 190);
}

// Función para crear la pantalla secundaria de la pestaña 3
void create_second_screen_tab3(lv_obj_t *padre) {

    lv_obj_t * screen2 = lv_obj_create(NULL);
    //lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    //lv_obj_set_style_bg_color(screen2, lv_color_hex(0xFFC97A), 0);
    lv_scr_load(screen2);

    // Inicializa la interfaz de usuario de la cámara
    setup_scr_camera(&guider_camera_ui);
    // Carga la interfaz de usuario de la cámara en la pantalla
    lv_scr_load(guider_camera_ui.camera);

    /*
    lv_obj_t * label = lv_label_create(screen2);
    lv_label_set_text(label, "Hola, has cambiado de pantalla");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t * symbol = lv_label_create(screen2);
    lv_label_set_text(symbol, "\xF3\xB0\xA9\x88");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);*/

    //create_button(screen2, symbol, BUTTON_STYLE_ORANGE, back_to_main_menu, 95, 110);
}

// Manejador de eventos para el botón que cambia a la pantalla secundaria de tab3
static void go_to_screen2_tab3(lv_event_t * e) {
    lv_obj_t * main_screen = lv_scr_act(); // Obtén la pantalla principal (donde están las tabs)
    scr_principal = main_screen;
    create_second_screen_tab3(main_screen);
}



//--------------------------------------PESTAÑA 4---------------------------------------------------
static void tab4_content(lv_obj_t * parent){
    general_title(parent, "MIS ESTADÍSTICAS", TITLE_STYLE_GREEN);

    lv_obj_t * chart = lv_chart_create(parent);
    lv_obj_set_size(chart, 145, 150);
    lv_obj_center(chart);
    lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 500);
    lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_Y, 0, 200);
    lv_chart_set_point_count(chart, 12);
    lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

    /*Add ticks and label to every axis*/
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 12, 3, true, 40);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 2, true, 50);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_SECONDARY_Y, 10, 5, 3, 4, true, 50);
    lv_chart_set_div_line_count(chart, 5, 5);

    /*Zoom in a little in X*/
    lv_chart_set_zoom_x(chart, 1500);

    /*Add two data series*/
    lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_palette_lighten(LV_PALETTE_GREEN, 2), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser2 = lv_chart_add_series(chart, lv_palette_darken(LV_PALETTE_GREEN, 2),
                                                   LV_CHART_AXIS_SECONDARY_Y);

    /*
    //Set the next points on 'ser1'
    lv_chart_set_next_value(chart, ser1, 100);
    lv_chart_set_next_value(chart, ser1, 110);
    lv_chart_set_next_value(chart, ser1, 137);
    lv_chart_set_next_value(chart, ser1, 200);
    lv_chart_set_next_value(chart, ser1, 256);
    lv_chart_set_next_value(chart, ser1, 257);
    lv_chart_set_next_value(chart, ser1, 50);
    lv_chart_set_next_value(chart, ser1, 20);
    lv_chart_set_next_value(chart, ser1, 37);
    lv_chart_set_next_value(chart, ser1, 67);
    lv_chart_set_next_value(chart, ser1, 49);
    lv_chart_set_next_value(chart, ser1, 55);*/


    lv_coord_t * ser1_array = lv_chart_get_y_array(chart, ser1);
    //Directly set points on 'ser2'
    ser1_array[0] = 100;
    ser1_array[1] = 110;
    ser1_array[2] = 137;
    ser1_array[3] = 200;
    ser1_array[4] = 256;
    ser1_array[5] = 257;
    ser1_array[6] = 50;
    ser1_array[7] = 20;
    ser1_array[8] = 37;
    ser1_array[9] = 67;
    ser1_array[10] = 49;
    ser1_array[11] = 55;


    /*
    lv_chart_set_next_value(chart, ser2, 1);
    lv_chart_set_next_value(chart, ser2, 1);
    lv_chart_set_next_value(chart, ser2, 1);
    lv_chart_set_next_value(chart, ser2, 1);
    lv_chart_set_next_value(chart, ser2, 2);
    lv_chart_set_next_value(chart, ser2, 2);
    lv_chart_set_next_value(chart, ser2, 2);
    lv_chart_set_next_value(chart, ser2, 3);
    lv_chart_set_next_value(chart, ser2, 4);
    lv_chart_set_next_value(chart, ser2, 4);
    lv_chart_set_next_value(chart, ser2, 4);
    lv_chart_set_next_value(chart, ser2, 5);*/


    lv_coord_t * ser2_array = lv_chart_get_y_array(chart, ser2);
    //Directly set points on 'ser2'
    ser2_array[0] = reto_pag_mes-ser1_array[0];
    ser2_array[1] = reto_pag_mes-ser1_array[1];
    ser2_array[2] = reto_pag_mes-ser1_array[2];
    ser2_array[3] = reto_pag_mes-ser1_array[3];
    ser2_array[4] = reto_pag_mes-ser1_array[4];
    ser2_array[5] = reto_pag_mes-ser1_array[5];
    ser2_array[6] = reto_pag_mes-ser1_array[6];
    ser2_array[7] = reto_pag_mes-ser1_array[7];
    ser2_array[8] = reto_pag_mes-ser1_array[8];
    ser2_array[9] = reto_pag_mes-ser1_array[9];
    ser2_array[10] = reto_pag_mes-ser1_array[10];
    ser2_array[11] = reto_pag_mes-ser1_array[11];

    int i;
    for(i = 0; i < 12; i++) {
        lv_chart_set_next_value(chart, ser1, lv_rand(60, 90));
        lv_chart_set_next_value(chart, ser2, lv_rand(10, 40));
    }

    lv_obj_set_style_pad_column(chart, 0, LV_PART_ITEMS);   /*Space between columns of the same index*/
    lv_obj_set_style_pad_column(chart, 4, LV_PART_MAIN);    /*Space between columns of the adjacent index*/

    lv_chart_refresh(chart); /*Required after direct set*/
}

static void draw_event_cb(lv_event_t * e) {
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL)) return;

    if(dsc->id == LV_CHART_AXIS_PRIMARY_X && dsc->text) {
        const char * month[] = {"Ene", "Feb", "Mar", "Abr", "May", "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"};
        lv_snprintf(dsc->text, dsc->text_length, "%s", month[dsc->value % 12]);
    }
}
