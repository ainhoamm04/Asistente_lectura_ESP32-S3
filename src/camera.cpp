#include "camera.h"
#include "ESP32CameraPins.h"

bool camera_flip_vertical_state = 0;
bool camera_mirror_horizontal_state = 1;

//CameraPins camera_pins = CAMERA_MODEL_ESP32S3_EYE;

//Initialize the camera drive
int camera_init(void) {
    Serial.print("hola");
  camera_config_t config;
  /*
    config.pin_d0 = camera_pins.Y2_GPIO_NUM;
    config.pin_d1 = camera_pins.Y3_GPIO_NUM;
    config.pin_d2 = camera_pins.Y4_GPIO_NUM;
    config.pin_d3 = camera_pins.Y5_GPIO_NUM;
    config.pin_d4 = camera_pins.Y6_GPIO_NUM;
    config.pin_d5 = camera_pins.Y7_GPIO_NUM;
    config.pin_d6 = camera_pins.Y8_GPIO_NUM;
    config.pin_d7 = camera_pins.Y9_GPIO_NUM;
    config.pin_xclk = camera_pins.XCLK_GPIO_NUM;
    config.pin_pclk = camera_pins.PCLK_GPIO_NUM;
    config.pin_vsync = camera_pins.VSYNC_GPIO_NUM;
    config.pin_href = camera_pins.HREF_GPIO_NUM;
    config.pin_sscb_sda = camera_pins.SIOD_GPIO_NUM;
    config.pin_sscb_scl = camera_pins.SIOC_GPIO_NUM;
    config.pin_pwdn = camera_pins.PWDN_GPIO_NUM;
    config.pin_reset = camera_pins.RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;*/
    config.frame_size = FRAMESIZE_240X240;
    //config.pixel_format = PIXFORMAT_JPEG; // for streaming
    //config.pixel_format = PIXFORMAT_RGB565;
    //config.pixel_format = PIXFORMAT_GRAYSCALE;
    config.grab_mode = CAMERA_GRAB_LATEST;
    //config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    //config.jpeg_quality = 10;
    //config.fb_count = 2;
  
  //camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\r\n", err);
    return 0;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  s->set_vflip(s, camera_flip_vertical_state);      // flip it back
  s->set_hmirror(s, camera_mirror_horizontal_state);    // horizontal mirror image
  s->set_brightness(s, 0); // up the brightness just a bit
  s->set_saturation(s, 0); // lower the saturation

  Serial.println("Camera configuration complete!");
  return 1;
}

bool camera_get_flip_vertical(void)
{
  return camera_flip_vertical_state;
}

bool camera_get_mirror_horizontal(void)
{
  return camera_mirror_horizontal_state;
}

void camera_set_flip_vertical(bool state)
{
  sensor_t * s = esp_camera_sensor_get();
  camera_flip_vertical_state = state;
  s->set_vflip(s, camera_flip_vertical_state);
}

void camera_set_mirror_horizontal(bool state)
{
  sensor_t * s = esp_camera_sensor_get();
  camera_mirror_horizontal_state = state;
  s->set_hmirror(s, camera_mirror_horizontal_state); 
}
