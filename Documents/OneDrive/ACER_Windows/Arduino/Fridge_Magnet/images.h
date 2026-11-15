#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_cold_white;
extern const lv_img_dsc_t img_cold;
extern const lv_img_dsc_t img_fireball_2;
extern const lv_img_dsc_t img_fireball_white;
extern const lv_img_dsc_t img_icons8_ai_48;
extern const lv_img_dsc_t img_icons8_ai_64;
extern const lv_img_dsc_t img_icons8_air_quality_50;
extern const lv_img_dsc_t img_icons8_cold_50;
extern const lv_img_dsc_t img_icons8_fan_48;
extern const lv_img_dsc_t img_icons8_fire_90;
extern const lv_img_dsc_t img_icons8_humidity_50;
extern const lv_img_dsc_t img_icons8_light_48;
extern const lv_img_dsc_t img_icons8_light_96;
extern const lv_img_dsc_t img_icons8_light_152;
extern const lv_img_dsc_t img_icons8_smart_home_94;
extern const lv_img_dsc_t img_icons8_sun_48;
extern const lv_img_dsc_t img_icons8_wi_fi_50;
extern const lv_img_dsc_t img_gas;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[18];


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/