#include <Arduino.h>
#include "display.h"
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "FT6336U.h"
#include "img_btn_book.h"

Display screen;


//-------------------------DECLARACIÓN DE FUNCIONES------------------------------------
lv_obj_t *scr_principal;

static lv_style_t style_btn_purple;
static lv_style_t style_btn_orange;
static lv_style_t style_btn_pressed;

void tab_function(void);
static void back_to_main_menu(lv_event_t * e);

static void tab1_content(lv_obj_t * parent);
void purple_title(lv_obj_t * parent, const char * titulo);
static void style_purple_btn(void);
void create_second_screen_tab1(lv_obj_t *padre);
static void go_to_screen2_tab1(lv_event_t * e);

static void tab2_content(lv_obj_t * parent);
void blue_title(lv_obj_t * parent, const char * titulo);

void tab3_content(lv_obj_t * parent);
void orange_title(lv_obj_t * parent, const char * titulo);
static void style_orange_btn(void);
void create_second_screen_tab3();
static void go_to_screen2_tab3(lv_event_t * e);

static void tab4_content(lv_obj_t * parent);
void green_title(lv_obj_t * parent, const char * titulo);
int reto_pag_mes = 300;


//-------------------------------SETUP------------------------------------
void setup() {
    Serial.begin(115200);
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
    /*Create a Tab view object*/
    lv_obj_t * tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 35);

    /*Add 4 tabs (the tabs are page (lv_page) and can be scrolled*/
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


// Manejador de eventos para el botón de volver en la pantalla secundaria de tab3
static void back_to_main_menu(lv_event_t * e) {
    lv_obj_t * current_screen = lv_obj_get_parent(lv_event_get_target(e)); // Obtener la pantalla actual (secundaria)
    lv_scr_load(scr_principal); // Obtener la pantalla principal (donde están las tabs)
    lv_obj_del(current_screen); // Eliminar la pantalla secundaria
}

//--------------------------------------PESTAÑA 1---------------------------------------------------
static void tab1_content(lv_obj_t * parent)
{
    purple_title(parent, "¡BIENVENIDO!");

    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, "Yo seré tu asistente\n" "\t\t\t\t de lectura :)" "\n\n¿Quieres saber cómo \n\t\t\t\t\t\tfunciono?\n");
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_pos(label, 28, 60);

    style_purple_btn();

    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_t * labelbtn = lv_label_create(btn);

    lv_obj_remove_style_all(btn);

    lv_obj_set_pos(btn, 75, 180);
    lv_obj_set_size(btn, 50, 50);
    lv_obj_add_style(btn, &style_btn_purple, 0);
    lv_obj_add_style(btn, &style_btn_pressed, LV_STATE_PRESSED);
    lv_label_set_text(labelbtn, "\xF3\xB0\xB3\xBD");
    lv_obj_set_style_text_font(labelbtn, &bigger_symbols, 0);
    lv_obj_center(labelbtn);

    lv_obj_add_event_cb(btn, go_to_screen2_tab1, LV_EVENT_CLICKED, NULL);
}

void purple_title(lv_obj_t * parent, const char * titulo) {
    static lv_style_t style_title;

    lv_obj_t * label = lv_label_create(parent); // Crea una etiqueta en la pantalla activa

    lv_label_set_text(label, titulo); // Establece el texto de la etiqueta

    lv_style_set_radius(&style_title, 5);
    lv_style_set_bg_opa(&style_title, LV_OPA_COVER);
    //lv_style_set_bg_color(&style_title, lv_palette_lighten(LV_PALETTE_GREY, 1));
    lv_style_set_border_width(&style_title, 2);
    lv_style_set_border_color(&style_title, lv_palette_main(LV_PALETTE_DEEP_PURPLE));
    lv_style_set_pad_all(&style_title, 5);

    lv_style_set_text_color(&style_title, lv_palette_main(LV_PALETTE_DEEP_PURPLE));
    lv_style_set_text_letter_space(&style_title, 2);
    lv_style_set_text_line_space(&style_title, 10);
    lv_obj_set_style_text_font(label, &ubuntu_bold_16, 0);

    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 5);

    lv_obj_add_style(label, &style_title, 0);
}

static void style_purple_btn(void)
{
    lv_style_init(&style_btn_purple);
    lv_style_set_radius(&style_btn_purple, 10);
    lv_style_set_bg_opa(&style_btn_purple, LV_OPA_COVER);
    lv_style_set_bg_color(&style_btn_purple, lv_palette_lighten(LV_PALETTE_PURPLE, 2));
    lv_style_set_border_width(&style_btn_purple, 2);
    lv_style_set_border_color(&style_btn_purple, lv_color_black());
    lv_style_set_border_opa(&style_btn_purple, LV_OPA_50);

    lv_style_init(&style_btn_pressed);
    lv_style_set_translate_y(&style_btn_pressed, 5); //Every time you trigger, move down 5 pixels
}

void create_second_screen_tab1(lv_obj_t *padre) {
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xE9AAFF), 0);
    lv_scr_load(screen2);

    lv_obj_t * label = lv_label_create(screen2);
    lv_label_set_text(label, "Hola, has cambiado de pantalla");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t * back_btn = lv_btn_create(screen2);
    lv_obj_set_size(back_btn, 80, 40);
    lv_obj_align(back_btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(back_btn, back_to_main_menu, LV_EVENT_CLICKED, NULL);
    label = lv_label_create(back_btn);
    lv_label_set_text(label, "\xF3\xB0\xA9\x88");
    lv_obj_set_style_text_font(label, &bigger_symbols, 0);
    lv_obj_center(label);
}

// Manejador de eventos para el botón que cambia a la pantalla secundaria de tab1
static void go_to_screen2_tab1(lv_event_t * e) {
    lv_obj_t * main_screen = lv_scr_act(); // Obtén la pantalla principal (donde están las tabs)
    scr_principal = main_screen;
    create_second_screen_tab1(main_screen);
}


//--------------------------------------PESTAÑA 2---------------------------------------------------
void tab2_content(lv_obj_t * parent) {
    blue_title(parent, "MIS LIBROS");

    /*
    lv_obj_t *label_title_book1 = lv_label_create(parent);
    lv_obj_set_style_text_font(label_title_book1, &ubuntu_bold_16, 0);
    lv_obj_t *label_author_book1 = lv_label_create(parent);
    lv_obj_t *label_pages_book1 = lv_label_create(parent);
    lv_obj_set_style_text_font(label_pages_book1, &ubuntu_italic_16, 0);

    lv_obj_t *label_title_book2 = lv_label_create(parent);
    lv_obj_set_style_text_font(label_title_book2, &ubuntu_bold_16, 0);
    lv_obj_t *label_author_book2 = lv_label_create(parent);
    lv_obj_t *label_pages_book2 = lv_label_create(parent);
    lv_obj_set_style_text_font(label_pages_book2, &ubuntu_italic_16, 0);

    lv_obj_t *label_title_book3 = lv_label_create(parent);
    lv_obj_set_style_text_font(label_title_book3, &ubuntu_bold_16, 0);
    lv_obj_t *label_author_book3 = lv_label_create(parent);
    lv_obj_t *label_pages_book3 = lv_label_create(parent);
    lv_obj_set_style_text_font(label_pages_book3, &ubuntu_italic_16, 0);

    lv_obj_t *label_title_book4 = lv_label_create(parent);
    lv_obj_set_style_text_font(label_title_book4, &ubuntu_bold_16, 0);
    lv_obj_t *label_author_book4 = lv_label_create(parent);
    lv_obj_t *label_pages_book4 = lv_label_create(parent);
    lv_obj_set_style_text_font(label_pages_book4, &ubuntu_italic_16, 0);

    lv_obj_t *label_title_book5 = lv_label_create(parent);
    lv_obj_set_style_text_font(label_title_book5, &ubuntu_bold_16, 0);
    lv_obj_t *label_author_book5 = lv_label_create(parent);
    lv_obj_t *label_pages_book5 = lv_label_create(parent);
    lv_obj_set_style_text_font(label_pages_book5, &ubuntu_italic_16, 0);


    lv_label_set_text(label_title_book1, "El valle de los lobos\n");
    lv_obj_align(label_title_book1,LV_ALIGN_TOP_LEFT, 0, 50);
    lv_label_set_text(label_author_book1, "Autora Laura Gallego\n");
    lv_obj_align(label_author_book1,LV_ALIGN_TOP_LEFT, 0, 65);
    lv_label_set_text(label_pages_book1, "271 páginas\n");
    lv_obj_align(label_pages_book1,LV_ALIGN_TOP_LEFT, 0, 80);

    lv_label_set_text(label_title_book2, "La maldición del maestro\n");
    lv_obj_align(label_title_book2,LV_ALIGN_TOP_LEFT, 0, 100);
    lv_label_set_text(label_author_book2, "Autora Laura Gallego\n");
    lv_obj_align(label_author_book2,LV_ALIGN_TOP_LEFT, 0, 115);
    lv_label_set_text(label_pages_book2, "239 páginas\n");
    lv_obj_align(label_pages_book2,LV_ALIGN_TOP_LEFT, 0, 130);

    lv_label_set_text(label_title_book3, "La llamada de los muertos\n");
    lv_obj_align(label_title_book3,LV_ALIGN_TOP_LEFT, 0, 150);
    lv_label_set_text(label_author_book3, "Autora Laura Gallego\n");
    lv_obj_align(label_author_book3,LV_ALIGN_TOP_LEFT, 0, 165);
    lv_label_set_text(label_pages_book3, "239 páginas\n");
    lv_obj_align(label_pages_book3,LV_ALIGN_TOP_LEFT, 0, 180);

    lv_label_set_text(label_title_book4, "Fenris, el elfo\n");
    lv_obj_align(label_title_book4,LV_ALIGN_TOP_LEFT, 0, 200);
    lv_label_set_text(label_author_book4, "Autora Laura Gallego\n");
    lv_obj_align(label_author_book4,LV_ALIGN_TOP_LEFT, 0, 215);
    lv_label_set_text(label_pages_book4, "271 páginas\n");
    lv_obj_align(label_pages_book4,LV_ALIGN_TOP_LEFT, 0, 230);

    lv_label_set_text(label_title_book5, "Invisible\n");
    lv_obj_align(label_title_book5,LV_ALIGN_TOP_LEFT, 0, 250);
    lv_label_set_text(label_author_book5, "Autor Eloy Moreno\n");
    lv_obj_align(label_author_book5,LV_ALIGN_TOP_LEFT, 0, 265);
    lv_label_set_text(label_pages_book5, "299 páginas\n");
    lv_obj_align(label_pages_book5,LV_ALIGN_TOP_LEFT, 0, 280);
    */

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
        lv_obj_align(label_title, LV_ALIGN_TOP_LEFT, 0, 50 + i*80);

        lv_obj_t *label_author = lv_label_create(parent);
        lv_label_set_text(label_author, books[i].author);
        lv_obj_align(label_author, LV_ALIGN_TOP_LEFT, 0, 70 + i*80);

        lv_obj_t *label_pages = lv_label_create(parent);
        lv_obj_set_style_text_font(label_pages, &ubuntu_italic_16, 0);
        lv_label_set_text(label_pages, books[i].pages);
        lv_obj_align(label_pages, LV_ALIGN_TOP_LEFT, 0, 90 + i*80);

        if (i < sizeof(books)/sizeof(Book) - 1) { // No añadimos guiones después del último libro
            lv_obj_t *label_separator = lv_label_create(parent);
            lv_label_set_text(label_separator, "---------------------------------------");
            lv_obj_align(label_separator, LV_ALIGN_TOP_LEFT, 0, 110 + i*80);
        }
    }

}

void blue_title(lv_obj_t * parent, const char * titulo) {
    static lv_style_t style_title;

    lv_obj_t * label = lv_label_create(parent); // Crea una etiqueta en la pantalla activa

    lv_label_set_text(label, titulo); // Establece el texto de la etiqueta

    lv_style_set_radius(&style_title, 5);
    lv_style_set_bg_opa(&style_title, LV_OPA_COVER);
    //lv_style_set_bg_color(&style_title, lv_palette_lighten(LV_PALETTE_GREY, 1));
    lv_style_set_border_width(&style_title, 2);
    lv_style_set_border_color(&style_title, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_pad_all(&style_title, 5);

    lv_style_set_text_color(&style_title, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_text_letter_space(&style_title, 2);
    lv_style_set_text_line_space(&style_title, 10);
    lv_obj_set_style_text_font(label, &ubuntu_bold_16, 0);

    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 5);

    lv_obj_add_style(label, &style_title, 0);

    //lv_obj_set_style_text_color(label, lv_color_make(128, 0, 128), 0);
    //lv_obj_set_style_bg_color(label, lv_color_make(128, 0, 128), 0);
    //lv_obj_set_style_text_font(label, &ubuntu_bold_16, 0);
}


//--------------------------------------PESTAÑA 3---------------------------------------------------
void tab3_content(lv_obj_t * parent) {
    orange_title(parent, "ESCANEAR LIBRO");

    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, "¿Deseas agregar un nuevo \n     libro a tu biblioteca?");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 50);

    lv_obj_t * label2 = lv_label_create(parent);
    lv_label_set_text(label2, "  Pincha en el botón para \nabrir la cámara y escanear \n               el libro");
    lv_obj_align(label2, LV_ALIGN_TOP_MID, 0, 110);

    style_orange_btn();

    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_t * labelbtn = lv_label_create(btn);
    lv_obj_add_event_cb(btn, go_to_screen2_tab3, LV_EVENT_CLICKED, NULL);

    lv_obj_remove_style_all(btn);

    lv_obj_set_pos(btn, 75, 190);
    lv_obj_set_size(btn, 50, 50);
    lv_obj_add_style(btn, &style_btn_orange, 0);
    lv_obj_add_style(btn, &style_btn_pressed, LV_STATE_PRESSED);
    lv_label_set_text(labelbtn, "\xF3\xB0\x84\x84");
    lv_obj_set_style_text_font(labelbtn, &bigger_symbols, 0);
    lv_obj_center(labelbtn);

}

void orange_title(lv_obj_t * parent, const char * titulo) {
    static lv_style_t style_title;

    lv_obj_t * label = lv_label_create(parent); // Crea una etiqueta en la pantalla activa

    lv_label_set_text(label, titulo); // Establece el texto de la etiqueta

    lv_style_set_radius(&style_title, 5);
    lv_style_set_bg_opa(&style_title, LV_OPA_COVER);
    //lv_style_set_bg_color(&style_title, lv_palette_lighten(LV_PALETTE_GREY, 1));
    lv_style_set_border_width(&style_title, 2);
    lv_style_set_border_color(&style_title, lv_palette_main(LV_PALETTE_DEEP_ORANGE));
    lv_style_set_pad_all(&style_title, 5);

    lv_style_set_text_color(&style_title, lv_palette_main(LV_PALETTE_DEEP_ORANGE));
    lv_style_set_text_letter_space(&style_title, 2);
    lv_style_set_text_line_space(&style_title, 10);
    lv_obj_set_style_text_font(label, &ubuntu_bold_16, 0);

    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 5);

    lv_obj_add_style(label, &style_title, 0);
}

static void style_orange_btn(void)
{
    lv_style_init(&style_btn_orange);
    lv_style_set_radius(&style_btn_orange, 10);
    lv_style_set_bg_opa(&style_btn_orange, LV_OPA_COVER);
    lv_style_set_bg_color(&style_btn_orange, lv_palette_lighten(LV_PALETTE_ORANGE, 2));
    lv_style_set_border_width(&style_btn_orange, 2);
    lv_style_set_border_color(&style_btn_orange, lv_color_black());
    lv_style_set_border_opa(&style_btn_orange, LV_OPA_50);

    lv_style_init(&style_btn_pressed);
    lv_style_set_translate_y(&style_btn_pressed, 5); //Every time you trigger, move down 5 pixels
}

// Función para crear la pantalla secundaria de la pestaña 3
void create_second_screen_tab3(lv_obj_t *padre) {
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xE9AAFF), 0);
    lv_scr_load(screen2);

    lv_obj_t * label = lv_label_create(screen2);
    lv_label_set_text(label, "Hola, has cambiado de pantalla");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t * back_btn = lv_btn_create(screen2);
    lv_obj_set_size(back_btn, 80, 40);
    lv_obj_align(back_btn, LV_ALIGN_CENTER, 0, 20);
    lv_obj_add_event_cb(back_btn, back_to_main_menu, LV_EVENT_CLICKED, NULL);
    label = lv_label_create(back_btn);
    lv_label_set_text(label, "\xF3\xB0\xA9\x88");
    lv_obj_set_style_text_font(label, &bigger_symbols, 0);
    lv_obj_center(label);

}

// Manejador de eventos para el botón que cambia a la pantalla secundaria de tab3
static void go_to_screen2_tab3(lv_event_t * e) {
    lv_obj_t * main_screen = lv_scr_act(); // Obtén la pantalla principal (donde están las tabs)
    scr_principal = main_screen;
    create_second_screen_tab3(main_screen);
}










//--------------------------------------PESTAÑA 4---------------------------------------------------
static void draw_event_cb(lv_event_t * e) {
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL)) return;

    if(dsc->id == LV_CHART_AXIS_PRIMARY_X && dsc->text) {
        const char * month[] = {"Ene", "Feb", "Mar", "Abr", "May", "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"};
        lv_snprintf(dsc->text, dsc->text_length, "%s", month[dsc->value % 12]);
    }
}


static void tab4_content(lv_obj_t * parent){
    green_title(parent, "MIS ESTADÍSTICAS");

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

void green_title(lv_obj_t * parent, const char * titulo) {
    static lv_style_t style_title;

    lv_obj_t * label = lv_label_create(parent); // Crea una etiqueta en la pantalla activa

    lv_label_set_text(label, titulo); // Establece el texto de la etiqueta

    lv_style_set_radius(&style_title, 5);
    lv_style_set_bg_opa(&style_title, LV_OPA_COVER);
    //lv_style_set_bg_color(&style_title, lv_palette_lighten(LV_PALETTE_GREY, 1));
    lv_style_set_border_width(&style_title, 2);
    lv_style_set_border_color(&style_title, lv_color_make(0, 167, 15));
    lv_style_set_pad_all(&style_title, 5);

    lv_style_set_text_color(&style_title, lv_color_make(0, 167, 15));
    lv_style_set_text_letter_space(&style_title, 2);
    lv_style_set_text_line_space(&style_title, 10);
    lv_obj_set_style_text_font(label, &ubuntu_bold_16, 0);

    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 5);

    lv_obj_add_style(label, &style_title, 0);
}