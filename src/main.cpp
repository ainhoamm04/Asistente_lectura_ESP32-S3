#include <Arduino.h>
#include "display.h"
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "FT6336U.h"

Display screen;


//-------------------------DECLARACIÓN DE FUNCIONES------------------------------------
static void style_init(void);
static lv_color_t darken(const lv_color_filter_dsc_t * dsc, lv_color_t color, lv_opa_t opa);

static void tab1_content(lv_obj_t * parent);
void tab_function(void);

static void btn_event_handler(lv_event_t * e);
static void back_btn_event_handler(lv_event_t * e);
void tab3_content(lv_obj_t * parent);
void create_second_screen_tab3();

int reto_pag_mes = 300;
static void tab4_content(lv_obj_t * parent);

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
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "H");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "B");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "\xF3\xB0\x81\xB1");
    lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "S");

    tab1_content(tab1);
    tab3_content(tab3);
    tab4_content(tab4);

    lv_obj_t * label;

    /*
    label = lv_label_create(tab2);
    //lv_label_set_text(label, "Second tab");
    static lv_style_t symbol_style;
    lv_style_init(&symbol_style);
    lv_style_set_text_font(&symbol_style, &my_font_symbols); // Asigna la fuente de FontAwesome
    lv_obj_add_style(label, &symbol_style, 1);
    lv_label_set_text(label, MY_BOOK_SYMBOL);*/

    //label = lv_label_create(tab3);
    //lv_label_set_text(label, "Third tab");

    label = lv_label_create(tab4);
    lv_label_set_text(label, "Fourth tab");
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











static void event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * chart = lv_event_get_target(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_invalidate(chart);
    }
    if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        // Realiza el cast explícito aquí
        lv_coord_t * s = (lv_coord_t *)lv_event_get_param(e);
        *s = LV_MAX(*s, 20);
    }
    else if(code == LV_EVENT_DRAW_POST_END) {
        int32_t id = lv_chart_get_pressed_point(chart);
        if(id == LV_CHART_POINT_NONE) return;

        LV_LOG_USER("Selected point %d", (int)id);

        lv_chart_series_t * ser = lv_chart_get_series_next(chart, NULL);
        while(ser) {
            lv_point_t p;
            lv_chart_get_point_pos_by_id(chart, ser, id, &p);

            lv_coord_t * y_array = lv_chart_get_y_array(chart, ser);
            lv_coord_t value = y_array[id];

            char buf[16];
            lv_snprintf(buf, sizeof(buf), "%d", value); // Corrección aquí

            lv_draw_rect_dsc_t draw_rect_dsc;
            lv_draw_rect_dsc_init(&draw_rect_dsc);
            draw_rect_dsc.bg_color = lv_color_black();
            draw_rect_dsc.bg_opa = LV_OPA_50;
            draw_rect_dsc.radius = 3;

            lv_area_t a;
            a.x1 = chart->coords.x1 + p.x - 20;
            a.x2 = chart->coords.x1 + p.x + 20;
            a.y1 = chart->coords.y1 + p.y - 30;
            a.y2 = chart->coords.y1 + p.y - 10;

            lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);
            lv_draw_rect(draw_ctx, &draw_rect_dsc, &a);

            lv_draw_label_dsc_t label_dsc;
            lv_draw_label_dsc_init(&label_dsc);
            label_dsc.font = lv_font_default();
            label_dsc.color = lv_color_white();

            //lv_draw_label(&a, draw_ctx, &label_dsc, buf, lv_label_dsc_init(&label_dsc), NULL);

            ser = lv_chart_get_series_next(chart, ser);
        }
    }
    else if(code == LV_EVENT_RELEASED) {
        lv_obj_invalidate(chart);
    }
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
    /*Create a chart*/
    lv_obj_t * chart = lv_chart_create(parent);
    lv_obj_set_size(chart, 170, 190);
    lv_obj_center(chart);

    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 500);
    lv_chart_set_point_count(chart, 12);

    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 12, 3, true, 40);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 2, true, 30);
    lv_chart_set_div_line_count(chart, 5, 5);

    lv_obj_add_event_cb(chart, event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_obj_refresh_ext_draw_size(chart);

    /*Zoom in a little in X*/
    lv_chart_set_zoom_x(chart, 1000);

    /*Add two data series*/
    lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    uint32_t i;
    for(i = 0; i < 12; i++) {
        lv_chart_set_next_value(chart, ser1, lv_rand(60, 90));
        lv_chart_set_next_value(chart, ser2, lv_rand(10, 40));
    }

}