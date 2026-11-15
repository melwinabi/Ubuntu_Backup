#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *light_switch;
    lv_obj_t *touch_to_speak;
    lv_obj_t *head;
    lv_obj_t *wifi_icon;
    lv_obj_t *ai_icon;
    lv_obj_t *head_label;
    lv_obj_t *home_icon;
    lv_obj_t *date_time;
    lv_obj_t *year;
    lv_obj_t *month;
    lv_obj_t *date;
    lv_obj_t *day;
    lv_obj_t *colon;
    lv_obj_t *minutes;
    lv_obj_t *hour;
    lv_obj_t *freze_image;
    lv_obj_t *hot_image;
    lv_obj_t *temp_indicator;
    lv_obj_t *celcious_symbol;
    lv_obj_t *temperature;
    lv_obj_t *degree;
    lv_obj_t *sensor_container;
    lv_obj_t *humidity_sensor_data;
    lv_obj_t *humidity_image;
    lv_obj_t *humidity_label;
    lv_obj_t *control_container;
    lv_obj_t *press;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};

void create_screen_main();
void tick_screen_main();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/