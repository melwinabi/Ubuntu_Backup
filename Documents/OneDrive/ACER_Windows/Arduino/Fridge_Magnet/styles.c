#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: Arch_blue
//

void init_style_arch_blue_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(0xff00cdff));
};

lv_style_t *get_style_arch_blue_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arch_blue_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_arch_blue(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_arch_blue_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_arch_blue(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_arch_blue_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: Arch_orange
//

void init_style_arch_orange_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(0xffec7208));
};

lv_style_t *get_style_arch_orange_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arch_orange_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_arch_orange(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_arch_orange_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_arch_orange(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_arch_orange_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: Arch_red
//

void init_style_arch_red_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(0xffcd1919));
};

lv_style_t *get_style_arch_red_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arch_red_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_arch_red(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_arch_red_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_arch_red(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_arch_red_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_arch_blue,
        add_style_arch_orange,
        add_style_arch_red,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_arch_blue,
        remove_style_arch_orange,
        remove_style_arch_red,
    };
    remove_style_funcs[styleIndex](obj);
}

