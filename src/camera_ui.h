#ifndef __CAMERA_UI_H
#define __CAMERA_UI_H

#include "lvgl.h"
#include "Arduino.h"
#include "ArduinoNvs.h"
#include "esp_camera.h"

extern camera_fb_t *fb;

typedef struct lvgl_camera
{
	lv_obj_t *camera;
	lv_obj_t *camera_video;
	lv_obj_t *camera_imgbtn_photo;
	lv_obj_t *camera_imgbtn_home;
}lvgl_camera_ui;

extern lv_img_dsc_t photo_show;              //apply an lvgl image variable
extern lvgl_camera_ui guider_camera_ui;      //camera ui structure

void create_camera_task(void);               //Create camera task thread
void stop_camera_task(void);                 //Close the camera thread
void loopTask_camera(void *pvParameters);    //camera task thread
void ui_set_photo_show(void);                //Initialize an lvgl image variable
void setup_scr_camera(lvgl_camera_ui *ui);   //Parameter configuration function on the camera screen

void back_to_main_menu(lv_event_t * e);
void go_to_screen2_tab2(lv_event_t * e);
void create_second_screen(lv_obj_t *padre);
//static void go_to_screen2(lv_event_t * e);

class Book {
public:
    String title;
    String author;
    String pages;
    String isbn;
    //int current_page;
    bool found; // Añade esta línea

    Book(String title, String author, String pages, String isbn, bool found = false)
        : title(title), author(author), pages(pages), isbn(isbn), found(found) {}

    int getCurrentPagde() {
        return NVS.getInt(isbn);
    }

    void setCurrentPage(int page) {
        NVS.setInt(isbn, page);
    }
};

// Crear los objetos Book de antemano
extern Book book1;
extern Book book2;
extern Book book3;
extern Book book4;
extern Book book5;
extern Book bookNotFound;

extern Book book_array[6];

// Declaración de selected_isbn como variable global
extern String selected_isbn;

//Book get_book_by_isbn(const String& isbn);
Book* search_by_isbn(const String& isbn);
void set_book_number();
String get_book_number();
void show_numeric_keyboard(lv_obj_t * label);
extern int camera_button_press_count;

#endif
