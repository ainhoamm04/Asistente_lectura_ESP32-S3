#include "camera_ui.h"
#include "camera.h"
#include "sd_card.h"
#include "lv_img.h"
#include "firebase_config.h"
#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>
#include "Freenove_WS2812_Lib_for_ESP32.h"

extern Freenove_ESP32_WS2812 strip;

lv_img_dsc_t photo_show;          //apply an lvgl image variable
lvgl_camera_ui guider_camera_ui;  //camera ui structure
camera_fb_t *fb = NULL;           //data structure of camera frame buffer
camera_fb_t *fb_buf = NULL;
TaskHandle_t cameraTaskHandle;    //camera thread task handle
static int camera_task_flag = 0;  //camera thread running flag


bool book_found = false;
String book_key;
String title;
String author;
int totalPages;
int currentPage;


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
        strip.setLedColorData(0, 0, 0, 0); // Apaga el LED
        strip.show(); // Actualiza los LEDs

        Serial.println("loopTask_camera deleted!");
    }
}







void applyGaussianFilter(uint8_t* image, int width, int height) {
    // Define the Gaussian kernel
    double kernel[3][3] = {
        {1.0 / 16, 2.0 / 16, 1.0 / 16},
        {2.0 / 16, 4.0 / 16, 2.0 / 16},
        {1.0 / 16, 2.0 / 16, 1.0 / 16}
    };

    uint8_t* temp = new uint8_t[width * height];
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            double sum = 0;
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    sum += kernel[x + 1][y + 1] * image[(i + x) * width + (j + y)];
                }
            }
            temp[i * width + j] = static_cast<uint8_t>(sum);
        }
    }
    memcpy(image, temp, width * height);
    delete[] temp;
}

void calculateGradient(uint8_t* image, int width, int height, double*& gradientX, double*& gradientY, double*& magnitude) {
    // Define the Sobel kernels
    int sobelX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobelY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    gradientX = new double[width * height];
    gradientY = new double[width * height];
    magnitude = new double[width * height];

    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            double gx = 0;
            double gy = 0;
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    gx += sobelX[x + 1][y + 1] * image[(i + x) * width + (j + y)];
                    gy += sobelY[x + 1][y + 1] * image[(i + x) * width + (j + y)];
                }
            }
            gradientX[i * width + j] = gx;
            gradientY[i * width + j] = gy;
            magnitude[i * width + j] = sqrt(gx * gx + gy * gy);
        }
    }
}

double calculateOtsuThreshold(uint8_t* image, int width, int height) {
    int histogram[256] = {0};

    // Calculate the histogram
    for (int i = 0; i < width * height; i++) {
        histogram[image[i]]++;
    }

    int total = width * height;
    float sum = 0;
    for (int i = 0; i < 256; i++) {
        sum += i * histogram[i];
    }

    float sumB = 0;
    int wB = 0;
    int wF = 0;
    float varMax = 0;
    int threshold = 0;

    for (int i = 0; i < 256; i++) {
        wB += histogram[i];
        if (wB == 0) {
            continue;
        }

        wF = total - wB;
        if (wF == 0) {
            break;
        }

        sumB += i * histogram[i];
        float mB = sumB / wB;
        float mF = (sum - sumB) / wF;

        float varBetween = wB * wF * (mB - mF) * (mB - mF);

        if (varBetween > varMax) {
            varMax = varBetween;
            threshold = i;
        }
    }

    return threshold;
}


void convertTo8Bit(double* image, uint8_t* output, int width, int height) {
    double minVal = *std::min_element(image, image + width * height);
    double maxVal = *std::max_element(image, image + width * height);

    for (int i = 0; i < width * height; i++) {
        output[i] = static_cast<uint8_t>((image[i] - minVal) / (maxVal - minVal) * 255);
    }
}

void thresholdImage(double* image, uint8_t* binaryImage, int width, int height) {
    // Convertir la imagen a 8 bits
    uint8_t* image8bit = new uint8_t[width * height];
    convertTo8Bit(image, image8bit, width, height);

    // Calcular el umbral utilizando el método de Otsu
    double threshold = calculateOtsuThreshold(image8bit, width, height);

    // Umbralizar la imagen
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            binaryImage[i * width + j] = (image[i * width + j] > threshold) ? 255 : 0;
        }
    }

    delete[] image8bit;
}


void dilate(uint8_t* image, uint8_t* output, int width, int height) {
    int structElem[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};

    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            uint8_t maxVal = 0;
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    if (structElem[x + 1][y + 1]) {
                        uint8_t val = image[(i + x) * width + (j + y)];
                        if (val > maxVal) {
                            maxVal = val;
                        }
                    }
                }
            }
            output[i * width + j] = maxVal;
        }
    }
}

void erode(uint8_t* image, uint8_t* output, int width, int height) {
    int structElem[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};

    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            uint8_t minVal = 255;
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    if (structElem[x + 1][y + 1]) {
                        uint8_t val = image[(i + x) * width + (j + y)];
                        if (val < minVal) {
                            minVal = val;
                        }
                    }
                }
            }
            output[i * width + j] = minVal;
        }
    }
}

void close(uint8_t* image, uint8_t* output, int width, int height) {
    uint8_t* temp = new uint8_t[width * height];
    dilate(image, temp, width, height);
    erode(temp, output, width, height);
    delete[] temp;
}









//camera thread
void loopTask_camera(void *pvParameters) {
    Serial.println("loopTask_camera start...");
    while (camera_task_flag) {
        fb = esp_camera_fb_get(); //obtener un frame de la cámara
        fb_buf = fb; //guardar el frame en un buffer
        esp_camera_fb_return(fb); //devolver el frame a la cámara para que pueda ser utilizado
        if (fb_buf != NULL) {

            //Para cambiar los colores de RGB a BGR
            for (int i = 0; i < fb_buf->len; i += 2) {
                uint8_t temp = 0;
                temp = fb_buf->buf[i];
                fb_buf->buf[i] = fb_buf->buf[i + 1];
                fb_buf->buf[i + 1] = temp;
            }

            /*
            //Para invertir los colores de la imagen. Los px oscuros se convierten en claros y viceversa
            for (int i = 0; i < fb_buf->len; i++) {
                fb_buf->buf[i] = 255 - fb_buf->buf[i]; // Invertir colores
            }*/

            strip.setLedColorData(0, 255, 255, 255); // Set the color of the first LED to white
            strip.show(); // Actualiza los LEDs

            // Copiar los datos de la imagen en el buffer
            //uint8_t* new_buf = new uint8_t[fb_buf->len];
            //memcpy(new_buf, fb_buf->buf, fb_buf->len);

            /*
            // Aplicar el filtro gaussiano
            applyGaussianFilter(fb_buf->buf, fb_buf->width, fb_buf->height);

            // Calcular el gradiente
            double* gradientX;
            double* gradientY;
            double* magnitude;
            calculateGradient(fb_buf->buf, fb_buf->width, fb_buf->height, gradientX, gradientY, magnitude);

            // Umbralizar la imagen
            uint8_t* binaryImage = new uint8_t[fb_buf->width * fb_buf->height];
            thresholdImage(magnitude, binaryImage, fb_buf->width, fb_buf->height);

            // Aplicar la operación de cierre
            uint8_t* outputImage = new uint8_t[fb_buf->width * fb_buf->height];
            close(binaryImage, outputImage, fb_buf->width, fb_buf->height);

            // Liberar la memoria
            delete[] gradientX;
            delete[] gradientY;
            delete[] magnitude;
            delete[] binaryImage;

            // Actualizar la imagen mostrada
            photo_show.data = outputImage;
            lv_img_set_src(guider_camera_ui.camera_video, &photo_show);*/

            photo_show.data = fb_buf->buf; //guardar el frame en la variable de imagen
            lv_img_set_src(guider_camera_ui.camera_video, &photo_show); //mostrar la imagen en la pantalla

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
    //header.cf = LV_IMG_CF_ALPHA_8BIT;
    photo_show.header = header;
    photo_show.data_size = 240 * 240 * 2; //2 bytes en RGB
    //photo_show.data_size = 240 * 240 * 1; //1 byte en grayscale
    photo_show.data = NULL;
}



// Modificación de la función camera_imgbtn_photo_event_handler
static void camera_imgbtn_photo_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        Serial.println("Clicked the camera button.");
        if (camera_task_flag == 1) {
            stop_camera_task();
            set_book_number();
            searchIsbnInDatabase();
            go_to_screen2(e);
            //create_camera_task();

            //----------------Echar foto---------------
            fb = esp_camera_fb_get();

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
        //create_camera_task();
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



typedef enum {
    BUTTON_STYLE_ORANGE
} button_style_t;
void create_orange_button(lv_obj_t * parent, lv_obj_t * label, button_style_t style, lv_event_cb_t event_cb, lv_coord_t pos_x, lv_coord_t pos_y) {
    // Estilos para los botones
    static lv_style_t style_orange;
    static lv_style_t style_btn_pressed;
    static lv_style_t style_btn;

    lv_style_init(&style_orange);
    lv_style_set_bg_color(&style_orange, lv_color_hex(0xFFA82A )); // Fondo naranja
    lv_style_set_border_color(&style_orange, lv_color_hex(0xD17C00)); // Borde naranja

    // Estilo para el botón presionado
    lv_style_init(&style_btn_pressed);
    lv_style_set_translate_y(&style_btn_pressed, 5);

    lv_style_init(&style_btn);
    lv_style_set_radius(&style_btn, 10);
    lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
    lv_style_set_border_width(&style_btn, 2);
    lv_style_set_border_opa(&style_btn, LV_OPA_50);

    // Crea el botón y aplica el estilo correspondiente
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_remove_style_all(btn); // Elimina estilos previos

    lv_obj_add_style(btn, &style_orange, 0);
    lv_obj_set_size(btn, 50, 50);

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


void create_second_screen(lv_obj_t *padre) {
    lv_obj_t * screen2 = lv_obj_create(NULL);
    lv_obj_set_size(screen2, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(screen2, lv_color_hex(0xFFCE7E), 0);
    lv_scr_load(screen2);

    if(book_found) {
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

        show_numeric_keyboard(label5);
    } else {
        lv_obj_t * label = lv_label_create(screen2);
        lv_label_set_text(label, "         LIBRO\n           NO\nENCONTRADO");
        lv_obj_set_style_text_font(label, &bigger_symbols, 0);
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 40);

        //Crear boton para regresar a la pantalla principal con estilo
        lv_obj_t * symbol = lv_label_create(screen2);
        lv_label_set_text(symbol, "\xF3\xB0\xA9\x88");
        lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

        create_orange_button(screen2, symbol, BUTTON_STYLE_ORANGE, back_to_main_menu, 95, 160);
    }
}


static void go_to_screen2(lv_event_t * e) {
    lv_obj_t * main_screen = lv_scr_act(); // Obtén la pantalla principal (donde están las tabs)
    create_second_screen(main_screen);
}


int camera_button_press_count = 0;
String isbn_aux;
void set_book_number() {
    camera_button_press_count++;
    if (camera_button_press_count == 1) {
        isbn_aux = "9788416588436";
    }
    else if (camera_button_press_count == 2) {
        isbn_aux = "9788416588435"; //Invisible
    } else if (camera_button_press_count == 3) {
        isbn_aux = "9788467539677"; //El valle de los lobos
    } else if (camera_button_press_count == 4) {
        isbn_aux = "9788467539677"; //El valle de los lobos
    } else if (camera_button_press_count == 5) {
        isbn_aux = "9788467539677"; //El valle de los lobos
    } else if (camera_button_press_count == 6) {
        isbn_aux = "9788416588435"; //Invisible
    } else if (camera_button_press_count == 7) {
        isbn_aux = "9788467539707"; //Fenris, el elfo
    } else {
        isbn_aux = "0000000000000";
    }
}

// Función para obtener el ISBN del libro actual
String get_book_number() {
    return isbn_aux;
}


void searchIsbnInDatabase() {
    // Obtén el ISBN aleatorio
    String randomIsbn = get_book_number();

    // Inicializa book_found a false
    book_found = false;

    // Llama a la función get_book_data() en firebase_config.cpp
    DynamicJsonDocument doc = get_book_data();

    // Recorre todos los libros en la base de datos
    for(JsonPair kv : doc.as<JsonObject>()) {
        // Compara el ISBN de cada libro con el ISBN aleatorio
        String isbn = kv.value()["isbn"].as<String>();
        if (isbn == randomIsbn) {
            book_found = true;

            // Almacena la clave del libro
            book_key = kv.key().c_str();

            // Si encuentras un libro con el mismo ISBN, guarda los datos
            title = kv.value()["titulo"].as<String>();
            author = kv.value()["autor"].as<String>();
            totalPages = kv.value()["paginas_total"].as<int>();
            currentPage = kv.value()["pagina_actual"].as<int>();

            // Aquí puedes hacer lo que necesites con los datos del libro
            // Por ejemplo, podrías imprimirlos en la consola
            Serial.println("Libro encontrado:");
            Serial.println("Titulo: " + title);
            Serial.println("Autor: " + author);
            Serial.println("Paginas totales: " + String(totalPages));
            Serial.println("Pagina actual: " + String(currentPage));
            break;
        }
    }

    // Si no se encontró el libro, imprime "Libro no encontrado"
    if (!book_found) {
        Serial.println("Libro no encontrado");
    }
}


// Declaración de la función de devolución de llamada
static void keyboard_event_cb(lv_event_t * e);

// Implementación de la función
void show_numeric_keyboard(lv_obj_t * label) {
    // Crear un objeto textarea para almacenar el número introducido por el usuario
    lv_obj_t * ta = lv_textarea_create(lv_scr_act());

    static lv_style_t style_ta;
    lv_style_init(&style_ta);
    lv_style_set_bg_color(&style_ta, lv_color_hex(0xFFF0BE)); // Cambia el color de fondo a tu gusto
    lv_obj_add_style(ta, &style_ta, 0);
    lv_obj_set_size(ta, 100, 40); // Ajusta el tamaño según tus necesidades
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 140); // Ajusta la posición según tus necesidades

    // Establecer el texto inicial del textarea a una cadena vacía
    lv_textarea_set_text(ta, "");

    static const char * kb_map[] = {"1", "2", "3", LV_SYMBOL_OK, "\n",
                                    "4", "5", "6", LV_SYMBOL_NEW_LINE, "\n",
                                    "7", "8", "9", LV_SYMBOL_BACKSPACE, "\n",
                                    "0", LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT, NULL
    };

    /*Set the relative width of the buttons and other controls*/
    static const lv_btnmatrix_ctrl_t kb_ctrl[] = {1, 1, 1, 2,
                                                  1, 1, 1, 2,
                                                  1, 1, 1, 2,
                                                  3, 1, 1
    };

    /*Create a keyboard and add the new map as USER_1 mode*/
    lv_obj_t * kb = lv_keyboard_create(lv_scr_act());
    lv_obj_set_size(kb, 240, 130);

    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_USER_1, kb_map, kb_ctrl);
    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_USER_1);
    lv_keyboard_set_textarea(kb, ta);

    static lv_style_t style_kb;
    lv_style_init(&style_kb);
    lv_style_set_bg_color(&style_kb, lv_color_hex(0xFFF0BE)); // Cambia el color de fondo a tu gusto
    lv_obj_add_style(kb, &style_kb, 0); // Aplica el estilo al teclado

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

        searchIsbnInDatabase();

        // Convierte el número de páginas a un entero
        int max_value = totalPages;

        if(number > max_value) {
            char max_value_str[32];
            sprintf(max_value_str, "%d", max_value);
            lv_textarea_set_text(ta, max_value_str);
            return;
        }

        // Actualizar la variable de página correspondiente al libro actual
        currentPage = number;

        // Actualizar el valor de currentPage en la base de datos
        //Firebase.RTDB.setInt(&fbdo, "/libros/" + book_key + "/pagina_actual", currentPage);
        update_current_page(book_key.c_str(), currentPage);

        char buffer[32];
        sprintf(buffer, "Página actual: %d", number);
        lv_label_set_text(label, buffer);

        lv_obj_del(kb);
        lv_obj_del(ta);

        //Crear boton para regresar a la pantalla principal con estilo
        lv_obj_t * symbol = lv_label_create(lv_scr_act());
        lv_label_set_text(symbol, "\xF3\xB0\xA9\x88");
        lv_obj_set_style_text_font(symbol, &bigger_symbols, 0);

        create_orange_button(lv_scr_act(), symbol, BUTTON_STYLE_ORANGE, back_to_main_menu, 95, 160);
    }
}

