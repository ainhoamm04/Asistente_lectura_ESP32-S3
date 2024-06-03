#include <Arduino.h>
//#include <ArduinoNvs.h>
#include "display.h"
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "FT6336U.h"
#include "sd_card.h"
#include "camera.h"
#include "camera_ui.h"
#include "Freenove_WS2812_Lib_for_ESP32.h"

#include "firebase_config.h"
#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>
#include <FirebaseJson.h>

//Definición aquí (en lugar de platformio.ini) para que no de error strip
#define LEDS_COUNT 1
#define LEDS_PIN 48
#define CHANNEL 0

//-------------------------DECLARACIÓN DE FUNCIONES------------------------------------
Display screen;

Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);

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

static void tab2_content(lv_obj_t * parent);
void create_second_screen_tab2(lv_obj_t * parent, const std::string& key);
void go_to_screen2_tab2(lv_event_t * e);

static void tab3_content(lv_obj_t * parent);
void create_second_screen_tab3(lv_obj_t *padre);
static void go_to_screen2_tab3(lv_event_t * e);

static void tab4_content(lv_obj_t * parent);
void create_second_screen_tab4(lv_obj_t *padre);
void go_to_screen2_tab4(lv_event_t * e);
static void event_handler_scroll1(lv_event_t * e);
static void event_handler_scroll2(lv_event_t * e);
static void event_handler_scroll3(lv_event_t * e);
static void event_handler_bottom(lv_event_t * e);
static void event_handler_top(lv_event_t * e);
static void draw_label_x_axis(lv_event_t * e);
static void draw_label_y_axis(lv_event_t * e);

void tabview_event_handler(lv_event_t * e);

//-------------------------------SETUP------------------------------------
void setup() {
    Serial.begin(115200);

    sdcard_init();
    camera_init();

    screen.init();

    setup_firebase();

    tab_function();

    //NVS.begin();

    strip.begin();
    strip.setBrightness(255);
}



//-------------------------------LOOP------------------------------------
String decoded_barcode;

void loop() {
    screen.routine(); /* let the GUI do its work */
    delay(5);
    Firebase.ready();

    if (Serial.available() > 0) {  // Si hay datos disponibles en el puerto serial
        char buffer[32];  // Buffer para almacenar los datos recibidos
        int bytesRead = Serial.readBytes(buffer, sizeof(buffer) - 1);  // Lee los bytes del puerto serie
        buffer[bytesRead] = '\0';  // Asegúrate de que la cadena esté terminada en null
        decoded_barcode = String(buffer);  // Convierte los bytes leídos en una cadena
        Serial.println(decoded_barcode);  // Imprime la cadena leída en el Monitor Serial
        Serial.println("Código leído desde Python");
    }
}



//-------------------------DEFINICIÓN DE FUNCIONES------------------------------------
lv_obj_t * tab1;
lv_obj_t * tab2;
lv_obj_t * tab3;
lv_obj_t * tab4;

void tab_function(void)
{
    //Create a Tab view object
    lv_obj_t * tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 35);

    //Add 4 tabs (the tabs are page (lv_page) and can be scrolled
    tab1 = lv_tabview_add_tab(tabview, "\xF3\xB0\x8B\x9C");
    tab2 = lv_tabview_add_tab(tabview, "\xF3\xB1\x89\x9F");
    tab3 = lv_tabview_add_tab(tabview, "\xF3\xB0\x81\xB2");
    tab4 = lv_tabview_add_tab(tabview, "\xF3\xB0\x84\xA8");

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

    lv_obj_add_event_cb(tabview, tabview_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
}


void tabview_event_handler(lv_event_t * e) {
    lv_obj_t * tabview = lv_event_get_target(e);
    uint16_t tab = lv_tabview_get_tab_act(tabview);
    if (tab == 1) { // Si la pestaña 2 está seleccionada (los índices de las pestañas comienzan en 0)
        lv_obj_clean(tab2);
        tab2_content(tab2); // Llama a tab2_content
        Serial.println("Tab 2");
    }
    else if (tab == 3) { // Si la pestaña 4 está seleccionada
        lv_obj_clean(tab4);
        tab4_content(tab4); // Llama a tab4_content
        Serial.println("Tab 4");
    }
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

    lv_obj_t * label1 = lv_label_create(parent);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label1, "Yo seré tu asistente de lectura :)\n\n¿Quieres saber cómo funciono?");
    lv_obj_set_style_text_font(label1, &ubuntu_regular_16, 0);
    lv_obj_set_width(label1, 160);
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -20);
}

void create_second_screen_tab1(lv_obj_t *padre) {
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xE9AAFF), 0);
    lv_scr_load(screen2);

    // Definir los textos de las etiquetas y sus posiciones Y en un array
    const char *textos[] = {
            "#8035A8 ¡HOLA!# " "#8035A8 \xEE\xAD\x94#",
            "Soy una herramienta para que puedas registrar de manera interactiva tus lecturas a través de mi cámara",
            "---------------------------------------",
            "\xF3\xB1\x89\x9F",
            "Aquí podrás almacenar los libros que estés leyendo",
            "\xF3\xB0\x81\xB2",
            "Aquí podrás registrar tus nuevos libros. Para ello deberás mostrar a la cámara el código de barras del libro",
            "\xF3\xB0\x84\xA8",
            "Aquí podrás comprobar tu avance con la lectura"
    };
    const int posicionesY[] = {20, 60, 150, 180, 210, 275, 305, 410, 440};

    // Crear y configurar las etiquetas en un bucle
    for(int i = 0; i < sizeof(textos) / sizeof(textos[0]); i++) {
        lv_obj_t *label = lv_label_create(screen2);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_label_set_recolor(label, true);
        lv_label_set_text(label, textos[i]);
        lv_obj_set_style_text_font(label, &ubuntu_regular_16, 0);
        lv_obj_set_width(label, 215);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
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
//Generación de estructura para hacer más fácil el manejo de los libros para ordenarlos
struct Book {
    String title;
    String key;
    long long timestamp;
};


static void tab2_content(lv_obj_t * parent) {
    general_title(parent, "MIS LIBROS", TITLE_STYLE_BLUE);

    lv_obj_t * list = lv_list_create(parent);

    static lv_style_t style_blue;
    lv_style_init(&style_blue);
    lv_style_set_bg_color(&style_blue, lv_color_hex(0xCBECFF));
    lv_style_set_border_width(&style_blue, 0);
    lv_style_set_border_color(&style_blue, lv_color_make(10, 154, 254));
    lv_style_set_radius(&style_blue, 1);

    lv_obj_add_style(list, &style_blue , LV_STATE_DEFAULT);

    DynamicJsonDocument doc = get_book_data();
    if (!doc.isNull()) {
        int num_books = doc.as<JsonObject>().size();
        int list_height = num_books * 40;
        lv_obj_set_size(list, 230, list_height);
        lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 55);

        std::vector<Book> books;

        for(JsonPair kv : doc.as<JsonObject>()) {
                String key = kv.key().c_str();
                String title = kv.value()["titulo"].as<String>();
                long long timestamp = kv.value()["ultima_modificacion"].as<long long>();

                Book book = {title, key, timestamp};
                books.push_back(book);
            }

        // Ordenar los libros por timestamp en orden descendente
        std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
                return a.timestamp > b.timestamp;
            });

        // Mostrar los libros en la lista
        for (const Book& book : books) {
            char* titleCopy = new char[book.title.length() + 1];
            strcpy(titleCopy, book.title.c_str());
            char* keyCopy = new char[book.key.length() + 1];
            strcpy(keyCopy, book.key.c_str());
            lv_obj_t * btn = lv_list_add_btn(list, "\xF3\xB1\x81\xAF", titleCopy);

            lv_obj_t * label = lv_obj_get_child(btn, NULL);
            lv_obj_set_style_text_font(label, &bigger_symbols, 0);

            lv_obj_set_style_text_font(btn, &ubuntu_regular_16, 0);
            lv_obj_set_style_text_color(btn, lv_color_black(), 0);
            lv_obj_set_style_bg_color(btn, lv_color_hex(0xCBECFF), 0);
            lv_obj_add_event_cb(btn, go_to_screen2_tab2, LV_EVENT_CLICKED, keyCopy); // Use the char* version of the key
        }
    } else {
        Serial.println("Failed to retrieve data.");
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

    int posY = -120;

    DynamicJsonDocument doc = get_book_data(key);
    if (!doc.isNull()) {
        String titulo = doc["titulo"].as<String>();
        String autor = doc["autor"].as<String>();
        int paginas_total = doc["paginas_total"].as<int>();
        int pagina_actual = doc["pagina_actual"].as<int>();

        // Crear las etiquetas y mostrar los datos del libro
        lv_obj_t * label_title = lv_label_create(screen2);
        lv_label_set_long_mode(label_title, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_width(label_title, 225);
        lv_label_set_text(label_title, titulo.c_str());
        lv_obj_set_style_text_font(label_title, &bigger_symbols, 0);
        lv_obj_set_style_text_align(label_title, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label_title, LV_ALIGN_CENTER, 0, posY += 30);

        lv_obj_t * label_author = lv_label_create(screen2);
        lv_label_set_long_mode(label_author, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(label_author, 225);
        lv_label_set_text(label_author, ("Autor: " + autor).c_str());
        lv_obj_set_style_text_font(label_author, &ubuntu_regular_16, 0);
        lv_obj_set_style_text_align(label_author, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label_author, LV_ALIGN_CENTER, 0, posY += 40);

        lv_obj_t * label_total_pages = lv_label_create(screen2);
        lv_label_set_long_mode(label_total_pages, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(label_total_pages, 225);
        lv_label_set_text(label_total_pages, (String(paginas_total) + " páginas en total").c_str());
        lv_obj_set_style_text_font(label_total_pages, &ubuntu_italic_16, 0);
        lv_obj_set_style_text_align(label_total_pages, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label_total_pages, LV_ALIGN_CENTER, 0, posY += 25);

        lv_obj_t * label_current_page = lv_label_create(screen2);
        lv_label_set_long_mode(label_current_page, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(label_current_page, 225);
        lv_label_set_text(label_current_page, ("Vas por la página: " + String(pagina_actual)).c_str());;
        lv_obj_set_style_text_font(label_current_page, &ubuntu_bold_16, 0);
        lv_obj_set_style_text_align(label_current_page, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label_current_page, LV_ALIGN_CENTER, 0, posY += 25);
    } else {
        Serial.println("Fallo al recuperar datos de Firebase.");
    }

    lv_obj_t * symbol = lv_label_create(screen2);
    lv_label_set_text(symbol, "\xF3\xB0\xA9\x88");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(screen2, symbol, BUTTON_STYLE_BLUE, back_to_main_menu, 95, 210);
}




//--------------------------------------PESTAÑA 3---------------------------------------------------
static void tab3_content(lv_obj_t * parent) {
    general_title(parent, "ESCANEAR LIBRO", TITLE_STYLE_ORANGE);

    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label, "¿Deseas agregar un nuevo libro a tu biblioteca?\n\nPincha en el botón para abrir la cámara y escanear el ISBN");
    lv_obj_set_style_text_font(label, &ubuntu_regular_16, 0);
    lv_obj_set_width(label, 200);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -10);

    lv_obj_t * symbol = lv_label_create(parent);
    lv_label_set_text(symbol, "\xF3\xB0\x84\x84");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(parent, symbol, BUTTON_STYLE_ORANGE, go_to_screen2_tab3, 75, 190);
}

// Función para crear la pantalla secundaria de la pestaña 3
void create_second_screen_tab3(lv_obj_t *padre) {

    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_scr_load(screen2);

    // Inicializa la interfaz de usuario de la cámara
    setup_scr_camera(&guider_camera_ui);
    // Carga la interfaz de usuario de la cámara en la pantalla
    lv_scr_load(guider_camera_ui.camera);
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
    DynamicJsonDocument doc = get_book_data(); // Get all books

    int total_pages = 0; // Variable para almacenar el total de páginas leídas

    std::vector<Book> books;

    // Create containers for each book status
    lv_obj_t * container_unstarted = lv_obj_create(parent);
    lv_obj_align(container_unstarted, LV_ALIGN_OUT_TOP_LEFT, 0, 290);
    lv_obj_set_size(container_unstarted, 210, 200);
    lv_obj_set_style_bg_color(container_unstarted, lv_color_hex(0xD7ECD5), LV_PART_MAIN);
    lv_obj_set_style_border_color(container_unstarted, lv_color_hex(0x25B619), LV_PART_MAIN);
    lv_obj_set_style_border_width(container_unstarted, 2, LV_PART_MAIN);

    lv_obj_t * container_in_progress = lv_obj_create(parent);
    lv_obj_align(container_in_progress, LV_ALIGN_OUT_TOP_LEFT, 0, 565);
    lv_obj_set_size(container_in_progress, 210, 200);
    lv_obj_set_style_bg_color(container_in_progress, lv_color_hex(0xD7ECD5), LV_PART_MAIN);
    lv_obj_set_style_border_color(container_in_progress, lv_color_hex(0x25B619), LV_PART_MAIN);
    lv_obj_set_style_border_width(container_in_progress, 2, LV_PART_MAIN);

    lv_obj_t * container_finished = lv_obj_create(parent);
    lv_obj_align(container_finished, LV_ALIGN_OUT_TOP_LEFT, 0, 855);
    lv_obj_set_size(container_finished, 210, 200);
    lv_obj_set_style_bg_color(container_finished, lv_color_hex(0xD7ECD5), LV_PART_MAIN);
    lv_obj_set_style_border_color(container_finished, lv_color_hex(0x25B619), LV_PART_MAIN);
    lv_obj_set_style_border_width(container_finished, 2, LV_PART_MAIN);

    // Create labels for each book status
    lv_obj_t * label_unstarted = lv_label_create(container_unstarted);
    lv_label_set_text(label_unstarted, "LIBROS SIN EMPEZAR");
    lv_obj_set_style_text_font(label_unstarted, &ubuntu_bold_16, 0);
    lv_obj_align(label_unstarted, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t * label_in_progress = lv_label_create(container_in_progress);
    lv_label_set_text(label_in_progress, "LIBROS A MEDIAS");
    lv_obj_set_style_text_font(label_in_progress, &ubuntu_bold_16, 0);
    lv_obj_align(label_in_progress, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t * label_finished = lv_label_create(container_finished);
    lv_label_set_text(label_finished, "LIBROS FINALIZADOS");
    lv_obj_set_style_text_font(label_finished, &ubuntu_bold_16, 0);
    lv_obj_align(label_finished, LV_ALIGN_TOP_LEFT, 0, 0);

    // Variables to keep track of the number of books in each status
    int count_unstarted = 0;
    int count_in_progress = 0;
    int count_finished = 0;

    // Iterate through each book
    for(JsonPair kv : doc.as<JsonObject>()) {
        int current_pages = kv.value()["pagina_actual"].as<int>();
        int total_pages_book = kv.value()["paginas_total"].as<int>();
        String title = kv.value()["titulo"].as<String>();
        String key = kv.key().c_str();
        long long timestamp = kv.value()["ultima_modificacion"].as<long long>();

        Book book = {title, key, timestamp};
        books.push_back(book);

        total_pages += current_pages; // Suma las páginas actuales al total

        if(current_pages > max_pages) {
            max_pages = current_pages;
            max_pages_book_title = title;
        }

        // Check the reading status of the book and create the corresponding label
        lv_obj_t * status_label = NULL;
        String status_label_text = title;

        if(current_pages == total_pages_book) {
            status_label = lv_label_create(container_finished);
            lv_obj_align(status_label, LV_ALIGN_TOP_LEFT, 0, 25 * (count_finished + 1));
            count_finished++;
        } else if(current_pages == 0) {
            status_label = lv_label_create(container_unstarted);
            lv_obj_align(status_label, LV_ALIGN_TOP_LEFT, 0, 25 * (count_unstarted + 1));
            count_unstarted++;
        } else {
            float percentage_read = float(current_pages) * 100 / total_pages_book;
            status_label_text += " (" + String(percentage_read) + "% leído)";
            status_label = lv_label_create(container_in_progress);
            lv_obj_align(status_label, LV_ALIGN_TOP_LEFT, 0, 25 * (count_in_progress + 1));
            count_in_progress++;
        }

        lv_label_set_text(status_label, status_label_text.c_str());
        lv_obj_set_style_text_font(status_label, &ubuntu_regular_16, 0);
    }


    Serial.println("\n" + String(count_finished) + "\n");
    Serial.println("\n" + String(count_unstarted) + "\n");
    Serial.println("\n" + String(count_in_progress) + "\n");

    // Create a label to display the actual book
    // Ordenar los libros por timestamp en orden descendente
    std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
        return a.timestamp > b.timestamp;
    });

    // El libro más reciente es el primero en el vector
    String most_recent_book_title = books[0].title;

    lv_obj_t * label1 = lv_label_create(parent);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label1, "Libro actual");
    lv_obj_set_style_text_font(label1, &ubuntu_bold_16, 0);
    lv_obj_set_width(label1, 230);
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -65);

    lv_obj_t * label11 = lv_label_create(parent);
    lv_label_set_long_mode(label11, LV_LABEL_LONG_SCROLL_CIRCULAR);
    String label1_text = most_recent_book_title;
    lv_label_set_text(label11, label1_text.c_str());
    lv_obj_set_style_text_font(label11, &ubuntu_regular_16, 0);
    lv_obj_set_width(label11, 230);
    lv_obj_set_style_text_align(label11, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label11, LV_ALIGN_CENTER, 0, -45);

    // Create a label to display the book with the most pages read
    lv_obj_t * label2 = lv_label_create(parent);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label2, "Libro con más páginas leídas");
    lv_obj_set_style_text_font(label2, &ubuntu_bold_16, 0);
    lv_obj_set_width(label2, 230);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, -12);

    lv_obj_t * label22 = lv_label_create(parent);
    lv_label_set_long_mode(label22, LV_LABEL_LONG_SCROLL_CIRCULAR);
    String label2_text = max_pages_book_title + " (" + String(max_pages) + " páginas)";
    lv_label_set_text(label22, label2_text.c_str());
    lv_obj_set_style_text_font(label22, &ubuntu_regular_16, 0);
    lv_obj_set_width(label22, 230);
    lv_obj_set_style_text_align(label22, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label22, LV_ALIGN_CENTER, 0, 8);

    // Create a label to display the total pages read
    lv_obj_t * label3 = lv_label_create(parent);
    lv_label_set_long_mode(label3, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label3, "Total de páginas leídas (entre todos los libros)");
    lv_obj_set_style_text_font(label3, &ubuntu_bold_16, 0);
    lv_obj_set_width(label3, 230);
    lv_obj_set_style_text_align(label3, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label3, LV_ALIGN_CENTER, 0, 50);

    lv_obj_t * label33 = lv_label_create(parent);
    lv_label_set_long_mode(label33, LV_LABEL_LONG_WRAP);
    String label3_text = String(total_pages) + " páginas";
    lv_label_set_text(label33, label3_text.c_str());
    lv_obj_set_style_text_font(label33, &ubuntu_regular_16, 0);
    lv_obj_set_width(label33, 230);
    lv_obj_set_style_text_align(label33, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label33, LV_ALIGN_CENTER, 0, 80);


    lv_obj_t * down_button1 = lv_btn_create(parent);
    lv_obj_set_size(down_button1, 20, 20); // Set the size of the button
    lv_obj_set_pos(down_button1, 90, 230); // Set the position of the button
    lv_obj_add_event_cb(down_button1, event_handler_scroll1, LV_EVENT_CLICKED, parent); // Set the click event handler

    lv_obj_t * down_symbol1 = lv_label_create(down_button1);
    lv_obj_center(down_symbol1);
    lv_label_set_text(down_symbol1, LV_SYMBOL_DOWN);

    lv_obj_t * down_button2 = lv_btn_create(parent);
    lv_obj_set_size(down_button2, 20, 20); // Set the size of the button
    lv_obj_set_pos(down_button2, 90, 505); // Set the position of the button
    lv_obj_add_event_cb(down_button2, event_handler_scroll2, LV_EVENT_CLICKED, parent); // Set the click event handler

    lv_obj_t * down_symbol2 = lv_label_create(down_button2);
    lv_obj_center(down_symbol2);
    lv_label_set_text(down_symbol2, LV_SYMBOL_DOWN);

    lv_obj_t * down_button3 = lv_btn_create(parent);
    lv_obj_set_size(down_button3, 20, 20); // Set the size of the button
    lv_obj_set_pos(down_button3, 90, 780); // Set the position of the button
    lv_obj_add_event_cb(down_button3, event_handler_scroll3, LV_EVENT_CLICKED, parent); // Set the click event handler

    lv_obj_t * down_symbol3 = lv_label_create(down_button3);
    lv_obj_center(down_symbol3);
    lv_label_set_text(down_symbol3, LV_SYMBOL_DOWN);

    lv_obj_t * up_button = lv_btn_create(parent);
    lv_obj_set_size(up_button, 20, 20); // Set the size of the button
    lv_obj_set_pos(up_button, 90, 830); // Set the position of the button
    lv_obj_add_event_cb(up_button, event_handler_top, LV_EVENT_CLICKED, parent); // Set the click event handler

    lv_obj_t * up_symbol = lv_label_create(up_button);
    lv_obj_center(up_symbol);
    lv_label_set_text(up_symbol, LV_SYMBOL_UP);


    // Create a style for the symbol
    static lv_style_t style_symbol_up_down;
    lv_style_init(&style_symbol_up_down);
    lv_style_set_text_color(&style_symbol_up_down, lv_color_black()); // Set the text color to black
    lv_obj_add_style(down_symbol1, &style_symbol_up_down, 0); // Apply the style to the down symbol
    lv_obj_add_style(down_symbol2, &style_symbol_up_down, 0); // Apply the style to the down symbol
    lv_obj_add_style(down_symbol3, &style_symbol_up_down, 0); // Apply the style to the down symbol
    lv_obj_add_style(up_symbol, &style_symbol_up_down, 0); // Apply the style to the up symbol

    // Set the button style to match the background color
    static lv_style_t style_btn_up_down;
    lv_style_init(&style_btn_up_down);
    lv_style_set_bg_color(&style_btn_up_down, lv_color_hex(0xCEF2D8)); // Set the background color to match the screen
    lv_obj_add_style(down_button1, &style_btn_up_down, 0);
    lv_obj_add_style(down_button2, &style_btn_up_down, 0);
    lv_obj_add_style(down_button3, &style_btn_up_down, 0);
    lv_obj_add_style(up_button, &style_btn_up_down, 0);


    //Botón para ir a pantalla que muestra gráfica
    lv_obj_t * symbol = lv_label_create(parent);
    lv_label_set_text(symbol, "\xF3\xB0\x84\xA8");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(parent, symbol, BUTTON_STYLE_GREEN, go_to_screen2_tab4, 75, 1060);

}



static void event_handler_scroll1(lv_event_t * e) {
    const lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * screen2 = (lv_obj_t *)lv_event_get_user_data(e); // Get the screen object from the user data
        lv_obj_scroll_to_y(screen2, 285, LV_ANIM_ON); // Scroll to the top of the screen
    }
}

static void event_handler_scroll2(lv_event_t * e) {
    const lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * screen2 = (lv_obj_t *)lv_event_get_user_data(e); // Get the screen object from the user data
        lv_obj_scroll_to_y(screen2, 560, LV_ANIM_ON); // Scroll to the top of the screen
    }
}

static void event_handler_scroll3(lv_event_t * e) {
    const lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * screen2 = (lv_obj_t *)lv_event_get_user_data(e); // Get the screen object from the user data
        lv_obj_scroll_to_y(screen2, 840, LV_ANIM_ON); // Scroll to the top of the screen
    }
}


void create_second_screen_tab4(lv_obj_t *padre) {
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xCEF2D8), 0);
    lv_scr_load(screen2);

    lv_obj_t * chart = lv_chart_create(screen2);

    // Incrementa la posición en el eje X en 10px
    lv_obj_set_pos(chart, 55, 370);

    lv_obj_set_size(chart, 160, 180);
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
    DynamicJsonDocument doc = get_book_data(); // Get all books

    // Set the tick labels for the X axis
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 5, 5, doc.size(), 1, true, 50); // Set the tick count to the number of books

    // Set the point count to the number of books
    lv_chart_set_point_count(chart, doc.size());

    int label_y_pos = 30; // Initialize the y position for the first label
    int legend_pos = 20; // Position of the legend labels

    // Create a container for the labels
    lv_obj_t * container = lv_obj_create(screen2);
    lv_obj_set_size(container, 210, 250); // Set the container size
    lv_obj_set_style_bg_color(container, lv_color_hex(0xCEF2D8), LV_PART_MAIN); // Set the background color to match the screen
    lv_obj_set_style_border_color(container, lv_color_hex(0x25B619), LV_PART_MAIN); // Set the border color to green
    lv_obj_set_style_border_width(container, 2, LV_PART_MAIN); // Set the border width
    lv_obj_set_pos(container, 13, legend_pos); // Set the position of the container



    //Botón para desplazar pantalla hacia abajo automáticamente
    lv_obj_t * down_button = lv_btn_create(screen2);
    lv_obj_set_size(down_button, 20, 20); // Set the size of the button
    lv_obj_set_pos(down_button, 110, 280); // Set the position of the button
    lv_obj_add_event_cb(down_button, event_handler_bottom, LV_EVENT_CLICKED, screen2); // Set the click event handler

    // Create a label for the down symbol and add it to the button
    lv_obj_t * down_symbol = lv_label_create(down_button);
    lv_obj_center(down_symbol);
    lv_label_set_text(down_symbol, LV_SYMBOL_DOWN);

    //Botón para desplazar pantalla hacia arriba automáticamente
    lv_obj_t * up_button = lv_btn_create(screen2);
    lv_obj_set_size(up_button, 20, 20); // Set the size of the button
    lv_obj_set_pos(up_button, 110, 330); // Set the position of the button
    lv_obj_add_event_cb(up_button, event_handler_top, LV_EVENT_CLICKED, screen2); // Set the click event handler

    // Create a label for the up symbol and add it to the button
    lv_obj_t * up_symbol = lv_label_create(up_button);
    lv_obj_center(up_symbol);
    lv_label_set_text(up_symbol, LV_SYMBOL_UP);


    // Create a style for the symbol
    static lv_style_t style_symbol_up_down;
    lv_style_init(&style_symbol_up_down);
    lv_style_set_text_color(&style_symbol_up_down, lv_color_black()); // Set the text color to black
    lv_obj_add_style(down_symbol, &style_symbol_up_down, 0); // Apply the style to the down symbol
    lv_obj_add_style(up_symbol, &style_symbol_up_down, 0); // Apply the style to the up symbol

    // Set the button style to match the background color
    static lv_style_t style_btn_up_down;
    lv_style_init(&style_btn_up_down);
    lv_style_set_bg_color(&style_btn_up_down, lv_color_hex(0xCEF2D8)); // Set the background color to match the screen
    lv_obj_add_style(down_button, &style_btn_up_down, 0);
    lv_obj_add_style(up_button, &style_btn_up_down, 0);


    lv_obj_t * label = lv_label_create(container);
    lv_label_set_text(label, "-----------LEYENDA-----------");
    lv_obj_set_style_text_font(label, &ubuntu_bold_16, 0);
    lv_obj_set_pos(label, 0, 0);

    // For each book, calculate the percentage of pages read and add a series to the chart
    for(JsonPair kv : doc.as<JsonObject>()) {
        String book_key = kv.key().c_str(); // Get the book key
        book_keys.push_back(book_key.c_str()); // Store the book key for later use
        JsonObject libro = kv.value().as<JsonObject>();

        // Calculate the percentage of pages read
        int paginas_total = libro["paginas_total"].as<int>();
        int pagina_actual = libro["pagina_actual"].as<int>();
        float porcentaje = ((float)pagina_actual / paginas_total) * 100;

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

        // Create a label for the book key and add it to the container
        lv_obj_t * label_key = lv_label_create(container);
        lv_label_set_text(label_key, book_key.c_str());
        lv_obj_set_style_text_font(label_key, &ubuntu_bold_16, 0);
        lv_obj_set_pos(label_key, 0, label_y_pos); // Position the label at the current y position

        // Create a label for the book title and add it to the container
        lv_obj_t * label_title = lv_label_create(container);
        lv_label_set_text(label_title, libro["titulo"].as<const char*>());
        lv_obj_set_style_text_font(label_title, &ubuntu_regular_16, 0);
        lv_obj_set_pos(label_title, 0, label_y_pos + 20); // Position the label 20px below the key label

        label_y_pos += 45;

        book_index++; // Increment the book index
    }

    lv_chart_refresh(chart); // Required after direct set

    lv_obj_t * symbol = lv_label_create(screen2);
    lv_label_set_text(symbol, "\xF3\xB0\xA9\x88");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(screen2, symbol, BUTTON_STYLE_GREEN, back_to_main_menu, 95, 580);

    lv_obj_t * space = lv_label_create(screen2);
    lv_label_set_text(space, "\n\n");
    lv_obj_align(space, LV_ALIGN_TOP_MID, 0, 590);
}



// Manejador de eventos para el botón que cambia a la pantalla secundaria de tab4
void go_to_screen2_tab4(lv_event_t * e) {
    lv_obj_t * main_screen = lv_scr_act(); // Obtén la pantalla principal (donde están las tabs)
    scr_principal = main_screen;
    create_second_screen_tab4(main_screen);
}


static void event_handler_bottom(lv_event_t * e) {
    const lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * screen2 = (lv_obj_t *)lv_event_get_user_data(e); // Get the screen object from the user data
        lv_obj_scroll_to_y(screen2, lv_obj_get_height(screen2), LV_ANIM_ON); // Scroll to the bottom of the screen
    }
}

static void event_handler_top(lv_event_t * e) {
    const lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * screen2 = (lv_obj_t *)lv_event_get_user_data(e); // Get the screen object from the user data
        lv_obj_scroll_to_y(screen2, 0, LV_ANIM_ON); // Scroll to the top of the screen
    }
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




