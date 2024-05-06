/*******************************************************************************
 * Size: 16 px
 * Bpp: 4
 * Opts: --bpp 4 --size 16 --lcd --use-color-info --font FontAwesome5-Solid+Brands+Regular.woff --range 62744 --format lvgl -o book_symbol.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include <lvgl.h>
#endif

#ifndef BOOK_SYMBOL
#define BOOK_SYMBOL 1
#endif

#if BOOK_SYMBOL

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+F518 "" */
    0x0, 0xff, 0xf9, 0x13, 0xe7, 0xfe, 0xee, 0x6e,
    0x62, 0xea, 0x61, 0xd5, 0x8, 0x40, 0x3f, 0xf8,
    0x42, 0x48, 0xaf, 0x13, 0x57, 0x98, 0xde, 0xe7,
    0xfe, 0xc7, 0x20, 0xc, 0xfa, 0x60, 0x18, 0x44,
    0x46, 0x64, 0x56, 0x78, 0xab, 0xde, 0xe6, 0x53,
    0xa0, 0x80, 0x5, 0x1e, 0xb3, 0xb9, 0xb7, 0x50,
    0xec, 0xa8, 0x66, 0x21, 0x10, 0x6, 0x3d, 0x70,
    0xf, 0xfe, 0x68, 0x9a, 0xc4, 0xb1, 0x11, 0xa6,
    0x14, 0xc4, 0x3, 0xff, 0xcf, 0xe0, 0xf, 0x0,
    0xff, 0xff, 0x80, 0x7f, 0xff, 0xc0, 0x3f, 0xff,
    0xe0, 0x1f, 0xff, 0xf0, 0xf, 0xff, 0xf8, 0x7,
    0xff, 0xfc, 0x40, 0x3f, 0xfd, 0x42, 0x1, 0x92,
    0x2e, 0x61, 0xdc, 0xcc, 0x55, 0x21, 0x90, 0x80,
    0x7f, 0xf3, 0x84, 0x8d, 0x15, 0x4c, 0xc7, 0x74,
    0x4d, 0xc2, 0x0, 0x71, 0x23, 0x3c, 0x42, 0x65,
    0x54, 0xbc, 0xde, 0xff, 0x6e, 0x5c, 0xba, 0x98,
    0xb7, 0x19, 0xb9, 0x84, 0xd5, 0xe6, 0xf3, 0x7f,
    0xdd, 0xb9, 0x75, 0x49, 0x94, 0x41, 0xd9, 0x8,
    0x3, 0xff, 0x94, 0x46, 0x8d, 0x15, 0x9d, 0x6c,
    0x22, 0x6b, 0xec, 0xa8, 0x64, 0x32, 0x0, 0xff,
    0xe3, 0x80
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 288, .box_w = 60, .box_h = 16, .ofs_x = -1, .ofs_y = -2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 62744, .range_length = 1, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 4,
    .kern_classes = 0,
    .bitmap_format = 1,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t book_symbol = {
#else
lv_font_t book_symbol = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 16,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_HOR,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -6,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if BOOK_SYMBOL*/

