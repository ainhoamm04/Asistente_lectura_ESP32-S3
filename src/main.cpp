#include <Arduino.h>
#include "display.h"
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "FT6336U.h"
#include "img_btn_book.h"

Display screen;


//-------------------------DECLARACIÓN DE FUNCIONES------------------------------------
static void style_init(void);
static lv_color_t darken(const lv_color_filter_dsc_t * dsc, lv_color_t color, lv_opa_t opa);

static void tab1_content(lv_obj_t * parent);
void tab_function(void);

static void tab2_content(lv_obj_t * parent);

void tab3_content(lv_obj_t * parent);
void create_second_screen_tab3();
static void btn_event_handler(lv_event_t * e);
static void back_btn_event_handler(lv_event_t * e);

static void tab4_content(lv_obj_t * parent);
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

static lv_style_t style_btn;
static lv_style_t style_btn_pressed;
static lv_style_t style_btn_red;

static lv_color_t darken(const lv_color_filter_dsc_t * dsc, lv_color_t color, lv_opa_t opa)
{
    LV_UNUSED(dsc);
    return lv_color_darken(color, opa);
}

static void style_init(void)
{
    //Create a simple button style
    lv_style_init(&style_btn);
    lv_style_set_radius(&style_btn, 10);
    lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
    lv_style_set_bg_color(&style_btn, lv_palette_lighten(LV_PALETTE_PURPLE, 3));
    lv_style_set_bg_grad_color(&style_btn, lv_palette_main(LV_PALETTE_PURPLE));
    lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_VER);

    lv_style_set_border_color(&style_btn, lv_color_black());
    lv_style_set_border_opa(&style_btn, LV_OPA_20);
    lv_style_set_border_width(&style_btn, 2);

    lv_style_set_text_color(&style_btn, lv_color_black());

    //Create a style for the pressed state.
    //Use a color filter to simply modify all colors in this state
    static lv_color_filter_dsc_t color_filter;
    lv_color_filter_dsc_init(&color_filter, darken);
    lv_style_init(&style_btn_pressed);
    lv_style_set_color_filter_dsc(&style_btn_pressed, &color_filter);
    lv_style_set_color_filter_opa(&style_btn_pressed, LV_OPA_20);

    //Create a red style. Change only some colors.
    lv_style_init(&style_btn_red);
    lv_style_set_bg_color(&style_btn_red, lv_palette_main(LV_PALETTE_DEEP_PURPLE));
    lv_style_set_bg_grad_color(&style_btn_red, lv_palette_lighten(LV_PALETTE_DEEP_PURPLE, 3));
}



void tab_function(void)
{
    /*Create a Tab view object*/
    lv_obj_t * tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 35);

    /*Add 4 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "\xF3\xB0\x8B\x9C");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "\xEE\xB7\xA2");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "\xF3\xB0\x81\xB2");
    lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "\xF3\xB0\x84\xA8");

    tab1_content(tab1);
    tab2_content(tab2);
    tab3_content(tab3);
    tab4_content(tab4);
}


static void tab1_content(lv_obj_t * parent)
{
    //lv_obj_t * panel1 = lv_obj_create(parent);
    //lv_obj_set_size(lv_scr_act(), 240, 320);
    //lv_obj_set_style_bg_color(panel1, lv_color_hex(0x13D4C0), LV_PART_MAIN);

    //Initialize the style
    style_init();

    /*Create a menu object*/
    lv_obj_t * menu = lv_menu_create(parent);
    lv_obj_set_style_bg_color(menu, lv_color_hex(0x13D4C0), LV_PART_MAIN);
    lv_obj_set_size(menu, 240, 270);
    lv_obj_center(menu);

    /*Modify the header*/
    lv_obj_t * back_btn = lv_menu_get_main_header_back_btn(menu);
    lv_obj_t * back_btn_label = lv_label_create(back_btn);
    lv_label_set_text(back_btn_label, "Back");

    /*----------------------Create sub pages----------------------------------------*/
    lv_obj_t * sub_1_page = lv_menu_page_create(menu, "Page 1");
    lv_obj_t * contenedorSub = lv_menu_cont_create(sub_1_page);
    lv_obj_t * labelSub = lv_label_create(contenedorSub);

    //lv_obj_set_style_bg_color(sub_1_page, lv_color_hex(0x13D4C0), LV_PART_MAIN);
    lv_label_set_text(labelSub, "Hello world");
    //lv_obj_set_flex_align(contenedorSub, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);
    lv_obj_set_pos(contenedorSub,50,100);

    /*---------------Create a main page-----------------------------------------------------*/
    lv_obj_t * main_page = lv_menu_page_create(menu, NULL);
    //lv_obj_t * contenedor = lv_menu_cont_create(main_page);


    /*
    //Create another button and use the red style too
    lv_obj_t * btn2 = lv_btn_create(contenedor);
    lv_obj_remove_style_all(btn2);                      //Remove the styles coming from the theme
    lv_obj_set_pos(btn2, 15, 230);
    lv_obj_set_size(btn2, 170, 40);
    lv_obj_add_style(btn2, &style_btn, 0);
    lv_obj_add_style(btn2, &style_btn_red, 0);
    lv_obj_add_style(btn2, &style_btn_pressed, LV_STATE_PRESSED);
    lv_obj_set_style_radius(btn2, LV_RADIUS_CIRCLE, 0); //Add a local style too

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Libro ya empezado");
    lv_obj_center(label);
     */

    //Change the active screen's background color
    //lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x13D4C0), LV_PART_MAIN);

    //Create a white label, set its text and align it to the center
    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_radius(&style, 5);
    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    lv_style_set_bg_color(&style, lv_palette_lighten(LV_PALETTE_GREY, 2));
    lv_style_set_border_width(&style, 2);
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_PURPLE));
    lv_style_set_pad_all(&style, 10);

    lv_style_set_text_color(&style, lv_palette_main(LV_PALETTE_PURPLE));
    lv_style_set_text_letter_space(&style, 3);
    lv_style_set_text_line_space(&style, 20);
    //lv_style_set_text_decor(&style, LV_TEXT_FLAG_RECOLOR);

    lv_obj_t * label = lv_label_create(main_page);
    lv_label_set_text(label, "\n");

    lv_obj_t * label1 = lv_label_create(main_page);
    lv_obj_add_style(label1, &style, 0);
    lv_label_set_text(label1, "BIENVENIDO!!");
    lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 15);

    lv_obj_t * label2 = lv_label_create(main_page);
    lv_label_set_text(label2, "\nYo sere tu asistente\n" "\t\t\t\t de lectura :)");
    lv_obj_set_style_text_color(label2, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_align(label2, LV_ALIGN_TOP_MID, 0, 80);

    lv_obj_t * label3 = lv_label_create(main_page);
    lv_label_set_text(label3, "\nA que menu deseas acceder?\n");
    lv_obj_set_style_text_color(label3, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_align(label3, LV_ALIGN_TOP_MID, 0, 140);

    lv_obj_t * btn = lv_btn_create(main_page);
    lv_obj_t * labelbtn = lv_label_create(btn);

    lv_obj_remove_style_all(btn);

    lv_obj_set_pos(btn, 15, 170);
    lv_obj_set_size(btn, 170, 40);
    lv_obj_add_style(btn, &style_btn, 0);
    lv_obj_add_style(btn, &style_btn_pressed, LV_STATE_PRESSED);
    lv_label_set_text(labelbtn, "Libro nuevo");
    lv_obj_center(labelbtn);

    lv_obj_update_layout(main_page);

    lv_menu_set_load_page_event(menu, btn, sub_1_page);
    lv_menu_set_page(menu, main_page);
}



void tab2_content(lv_obj_t * parent) {
    // Crear una etiqueta con la primera fuente
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, "Texto con estilo de fuente 1\n\n\n");
    lv_obj_set_style_text_font(label, &ubuntu_regular_16, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 80);

    // Crear una etiqueta con la segunda fuente
    lv_obj_t *label2 = lv_label_create(parent);
    lv_label_set_text(label2, "Texto con estilo de fuente 2");
    lv_obj_set_style_text_font(label2, &ubuntu_bold_16, 0); // Asignar la segunda fuente
    lv_obj_align(label2, LV_ALIGN_TOP_MID, 0, 140);

    // Crear una etiqueta con la tercera fuente
    lv_obj_t *label3 = lv_label_create(parent);
    lv_label_set_text(label3, "Texto con estilo de fuente 3");
    lv_obj_set_style_text_font(label3, &ubuntu_italic_16, 0); // Asignar la segunda fuente
    lv_obj_align(label3, LV_ALIGN_TOP_MID, 0, 170);
}



// Función para crear el contenido de la pestaña 3
void tab3_content(lv_obj_t * parent) {
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, "Hola, estás en la pestaña 3");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 150, 40);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btn, btn_event_handler, LV_EVENT_CLICKED, NULL);
    label = lv_label_create(btn);
    lv_label_set_text(label, "Cambiar pantalla");
    lv_obj_center(label);

}

lv_obj_t *scr_principal;

// Función para crear la pantalla secundaria de la pestaña 3
void create_second_screen_tab3(lv_obj_t *padre) {
    lv_obj_t * pink_screen = lv_obj_create(NULL);
    //lv_obj_set_user_data(pink_screen, padre);
    lv_obj_set_size(pink_screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(pink_screen, lv_color_hex(0xFFC0CB), 0); // Color rosa
    lv_scr_load(pink_screen);

    lv_obj_t * label = lv_label_create(pink_screen);
    lv_label_set_text(label, "Hola, has cambiado de pantalla");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t * back_btn = lv_btn_create(pink_screen);
    lv_obj_set_size(back_btn, 80, 40);
    lv_obj_align(back_btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(back_btn, back_btn_event_handler, LV_EVENT_CLICKED, NULL);
    label = lv_label_create(back_btn);
    lv_label_set_text(label, "Volver");
    lv_obj_center(label);

}

// Manejador de eventos para el botón que cambia a la pantalla secundaria de tab3
static void btn_event_handler(lv_event_t * e) {
    lv_obj_t * main_screen = lv_scr_act(); // Obtén la pantalla principal (donde están las tabs)
    scr_principal = main_screen;
    create_second_screen_tab3(main_screen);
}

// Manejador de eventos para el botón de volver en la pantalla secundaria de tab3
static void back_btn_event_handler(lv_event_t * e) {
    lv_obj_t * current_screen = lv_obj_get_parent(lv_event_get_target(e)); // Obtén la pantalla actual (secundaria)
    //lv_obj_t * main_screen = lv_scr_act(); // Obtén la pantalla principal (donde están las tabs)

    // Carga la pantalla principal con una animación (puedes elegir el tipo de animación que prefieras)
    //lv_scr_load_anim(main_screen, LV_SCR_LOAD_ANIM_NONE, 0, 0, true);
    lv_scr_load(scr_principal);

    // Después de la animación, elimina la pantalla rosa
    lv_obj_del(current_screen);
}









static void draw_event_cb(lv_event_t * e) {
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL)) return;

    if(dsc->id == LV_CHART_AXIS_PRIMARY_X && dsc->text) {
        const char * month[] = {"Ene", "Feb", "Mar", "Abr", "May", "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"};
        lv_snprintf(dsc->text, dsc->text_length, "%s", month[dsc->value % 12]);
    }
}


static void tab4_content(lv_obj_t * parent){
    lv_obj_t * chart = lv_chart_create(parent);
    lv_obj_set_size(chart, 150, 200);
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