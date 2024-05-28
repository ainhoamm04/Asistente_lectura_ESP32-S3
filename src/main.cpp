#include <Arduino.h>
#include <ArduinoNvs.h>
#include "display.h"
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "FT6336U.h"
#include "sd_card.h"
#include "camera.h"
#include "camera_ui.h"
#include "firebase_config.h"


#include <WiFi.h>
#include <Firebase_ESP_Client.h>
/*
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
 */

#include <ArduinoJson.h> // Include the ArduinoJson library
#include <FirebaseJson.h>

// Provide the token generation process info.
//#include <addons/TokenHelper.h>
// Provide the RTDB payload printing info and other helper functions.
//#include <addons/RTDBHelper.h>


//-------------------------DECLARACIÓN DE FUNCIONES------------------------------------
// Define Firebase Data object
/*
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
unsigned long count = 0;
bool signupOK = false;
bool libraryLoaded = false;
String path = "/libros";*/

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
void create_second_screen_tab2(lv_obj_t * parent, const std::string& key);
void go_to_screen2_tab2(lv_event_t * e);

void tab3_content(lv_obj_t * parent);
void create_second_screen_tab3(lv_obj_t *padre);
static void go_to_screen2_tab3(lv_event_t * e);
void searchIsbnInDatabase();
void initialize_and_load_camera();

static void tab4_content(lv_obj_t * parent);
void create_second_screen_tab4(lv_obj_t *padre);
void go_to_screen2_tab4(lv_event_t * e);
static void draw_label_x_axis(lv_event_t * e);
static void draw_label_y_axis(lv_event_t * e);
int reto_pag_mes = 300;


//-------------------------------SETUP------------------------------------
void setup() {
    Serial.begin(115200);

    // Configuración y conexión de Firebase
    setup_firebase();

    /*
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
    //-------------------------------------------------------------------------------------
    */

    NVS.begin();

    sdcard_init();
    camera_init();
    screen.init();

    tab_function();

    searchIsbnInDatabase();
}



//-------------------------------LOOP------------------------------------
void loop() {
    screen.routine(); /* let the GUI do its work */
    delay(5);

    //Firebase.ready();
}



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
                lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 55);

                for(JsonPair kv : doc.as<JsonObject>()) {
                    String key = kv.key().c_str();
                    char* keyCopy = new char[key.length() + 1]; // Crear un nuevo array de caracteres en la memoria dinámica
                    strcpy(keyCopy, key.c_str()); // Copiar la clave en el nuevo array de caracteres
                    //keys.push_back(std::string(key.c_str())); // Convert Arduino String to std::string

                    String title = kv.value()["titulo"].as<String>();

                    lv_obj_t * btn = lv_list_add_btn(list, "\xF3\xB1\x81\xAF", title.c_str());

                    lv_obj_t * label = lv_obj_get_child(btn, NULL);
                    lv_obj_set_style_text_font(label, &bigger_symbols, 0);

                    lv_obj_set_style_text_font(btn, &ubuntu_regular_16, 0);
                    lv_obj_set_style_text_color(btn, lv_color_black(), 0);
                    lv_obj_set_style_bg_color(btn, lv_color_hex(0xCBECFF), 0);
                    lv_obj_add_event_cb(btn, go_to_screen2_tab2, LV_EVENT_CLICKED, keyCopy); // Use the char* version of the key
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


void go_to_screen2_tab2(lv_event_t * e) {
    // Obtén la pantalla principal (donde están las tabs)
    lv_obj_t * main_screen = lv_scr_act();
    scr_principal = main_screen;

    // Obtén la clave del libro desde el evento
    std::string key = std::string(static_cast<char*>(lv_event_get_user_data(e)));

    // Llama a la función para crear la segunda pantalla
    create_second_screen_tab2(main_screen, key);
}


lv_obj_t * label_title = NULL;
lv_obj_t * label_author = NULL;
lv_obj_t * label_total_pages = NULL;
lv_obj_t * label_current_page = NULL;

// Manejador de eventos para el botón que cambia a la pantalla secundaria de tab1
void create_second_screen_tab2(lv_obj_t * parent, const std::string& key) {
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xCBECFF), 0);
    lv_scr_load(screen2);

    lv_obj_t * label_symbols1 = lv_label_create(lv_scr_act());
    std::string symbols = "";
    for(int i = 0; i < 9; i++) {
        symbols += " \xEE\xB7\xA2  ";
    }
    lv_label_set_text(label_symbols1, symbols.c_str());
    lv_obj_set_style_text_font(label_symbols1, &ubuntu_regular_16, 0);
    lv_obj_set_pos(label_symbols1, 0, 10);

    lv_obj_t * label_symbols2 = lv_label_create(lv_scr_act());
    lv_label_set_text(label_symbols2, symbols.c_str());
    lv_obj_set_style_text_font(label_symbols2, &ubuntu_regular_16, 0);
    lv_obj_set_pos(label_symbols2, 0, 290);

    int posY = -90; // Posición inicial en Y para las etiquetas

    // Construir la ruta específica de los datos del libro
    String path = "/libros/" + String(key.c_str());
    Serial.println("\nIntentando recuperar datos desde la ruta: " + path);

    if (Firebase.RTDB.get(&fbdo, path.c_str())) {
        if (fbdo.dataType() == "json") {
            Serial.println("\nTipo de datos: JSON\n");
            DynamicJsonDocument doc(1024);

            String jsonData = fbdo.jsonData().stringValue;

            String pathtitle = "/libros/" + String(key.c_str()) + "/titulo";
            String pathauthor = "/libros/" + String(key.c_str()) + "/autor";
            String pathtotalpages = "/libros/" + String(key.c_str()) + "/paginas_total";
            String pathcurrentpage = "/libros/" + String(key.c_str()) + "/pagina_actual";

            if (Firebase.RTDB.getString(&fbdo, pathtitle.c_str())) { // Intenta obtener el título del libro desde Firebase
                if (fbdo.dataType() == "string") { // Si los datos obtenidos son de tipo string
                    String titulo = fbdo.stringData(); // Almacena el título en una variable
                    Serial.println("El titulo del libro es: " + titulo); // Imprime el título en el monitor serie

                    label_title = lv_label_create(screen2);
                    lv_label_set_text(label_title, (titulo).c_str());
                    lv_obj_set_style_text_font(label_title, &bigger_symbols, 0);
                    lv_label_set_long_mode(label_title, LV_LABEL_LONG_SCROLL_CIRCULAR); // Ajusta el texto para que se pase a la siguiente línea si es demasiado largo
                    lv_obj_set_width(label_title, 225); // Establece el ancho máximo de la etiqueta
                    lv_obj_align(label_title, LV_ALIGN_LEFT_MID, 15, posY);

                    posY = posY + 40;
                } else {
                    Serial.println("Error: los datos obtenidos no son de tipo string");
                }
            } else {
                Serial.println("Error al obtener los datos desde Firebase: " + fbdo.errorReason()); // Imprime el error
            }

            if (Firebase.RTDB.getString(&fbdo, pathauthor.c_str())) { // Intenta obtener el título del libro desde Firebase
                if (fbdo.dataType() == "string") { // Si los datos obtenidos son de tipo string
                    String autor = fbdo.stringData(); // Almacena el título en una variable
                    Serial.println("El autor del libro es: " + autor); // Imprime el título en el monitor serie

                    // Crea una etiqueta para el autor
                    label_author = lv_label_create(screen2);
                    lv_label_set_text(label_author, ("Autor: " + autor).c_str());
                    lv_obj_set_style_text_font(label_author, &ubuntu_regular_16, 0);
                    lv_obj_align(label_author, LV_ALIGN_LEFT_MID, 15, posY);

                    posY = posY + 25;
                } else {
                    Serial.println("Error: los datos obtenidos no son de tipo string");
                }
            } else {
                Serial.println("Error al obtener los datos desde Firebase: " + fbdo.errorReason()); // Imprime el error
            }

            if (Firebase.RTDB.getInt(&fbdo, pathtotalpages.c_str())) { // Intenta obtener el título del libro desde Firebase
                if (fbdo.dataType() == "int") { // Si los datos obtenidos son de tipo string
                    int paginas_total = fbdo.intData(); // Almacena el título en una variable
                    Serial.println("El numero de paginas total es: " + String(paginas_total)); // Imprime el título en el monitor serie

                    // Crea una etiqueta para el total de páginas
                    label_total_pages = lv_label_create(screen2);
                    lv_label_set_text(label_total_pages, (String(paginas_total) + " páginas en total").c_str());
                    lv_obj_set_style_text_font(label_total_pages, &ubuntu_italic_16, 0);
                    lv_obj_align(label_total_pages, LV_ALIGN_LEFT_MID, 15, posY);

                    posY = posY + 25;
                } else {
                    Serial.println("Error: los datos obtenidos no son de tipo int");
                }
            } else {
                Serial.println("Error al obtener los datos desde Firebase: " + fbdo.errorReason()); // Imprime el error
            }

            if (Firebase.RTDB.getInt(&fbdo, pathcurrentpage.c_str())) { // Intenta obtener el título del libro desde Firebase
                if (fbdo.dataType() == "int") { // Si los datos obtenidos son de tipo string
                    int pagina_actual = fbdo.intData(); // Almacena el título en una variable
                    Serial.println("La ultima pagina leida es la: " + String(pagina_actual)); // Imprime el título en el monitor serie

                    // Crea una etiqueta para la página actual
                    label_current_page = lv_label_create(screen2);
                    lv_label_set_text(label_current_page, ("Vas por la página: " + String(pagina_actual)).c_str());
                    lv_obj_set_style_text_font(label_current_page, &ubuntu_bold_16, 0);
                    lv_obj_align(label_current_page, LV_ALIGN_LEFT_MID, 15, posY);
                } else {
                    Serial.println("Error: los datos obtenidos no son de tipo int");
                }
            } else {
                Serial.println("Error al obtener los datos desde Firebase: " + fbdo.errorReason()); // Imprime el error
            }
        } else {
            Serial.println("Los datos recuperados no son de tipo JSON.");
        }
    } else {
        Serial.println("Fallo al recuperar datos de Firebase.");
    }

    lv_obj_t * symbol = lv_label_create(screen2);
    lv_label_set_text(symbol, "\xF3\xB0\xA9\x88");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(screen2, symbol, BUTTON_STYLE_BLUE, back_to_main_menu, 95, 200);

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
// Array para almacenar las claves de los libros
std::vector<std::string> book_keys;
int max_pages = 0;
String max_pages_book_title;


static void tab4_content(lv_obj_t * parent) {
    general_title(parent, "MIS ESTADÍSTICAS", TITLE_STYLE_GREEN);


    // Get the book data from Firebase
    if (Firebase.RTDB.get(&fbdo, "/libros")) {
        if (fbdo.dataType() == "json") {
            FirebaseJson* json = fbdo.jsonObjectPtr();
            String jsonString;
            json->toString(jsonString);

            DynamicJsonDocument doc(1024);
            deserializeJson(doc, jsonString);

            int total_pages = 0; // Variable para almacenar el total de páginas leídas

            // Iterate through each book
            for(JsonPair kv : doc.as<JsonObject>()) {
                int current_pages = kv.value()["pagina_actual"].as<int>();
                String title = kv.value()["titulo"].as<String>();

                total_pages += current_pages; // Suma las páginas actuales al total

                // Check if this book has more pages read than the current max
                if(current_pages > max_pages) {
                    max_pages = current_pages;
                    max_pages_book_title = title;
                }
            }

            // Create a label to display the book with the most pages read
            lv_obj_t * label = lv_label_create(parent);
            String label_text = "Libro con más páginas leídas: \n" + max_pages_book_title + " (" + String(max_pages) + " páginas)";
            lv_label_set_text(label, label_text.c_str());
            lv_obj_set_style_text_font(label, &ubuntu_regular_16, 0);
            lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 50);

            // Create a label to display the total pages read
            lv_obj_t * total_label = lv_label_create(parent);
            String total_label_text = "Total de páginas leídas: " + String(total_pages);
            lv_label_set_text(total_label, total_label_text.c_str());
            lv_obj_set_style_text_font(total_label, &ubuntu_regular_16, 0);
            lv_obj_align(total_label, LV_ALIGN_TOP_LEFT, 0, 100); // Ajusta la posición en Y según sea necesario
        }
    }



    //Botón para ir a pantalla que muestra gráfica
    lv_obj_t * symbol = lv_label_create(parent);
    lv_label_set_text(symbol, "\xF3\xB0\x84\xA8");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(parent, symbol, BUTTON_STYLE_GREEN, go_to_screen2_tab4, 75, 200);

}


void create_second_screen_tab4(lv_obj_t *padre) {
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xCEF2D8), 0);
    lv_scr_load(screen2);


    lv_obj_t * chart = lv_chart_create(screen2);

    // Incrementa la posición en el eje X en 10px
    lv_obj_set_pos(chart, 45, 60);

    lv_obj_set_size(chart, 160, 165);
    lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100); // Set the range from 0 to 100
    lv_obj_add_event_cb(chart, draw_label_x_axis, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_obj_add_event_cb(chart, draw_label_y_axis, LV_EVENT_DRAW_PART_BEGIN, NULL);

    // Set the tick labels for the Y axis
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 2, true, 50); // Set the tick count to 5 (0%, 20%, 40%, 60%, 80%, 100%)

    lv_chart_set_div_line_count(chart, 6, 0); // Set the division line count

    lv_chart_set_zoom_x(chart, 500);

    lv_chart_series_t * ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_LIGHT_GREEN), LV_CHART_AXIS_PRIMARY_Y); // Add color for the series

    //Para que las etiquetas de los ejes aparezcan en negro y se vean mejor (antes en gris)
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_line_color(&style, lv_color_hex(0x000000));
    lv_style_set_text_color(&style, lv_color_hex(0x000000));
    lv_obj_add_style(chart, &style, LV_PART_TICKS);

    int book_index = 0; // Book index

    // Get the book data from Firebase
    if (Firebase.RTDB.get(&fbdo, "/libros")) {
        if (fbdo.dataType() == "json") {
            FirebaseJson *json = fbdo.jsonObjectPtr();
            String jsonString;
            json->toString(jsonString);

            // Print the JSON string to verify it's loading correctly
            Serial.println("JSON string loaded from Firebase:");
            Serial.println(jsonString);

            DynamicJsonDocument doc(1024);
            deserializeJson(doc, jsonString);

            // Set the tick labels for the X axis
            lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 5, 5, doc.size(), 1, true, 50); // Set the tick count to the number of books

            // Set the point count to the number of books
            lv_chart_set_point_count(chart, doc.size());

            // For each book, calculate the percentage of pages read and add a series to the chart
            for(JsonPair kv : doc.as<JsonObject>()) {
                String book_key = kv.key().c_str(); // Get the book key
                book_keys.push_back(book_key.c_str()); // Store the book key for later use
                JsonObject libro = kv.value().as<JsonObject>();

                // Calculate the percentage of pages read
                int paginas_total = libro["paginas_total"].as<int>();
                int pagina_actual = libro["pagina_actual"].as<int>();
                float porcentaje = ((float)pagina_actual / paginas_total) * 100;

                // Multiplica el porcentaje por 10 para mostrar un decimal
                //int porcentaje_ajustado = round(porcentaje * 10);

                // Añade el porcentaje ajustado a la serie del gráfico
                lv_chart_set_next_value(chart, ser, porcentaje);

                // Print the book data to verify they're being processed correctly
                Serial.println("Book data processed:");
                Serial.print("Key: ");
                Serial.println(book_key);
                Serial.print("Total pages: ");
                Serial.println(paginas_total);
                Serial.print("Current page: ");
                Serial.println(pagina_actual);
                Serial.print("Percentage: ");
                Serial.println(porcentaje);

                // Add a series to the chart with the calculated percentage
                //lv_chart_set_next_value(chart, ser, porcentaje);

                // Create a label for the book title and position it below the chart
                lv_obj_t * label = lv_label_create(screen2);
                lv_label_set_text_fmt(label, "%s: %s", book_key.c_str(), libro["titulo"].as<const char*>());
                lv_obj_set_pos(label, 10, 300 + 20 * book_index); // Position the label below the chart

                book_index++; // Increment the book index
            }
        }
    }

    lv_chart_refresh(chart); // Required after direct set




    lv_obj_t * symbol = lv_label_create(screen2);
    lv_label_set_text(symbol, "\xF3\xB0\xA9\x88");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(screen2, symbol, BUTTON_STYLE_GREEN, back_to_main_menu, 95, 310);

}


// Manejador de eventos para el botón que cambia a la pantalla secundaria de tab4
void go_to_screen2_tab4(lv_event_t * e) {
    lv_obj_t * main_screen = lv_scr_act(); // Obtén la pantalla principal (donde están las tabs)
    scr_principal = main_screen;
    create_second_screen_tab4(main_screen);
}


static void draw_label_x_axis(lv_event_t * e) {
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL)) return;

    if(dsc->id == LV_CHART_AXIS_PRIMARY_X && dsc->text) {
        lv_snprintf(dsc->text, dsc->text_length, "%s", book_keys[dsc->value % book_keys.size()].c_str());
    }
}

static void draw_label_y_axis(lv_event_t * e) {
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL)) return;

    if(dsc->id == LV_CHART_AXIS_PRIMARY_Y && dsc->text) {
        lv_snprintf(dsc->text, dsc->text_length, "%d%%", dsc->value);
    }
}




