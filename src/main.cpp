#include <Arduino.h>
#include "display.h"
#include <lvgl.h>
#include "Freenove_WS2812_Lib_for_ESP32.h"
#include "lv_img.h"
#include "firebase_config.h"
#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>

// El fabricante me indica que este es el modelo de cámara del kit
#define CAMERA_MODEL_ESP32S3_EYE \
  {                                 \
    .PWDN_GPIO_NUM = -1,             \
    .RESET_GPIO_NUM = -1,           \
    .XCLK_GPIO_NUM = 15,            \
    .SIOD_GPIO_NUM = 4,            \
    .SIOC_GPIO_NUM = 5,            \
    .Y9_GPIO_NUM = 16,              \
    .Y8_GPIO_NUM = 17,              \
    .Y7_GPIO_NUM = 18,              \
    .Y6_GPIO_NUM = 12,              \
    .Y5_GPIO_NUM = 10,               \
    .Y4_GPIO_NUM = 8,              \
    .Y3_GPIO_NUM = 9,              \
    .Y2_GPIO_NUM = 11,              \
    .VSYNC_GPIO_NUM = 6,           \
    .HREF_GPIO_NUM = 7,            \
    .PCLK_GPIO_NUM = 13,            \
  }

#include <ESP32QRCodeReader.h>

// Definición aquí (en lugar de platformio.ini) para que no de error al definir el strip
// Para controlar el LED WS2812
#define LEDS_COUNT 1
#define LEDS_PIN 48
#define CHANNEL 0

//-------------------------DECLARACIÓN DE VARIABLES Y FUNCIONES------------------------------------
Display screen;

ESP32QRCodeReader reader(CAMERA_MODEL_ESP32S3_EYE);
String qrCodeContentGlobal;
TaskHandle_t qrCodeTaskHandle;
static int qr_task_flag = 0;
void onQrCodeTask(void *pvParameters);
void create_qr_task();
void stop_qr_task();
bool qrCodeFound = false;

bool book_found = false;
String book_key;
String title;
String author;
int totalPages;
int currentPage;
long long timestamp;

//Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);

lv_obj_t * scr_principal;

void tab_function();
void back_to_main_menu(lv_event_t * e);
void tabview_event_handler(lv_event_t * e);

void tab1_content(lv_obj_t * parent);
void create_second_screen_tab1(lv_obj_t * parent);
void go_to_screen2_tab1(lv_event_t * e);

void tab2_content(lv_obj_t * parent);
void create_second_screen_tab2(lv_obj_t * parent, const std::string & key);
void go_to_screen2_tab2(lv_event_t * e);

void tab3_content(lv_obj_t * parent);
void create_second_screen_tab3(lv_obj_t * parent);
void go_to_screen2_tab3(lv_event_t * e);
void searchIsbnInDatabase(const String& qrCodeContent);
void keyboard_event_cb(lv_event_t * e);
void create_keyboard_screen(lv_obj_t * parent);
void show_numeric_keyboard(lv_obj_t * label);

void tab4_content(lv_obj_t * parent);
void create_second_screen_tab4(lv_obj_t * parent);
void go_to_screen2_tab4(lv_event_t * e);
void event_handler_scroll1(lv_event_t * e);
void event_handler_scroll2(lv_event_t * e);
void event_handler_scroll3(lv_event_t * e);
void event_handler_bottom(lv_event_t * e);
void event_handler_top(lv_event_t * e);
void draw_label_x_axis(lv_event_t * e);
void draw_label_y_axis(lv_event_t * e);

// Enumeración para los diferentes estilos de títulos
typedef enum {
    TITLE_STYLE_PURPLE,
    TITLE_STYLE_BLUE,
    TITLE_STYLE_ORANGE,
    TITLE_STYLE_GREEN
} title_style;
void general_title(lv_obj_t * parent, const char * titulo, title_style style);

// Enumeración para los diferentes estilos de botones
typedef enum {
    BUTTON_STYLE_PURPLE,
    BUTTON_STYLE_BLUE,
    BUTTON_STYLE_ORANGE,
    BUTTON_STYLE_GREEN
} button_style;
void create_button(lv_obj_t * parent, lv_obj_t * label, button_style style, lv_event_cb_t event_cb, lv_coord_t pos_x, lv_coord_t pos_y);


//----------------------------------SETUP------------------------------------
void setup() {
    // Inicialización de la comunicación serial a 115200 baudios (comunicación entre la placa y PC a través del puerto serie)
    Serial.begin(115200);

    // Inicialización de la pantalla
    screen.init();

    // Inicialización de Firebase (conexión con base de datos)
    setup_firebase();

    // Configuración de pestañas interfaz gráfica
    tab_function();

    // Inicialización del lector de QR
    reader.setup();
    Serial.println("Setup QRCode Reader");

    // Inicialización del LED
    //strip.begin();
    //strip.setBrightness(255);
}



//------------------------------------LOOP------------------------------------------
void loop() {
    // Para mantener la interfaz gráfica actualizada
    screen.routine();
    delay(5);

    // Cuando se decodifica el QR se busca en la base de datos y se muestra la información
    if (qrCodeFound) {
        qrCodeFound = false;
        Serial.println(qrCodeContentGlobal);
        searchIsbnInDatabase(qrCodeContentGlobal);
        create_keyboard_screen(lv_scr_act());
    }
}



//---------------------------DEFINICIÓN DE FUNCIONES------------------------------------
lv_obj_t * tab1;
lv_obj_t * tab2;
lv_obj_t * tab3;
lv_obj_t * tab4;

void tab_function()
{
    // Crear un objeto de vista de pestañas
    lv_obj_t * tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 35);

    // Agregar 4 pestañas al objeto de vista de pestañas
    tab1 = lv_tabview_add_tab(tabview, "\xF3\xB0\x8B\x9C");
    tab2 = lv_tabview_add_tab(tabview, "\xF3\xB1\x89\x9F");
    tab3 = lv_tabview_add_tab(tabview, "\xF3\xB0\x81\xB2");
    tab4 = lv_tabview_add_tab(tabview, "\xF3\xB0\x84\xA8");

    // Obtener los botones de pestañas para aplicarles estilo de fuente
    lv_obj_t * tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_text_font(tab_btns, &bigger_symbols, 0);

    // Estilo pantalla principal pestaña 1
    lv_obj_set_style_radius(tab1, 4,0);
    lv_obj_set_style_bg_opa(tab1, LV_OPA_30, 0);
    lv_obj_set_style_bg_color(tab1, lv_palette_lighten(LV_PALETTE_PURPLE, 1), 0);
    lv_obj_set_style_border_width(tab1, 4, 0);
    lv_obj_set_style_border_color(tab1, lv_color_make(155, 4, 207), 0);

    // Estilo pantalla principal pestaña 2
    lv_obj_set_style_radius(tab2, 4,0);
    lv_obj_set_style_bg_opa(tab2, LV_OPA_30, 0);
    lv_obj_set_style_bg_color(tab2, lv_color_hex(0x6CE0FF), 0);
    lv_obj_set_style_border_width(tab2, 4, 0);
    lv_obj_set_style_border_color(tab2, lv_color_make(10, 154, 254), 0);

    // Estilo pantalla principal pestaña 3
    lv_obj_set_style_radius(tab3, 4,0);
    lv_obj_set_style_bg_opa(tab3, LV_OPA_60, 0);
    lv_obj_set_style_bg_color(tab3, lv_palette_lighten(LV_PALETTE_ORANGE, 1), 0);
    lv_obj_set_style_border_width(tab3, 4, 0);
    lv_obj_set_style_border_color(tab3, lv_color_make(255, 104, 0), 0);

    // Estilo pantalla principal pestaña 4
    lv_obj_set_style_radius(tab4, 4,0);
    lv_obj_set_style_bg_opa(tab4, LV_OPA_30, 0);
    lv_obj_set_style_bg_color(tab4, lv_palette_lighten(LV_PALETTE_LIGHT_GREEN, 1), 0);
    lv_obj_set_style_border_width(tab4, 4, 0);
    lv_obj_set_style_border_color(tab4, lv_color_make(30, 217, 0), 0);

    // Funciones para el contenido de las pestañas
    tab1_content(tab1);
    tab2_content(tab2);
    tab3_content(tab3);
    tab4_content(tab4);

    // Agrega un manejador de eventos a la vista de pestañas para manejar el cambio de pestaña
    lv_obj_add_event_cb(tabview, tabview_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
}

// Para actulizar el contenido de la pantalla principal de las pestañas 2 y 4
void tabview_event_handler(lv_event_t * e) {
    // Obtener la vista de pestañas que disparó el evento
    lv_obj_t * tabview = lv_event_get_target(e);

    // Obtener la pestaña activa (seleccionada)
    uint16_t tab = lv_tabview_get_tab_act(tabview);

    // Si la pestaña 2 está seleccionada (los índices de las pestañas comienzan en 0)
    if (tab == 1) {
        // Limpiar el contenido para volver a cargarlo actualizado
        lv_obj_clean(tab2);
        tab2_content(tab2);
        //Serial.println("Tab 2");
    }
    // Si la pestaña 4 está seleccionada
    else if (tab == 3) {
        lv_obj_clean(tab4);
        tab4_content(tab4); // Llama a tab4_content
        //Serial.println("Tab 4");
    }
}



//---------------------------------Funciones generales------------------------------------------
// Función a la que llaman los botones de las pantallas secundarias para volver a la principal de la tab activa
void back_to_main_menu(lv_event_t * e) {
    lv_obj_t * current_screen = lv_obj_get_parent(lv_event_get_target(e)); // Obtener la pantalla actual (secundaria)
    lv_scr_load(scr_principal); // Cargar la pantalla principal (donde están las tabs)
    lv_obj_del(current_screen); // Eliminar la pantalla secundaria
}


// Función para crear un título con estilo de color específico
void general_title(lv_obj_t * parent, const char * titulo, title_style style) {
    static lv_style_t style_title; // Estilo para el título
    static lv_style_t style_title_purple, style_title_blue, style_title_orange, style_title_green; // Estilo para los colores

    // Los estilos se inicializan una sola vez
    static bool styles_initialized = false; // ahora es false pero luego será siempre true y el if solo se ejecuta la primera vez (en false)
    if (!styles_initialized) {
        //Serial.println("SE ESTAN INICIANDO LOS TITULOS...");

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

    lv_obj_t * label = lv_label_create(parent); // Crear una etiqueta en la pantalla activa
    lv_label_set_text(label, titulo); // Establecer el texto de la etiqueta
    lv_obj_remove_style_all(label); // Eliminar estilos previos

    lv_obj_set_style_text_font(label, &ubuntu_bold_16, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 5);

    // Configurar el estilo del color del título basado en el estilo seleccionado
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

    lv_obj_add_style(label, &style_title, 0); // Aplica el nuevo estilo general para todos los títulos
}


// Función para crear un botón con estilo de color específico
void create_button(lv_obj_t * parent, lv_obj_t * label, button_style style, lv_event_cb_t event_cb, lv_coord_t pos_x, lv_coord_t pos_y) {
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
        lv_style_set_translate_y(&style_btn_pressed, 5); //baja 5 px cuando se presiona el botón

        lv_style_init(&style_btn);
        lv_style_set_radius(&style_btn, 10);
        lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
        lv_style_set_border_width(&style_btn, 2);
        lv_style_set_border_opa(&style_btn, LV_OPA_50);

        styles_initialized = true;
    }

    // Crear el botón
    lv_obj_t * btn = lv_btn_create(parent);

    // Eliminar estilos previos
    lv_obj_remove_style_all(btn);

    switch (style) {
        case BUTTON_STYLE_PURPLE:
            lv_obj_add_style(btn, &style_purple, 0);
            break;
        case BUTTON_STYLE_BLUE:
            lv_obj_add_style(btn, &style_blue, 0);
            break;
        case BUTTON_STYLE_ORANGE:
            lv_obj_add_style(btn, &style_orange, 0);
            break;
        case BUTTON_STYLE_GREEN:
            lv_obj_add_style(btn, &style_green, 0);
            break;
    }

    // Aplicar estilos comunes a todos los botones
    lv_obj_add_style(btn, &style_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_style(btn, &style_btn, 0);

    // Aplicar la etiqueta al botón
    lv_obj_set_parent(label, btn);
    lv_obj_center(label);

    // Configurar tamaño del botón
    lv_obj_set_size(btn, 50, 50);

    // Configurar la posición modificable del botón
    lv_obj_set_pos(btn, pos_x, pos_y);

    // Evento de presionado
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);
}







//--------------------------------------PESTAÑA 1---------------------------------------------------
// Pantalla principal de la pestaña 1 (es estática)
void tab1_content(lv_obj_t * parent) {
    // Título
    general_title(parent, "¡BIENVENIDO!", TITLE_STYLE_PURPLE);

    // Texto de bienvenida
    lv_obj_t * label1 = lv_label_create(parent);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP); // si el texto es muy largo se pasa a la línea siguiente
    lv_label_set_text(label1, "Yo seré tu asistente de lectura :)\n\n¿Quieres saber cómo funciono?");
    lv_obj_set_style_text_font(label1, &ubuntu_regular_16, 0);
    lv_obj_set_width(label1, 160);
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -20);

    // Botón con símbolo para ir a la pantalla secundaria de la pestaña 1
    lv_obj_t * symbol = lv_label_create(parent);
    lv_label_set_text(symbol, "\xF3\xB0\xB3\xBD");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(parent, symbol, BUTTON_STYLE_PURPLE, go_to_screen2_tab1, 75, 180);
}

// Nueva pantalla con información adicional sobre el funcionamiento de la interfaz de usuario
void create_second_screen_tab1(lv_obj_t * parent) {
    lv_obj_t * screen2 = lv_obj_create(NULL); // Objeto que se utilizará para cargar la nueva pantalla
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xE9AAFF), 0);
    lv_scr_load(screen2); // Cargar la nueva pantalla

    // Definir los textos de las etiquetas y sus posiciones Y en un array
    const char * textos[] = {
            "#8035A8 ¡HOLA!# " "#8035A8 \xEE\xAD\x94#",
            "Soy una herramienta para que puedas registrar de manera interactiva tus lecturas a través de mi cámara",
            "---------------------------------------",
            "\xF3\xB1\x89\x9F",
            "Aquí podrás almacenar los libros que estés leyendo",
            "\xF3\xB0\x81\xB2",
            "Aquí podrás registrar tus nuevos libros. Para ello deberás mostrar a la cámara el QR asignado a cada libro",
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

    // Botón con símbolo para ir a la pantalla principal de la pestaña 1
    lv_obj_t * symbol = lv_label_create(screen2);
    lv_label_set_text(symbol, "\xF3\xB0\xA9\x88");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(screen2, symbol, BUTTON_STYLE_PURPLE, back_to_main_menu, 95, 510);

    lv_obj_t * space = lv_label_create(screen2);
    lv_label_set_text(space, "\n\n");
    lv_obj_align(space, LV_ALIGN_TOP_MID, 0, 510);
}

// Manejador de eventos para el botón que cambia a la pantalla secundaria de tab1
void go_to_screen2_tab1(lv_event_t * e) {
    lv_obj_t * main_screen = lv_scr_act(); //Obtener la pantalla principal (donde están las tabs)
    scr_principal = main_screen; //Guardar la pantalla principal en una variable global
    create_second_screen_tab1(main_screen); //Cargar la pantalla secundaria
}






//--------------------------------------PESTAÑA 2---------------------------------------------------
// Generación de estructura para hacer más fácil el manejo de los libros para ordenarlos
struct Book {
    String title;
    String book_key;
    long long timestamp;
};

// Pantalla principal de la pestaña 2 (es dinámica y está sincronizada con la base de datos)
void tab2_content(lv_obj_t * parent) {
    // Título
    general_title(parent, "MIS LIBROS", TITLE_STYLE_BLUE);

    // Crear la lista donde se mostrarán los libros
    lv_obj_t * list = lv_list_create(parent);

    // Estilo para la lista
    static lv_style_t style_blue;
    lv_style_init(&style_blue);
    lv_style_set_bg_color(&style_blue, lv_color_hex(0xCBECFF));
    lv_style_set_border_width(&style_blue, 0);
    lv_style_set_border_color(&style_blue, lv_color_make(10, 154, 254));
    lv_style_set_radius(&style_blue, 1);

    // Aplicar el estilo a la lista
    lv_obj_add_style(list, &style_blue , LV_STATE_DEFAULT);

    // Obtener los datos de los libros de la base de datos
    DynamicJsonDocument doc = get_book_data();
    if (!doc.isNull()) {
        // Obtener el número de libros y calcular la altura de la lista
        int num_books = doc.as<JsonObject>().size();
        int list_height = num_books * 40;
        lv_obj_set_size(list, 230, list_height);
        lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 55);

        // Vector para almacenar los libros
        std::vector<Book> books;

        // Recorrer los datos obtenidos y crear un objeto Book para cada libro
        for(JsonPair kv : doc.as<JsonObject>()) {
            book_key = kv.key().c_str();
            title = kv.value()["titulo"].as<String>();
            timestamp = kv.value()["ultima_modificacion"].as<long long>();

            Book book = {title, book_key, timestamp};
            books.push_back(book); // va añadiendo los libros al vector
        }

        // Ordenar los libros por timestamp en orden descendente
        std::sort(books.begin(), books.end(), [](const Book & a, const Book & b) {
            return a.timestamp > b.timestamp;
        });

        // Mostrar los libros en la lista
        for (const Book & book : books) {
            char * titleCopy = new char[book.title.length() + 1]; // Reservar espacio para una copia del título del libro, incluyendo espacio para el carácter nulo de terminación
            strcpy(titleCopy, book.title.c_str()); // Copiar el título del libro a la nueva memoria reservada
            char * keyCopy = new char[book.book_key.length() + 1]; // Reservar espacio para una copia de la clave del libro, incluyendo espacio para el carácter nulo de terminación
            strcpy(keyCopy, book.book_key.c_str()); // Copiar la clave del libro a la nueva memoria reservada
            lv_obj_t * btn = lv_list_add_btn(list, "\xF3\xB1\x81\xAF", titleCopy); // Crear un botón en la lista con el título del libro

            // Configurar el estilo del botón
            lv_obj_t * label = lv_obj_get_child(btn, 0);
            lv_obj_set_style_text_font(label, &bigger_symbols, 0);

            lv_obj_set_style_text_font(btn, &ubuntu_regular_16, 0);
            lv_obj_set_style_text_color(btn, lv_color_black(), 0);
            lv_obj_set_style_bg_color(btn, lv_color_hex(0xCBECFF), 0);

            // Agregar un manejador de eventos al botón para que llame a go_to_screen2_tab2 cuando se haga clic en él
            lv_obj_add_event_cb(btn, go_to_screen2_tab2, LV_EVENT_CLICKED, keyCopy); // Use the char* version of the key
        }
    } else {
        Serial.println("Fallo al recuperar datos.");
    }

}

// Manejador de eventos para el botón que cambia a la pantalla secundaria de tab2
void go_to_screen2_tab2(lv_event_t * e) {
    // Obtener la pantalla principal (donde están las tabs)
    lv_obj_t * main_screen = lv_scr_act();
    scr_principal = main_screen;

    // Obtener la clave del libro desde el evento
    /* Recupera los datos de usuario asociados a un evento y los convierte a una cadena de caracteres
       Primero, se realiza un casting estático para convertir el puntero a void devuelto por lv_event_get_user_data(e) a un puntero a char
       Luego, se utiliza el constructor de std::string para convertir el puntero a char en una cadena de caracteres de C++
       El resultado se almacena en la variable 'key'*/
    std::string key = std::string(static_cast<char*>(lv_event_get_user_data(e)));

    // Llamar a la función para crear la segunda pantalla pasando la key del libro seleccionado de la lista
    create_second_screen_tab2(main_screen, key);
}


// Crear la pantalla secundaria de la pestaña 2 con los detalles del libro seleccionado
void create_second_screen_tab2(lv_obj_t * parent, const std::string & key) {
    // Crear una nueva pantalla y cargarla
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xCBECFF), 0);
    lv_scr_load(screen2);

    // Decoración con símbolos arriba y abajo de la pantalla
    lv_obj_t * label_symbols1 = lv_label_create(screen2);
    std::string symbols = "";
    for(int i = 0; i < 9; i++) {
        symbols += " \xEE\xB7\xA2  ";
    }
    lv_label_set_text(label_symbols1, symbols.c_str());
    lv_obj_set_style_text_font(label_symbols1, &ubuntu_regular_16, 0);
    lv_obj_set_pos(label_symbols1, 0, 10);

    lv_obj_t * label_symbols2 = lv_label_create(screen2);
    lv_label_set_text(label_symbols2, symbols.c_str());
    lv_obj_set_style_text_font(label_symbols2, &ubuntu_regular_16, 0);
    lv_obj_set_pos(label_symbols2, 0, 290);

    int posY = -120;

    // Obtener los datos del libro seleccionado de la base de datos
    DynamicJsonDocument doc = get_book_data(key);
    if (!doc.isNull()) {
        title = doc["titulo"].as<String>();
        author = doc["autor"].as<String>();
        totalPages = doc["paginas_total"].as<int>();
        currentPage = doc["pagina_actual"].as<int>();

        // Crear las etiquetas y mostrar los datos del libro
        lv_obj_t * label_title = lv_label_create(screen2);
        lv_label_set_long_mode(label_title, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_width(label_title, 225);
        lv_label_set_text(label_title, title.c_str());
        lv_obj_set_style_text_font(label_title, &bigger_symbols, 0);
        lv_obj_set_style_text_align(label_title, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label_title, LV_ALIGN_CENTER, 0, posY += 30);

        lv_obj_t * label_author = lv_label_create(screen2);
        lv_label_set_long_mode(label_author, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(label_author, 225);
        lv_label_set_text(label_author, ("Autor: " + author).c_str());
        lv_obj_set_style_text_font(label_author, &ubuntu_regular_16, 0);
        lv_obj_set_style_text_align(label_author, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label_author, LV_ALIGN_CENTER, 0, posY += 40);

        lv_obj_t * label_total_pages = lv_label_create(screen2);
        lv_label_set_long_mode(label_total_pages, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(label_total_pages, 225);
        lv_label_set_text(label_total_pages, (String(totalPages) + " páginas en total").c_str());
        lv_obj_set_style_text_font(label_total_pages, &ubuntu_italic_16, 0);
        lv_obj_set_style_text_align(label_total_pages, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label_total_pages, LV_ALIGN_CENTER, 0, posY += 25);

        lv_obj_t * label_current_page = lv_label_create(screen2);
        lv_label_set_long_mode(label_current_page, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(label_current_page, 225);
        lv_label_set_text(label_current_page, ("Vas por la página: " + String(currentPage)).c_str());;
        lv_obj_set_style_text_font(label_current_page, &ubuntu_bold_16, 0);
        lv_obj_set_style_text_align(label_current_page, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label_current_page, LV_ALIGN_CENTER, 0, posY += 25);
    } else {
        Serial.println("Fallo al recuperar datos de Firebase.");
    }

    // Botón con símbolo para volver a la pantalla principal de la pestaña 2
    lv_obj_t * symbol = lv_label_create(screen2);
    lv_label_set_text(symbol, "\xF3\xB0\xA9\x88");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(screen2, symbol, BUTTON_STYLE_BLUE, back_to_main_menu, 95, 210);
}






//--------------------------------------PESTAÑA 3---------------------------------------------------
// Pantalla principal de la pestaña 3 (es estática)
void tab3_content(lv_obj_t * parent) {
    // Título
    general_title(parent, "ESCANEAR LIBRO", TITLE_STYLE_ORANGE);

    // Texto explicativo
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label, "¿Quieres actualizar la página en la que te has quedado?\n\nPincha en el botón para abrir la cámara y escanear el libro");
    lv_obj_set_style_text_font(label, &ubuntu_regular_16, 0);
    lv_obj_set_width(label, 220);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -10);

    // Botón con símbolo para ir a la pantalla secundaria de la pestaña 3 (escanear QR)
    lv_obj_t * symbol = lv_label_create(parent);
    lv_label_set_text(symbol, "\xF3\xB0\x84\x84");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(parent, symbol, BUTTON_STYLE_ORANGE, go_to_screen2_tab3, 75, 190);
}


// Manejador de eventos para el botón que cambia a la pantalla secundaria de tab3
void go_to_screen2_tab3(lv_event_t * e) {
    lv_obj_t * main_screen = lv_scr_act(); // Obtener la pantalla principal (donde están las tabs)
    scr_principal = main_screen; // Guardar la pantalla principal en una variable global
    create_second_screen_tab3(main_screen); // Cargar la pantalla secundaria (donde se escanea el QR)
}


// Función para crear la pantalla secundaria de la pestaña 3 (escanear QR)
void create_second_screen_tab3(lv_obj_t * parent) {
    // Crear una nueva pantalla y cargarla
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xFFCE7E), 0);
    lv_scr_load(screen2);

    // Activar el lector de códigos QR
    reader.begin();
    Serial.println("Begin ESP32QRCodeReader");

    // Verificar si la tarea ya existe antes de intentar crearla
    if (qrCodeTaskHandle == NULL) {
        // Inicia la tarea de escaneo de códigos QR
        Serial.println("Iniciando tarea de escaneo de QR");
        create_qr_task();
    } else {
        Serial.println("La tarea de escaneo de QR ya estaba en ejecucion");
        stop_qr_task();
        create_qr_task();
    }

    // Crear etiquetas con texto y estilos
    lv_obj_t * label1 = lv_label_create(screen2);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);
    lv_label_set_recolor(label1, true);
    lv_label_set_text(label1, "#FF7C00 Muestra el QR del libro a la #FF7C00 cámara#");
    lv_obj_set_style_text_font(label1, &ubuntu_bold_16, 0);
    lv_obj_set_width(label1, 230);
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -95);

    // Símbolos decorativos arriba y abajo de la pantalla en cada esquina
    lv_obj_t * label2 = lv_label_create(screen2);
    lv_label_set_recolor(label2, true);
    lv_label_set_text(label2, (String("#FF7C00  \xEE\xAB\x9A                     #" "#FF7C00          \xEE\xAA\xA4#")).c_str());
    lv_obj_set_style_text_font(label2, &bigger_symbols, 0);
    lv_obj_set_pos(label2, 0, 0);

    lv_obj_t * label3 = lv_label_create(screen2);
    lv_label_set_recolor(label3, true);
    lv_label_set_text(label3, (String("#FF7C00  \xEE\xAA\xA4                      #" "#FF7C00         \xEE\xAB\x9A#")).c_str());
    lv_obj_set_style_text_font(label3, &bigger_symbols, 0);
    lv_obj_set_pos(label3, 0, 290);

    // Imagen explicativa
    show_image(screen2);

    // Botón con símbolo para volver a la pantalla principal de la pestaña 3 (si no se desea escanear ningún libro)
    lv_obj_t * symbol = lv_label_create(screen2);
    lv_label_set_text(symbol, "\xF3\xB0\xA9\x88");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(screen2, symbol, BUTTON_STYLE_ORANGE, back_to_main_menu, 95, 340);
}


// Crear la tarea de escaneo de QR
void create_qr_task() {
    if (qr_task_flag == 0) {
        qr_task_flag = 1;
        xTaskCreate(onQrCodeTask, "onQrCode", 4 * 1024, NULL, 4, &qrCodeTaskHandle);
    } else {
        Serial.println("onQrCodeTask is running...");
    }
}


// Detener la tarea de escaneo de QR (espera hasta que la tarea se elimine)
void stop_qr_task() {
    if (qr_task_flag == 1) {
        qr_task_flag = 0;

        while (1) {
            if (eTaskGetState(qrCodeTaskHandle) == eDeleted) {
                break;
            }
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        Serial.println("onQrCodeTask deleted!");
    }
}


// Escanear el código QR, decodificarlo y almacenar el contenido en una variable global
void onQrCodeTask(void *pvParameters) {
    Serial.println("Iniciando escaneo de QR");

    struct QRCodeData qrCodeData;

    qrCodeContentGlobal = ""; // Limpia de la variable global

    //strip.setLedColorData(0, 255, 255, 255); // Color blanco
    //strip.show(); // Actualiza los LEDs

    while (qr_task_flag) {
        // Si se recibe un código QR, decodifica y almacena el contenido en la variable global
        if (reader.receiveQrCode(&qrCodeData, 100)) {
            Serial.println("Found QRCode");

            // Si el código QR es válido, lo guarda en la variable global y
            if (qrCodeData.valid) {
                Serial.print("Payload: ");
                Serial.println((const char *)qrCodeData.payload);

                // Guarda el contenido del código QR en la variable global
                qrCodeContentGlobal = String((const char *)qrCodeData.payload);
                Serial.println("Contenido decodificado " + qrCodeContentGlobal);

                qrCodeFound = true;  // Para entrar en el condicional del loop y poder buscar en la base de datos y mostrar la siguiente pantalla
                qr_task_flag = 1; // Para detener la tarea

                //strip.setLedColorData(0, 0, 0, 0); // Apaga el LED
                //strip.show(); // Actualiza los LEDs
            } else {
                Serial.print("Invalid: ");
                Serial.println((const char *)qrCodeData.payload);
            }
        }
    }
    vTaskDelete(qrCodeTaskHandle); // Elimina la tarea
}


// Buscar en la base de datos si existe el ISBN decodificado del código QR
void searchIsbnInDatabase(const String & qrCodeContentGlobal) {
    // Guardar el contenido del código QR en una variable
    String qrIsbn = qrCodeContentGlobal;

    // De momento el libro no se ha encontrado
    book_found = false;

    // Crear un documento JSON dinámico para almacenar los datos de los libros
    DynamicJsonDocument doc = get_book_data();

    // Recorrer todos los libros en la base de datos y comparar el ISBN de cada libro con el ISBN decodificado
    for(JsonPair kv : doc.as<JsonObject>()) {
        String isbn = kv.value()["isbn"].as<String>();
        if (isbn == qrIsbn) {
            book_found = true;

            // Almacenar la clave del libro para poder recuperar los datos del libro
            book_key = kv.key().c_str();

            // Almacenar los datos del libro encontrado
            title = kv.value()["titulo"].as<String>();
            author = kv.value()["autor"].as<String>();
            totalPages = kv.value()["paginas_total"].as<int>();
            currentPage = kv.value()["pagina_actual"].as<int>();

            /*
            Serial.println("Libro encontrado:");
            Serial.println("Titulo: " + title);
            Serial.println("Autor: " + author);
            Serial.println("Paginas totales: " + String(totalPages));
            Serial.println("Pagina actual: " + String(currentPage));*/
            break; // Salir del bucle si se encuentra el libro
        }
    }

    // Si no se encontró el libro, imprime "Libro no encontrado"
    if (!book_found) {
        Serial.println("Libro no encontrado");
    }

}


// Crear la pantalla de la pestaña 3 con los datos del libro escaneado para modificar la página actual
void create_keyboard_screen(lv_obj_t * parent) {
    // Crear una nueva pantalla y cargarla
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xFFCE7E), 0);
    lv_scr_load(screen2);

    if(book_found) {
        // Crear etiquetas con los datos del libro
        lv_obj_t * label1 = lv_label_create(screen2);
        lv_label_set_text(label1, title.c_str());
        lv_obj_set_style_text_font(label1, &ubuntu_bold_16, 0);
        lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 20);

        lv_obj_t * label2 = lv_label_create(screen2);
        lv_label_set_text(label2, author.c_str());
        lv_obj_set_style_text_font(label2, &ubuntu_regular_16, 0);
        lv_obj_align(label2, LV_ALIGN_TOP_MID, 0, 40);

        lv_obj_t * label3 = lv_label_create(screen2);
        lv_label_set_text(label3, (String(totalPages) + " páginas").c_str());
        lv_obj_set_style_text_font(label3, &ubuntu_italic_16, 0);
        lv_obj_align(label3, LV_ALIGN_TOP_MID, 0, 60);

        lv_obj_t * label4 = lv_label_create(screen2);
        lv_label_set_text(label4, ("Página anterior: " + String(currentPage)).c_str());
        lv_obj_set_style_text_font(label4, &ubuntu_regular_16, 0);
        lv_obj_align(label4, LV_ALIGN_TOP_MID, 0, 85);

        lv_obj_t * label5 = lv_label_create(screen2);
        lv_obj_align(label5, LV_ALIGN_TOP_MID, 0, 110);
        lv_label_set_text(label5, "¿En qué página te encuentras?");
        lv_obj_set_style_text_font(label5, &ubuntu_regular_16, 0);

        // Asociación de la etiqueta con el teclado numérico para que cuando el usuario introduzca el número de página, se actualice la etiqueta
        show_numeric_keyboard(label5);
    } else {
        // Si no se encuentra el libro
        lv_obj_t * label = lv_label_create(screen2);
        lv_label_set_text(label, "         LIBRO\n           NO\nENCONTRADO");
        lv_obj_set_style_text_font(label, &bigger_symbols, 0);
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 40);

        // Crear botón para regresar a la pantalla principal de la pestaña 3
        lv_obj_t * symbol = lv_label_create(screen2);
        lv_label_set_text(symbol, "\xF3\xB0\xA9\x88");
        lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

        create_button(screen2, symbol, BUTTON_STYLE_ORANGE, back_to_main_menu, 95, 160);
    }
}


// Implementación de la función
void show_numeric_keyboard(lv_obj_t * label) {
    // Crear un objeto textarea para almacenar el número introducido por el usuario
    lv_obj_t * ta = lv_textarea_create(lv_scr_act());

    // Establecer el estilo del textarea
    static lv_style_t style_ta;
    lv_style_init(&style_ta);
    lv_style_set_bg_color(&style_ta, lv_color_hex(0xFFF0BE));
    lv_obj_add_style(ta, &style_ta, 0);
    lv_obj_set_size(ta, 100, 40);
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 140);

    // Establecer el texto inicial del textarea a una cadena vacía
    lv_textarea_set_text(ta, "");

    // Crear un mapa de teclado personalizado
    static const char * kb_map[] = {"1", "2", "3", LV_SYMBOL_OK, "\n",
                                    "4", "5", "6", LV_SYMBOL_NEW_LINE, "\n",
                                    "7", "8", "9", LV_SYMBOL_BACKSPACE, "\n",
                                    "0", LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT, NULL
    };

    // Modificar el ancho de los botones
    static const lv_btnmatrix_ctrl_t kb_ctrl[] = {1, 1, 1, 2,
                                                  1, 1, 1, 2,
                                                  1, 1, 1, 2,
                                                  3, 1, 1
    };

    // Crear un teclado numérico personalizado
    lv_obj_t * kb = lv_keyboard_create(lv_scr_act());
    lv_obj_set_size(kb, 240, 130);

    // Establecer el mapa de teclado personalizado y el modo de teclado
    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_USER_1, kb_map, kb_ctrl);
    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_USER_1);
    lv_keyboard_set_textarea(kb, ta);

    // Establecer el estilo del teclado
    static lv_style_t style_kb;
    lv_style_init(&style_kb);
    lv_style_set_bg_color(&style_kb, lv_color_hex(0xFFF0BE));
    lv_obj_add_style(kb, &style_kb, 0);

    // Registrar la función de devolución de llamada para el evento LV_EVENT_READY
    lv_obj_add_event_cb(kb, keyboard_event_cb, LV_EVENT_READY, label);
}


// Manejar la entrada del usuario en el teclado numérico
// Actualizar la página actual del libro en la base de datos y en la interfaz de usuario
// Limpiar la pantalla
void keyboard_event_cb(lv_event_t * e) {
    // Obtener el teclado, textarea y etiqueta asociados con el evento
    lv_obj_t * kb = lv_event_get_target(e);
    lv_obj_t * ta = lv_keyboard_get_textarea(kb);
    lv_obj_t * label = (lv_obj_t *)lv_event_get_user_data(e);

    // Verificar si el evento es LV_EVENT_READY
    if(lv_event_get_code(e) == LV_EVENT_READY) {
        // Obtener el número de página introducido por el usuario (convertir cadena de caracteres a un int)
        int number = atoi(lv_textarea_get_text(ta));

        // El valor máximo posible a introducir es el máximo de páginas que tiene el libro
        int max_value = totalPages;

        // Verificar si el número introducido es mayor que el número máximo de páginas
        if(number > max_value) {
            std::string max_value_str = std::to_string(max_value); // Convertir el número máximo a una cadena de caracteres
            lv_textarea_set_text(ta, max_value_str.c_str());
            return;
        }

        // Actualizar la variable de página correspondiente al libro actual
        currentPage = number;

        // Actualizar el valor de currentPage en la base de datos
        update_current_page(book_key.c_str(), currentPage);

        // Actualizar la etiqueta con el nuevo valor de la página actual
        std::string buffer = "Página actual: " + std::to_string(number);
        lv_label_set_text(label, buffer.c_str());

        // Limpiar la pantalla
        lv_obj_del(kb);
        lv_obj_del(ta);

        // Crear botón para regresar a la pantalla principal de la pestaña 3
        lv_obj_t * symbol = lv_label_create(lv_scr_act());
        lv_label_set_text(symbol, "\xF3\xB0\xA9\x88");
        lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

        create_button(lv_scr_act(), symbol, BUTTON_STYLE_ORANGE, back_to_main_menu, 95, 160);
    }
}






//--------------------------------------PESTAÑA 4---------------------------------------------------
std::vector<std::string> book_keys; // Array para almacenar las claves de los libros (array de cadenas de texto)
int max_pages = 0;
String max_pages_book_title;

// Pantalla principal de la pestaña 4 (es dinámica y está sincronizada con la base de datos)
void tab4_content(lv_obj_t * parent) {
    // Título
    general_title(parent, "MIS ESTADÍSTICAS", TITLE_STYLE_GREEN);

    // Obtener los datos de los libros de la base de datos
    DynamicJsonDocument doc = get_book_data();

    // Variable para almacenar el total de páginas leídas
    int total_pages = 0;

    // Vector para almacenar los libros
    std::vector<Book> books;

    // Crear contenedores para cada estado de los libros (empezado, sin empezar, finalizado)
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

    // Crear etiquetas para cada contenedor
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

    // Contadores para cada estado de los libros
    int count_unstarted = 0;
    int count_in_progress = 0;
    int count_finished = 0;

    // Recorrer los datos obtenidos y crear un objeto Book para cada libro
    for(JsonPair kv : doc.as<JsonObject>()) {
        currentPage = kv.value()["pagina_actual"].as<int>();
        totalPages = kv.value()["paginas_total"].as<int>();
        title = kv.value()["titulo"].as<String>();
        book_key = kv.key().c_str();
        timestamp = kv.value()["ultima_modificacion"].as<long long>();

        Book book = {title, book_key, timestamp}; // Crear un objeto Book con los datos del libro
        books.push_back(book); // Añadir el libro al vector

        total_pages += currentPage; // Suma las páginas actuales al total

        // Actualizar el libro con más páginas leídas
        if(currentPage > max_pages) {
            max_pages = currentPage;
            max_pages_book_title = title;
        }

        // Crear una etiqueta para mostrar el estado de cada libro
        lv_obj_t * status_label = NULL;
        String status_label_text = title;

        // Comprobar estado de cada libro
        if(currentPage == totalPages) {
            status_label = lv_label_create(container_finished);
            lv_obj_align(status_label, LV_ALIGN_TOP_LEFT, 0, 25 * (count_finished + 1));
            count_finished++;
        } else if(currentPage == 0) {
            status_label = lv_label_create(container_unstarted);
            lv_obj_align(status_label, LV_ALIGN_TOP_LEFT, 0, 25 * (count_unstarted + 1));
            count_unstarted++;
        } else {
            float percentage_read = float(currentPage) * 100 / totalPages;
            status_label_text += " (" + String(percentage_read) + "% leído)";
            status_label = lv_label_create(container_in_progress);
            lv_obj_align(status_label, LV_ALIGN_TOP_LEFT, 0, 25 * (count_in_progress + 1));
            count_in_progress++;
        }

        // Configurar la etiqueta con el título del libro
        lv_label_set_text(status_label, status_label_text.c_str());
        lv_obj_set_style_text_font(status_label, &ubuntu_regular_16, 0);
    }

    // Ordenar los libros por timestamp en orden descendente
    std::sort(books.begin(), books.end(), [](const Book & a, const Book & b) {
        return a.timestamp > b.timestamp;
    });

    // El libro más reciente es el primero en el vector
    String most_recent_book_title = books[0].title;

    // Crear una etiqueta para mostrar el libro más reciente
    lv_obj_t * label1 = lv_label_create(parent);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label1, "Libro actual");
    lv_obj_set_style_text_font(label1, &ubuntu_bold_16, 0);
    lv_obj_set_width(label1, 230);
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -65);

    lv_obj_t * label11 = lv_label_create(parent);
    lv_label_set_long_mode(label11, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(label11, most_recent_book_title.c_str());
    lv_obj_set_style_text_font(label11, &ubuntu_regular_16, 0);
    lv_obj_set_width(label11, 230);
    lv_obj_set_style_text_align(label11, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label11, LV_ALIGN_CENTER, 0, -45);

    // Crear una etiqueta para mostrar el libro con más páginas leídas
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

    // Crear una etiqueta para mostrar el total de páginas leídas
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

    // Botón para ir desplazándose más facilmente por la pantalla hacia abajo
    lv_obj_t * down_button1 = lv_btn_create(parent);
    lv_obj_set_size(down_button1, 20, 20);
    lv_obj_set_pos(down_button1, 90, 230);
    lv_obj_add_event_cb(down_button1, event_handler_scroll1, LV_EVENT_CLICKED, parent);

    lv_obj_t * down_symbol1 = lv_label_create(down_button1);
    lv_obj_center(down_symbol1);
    lv_label_set_text(down_symbol1, LV_SYMBOL_DOWN);

    // Botón para ir desplazándose más facilmente por la pantalla hacia abajo
    lv_obj_t * down_button2 = lv_btn_create(parent);
    lv_obj_set_size(down_button2, 20, 20);
    lv_obj_set_pos(down_button2, 90, 505);
    lv_obj_add_event_cb(down_button2, event_handler_scroll2, LV_EVENT_CLICKED, parent);

    lv_obj_t * down_symbol2 = lv_label_create(down_button2);
    lv_obj_center(down_symbol2);
    lv_label_set_text(down_symbol2, LV_SYMBOL_DOWN);

    // Botón para ir desplazándose más facilmente por la pantalla hacia abajo
    lv_obj_t * down_button3 = lv_btn_create(parent);
    lv_obj_set_size(down_button3, 20, 20);
    lv_obj_set_pos(down_button3, 90, 780);
    lv_obj_add_event_cb(down_button3, event_handler_scroll3, LV_EVENT_CLICKED, parent);

    lv_obj_t * down_symbol3 = lv_label_create(down_button3);
    lv_obj_center(down_symbol3);
    lv_label_set_text(down_symbol3, LV_SYMBOL_DOWN);

    // Botón para ir desplazándose más facilmente por la pantalla hacia arriba
    lv_obj_t * up_button = lv_btn_create(parent);
    lv_obj_set_size(up_button, 20, 20); // Set the size of the button
    lv_obj_set_pos(up_button, 90, 830); // Set the position of the button
    lv_obj_add_event_cb(up_button, event_handler_top, LV_EVENT_CLICKED, parent); // Set the click event handler

    lv_obj_t * up_symbol = lv_label_create(up_button);
    lv_obj_center(up_symbol);
    lv_label_set_text(up_symbol, LV_SYMBOL_UP);


    // Estilo para los botones de desplazamiento por la pantalla
    static lv_style_t style_symbol_up_down;
    lv_style_init(&style_symbol_up_down);
    lv_style_set_text_color(&style_symbol_up_down, lv_color_black());
    lv_obj_add_style(down_symbol1, &style_symbol_up_down, 0);
    lv_obj_add_style(down_symbol2, &style_symbol_up_down, 0);
    lv_obj_add_style(down_symbol3, &style_symbol_up_down, 0);
    lv_obj_add_style(up_symbol, &style_symbol_up_down, 0);

    // Color de fondo del botón
    static lv_style_t style_btn_up_down;
    lv_style_init(&style_btn_up_down);
    lv_style_set_bg_color(&style_btn_up_down, lv_color_hex(0xCEF2D8));
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


// Manejador de eventos para el botón de desplazar la pantalla
void event_handler_scroll1(lv_event_t * e) {
    const lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * screen2 = (lv_obj_t *)lv_event_get_user_data(e);
        lv_obj_scroll_to_y(screen2, 285, LV_ANIM_ON);
    }
}

// Manejador de eventos para el botón de desplazar la pantalla
void event_handler_scroll2(lv_event_t * e) {
    const lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * screen2 = (lv_obj_t *)lv_event_get_user_data(e);
        lv_obj_scroll_to_y(screen2, 560, LV_ANIM_ON);
    }
}

// Manejador de eventos para el botón de desplazar la pantalla
void event_handler_scroll3(lv_event_t * e) {
    const lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * screen2 = (lv_obj_t *)lv_event_get_user_data(e);
        lv_obj_scroll_to_y(screen2, 840, LV_ANIM_ON);
    }
}


// Segunda pantalla de pestaña 4 con la gráfica de barras
void create_second_screen_tab4(lv_obj_t * parent) {
    // Crear una nueva pantalla y cargarla
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xCEF2D8), 0);
    lv_scr_load(screen2);

    // Crear un gráfico de barras en la pantalla
    lv_obj_t * chart = lv_chart_create(screen2);

    // Posición y tamaño del gráfico
    lv_obj_set_pos(chart, 55, 370);
    lv_obj_set_size(chart, 160, 180);

    // Configuración del gráfico
    lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100); // Rango de 0 a 100
    lv_obj_add_event_cb(chart, draw_label_x_axis, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_obj_add_event_cb(chart, draw_label_y_axis, LV_EVENT_DRAW_PART_BEGIN, NULL);

    // Configuración del eje Y
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 2, true, 50);

    // Configuración de las líneas divisorias
    lv_chart_set_div_line_count(chart, 6, 0);

    // Zoom en el eje X
    lv_chart_set_zoom_x(chart, 500);

    // Crear una serie para el gráfico y establecer el color
    lv_chart_series_t * ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_LIGHT_GREEN), LV_CHART_AXIS_PRIMARY_Y);

    // Para que las etiquetas de los ejes aparezcan en negro y se vean mejor (antes en gris)
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_line_color(&style, lv_color_hex(0x000000));
    lv_style_set_text_color(&style, lv_color_hex(0x000000));
    lv_obj_add_style(chart, &style, LV_PART_TICKS);

    int book_index = 0;

    // Buscar en la base de datos
    DynamicJsonDocument doc = get_book_data();

    // Configuración del eje X
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 5, 5, doc.size(), 1, true, 50); // Set the tick count to the number of books

    // Establece el número de puntos en el gráfico al número de libros
    lv_chart_set_point_count(chart, doc.size());

    int label_y_pos = 30; // Inicializa la posición Y de la etiqueta para la primera etiqueta
    int legend_pos = 20; // Posición de las etiquetas de la leyenda

    // Crear un contenedor para las etiquetas de la leyenda
    lv_obj_t * container = lv_obj_create(screen2);
    lv_obj_set_size(container, 210, 250);
    lv_obj_set_style_bg_color(container, lv_color_hex(0xCEF2D8), LV_PART_MAIN);
    lv_obj_set_style_border_color(container, lv_color_hex(0x25B619), LV_PART_MAIN);
    lv_obj_set_style_border_width(container, 2, LV_PART_MAIN);
    lv_obj_set_pos(container, 13, legend_pos);

    // Botón para desplazar pantalla hacia abajo automáticamente
    lv_obj_t * down_button = lv_btn_create(screen2);
    lv_obj_set_size(down_button, 20, 20);
    lv_obj_set_pos(down_button, 110, 280);
    lv_obj_add_event_cb(down_button, event_handler_bottom, LV_EVENT_CLICKED, screen2);

    // Crear una etiqueta para el símbolo de abajo y añadir al botón
    lv_obj_t * down_symbol = lv_label_create(down_button);
    lv_obj_center(down_symbol);
    lv_label_set_text(down_symbol, LV_SYMBOL_DOWN);

    //Botón para desplazar pantalla hacia arriba automáticamente
    lv_obj_t * up_button = lv_btn_create(screen2);
    lv_obj_set_size(up_button, 20, 20);
    lv_obj_set_pos(up_button, 110, 330);
    lv_obj_add_event_cb(up_button, event_handler_top, LV_EVENT_CLICKED, screen2);

    // Crear una etiqueta para el símbolo de arriba y añadir al botón
    lv_obj_t * up_symbol = lv_label_create(up_button);
    lv_obj_center(up_symbol);
    lv_label_set_text(up_symbol, LV_SYMBOL_UP);


    // Crear estilo para los símbolos de arriba y abajo
    static lv_style_t style_symbol_up_down;
    lv_style_init(&style_symbol_up_down);
    lv_style_set_text_color(&style_symbol_up_down, lv_color_black());
    lv_obj_add_style(down_symbol, &style_symbol_up_down, 0);
    lv_obj_add_style(up_symbol, &style_symbol_up_down, 0);

    // Establecer el estilo del botón para que coincida con el color de fondo
    static lv_style_t style_btn_up_down;
    lv_style_init(&style_btn_up_down);
    lv_style_set_bg_color(&style_btn_up_down, lv_color_hex(0xCEF2D8));
    lv_obj_add_style(down_button, &style_btn_up_down, 0);
    lv_obj_add_style(up_button, &style_btn_up_down, 0);

    // Crear una etiqueta para la leyenda
    lv_obj_t * label = lv_label_create(container);
    lv_label_set_text(label, "-----------LEYENDA-----------");
    lv_obj_set_style_text_font(label, &ubuntu_bold_16, 0);
    lv_obj_set_pos(label, 0, 0);

    // Para cada libro, calcular el porcentaje de páginas leídas y añadir una serie al gráfico
    for(JsonPair kv : doc.as<JsonObject>()) {
        book_key = kv.key().c_str(); // Obtener la clave del libro
        book_keys.push_back(book_key.c_str()); // Almacenar la clave del libro para su uso posterior
        JsonObject libro = kv.value().as<JsonObject>(); // Obtener el valor del libro como un objeto JSON

        // Calcular el porcentaje de páginas leídas
        totalPages = libro["paginas_total"].as<int>();
        currentPage = libro["pagina_actual"].as<int>();
        float porcentaje = ((float)currentPage / totalPages) * 100;

        // Añadir el porcentaje a la serie del gráfico
        lv_chart_set_next_value(chart, ser, porcentaje);

        // Crear una etiqueta para la clave y añadirla al contenedor de leyenda
        lv_obj_t * label_key = lv_label_create(container);
        lv_label_set_text(label_key, book_key.c_str());
        lv_obj_set_style_text_font(label_key, &ubuntu_bold_16, 0);
        lv_obj_set_pos(label_key, 0, label_y_pos);

        // Crear una etiqueta para el título del libro y añadirla al contenedor de leyenda
        lv_obj_t * label_title = lv_label_create(container);
        lv_label_set_text(label_title, libro["titulo"].as<const char*>());
        lv_obj_set_style_text_font(label_title, &ubuntu_regular_16, 0);
        lv_obj_set_pos(label_title, 0, label_y_pos + 20);

        label_y_pos += 45;

        book_index++;
    }

    // Actualizar el gráfico
    lv_chart_refresh(chart);

    // Crear un botón para regresar a la pantalla principal de la pestaña 4
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


// Manejador de eventos para el botón de desplazar la pantalla hacia abajo
void event_handler_bottom(lv_event_t * e) {
    const lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * screen2 = (lv_obj_t *)lv_event_get_user_data(e); // Obtener el objeto de la pantalla de los datos del evento
        lv_obj_scroll_to_y(screen2, lv_obj_get_height(screen2), LV_ANIM_ON); // Desplazar la pantalla hacia abajo
    }
}


// Manejador de eventos para el botón de desplazar la pantalla hacia arriba
void event_handler_top(lv_event_t * e) {
    const lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * screen2 = (lv_obj_t *)lv_event_get_user_data(e); // Obtener el objeto de la pantalla de los datos del evento
        lv_obj_scroll_to_y(screen2, 0, LV_ANIM_ON); // Desplazar la pantalla hacia arriba
    }
}


// Función para dibujar las etiquetas del eje X (las claves de los libros)
void draw_label_x_axis(lv_event_t * e) {
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL)) return;

    if(dsc->id == LV_CHART_AXIS_PRIMARY_X && dsc->text) {
        lv_snprintf(dsc->text, dsc->text_length, "%s", book_keys[dsc->value % book_keys.size()].c_str());
    }
}


// Función para dibujar las etiquetas del eje Y (los porcentajes de páginas leídas)
void draw_label_y_axis(lv_event_t * e) {
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL)) return;

    if(dsc->id == LV_CHART_AXIS_PRIMARY_Y && dsc->text) {
        lv_snprintf(dsc->text, dsc->text_length, "%d%%", dsc->value);
    }
}