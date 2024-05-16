#include "camera_ui.h"
#include "camera.h"
#include "sd_card.h"
#include "lv_img.h"

lv_img_dsc_t photo_show;          //apply an lvgl image variable
lvgl_camera_ui guider_camera_ui;  //camera ui structure
camera_fb_t *fb = NULL;           //data structure of camera frame buffer
camera_fb_t *fb_buf = NULL;
TaskHandle_t cameraTaskHandle;    //camera thread task handle
static int camera_task_flag = 0;  //camera thread running flag

//Create camera task thread
void create_camera_task(void) {
  if (camera_task_flag == 0) {
    camera_task_flag = 1;
    ui_set_photo_show();
    //disableCore0WDT();
    xTaskCreate(loopTask_camera, "loopTask_camera", 8192, NULL, 1, &cameraTaskHandle);
  } else {
    Serial.println("loopTask_camera is running...");
  }
}

//Close the camera thread
void stop_camera_task(void) {
  if (camera_task_flag == 1) {
    camera_task_flag = 0;
    while (1) {
      if (eTaskGetState(cameraTaskHandle) == eDeleted) {
        break;
      }
      vTaskDelay(10);
    }
    Serial.println("loopTask_camera deleted!");
  }
}

//camera thread
void loopTask_camera(void *pvParameters) {
  Serial.println("loopTask_camera start...");
  while (camera_task_flag) {
    fb = esp_camera_fb_get();
    fb_buf = fb;
    esp_camera_fb_return(fb);
    if (fb_buf != NULL) {
      for (int i = 0; i < fb_buf->len; i += 2) {
        uint8_t temp = 0;
        temp = fb_buf->buf[i];
        fb_buf->buf[i] = fb_buf->buf[i + 1];
        fb_buf->buf[i + 1] = temp;
      }
      photo_show.data = fb_buf->buf;
      lv_img_set_src(guider_camera_ui.camera_video, &photo_show);
      //cargar aqui programa leer codigo de barras
    }
  }
  vTaskDelete(cameraTaskHandle);
}

//Initialize an lvgl image variable
void ui_set_photo_show(void) {
  lv_img_header_t header;
  header.always_zero = 0;
  header.w = 240;
  header.h = 240;
  header.cf = LV_IMG_CF_TRUE_COLOR;
  photo_show.header = header;
  photo_show.data_size = 240 * 240 * 2;
  photo_show.data = NULL;
}

//Click the photo icon, callback function: goes to the main ui interface
/*
static void camera_imgbtn_photo_event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    Serial.println("Clicked the camera button.");
    if (camera_task_flag == 1) {
        Serial.println("1");
      stop_camera_task();

        Serial.println("2");

      fb = esp_camera_fb_get();

        Serial.println("3");

      if (fb != NULL) {

          Serial.println("4a");
        for (int i = 0; i < fb->len; i += 2) {
          uint8_t temp = 0;
          temp = fb->buf[i];
          fb->buf[i] = fb->buf[i + 1];
          fb->buf[i + 1] = temp;
        }
        int photo_index = list_count_number(list_picture);
        Serial.printf("photo_index: %d\r\n", photo_index);
        if (photo_index != -1) {
          String path = String(PICTURE_FOLDER) + "/" + String(++photo_index) + ".bmp";  //You can view it directly from your computer
          write_rgb565_to_bmp((char *)path.c_str(), fb->buf, fb->len, fb->height, fb->width);
          list_insert_tail(list_picture, (char *)path.c_str());
        }
      } else {

          Serial.println("4b");
        Serial.println("Camera capture failed.");
      }
      esp_camera_fb_return(fb);
    }
    create_camera_task();
  }
}
*/

String selected_isbn;

// Modificación de la función camera_imgbtn_photo_event_handler
static void camera_imgbtn_photo_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        Serial.println("Clicked the camera button.");
        if (camera_task_flag == 1) {
            stop_camera_task();
            set_book_number(); // Llama a set_book_number
            selected_isbn = get_book_number(); // Asigna el valor devuelto por get_book_number a selected_isbn
            Book* book = search_by_isbn(selected_isbn);
            if (book != nullptr) {
                book->found = true;
                Serial.println("Book found: " + book->title);
            } else {
                Serial.println("Book not found");
            }
            go_to_screen2(e);
            create_camera_task();
        }
    }
}






Book book1("Invisible", "Eloy Moreno", "299 paginas", "9788416588435", 0);
Book book2("El valle de los lobos", "Laura Gallego", "271 paginas", "9788467539677", 0);
Book book3("La maldicion del maestro", "Laura Gallego", "239 paginas", "9788467539684", 0);
Book book4("La llamada de los muertos", "Laura Gallego", "239 paginas", "9788467539691", 0);
Book book5("Fenris, el elfo", "Laura Gallego", "299 paginas", "9788467539707", 0);
Book bookNotFound("LIBRO NO ENCONTRADO", "", "", "", 0);

Book book_array[6] = {book1, book2, book3, book4, book5, bookNotFound};

void create_second_screen(lv_obj_t *padre) {
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xFFFFFF), 0);
    lv_scr_load(screen2);

    Book* book = search_by_isbn(get_book_number());
    lv_obj_t * label1 = lv_label_create(screen2);
    lv_label_set_text(label1, book->title.c_str());
    lv_obj_set_style_text_font(label1, &ubuntu_bold_16, 0);
    lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t * label2 = lv_label_create(screen2);
    lv_label_set_text(label2, book->author.c_str());
    lv_obj_set_style_text_font(label2, &ubuntu_regular_16, 0);
    lv_obj_align(label2, LV_ALIGN_TOP_MID, 0, 40);

    lv_obj_t * label3 = lv_label_create(screen2);
    lv_label_set_text(label3, book->pages.c_str());
    lv_obj_set_style_text_font(label3, &ubuntu_italic_16, 0);
    lv_obj_align(label3, LV_ALIGN_TOP_MID, 0, 60);

    // Si el libro fue encontrado, muestra la etiqueta y el teclado numérico
    if (book->title != "LIBRO NO ENCONTRADO") {
        // Crea una etiqueta para mostrar el número introducido por el usuario
        lv_obj_t * label4 = lv_label_create(screen2);
        lv_obj_align(label4, LV_ALIGN_TOP_MID, 0, 85);
        lv_label_set_text(label4, "¿En qué página te encuentras?");
        lv_obj_set_style_text_font(label4, &ubuntu_regular_16, 0);

        // Muestra el teclado numérico
        show_numeric_keyboard(label4);

        // Obtén el libro actual
        Book* current_book = search_by_isbn(get_book_number());

        // Crea una etiqueta para mostrar la página actual del libro
        lv_obj_t * label_current_page = lv_label_create(screen2);
        lv_obj_set_style_text_font(label_current_page, &ubuntu_regular_16, 0);
        lv_obj_align(label_current_page, LV_ALIGN_TOP_MID, 0, 350); // Ajusta la posición según tus necesidades

        // Convierte la página actual a string
        char current_page_str[32];
        sprintf(current_page_str, "%d", current_book->current_page);

        // Establece el texto de la etiqueta al valor de la página actual del libro
        lv_label_set_text_fmt(label_current_page, "Página actual: %s", current_page_str);
    } else {
        lv_obj_t * home_btn = lv_imgbtn_create(screen2);
        lv_img_set_src(home_btn, &img_home); // img_home debe ser un recurso de imagen que represente una casa o un icono de "volver"

        static lv_style_t style_btn_pressed;
        lv_style_init(&style_btn_pressed);
        lv_style_set_translate_y(&style_btn_pressed, 5);
        lv_obj_add_style(home_btn, &style_btn_pressed, LV_STATE_PRESSED);

        lv_obj_set_pos(home_btn, 75, 130); // Ajusta la posición según tus necesidades
        lv_obj_set_size(home_btn, 80, 80); // Ajusta el tamaño según tus necesidades
        lv_obj_add_event_cb(home_btn, back_to_main_menu, LV_EVENT_CLICKED, NULL); // back_to_main_menu debe ser una función que cambie la pantalla activa a la pantalla principal
    }

    // Botón de casa para volver a la pantalla principal
    /*
    lv_obj_t * home_btn = lv_imgbtn_create(screen2);
    lv_img_set_src(home_btn, &img_home); // img_home debe ser un recurso de imagen que represente una casa o un icono de "volver"

    static lv_style_t style_btn_pressed;
    lv_style_init(&style_btn_pressed);
    lv_style_set_translate_y(&style_btn_pressed, 5);
    lv_obj_add_style(home_btn, &style_btn_pressed, LV_STATE_PRESSED);

    lv_obj_set_pos(home_btn, 75, 350); // Ajusta la posición según tus necesidades
    lv_obj_set_size(home_btn, 80, 80); // Ajusta el tamaño según tus necesidades
    lv_obj_add_event_cb(home_btn, back_to_main_menu, LV_EVENT_CLICKED, NULL); // back_to_main_menu debe ser una función que cambie la pantalla activa a la pantalla principal
    */
}

// Manejador de eventos para el botón que cambia a la pantalla secundaria de tab1
static void go_to_screen2(lv_event_t * e) {
    lv_obj_t * main_screen = lv_scr_act(); // Obtén la pantalla principal (donde están las tabs)
    create_second_screen(main_screen);
}


int camera_button_press_count = 0;
String isbn_aux;
void set_book_number() {
    camera_button_press_count++;
    if (camera_button_press_count == 1) {
        isbn_aux = "9788416588435"; //Invisible
        //return isbn;
    } else if (camera_button_press_count == 2) {
        isbn_aux = "9788467539677"; //El valle de los lobos
        //return isbn;
    } else if (camera_button_press_count == 3) {
        isbn_aux = "9788467539677"; //El valle de los lobos
        //return isbn;
    } else if (camera_button_press_count == 4) {
        isbn_aux = "9788467539677"; //El valle de los lobos
        //return isbn;
    } else if (camera_button_press_count == 5) {
        isbn_aux = "9788416588435"; //Invisible
        //return isbn;
    } else if (camera_button_press_count == 6) {
        isbn_aux = "9788467539707"; //Fenris, el elfo
        //return isbn;
    }

    else {
        isbn_aux = "";
        //return ""; // Devuelve una cadena vacía o cualquier valor por defecto después de la segunda pulsación
    }
}

// Función para obtener el ISBN del libro actual
String get_book_number() {
    return isbn_aux;
}



/*
Book search_by_isbn(String isbn_aux){
    int i = 0;
    int salida = 0;
    while(salida == 0){
        if (isbn_aux == book_array[i].isbn){
            return book_array[i];
            salida = 1;
        } else {
            i++;
        } if (i == 5){
            return book_array[5];
            salida = 1;
        }
    }
}*/

Book* search_by_isbn(const String& isbn) {
    for(int i = 0; i < 6; i++) {
        if(book_array[i].isbn == isbn) {
            return &book_array[i];
        }
    }
    return &bookNotFound; // return pointer to bookNotFound if no match is found
}

/*
Book get_book_by_isbn(const String& isbn, int number) {
    if (isbn == "9788416588435") {
        isbnMatrix[0][1] = number;
        return book1;
    } else if (isbn == "9788467539677") {
        isbnMatrix[1][1] = number;
        return book2;
    } else if (isbn == "9788467539684") {
        isbnMatrix[2][1] = number;
        return book3;
    } else if (isbn == "9788467539691") {
        isbnMatrix[3][1] = number;
        return book4;
    } else if (isbn == "9788467539707") {
        isbnMatrix[4][1] = number;
        return book5;
    } else {
        return bookNotFound;
    }
}*/


// Declaración de la función de devolución de llamada
static void keyboard_event_cb(lv_event_t * e);

// Implementación de la función
void show_numeric_keyboard(lv_obj_t * label) {
    // Crear un objeto textarea para almacenar el número introducido por el usuario
    lv_obj_t * ta = lv_textarea_create(lv_scr_act());
    lv_obj_set_size(ta, 100, 40); // Ajusta el tamaño según tus necesidades
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 110); // Ajusta la posición según tus necesidades

    // Establecer el texto inicial del textarea a una cadena vacía
    lv_textarea_set_text(ta, "");

    /*
    // Crear un teclado numérico y asociarlo al textarea
    lv_obj_t * kb = lv_keyboard_create(lv_scr_act());
    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUMBER);
    lv_keyboard_set_textarea(kb, ta);
     */

    static const char * kb_map[] = {"1", "2", "3", LV_SYMBOL_BACKSPACE, "\n",
                                    "4", "5", "6", LV_SYMBOL_NEW_LINE, "\n",
                                    "7", "8", "9", LV_SYMBOL_OK, "\n",
                                    "0", LV_SYMBOL_RIGHT, LV_SYMBOL_LEFT, NULL
    };

    /*Set the relative width of the buttons and other controls*/
    static const lv_btnmatrix_ctrl_t kb_ctrl[] = {1, 1, 1, 2,
                                                     1, 1, 1, 2,
                                                     1, 1, 1, 2,
                                                     3, 1, 1
    };

    /*Create a keyboard and add the new map as USER_1 mode*/
    lv_obj_t * kb = lv_keyboard_create(lv_scr_act());

    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_USER_1, kb_map, kb_ctrl);
    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_USER_1);
    lv_keyboard_set_textarea(kb, ta);

    // Registrar la función de devolución de llamada para el evento LV_EVENT_READY
    lv_obj_add_event_cb(kb, keyboard_event_cb, LV_EVENT_READY, label);
}

// Función de devolución de llamada para el evento LV_EVENT_READY del teclado
static void keyboard_event_cb(lv_event_t * e) {
    lv_obj_t * kb = lv_event_get_target(e);
    lv_obj_t * ta = lv_keyboard_get_textarea(kb);
    lv_obj_t * label = (lv_obj_t *)lv_event_get_user_data(e);

    if(lv_event_get_code(e) == LV_EVENT_READY) {
        int number = atoi(lv_textarea_get_text(ta));

        // Obtén el libro actual
        Book* current_book = search_by_isbn(get_book_number());

        // Convierte el número de páginas a un entero
        int max_value = atoi(current_book->pages.c_str());

        if(number > max_value) {
            char max_value_str[32];
            sprintf(max_value_str, "%d", max_value);
            lv_textarea_set_text(ta, max_value_str);
            return;
        }

        // Actualizar la variable de página correspondiente al libro actual
        current_book->current_page = number;

        char buffer[32];
        sprintf(buffer, "Página actual: %d", number);
        lv_label_set_text(label, buffer);

        lv_obj_del(kb);
        lv_obj_del(ta);

        lv_obj_t * home_btn = lv_imgbtn_create(lv_scr_act());
        lv_img_set_src(home_btn, &img_home); // img_home debe ser un recurso de imagen que represente una casa o un icono de "volver"

        static lv_style_t style_btn_pressed;
        lv_style_init(&style_btn_pressed);
        lv_style_set_translate_y(&style_btn_pressed, 5);
        lv_obj_add_style(home_btn, &style_btn_pressed, LV_STATE_PRESSED);

        lv_obj_set_pos(home_btn, 75, 130); // Ajusta la posición según tus necesidades
        lv_obj_set_size(home_btn, 80, 80); // Ajusta el tamaño según tus necesidades
        lv_obj_add_event_cb(home_btn, back_to_main_menu, LV_EVENT_CLICKED, NULL); // back_to_main_menu debe ser una función que cambie la pantalla activa a la pantalla principal
    }
}






//Click the logo icon, callback function: goes to the main ui interface
static void camera_imgbtn_home_event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        stop_camera_task();
        Serial.println("Clicked the home button.");
        back_to_main_menu(e); // Llamada a la función
    }
}

/*
//Slide the screen to flip the screen
static void camera_screen_gesture_event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_GESTURE) {
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    bool state=0;
    switch (dir) {
      case LV_DIR_LEFT:
      case LV_DIR_RIGHT:
        state = camera_get_mirror_horizontal();
        state = !state;
        camera_set_mirror_horizontal(state);
        break;
      case LV_DIR_TOP:
      case LV_DIR_BOTTOM:
        state = camera_get_flip_vertical();
        state = !state;
        camera_set_flip_vertical(state);
        break;
    }
  }
}
 */

//Parameter configuration function on the camera screen
void setup_scr_camera(lvgl_camera_ui *ui) {
  //Write codes camera
  ui->camera = lv_obj_create(NULL);
  setup_list_head_picture();  //Generate a linked list based on the SD card's picture folder
  lv_img_home_init();
  lv_img_camera_init();

  static lv_style_t bg_style;
  lv_style_init(&bg_style);
  lv_style_set_bg_color(&bg_style, lv_color_hex(0xffffff));
  lv_obj_add_style(ui->camera, &bg_style, LV_PART_MAIN);

  /*Init the pressed style*/
  static lv_style_t style_pr;              //Apply for a style
  lv_style_init(&style_pr);                //Initialize it
  lv_style_set_translate_y(&style_pr, 5);  //Style: Every time you trigger, move down 5 pixels

  //Write codes camera_video
  ui->camera_video = lv_img_create(ui->camera);
  lv_obj_set_pos(ui->camera_video, 0, 0);
  lv_obj_set_size(ui->camera_video, 240, 240);

  //Write codes camera_photo
  ui->camera_imgbtn_photo = lv_imgbtn_create(ui->camera);
  lv_obj_set_pos(ui->camera_imgbtn_photo, 20, 240);
  lv_obj_set_size(ui->camera_imgbtn_photo, 80, 80);
  lv_img_set_src(ui->camera_imgbtn_photo, &img_camera);
  lv_obj_add_style(ui->camera_imgbtn_photo, &style_pr, LV_STATE_PRESSED);  //Triggered when the button is pressed

  //Write codes camera_return
  ui->camera_imgbtn_home = lv_imgbtn_create(ui->camera);
  lv_obj_set_pos(ui->camera_imgbtn_home, 140, 240);
  lv_obj_set_size(ui->camera_imgbtn_home, 80, 80);
  lv_img_set_src(ui->camera_imgbtn_home, &img_home);
  lv_obj_add_style(ui->camera_imgbtn_home, &style_pr, LV_STATE_PRESSED);  //Triggered when the button is pressed

  lv_obj_add_event_cb(ui->camera_imgbtn_photo, camera_imgbtn_photo_event_handler, LV_EVENT_ALL, NULL);
  lv_obj_add_event_cb(ui->camera_imgbtn_home, camera_imgbtn_home_event_handler, LV_EVENT_ALL, NULL);
  //lv_obj_add_event_cb(ui->camera, camera_screen_gesture_event_handler, LV_EVENT_ALL, NULL);
  create_camera_task();
}

