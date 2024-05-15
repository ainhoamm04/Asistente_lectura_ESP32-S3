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


static void camera_imgbtn_photo_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        Serial.println("Clicked the camera button.");
        if (camera_task_flag == 1) {
            stop_camera_task();
            go_to_screen2(e);
            create_camera_task();
        }
    }
}

//lv_obj_t *scr_principal;

void create_second_screen(lv_obj_t *padre) {
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xFFFFFF), 0);
    lv_scr_load(screen2);

    lv_obj_t * label = lv_label_create(screen2);
    lv_label_set_text(label, "Hola, has cambiado de pantalla");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    // Botón de casa para volver a la pantalla principal
    lv_obj_t * home_btn = lv_imgbtn_create(screen2);
    lv_img_set_src(home_btn, &img_home); // img_home debe ser un recurso de imagen que represente una casa o un icono de "volver"

    static lv_style_t style_btn_pressed;
    lv_style_init(&style_btn_pressed);
    lv_style_set_translate_y(&style_btn_pressed, 5);
    lv_obj_add_style(home_btn, &style_btn_pressed, LV_STATE_PRESSED);

    lv_obj_set_pos(home_btn, 75, 240); // Ajusta la posición según tus necesidades
    lv_obj_set_size(home_btn, 80, 80); // Ajusta el tamaño según tus necesidades
    lv_obj_add_event_cb(home_btn, back_to_main_menu, LV_EVENT_CLICKED, NULL); // back_to_main_menu debe ser una función que cambie la pantalla activa a la pantalla principal
}

// Manejador de eventos para el botón que cambia a la pantalla secundaria de tab1
static void go_to_screen2(lv_event_t * e) {
    lv_obj_t * main_screen = lv_scr_act(); // Obtén la pantalla principal (donde están las tabs)
    create_second_screen(main_screen);
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
