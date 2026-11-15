#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: Arch_blue
lv_style_t *get_style_arch_blue_INDICATOR_DEFAULT();
void add_style_arch_blue(lv_obj_t *obj);
void remove_style_arch_blue(lv_obj_t *obj);

// Style: Arch_orange
lv_style_t *get_style_arch_orange_INDICATOR_DEFAULT();
void add_style_arch_orange(lv_obj_t *obj);
void remove_style_arch_orange(lv_obj_t *obj);

// Style: Arch_red
lv_style_t *get_style_arch_red_INDICATOR_DEFAULT();
void add_style_arch_red(lv_obj_t *obj);
void remove_style_arch_red(lv_obj_t *obj);



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/