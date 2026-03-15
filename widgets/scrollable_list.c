#include "scrollable_list.h"
#include "common.h"
#include <attr.h>
#include <raylib.h>
#include <raymath.h>
#include <rlog.h>
#include <stdlib.h>
#include <string.h>
#include <ui/ui.h>

constexpr u32 DEFAULT_FONT_SIZE = 14;
constexpr u32 DEFAULT_FONT_SPACING = 2;
constexpr u32 DEFAULT_BORDER_WEIGHT = 5;
constexpr f32 DEFAULT_SCROLL_SPEED = 5.0f;
constexpr Color DEFAULT_BORDER_COLOR = BLACK;
constexpr Color DEFAULT_BACKGROUND_COLOR = WHITE;
constexpr Color DEFAULT_TEXT_COLOR = BLACK;
constexpr Color DEFAULT_HOVER_COLOR = (Color){.r = 80, .g = 80, .b = 255, .a = 128};

constexpr i32 INVALID_HOVERED_ITEM = -1;

struct __SelItemNode {
        u32 item_index;
        __SelItemNode* next;
};

static void uiScrollableListFreeWidget(void* self) {
    UiScrollableList* sc_list = (UiScrollableList*)self;
    __SelItemNode* node = sc_list->selected_items;
    while (node) {
        __SelItemNode* next = node->next;
        free(node);
        node = next;
    }
    free(sc_list->items);
    UnloadRenderTexture(sc_list->list_texture);
    free(sc_list);
}

UiWidget uiScrollableListCreate(u32 x, u32 y, u32 width, u32 height) {
    UiScrollableList* scrollable_list = (UiScrollableList*)malloc(sizeof(UiScrollableList));
    *scrollable_list = (UiScrollableList){
        .items = (char**)malloc(1 * sizeof(char*)),
        .selected_items = nullptr,
        .hovered_item = INVALID_HOVERED_ITEM,
        .num_items = 0,
        .items_cap = 1,
        .list_rect =
            (Rectangle){
                .x = x,
                .y = y,
                .width = width,
                .height = height,
            },
        .border_weight = DEFAULT_BORDER_WEIGHT,
        .fontsize = DEFAULT_FONT_SIZE,
        .font_spacing = DEFAULT_FONT_SPACING,
        .border_color = DEFAULT_BORDER_COLOR,
        .backgroud_color = DEFAULT_BACKGROUND_COLOR,
        .text_color = DEFAULT_TEXT_COLOR,
        .selection_color = DEFAULT_HOVER_COLOR,
        .list_texture = LoadRenderTexture(width - DEFAULT_BORDER_WEIGHT * 2, height - DEFAULT_BORDER_WEIGHT * 2),
        .scroll_offset = 0.0f,
        .scroll_speed = DEFAULT_SCROLL_SPEED,
    };

    // Copy the ID for verification
    memcpy(&scrollable_list->_id, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);

    return (UiWidget){
        ._widget = scrollable_list,
        .update = uiScrollableListUpdate,
        .render = uiScrollableListRender,
        .free_widget = uiScrollableListFreeWidget,
    };
}

// Configuration functions
void uiScrollableListAddItem(UiWidget* self, char* item) {
    __CHECK_WIDGET__(UiScrollableList, self->_widget, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)(self->_widget);

    if (sc_list->num_items + 1 >= sc_list->items_cap) {
        sc_list->items_cap *= 2;
        sc_list->items = (char**)realloc(sc_list->items, sc_list->items_cap * sizeof(char*));
    }

    sc_list->items[sc_list->num_items++] = item;
}

void uiScrollableListClearItems(UiWidget* self) {
    __CHECK_WIDGET__(UiScrollableList, self->_widget, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)(self->_widget);

    sc_list->items = nullptr;
    sc_list->selected_items = nullptr;
    sc_list->num_items = 0;
}

void uiScrollableListGetSelectedItems(UiWidget* self, char*** selected_items, usize* num_selected) {
    __CHECK_WIDGET__(UiScrollableList, self->_widget, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)(self->_widget);
    __SelItemNode* current_node = sc_list->selected_items;
    usize count = 0;

    while (current_node != nullptr) {
        count++;
        current_node = current_node->next;
    }

    *num_selected = count;

    *selected_items = (char**)malloc(*num_selected * sizeof(char*));

    u32 i;
    for (current_node = sc_list->selected_items, i = 0; current_node != nullptr; i++, current_node = current_node->next) {
        (*selected_items)[i] = sc_list->items[current_node->item_index];
    }
}

void uiScrollableListGetSelectedItemsIndices(UiWidget* self, usize** indices, usize* num_selected) {
    __CHECK_WIDGET__(UiScrollableList, self->_widget, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)(self->_widget);
    __SelItemNode* current_node = sc_list->selected_items;
    usize count = 0;

    while (current_node != nullptr) {
        count++;
        current_node = current_node->next;
    }

    *num_selected = count;

    *indices = (usize*)malloc(*num_selected * sizeof(usize));

    u32 i;
    for (current_node = sc_list->selected_items, i = 0; current_node != nullptr; i++, current_node = current_node->next) {
        (*indices)[i] = current_node->item_index;
    }
}

void uiScrollableListSetPosition(UiWidget* self, Vector2 pos) {
    __CHECK_WIDGET__(UiScrollableList, self->_widget, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)(self->_widget);
    sc_list->list_rect.x = pos.x;
    sc_list->list_rect.y = pos.y;
}

void uiScrollableListSetDimensions(UiWidget* self, Vector2 dims) {
    __CHECK_WIDGET__(UiScrollableList, self->_widget, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)(self->_widget);
    sc_list->list_rect.width = dims.x;
    sc_list->list_rect.height = dims.y;
    UnloadRenderTexture(sc_list->list_texture);
    sc_list->list_texture =
        LoadRenderTexture(sc_list->list_rect.width - sc_list->border_weight * 2, sc_list->list_rect.height - sc_list->border_weight * 2);
}

void uiScrollableListSetBorderWeight(UiWidget* self, u32 weight) {
    __CHECK_WIDGET__(UiScrollableList, self->_widget, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)(self->_widget);
    sc_list->border_weight = weight;
    UnloadRenderTexture(sc_list->list_texture);
    sc_list->list_texture =
        LoadRenderTexture(sc_list->list_rect.width - sc_list->border_weight * 2, sc_list->list_rect.height - sc_list->border_weight * 2);
}

void uiScrollableListSetFontSize(UiWidget* self, u32 fontsize) {
    __CHECK_WIDGET__(UiScrollableList, self->_widget, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)(self->_widget);
    sc_list->fontsize = fontsize;
}

void uiScrollableListSetFontSpacing(UiWidget* self, u32 spacing) {
    __CHECK_WIDGET__(UiScrollableList, self->_widget, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)(self->_widget);
    sc_list->font_spacing = spacing;
}

void uiScrollableListSetScrollSpeed(UiWidget* self, f32 speed) {
    __CHECK_WIDGET__(UiScrollableList, self->_widget, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)(self->_widget);
    sc_list->scroll_speed = speed;
}

void uiScrollableListSetBorderColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiScrollableList, self->_widget, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)(self->_widget);
    sc_list->border_color = color;
}

void uiScrollableListSetBackgroundColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiScrollableList, self->_widget, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)(self->_widget);
    sc_list->backgroud_color = color;
}

void uiScrollableListSetTextColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiScrollableList, self->_widget, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)(self->_widget);
    sc_list->text_color = color;
}

void uiScrollableListSetHoverColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiScrollableList, self->_widget, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)(self->_widget);
    sc_list->selection_color = color;
}

void __uiScrollableListSelectItem(UiScrollableList* sc_list, u32 item_number) {
    // If there are no selected items then just select it
    if (sc_list->selected_items == nullptr) {
        sc_list->selected_items = (__SelItemNode*)calloc(1, sizeof(__SelItemNode));
        sc_list->selected_items->item_index = item_number;
        RLOG(LL_DEBUG, "Selected item %u", item_number);
        return;
    }

    // First go though the selected items list, if it is in there then remove it
    __SelItemNode* current_node = sc_list->selected_items;

    // If the head node is the one we selected then just change the pointer to the next one
    if (current_node->item_index == item_number) {
        sc_list->selected_items = current_node->next;
        RLOG(LL_DEBUG, "Removed item %u", item_number);
        return;
    }

    // If the item number is less than the current node then insert it before
    if (current_node->item_index > item_number) {
        RLOG(LL_DEBUG, "Inserted item %u", item_number);
        __SelItemNode* created_node = (__SelItemNode*)calloc(1, sizeof(__SelItemNode));
        created_node->item_index = item_number;
        created_node->next = current_node;
        current_node->next = nullptr;
        sc_list->selected_items = created_node;
        return;
    }

    while (current_node->next != nullptr) {
        __SelItemNode* next_node = current_node->next;

        if (next_node) {
            if (next_node->item_index == item_number) {
                // Remove the next node
                RLOG(LL_DEBUG, "Removed item %u", item_number);
                current_node->next = next_node->next;
                return;
            }

            // Insert it in order
            if (next_node->item_index > item_number) {
                RLOG(LL_DEBUG, "Inserted item %u", item_number);
                __SelItemNode* created_node = (__SelItemNode*)calloc(1, sizeof(__SelItemNode));
                created_node->item_index = item_number;
                created_node->next = next_node;
                current_node->next = created_node;
                return;
            }
        }

        current_node = next_node;
    }

    // If we have treversed and haven't found the item, then select it
    current_node->next = (__SelItemNode*)calloc(1, sizeof(__SelItemNode));
    current_node->next->item_index = item_number;
    RLOG(LL_DEBUG, "Selected item %u", item_number);
}

void __uiScrollableListSelectSingleItem(UiScrollableList* sc_list, u32 item_number) {
    if (sc_list->selected_items == nullptr) {
        sc_list->selected_items = (__SelItemNode*)calloc(1, sizeof(__SelItemNode));
        sc_list->selected_items->item_index = item_number;
        RLOG(LL_DEBUG, "Selected item %u", item_number);
        return;
    }

    // If it is already selected, then just unselect it
    if (sc_list->selected_items->item_index == item_number && sc_list->selected_items->next == nullptr) {
        sc_list->selected_items = nullptr;
        return;
    }

    sc_list->selected_items->item_index = item_number;
    sc_list->selected_items->next = nullptr;
    RLOG(LL_DEBUG, "Selected item %u", item_number);
}

__DEV_ONLY__ void __uiScrollableListPrintSelectedOrder(UiScrollableList* sc_list) {
    __SelItemNode* node = sc_list->selected_items;
    while (node != nullptr) {
        RLOG(LL_DEBUG, "Selected word %s", sc_list->items[node->item_index]);
        node = node->next;
    }
}

// Ui Trait functions
void uiScrollableListUpdate(void* self, Vector2 offset) {
    __CHECK_WIDGET__(UiScrollableList, self, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)self;
    sc_list->hovered_item = INVALID_HOVERED_ITEM;

    Vector2 mouse_pos = Vector2Subtract(GetMousePosition(), offset);
    Rectangle inner_rect = {
        .x = sc_list->list_rect.x + sc_list->border_weight,
        .y = sc_list->list_rect.y + sc_list->border_weight,
        .width = sc_list->list_texture.texture.width,
        .height = sc_list->list_texture.texture.height,
    };
    // NOTE: Might want to check with inner rectangle instead
    if (CheckCollisionPointRec(mouse_pos, inner_rect)) {
        Vector2 mouse_wheel_delta = GetMouseWheelMoveV();
        sc_list->scroll_offset += mouse_wheel_delta.y * sc_list->scroll_speed;

        // clamp the scroll offset so we can't scroll too far
        sc_list->scroll_offset = __MIN__(
            0.0f, __MAX__(sc_list->scroll_offset, -((f32)(sc_list->num_items) * (f32)(sc_list->fontsize + sc_list->font_spacing) - inner_rect.height))
        );

        u32 item_index = (mouse_pos.y - inner_rect.y - sc_list->scroll_offset) / (sc_list->fontsize + sc_list->font_spacing);

        if (item_index >= sc_list->num_items) {
            sc_list->hovered_item = INVALID_HOVERED_ITEM;
        } else {
            sc_list->hovered_item = item_index;
        }

        if (sc_list->hovered_item != INVALID_HOVERED_ITEM) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (IsKeyDown(KEY_LEFT_CONTROL)) {
                    __uiScrollableListSelectItem(sc_list, sc_list->hovered_item);
                } else {
                    __uiScrollableListSelectSingleItem(sc_list, sc_list->hovered_item);
                }
            }
        }
    }
}

void uiScrollableListRender(void* self) {
    __CHECK_WIDGET__(UiScrollableList, self, SCROLLABLE_LIST_ID, SCROLLABLE_LIST_ID_LEN);
    UiScrollableList* sc_list = (UiScrollableList*)(self);

    // Draw the border first
    DrawRectangleRec(sc_list->list_rect, sc_list->border_color);

    BeginTextureMode(sc_list->list_texture);
    Rectangle background_rec = {
        .x = 0,
        .y = 0,
        .width = sc_list->list_texture.texture.width,
        .height = sc_list->list_texture.texture.height,
    };
    DrawRectangleRec(background_rec, sc_list->backgroud_color);

    // Draw the rectangle around all the selected items
    __SelItemNode* current_node = sc_list->selected_items;
    while (current_node) {
        Rectangle selected_rect = {
            .x = 0,
            .y = current_node->item_index * (sc_list->fontsize + sc_list->font_spacing) + sc_list->scroll_offset,
            .width = sc_list->list_rect.width - sc_list->border_weight * 2,
            .height = sc_list->fontsize + sc_list->font_spacing
        };
        DrawRectangleRec(selected_rect, sc_list->selection_color);
        current_node = current_node->next;
    }

    // Draw the rectangle around the hovered item if avaliable
    if (sc_list->hovered_item != INVALID_HOVERED_ITEM) {
        Rectangle hovered_rect = {
            .x = 0,
            .y = sc_list->hovered_item * (sc_list->fontsize + sc_list->font_spacing) + sc_list->scroll_offset,
            .width = sc_list->list_rect.width - sc_list->border_weight * 2,
            .height = sc_list->fontsize + sc_list->font_spacing
        };
        DrawRectangleRec(hovered_rect, sc_list->selection_color);
    }

    i32 y_pos = (i32)sc_list->scroll_offset;

    for (u32 i = 0; i < sc_list->num_items; i++) {
        DrawText(sc_list->items[i], sc_list->font_spacing, y_pos + sc_list->font_spacing, sc_list->fontsize, sc_list->text_color);
        y_pos += sc_list->fontsize + sc_list->font_spacing;
    }
    EndTextureMode();

    Rectangle source_rect = {
        .x = 0,
        .y = 0,
        .width = sc_list->list_texture.texture.width,
        .height = -sc_list->list_texture.texture.height,
    };

    Rectangle dest_rect = {
        .x = sc_list->list_rect.x + sc_list->border_weight,
        .y = sc_list->list_rect.y + sc_list->border_weight,
        .width = sc_list->list_rect.width - sc_list->border_weight * 2,
        .height = sc_list->list_rect.height - sc_list->border_weight * 2
    };

    DrawTexturePro(
        sc_list->list_texture.texture, source_rect, dest_rect, (Vector2){0.0f, 0.0f}, // Origin,
        0.0f,
        WHITE // White means no tint to the texture
    );
}
