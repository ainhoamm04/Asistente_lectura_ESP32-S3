#ifndef __CAMERA_UI_H
#define __CAMERA_UI_H

#include "lvgl.h"
#include "Arduino.h"
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
void create_second_screen(lv_obj_t *padre);
static void go_to_screen2(lv_event_t * e);

class Book {
public:
    String title;
    String author;
    String pages;
    String isbn;
    String current_page;

    Book(String title, String author, String pages, String isbn, String current_page) : title(title), author(author), pages(pages), isbn(isbn), current_page(current_page) {}
};

// Crear los objetos Book de antemano
Book book1("Invisible", "Eloy Moreno", "299 paginas", "9788416588435", "0");
Book book2("El valle de los lobos", "Laura Gallego", "271 paginas", "9788467539677", "0");
Book book3("La maldicion del maestro", "Laura Gallego", "239 paginas", "9788467539684", "0");
Book book4("La llamada de los muertos", "Laura Gallego", "239 paginas", "9788467539691", "0");
Book book5("Fenris, el elfo", "Laura Gallego", "299 paginas", "9788467539707", "0");
Book bookNotFound("LIBRO NO ENCONTRADO", "", "", "", "");

extern Book book_array[6] = {book1, book2, book3, book4, book5, bookNotFound};


Book get_book_by_isbn(const String& isbn);
void set_book_number();
String get_book_number();
void show_numeric_keyboard(lv_obj_t * label);
extern int camera_button_press_count;

#endif
