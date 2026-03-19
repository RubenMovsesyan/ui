#ifndef DROPDOWN_H
#define DROPDOWN_H

#include <common.h>
#include <raylib.h>
#include <ui/ui.h>

constexpr const u8 DROPDOWN_ID[]   = "_ui_dropdown";
constexpr usize    DROPDOWN_ID_LEN = sizeof(DROPDOWN_ID);

// A single item in the dropdown list.
// render_item  — draw the item inside `bounds` each frame.
// render_ghost — draw a ghost version centered at `center` while dragging.
// can_drop     — return true if dropping at `screen_pos` is valid.
// on_drop      — called when the item is released over a valid drop zone.
typedef struct {
    void*  user_data;
    char*  label;
    f32    height;
    void (*render_item)(Rectangle bounds, void* user_data, bool hovered);
    void (*render_ghost)(Vector2 center, void* user_data);
    bool (*can_drop)(Vector2 screen_pos, void* user_data);
    void (*on_drop)(Vector2 screen_pos, void* user_data);
} DropdownItem;

typedef struct {
    u8 _id[DROPDOWN_ID_LEN];

    Vector2 position;
    f32     width;
    f32     header_height;
    char*   label;
    bool    open;

    // Header hover/press state
    bool header_hovered;
    bool header_pressed;

    // Items
    DropdownItem* items;
    usize         item_count;
    usize         item_cap;

    // List scrolling
    f32 list_max_height;
    f32 scroll_offset;
    f32 total_items_height;
    f32 scroll_speed;
    i32 hovered_item;

    // Drag & drop
    bool    drag_tracking;  // mouse held on an item
    bool    drag_active;    // moved past threshold — ghost now shown
    usize   drag_item_idx;
    Vector2 drag_start;
    Vector2 drag_pos;
    f32     drag_threshold;

    // Styling
    u32   border_weight;
    f32   font_size;
    Color bg_color;
    Color border_color;
    Color hover_color;
    Color pressed_color;
    Color text_color;
    Color arrow_color;
    Color list_bg_color;
    Color item_hover_color;
} UiDropdown;

UiWidget uiDropdownCreate(f32 x, f32 y, f32 width, f32 header_height);

// Configuration
void uiDropdownSetLabel(UiWidget* self, char* label);
void uiDropdownSetListMaxHeight(UiWidget* self, f32 max_height);
void uiDropdownSetScrollSpeed(UiWidget* self, f32 speed);
void uiDropdownSetBorderWeight(UiWidget* self, u32 weight);
void uiDropdownSetFontSize(UiWidget* self, f32 font_size);
void uiDropdownSetBgColor(UiWidget* self, Color color);
void uiDropdownSetBorderColor(UiWidget* self, Color color);
void uiDropdownSetHoverColor(UiWidget* self, Color color);
void uiDropdownSetPressedColor(UiWidget* self, Color color);
void uiDropdownSetTextColor(UiWidget* self, Color color);
void uiDropdownSetArrowColor(UiWidget* self, Color color);
void uiDropdownSetListBgColor(UiWidget* self, Color color);
void uiDropdownSetItemHoverColor(UiWidget* self, Color color);
void uiDropdownAddItem(UiWidget* self, DropdownItem item);

// Trait functions
void uiDropdownUpdate(void* self, Vector2 offset);
void uiDropdownRender(void* self);

#endif // DROPDOWN_H
