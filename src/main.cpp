#include <Arduino.h>
#include "display.h"
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "FT6336U.h"
#include "sd_card.h"
#include "camera.h"
#include "camera_ui.h"

Display screen;

//-------------------------DECLARACIÓN DE FUNCIONES------------------------------------
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
    BUTTON_STYLE_ORANGE,
    BUTTON_STYLE_GREEN
} button_style_t;
void create_button(lv_obj_t * parent, lv_obj_t * label, button_style_t style, lv_event_cb_t event_cb, lv_coord_t pos_x, lv_coord_t pos_y);

static void tab1_content(lv_obj_t * parent);
void create_second_screen_tab1(lv_obj_t *padre);
static void go_to_screen2_tab1(lv_event_t * e);

static void tab2_content(lv_obj_t * parent);
void create_second_screen_tab2(lv_obj_t *padre);
static void go_to_screen2_tab2(lv_event_t * e);

void tab3_content(lv_obj_t * parent);
void create_second_screen_tab3(lv_obj_t *padre);
static void go_to_screen2_tab3(lv_event_t * e);
void initialize_and_load_camera();

static void tab4_content(lv_obj_t * parent);
static void draw_event_cb(lv_event_t * e);
int reto_pag_mes = 300;


//-------------------------------SETUP------------------------------------
void setup() {
    Serial.begin(115200);
    //Serial.setDebugOutput(true);

    sdcard_init();
    camera_init();
    screen.init();

    tab_function();
}


//-------------------------------LOOP------------------------------------
void loop() {
    screen.routine(); /* let the GUI do its work */
    delay(5);
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
    lv_obj_set_style_bg_color(tab2, lv_palette_lighten(LV_PALETTE_CYAN, 1), 0);
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

    lv_obj_add_style(btn, &style_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_style(btn, &style_btn, 0); // Aplica el nuevo estilo

    // Aplica la etiqueta al botón
    lv_obj_set_parent(label, btn); // Establece el botón como el nuevo padre de la etiqueta
    lv_obj_center(label); // Centra la etiqueta en el botón

    // Configura el tamaño y la posición del botón
    lv_obj_set_size(btn, 50, 50); // Tamaño del botón
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
static void go_to_screen2_tab1(lv_event_t * e) {
    lv_obj_t * main_screen = lv_scr_act(); // Obtén la pantalla principal (donde están las tabs)
    scr_principal = main_screen;
    create_second_screen_tab1(main_screen);
}



//--------------------------------------PESTAÑA 2---------------------------------------------------
void tab2_content(lv_obj_t * parent) {
    general_title(parent, "MIS LIBROS", TITLE_STYLE_BLUE);

    lv_obj_t * symbol = lv_label_create(parent);
    lv_label_set_text(symbol, "\xF3\xB1\x81\xAF");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(parent, symbol, BUTTON_STYLE_BLUE, go_to_screen2_tab2, 75, 50);

    struct Book {
        const char* title;
        const char* author;
        const char* pages;
    };

    Book books[] = {
            {"El valle de los lobos", "Autora Laura Gallego", "271 páginas"},
            {"La maldición del maestro", "Autora Laura Gallego", "239 páginas"},
            {"La llamada de los muertos", "Autora Laura Gallego", "239 páginas"},
            {"Fenris, el elfo", "Autora Laura Gallego", "271 páginas"},
            {"Invisible", "Autor Eloy Moreno", "299 páginas"}
    };

    for(int i = 0; i < sizeof(books)/sizeof(Book); i++) {
        lv_obj_t *label_title = lv_label_create(parent);
        lv_obj_set_style_text_font(label_title, &ubuntu_bold_16, 0);
        lv_label_set_text(label_title, books[i].title);
        lv_obj_align(label_title, LV_ALIGN_TOP_LEFT, 0, 120 + i*80);

        lv_obj_t *label_author = lv_label_create(parent);
        lv_label_set_text(label_author, books[i].author);
        lv_obj_align(label_author, LV_ALIGN_TOP_LEFT, 0, 140 + i*80);

        lv_obj_t *label_pages = lv_label_create(parent);
        lv_obj_set_style_text_font(label_pages, &ubuntu_italic_16, 0);
        lv_label_set_text(label_pages, books[i].pages);
        lv_obj_align(label_pages, LV_ALIGN_TOP_LEFT, 0, 160 + i*80);

        if (i < sizeof(books)/sizeof(Book) - 1) { // No añadimos guiones después del último libro
            lv_obj_t *label_separator = lv_label_create(parent);
            lv_label_set_text(label_separator, "---------------------------------------");
            lv_obj_align(label_separator, LV_ALIGN_TOP_LEFT, 0, 180 + i*80);
        }
    }
}

void create_second_screen_tab2(lv_obj_t *padre) {
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0x9DE4FF), 0);
    lv_scr_load(screen2);

    lv_obj_t * label = lv_label_create(screen2);
    lv_label_set_text(label, "Hola, has cambiado de pantalla");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t * symbol = lv_label_create(screen2);
    lv_label_set_text(symbol, "\xF3\xB0\xA9\x88");
    lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

    create_button(screen2, symbol, BUTTON_STYLE_BLUE, back_to_main_menu, 95, 110);
}

// Manejador de eventos para el botón que cambia a la pantalla secundaria de tab1
static void go_to_screen2_tab2(lv_event_t * e) {
    lv_obj_t * main_screen = lv_scr_act(); // Obtén la pantalla principal (donde están las tabs)
    scr_principal = main_screen;
    create_second_screen_tab2(main_screen);
}



//--------------------------------------PESTAÑA 3---------------------------------------------------
void tab3_content(lv_obj_t * parent) {
    general_title(parent, "ESCANEAR LIBRO", TITLE_STYLE_ORANGE);

    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, "¿Deseas agregar un nuevo \n     libro a tu biblioteca?");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 50);

    lv_obj_t * label2 = lv_label_create(parent);
    lv_label_set_text(label2, "  Pincha en el botón para \nabrir la cámara y escanear \n               el libro");
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
