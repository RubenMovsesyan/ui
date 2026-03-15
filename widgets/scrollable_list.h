#ifndef SCROLLABLE_LIST_H
#define SCROLLABLE_LIST_H

#include <common.h>
#include <raylib.h>
#include <ui/ui.h>

constexpr const u8 SCROLLABLE_LIST_ID[] = "_scrollable_list";
constexpr usize SCROLLABLE_LIST_ID_LEN = sizeof(SCROLLABLE_LIST_ID);

typedef struct __SelItemNode __SelItemNode;

typedef struct {
        u8 _id[SCROLLABLE_LIST_ID_LEN];

        // List data
        char** items;
        usize num_items;
        usize items_cap;
        __SelItemNode* selected_items;
        i32 hovered_item;
        f32 scroll_offset;

        // Layout and rendering
        Rectangle list_rect;
        RenderTexture2D list_texture; // for rendering text onto to avoid overdraw

        // Styling properties
        u32 border_weight;
        u32 fontsize;
        u32 font_spacing;
        f32 scroll_speed;
        Color border_color;
        Color backgroud_color;
        Color text_color;
        Color selection_color;
} UiScrollableList;

UiWidget uiScrollableListCreate(u32 x, u32 y, u32 width, u32 height);

// Configuration functions
void uiScrollableListAddItem(UiWidget* self, char* item);
void uiScrollableListClearItems(UiWidget* self);
void uiScrollableListGetSelectedItems(UiWidget* self, char*** selected_items, usize* num_selected);
void uiScrollableListGetSelectedItemsIndices(UiWidget* self, usize** indices, usize* num_selected);
void uiScrollableListSetPosition(UiWidget* self, Vector2 pos);
void uiScrollableListSetDimensions(UiWidget* self, Vector2 dims);
void uiScrollableListSetBorderWeight(UiWidget* self, u32 weight);
void uiScrollableListSetFontSize(UiWidget* self, u32 fontsize);
void uiScrollableListSetFontSpacing(UiWidget* self, u32 spacing);
void uiScrollableListSetScrollSpeed(UiWidget* self, f32 speed);
void uiScrollableListSetBorderColor(UiWidget* self, Color color);
void uiScrollableListSetBackgroundColor(UiWidget* self, Color color);
void uiScrollableListSetTextColor(UiWidget* self, Color color);
void uiScrollableListSetHoverColor(UiWidget* self, Color color);

// Ui Trait functions
void uiScrollableListUpdate(void* self, Vector2 offset);
void uiScrollableListRender(void* self);

#endif // SCROLLABLE_LIST_H
