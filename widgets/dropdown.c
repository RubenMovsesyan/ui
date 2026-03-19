#include "dropdown.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <rlog.h>
#include <stdlib.h>
#include <string.h>
#include <ui/ui.h>

constexpr f32 DEFAULT_LIST_MAX_HEIGHT = 240.0f;
constexpr f32 DEFAULT_SCROLL_SPEED = 12.0f;
constexpr u32 DEFAULT_BORDER_WEIGHT = 2;
constexpr f32 DEFAULT_FONT_SIZE = 14.0f;
constexpr f32 DEFAULT_DRAG_THRESHOLD = 6.0f;
constexpr usize DEFAULT_ITEMS_CAP = 4;
constexpr i32 INVALID_HOVERED_ITEM = -1;

constexpr Color DEFAULT_BG_COLOR = (Color){50, 50, 50, 255};
constexpr Color DEFAULT_BORDER_COLOR = (Color){110, 110, 110, 255};
constexpr Color DEFAULT_HOVER_COLOR = (Color){72, 72, 72, 255};
constexpr Color DEFAULT_PRESSED_COLOR = (Color){35, 35, 35, 255};
constexpr Color DEFAULT_TEXT_COLOR = (Color){230, 230, 230, 255};
constexpr Color DEFAULT_ARROW_COLOR = (Color){200, 200, 200, 255};
constexpr Color DEFAULT_LIST_BG_COLOR = (Color){38, 38, 38, 255};
constexpr Color DEFAULT_ITEM_HOVER_COLOR = (Color){70, 80, 130, 200};

// ---------------------------------------------------------------------------
// Creation
// ---------------------------------------------------------------------------

static void uiDropdownFreeWidget(void* self) {
    UiDropdown* dd = (UiDropdown*)self;
    free(dd->items);
    free(dd);
}

UiWidget uiDropdownCreate(f32 x, f32 y, f32 width, f32 header_height) {
    UiDropdown* dd = (UiDropdown*)malloc(sizeof(UiDropdown));
    *dd = (UiDropdown){
        .position = (Vector2){x, y},
        .width = width,
        .header_height = header_height,
        .label = nullptr,
        .open = false,
        .header_hovered = false,
        .header_pressed = false,
        .items = (DropdownItem*)malloc(DEFAULT_ITEMS_CAP * sizeof(DropdownItem)),
        .item_count = 0,
        .item_cap = DEFAULT_ITEMS_CAP,
        .list_max_height = DEFAULT_LIST_MAX_HEIGHT,
        .scroll_offset = 0.0f,
        .total_items_height = 0.0f,
        .scroll_speed = DEFAULT_SCROLL_SPEED,
        .hovered_item = INVALID_HOVERED_ITEM,
        .drag_tracking = false,
        .drag_active = false,
        .drag_item_idx = 0,
        .drag_start = (Vector2){0, 0},
        .drag_pos = (Vector2){0, 0},
        .drag_threshold = DEFAULT_DRAG_THRESHOLD,
        .border_weight = DEFAULT_BORDER_WEIGHT,
        .font_size = DEFAULT_FONT_SIZE,
        .bg_color = DEFAULT_BG_COLOR,
        .border_color = DEFAULT_BORDER_COLOR,
        .hover_color = DEFAULT_HOVER_COLOR,
        .pressed_color = DEFAULT_PRESSED_COLOR,
        .text_color = DEFAULT_TEXT_COLOR,
        .arrow_color = DEFAULT_ARROW_COLOR,
        .list_bg_color = DEFAULT_LIST_BG_COLOR,
        .item_hover_color = DEFAULT_ITEM_HOVER_COLOR,
    };
    memcpy(&dd->_id, DROPDOWN_ID, DROPDOWN_ID_LEN);

    return (UiWidget){
        ._widget = dd,
        .update = uiDropdownUpdate,
        .render = uiDropdownRender,
        .free_widget = uiDropdownFreeWidget,
    };
}

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

void uiDropdownSetLabel(UiWidget* self, char* label) {
    __CHECK_WIDGET__(UiDropdown, self->_widget, DROPDOWN_ID, DROPDOWN_ID_LEN);
    ((UiDropdown*)self->_widget)->label = label;
}

void uiDropdownSetListMaxHeight(UiWidget* self, f32 max_height) {
    __CHECK_WIDGET__(UiDropdown, self->_widget, DROPDOWN_ID, DROPDOWN_ID_LEN);
    ((UiDropdown*)self->_widget)->list_max_height = max_height;
}

void uiDropdownSetScrollSpeed(UiWidget* self, f32 speed) {
    __CHECK_WIDGET__(UiDropdown, self->_widget, DROPDOWN_ID, DROPDOWN_ID_LEN);
    ((UiDropdown*)self->_widget)->scroll_speed = speed;
}

void uiDropdownSetBorderWeight(UiWidget* self, u32 weight) {
    __CHECK_WIDGET__(UiDropdown, self->_widget, DROPDOWN_ID, DROPDOWN_ID_LEN);
    ((UiDropdown*)self->_widget)->border_weight = weight;
}

void uiDropdownSetFontSize(UiWidget* self, f32 font_size) {
    __CHECK_WIDGET__(UiDropdown, self->_widget, DROPDOWN_ID, DROPDOWN_ID_LEN);
    ((UiDropdown*)self->_widget)->font_size = font_size;
}

void uiDropdownSetBgColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiDropdown, self->_widget, DROPDOWN_ID, DROPDOWN_ID_LEN);
    ((UiDropdown*)self->_widget)->bg_color = color;
}

void uiDropdownSetBorderColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiDropdown, self->_widget, DROPDOWN_ID, DROPDOWN_ID_LEN);
    ((UiDropdown*)self->_widget)->border_color = color;
}

void uiDropdownSetHoverColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiDropdown, self->_widget, DROPDOWN_ID, DROPDOWN_ID_LEN);
    ((UiDropdown*)self->_widget)->hover_color = color;
}

void uiDropdownSetPressedColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiDropdown, self->_widget, DROPDOWN_ID, DROPDOWN_ID_LEN);
    ((UiDropdown*)self->_widget)->pressed_color = color;
}

void uiDropdownSetTextColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiDropdown, self->_widget, DROPDOWN_ID, DROPDOWN_ID_LEN);
    ((UiDropdown*)self->_widget)->text_color = color;
}

void uiDropdownSetArrowColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiDropdown, self->_widget, DROPDOWN_ID, DROPDOWN_ID_LEN);
    ((UiDropdown*)self->_widget)->arrow_color = color;
}

void uiDropdownSetListBgColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiDropdown, self->_widget, DROPDOWN_ID, DROPDOWN_ID_LEN);
    ((UiDropdown*)self->_widget)->list_bg_color = color;
}

void uiDropdownSetItemHoverColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiDropdown, self->_widget, DROPDOWN_ID, DROPDOWN_ID_LEN);
    ((UiDropdown*)self->_widget)->item_hover_color = color;
}

void uiDropdownAddItem(UiWidget* self, DropdownItem item) {
    __CHECK_WIDGET__(UiDropdown, self->_widget, DROPDOWN_ID, DROPDOWN_ID_LEN);
    UiDropdown* dd = (UiDropdown*)self->_widget;

    if (dd->item_count + 1 >= dd->item_cap) {
        dd->item_cap *= 2;
        dd->items = (DropdownItem*)realloc(dd->items, dd->item_cap * sizeof(DropdownItem));
    }

    dd->items[dd->item_count++] = item;
    dd->total_items_height += item.height;
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static Rectangle __headerRect(UiDropdown* dd) { return (Rectangle){dd->position.x, dd->position.y, dd->width, dd->header_height}; }

static Rectangle __listRect(UiDropdown* dd) {
    f32 list_height = __MIN__(dd->total_items_height, dd->list_max_height);
    return (Rectangle){dd->position.x, dd->position.y + dd->header_height, dd->width, list_height};
}

// ---------------------------------------------------------------------------
// Update
// ---------------------------------------------------------------------------

void uiDropdownUpdate(void* self, Vector2 offset) {
    __CHECK_WIDGET__(UiDropdown, self, DROPDOWN_ID, DROPDOWN_ID_LEN);
    UiDropdown* dd = (UiDropdown*)self;

    // While tracking/dragging, handle only drag logic and return early.
    if (dd->drag_tracking) {
        dd->drag_pos = GetMousePosition();

        if (!dd->drag_active) {
            f32 dx = dd->drag_pos.x - dd->drag_start.x;
            f32 dy = dd->drag_pos.y - dd->drag_start.y;
            if (sqrtf(dx * dx + dy * dy) > dd->drag_threshold) {
                dd->drag_active = true;
                RLOG(LL_DEBUG, "Dropdown drag started for item %zu", dd->drag_item_idx);
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (dd->drag_active) {
                DropdownItem* item = &dd->items[dd->drag_item_idx];
                if (item->can_drop && item->can_drop(dd->drag_pos, item->user_data)) {
                    if (item->on_drop) {
                        item->on_drop(dd->drag_pos, item->user_data);
                    }
                }
            }
            dd->drag_tracking = false;
            dd->drag_active = false;
            RLOG(LL_DEBUG, "Dropdown drag ended");
        }
        return;
    }

    Vector2 mouse_pos = Vector2Subtract(GetMousePosition(), offset);
    Rectangle hdr_rect = __headerRect(dd);

    // Header interaction
    if (CheckCollisionPointRec(mouse_pos, hdr_rect)) {
        dd->header_hovered = true;
        dd->header_pressed = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            dd->open = !dd->open;
            dd->scroll_offset = 0.0f;
            RLOG(LL_DEBUG, "Dropdown %s", dd->open ? "opened" : "closed");
        }
    } else {
        dd->header_hovered = false;
        dd->header_pressed = false;
    }

    // List interaction (only while open)
    if (dd->open) {
        Rectangle list_rect = __listRect(dd);
        dd->hovered_item = INVALID_HOVERED_ITEM;

        if (CheckCollisionPointRec(mouse_pos, list_rect)) {
            // Scroll wheel
            Vector2 wheel = GetMouseWheelMoveV();
            dd->scroll_offset -= wheel.y * dd->scroll_speed;
            f32 inner_h = list_rect.height - (f32)(dd->border_weight * 2);
            f32 max_scroll = __MAX__(0.0f, dd->total_items_height - inner_h);
            dd->scroll_offset = __MIN__(__MAX__(dd->scroll_offset, 0.0f), max_scroll);

            // Determine which item the cursor is over
            f32 inner_top = list_rect.y + (f32)dd->border_weight;
            f32 y_in_list = (mouse_pos.y - inner_top) + dd->scroll_offset;
            f32 cursor = y_in_list;
            for (usize i = 0; i < dd->item_count; i++) {
                if (cursor < dd->items[i].height) {
                    dd->hovered_item = (i32)i;
                    break;
                }
                cursor -= dd->items[i].height;
            }

            // Begin drag tracking on press
            if (dd->hovered_item != INVALID_HOVERED_ITEM && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                dd->drag_tracking = true;
                dd->drag_active = false;
                dd->drag_item_idx = (usize)dd->hovered_item;
                dd->drag_start = GetMousePosition();
                dd->drag_pos = dd->drag_start;
            }
        }

        // Close when clicking outside both header and list
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(mouse_pos, hdr_rect) &&
            !CheckCollisionPointRec(mouse_pos, list_rect)) {
            dd->open = false;
            dd->scroll_offset = 0.0f;
        }
    }
}

// ---------------------------------------------------------------------------
// Render
// ---------------------------------------------------------------------------

// Draw a right-pointing triangle (▶) at center, half-size s (CCW in screen space).
static void __drawArrowRight(Vector2 center, f32 s, Color color) {
    DrawTriangle(
        (Vector2){center.x - s * 0.5f, center.y + s}, // bottom-left
        (Vector2){center.x + s * 0.5f, center.y},     // right
        (Vector2){center.x - s * 0.5f, center.y - s}, // top-left
        color
    );
}

// Draw a down-pointing triangle (▼) at center, half-size s (CCW in screen space).
static void __drawArrowDown(Vector2 center, f32 s, Color color) {
    DrawTriangle(
        (Vector2){center.x - s, center.y - s * 0.5f}, // top-left
        (Vector2){center.x, center.y + s * 0.5f},     // bottom-center
        (Vector2){center.x + s, center.y - s * 0.5f}, // top-right
        color
    );
}

void uiDropdownRender(void* self) {
    __CHECK_WIDGET__(UiDropdown, self, DROPDOWN_ID, DROPDOWN_ID_LEN);
    UiDropdown* dd = (UiDropdown*)self;

    // --- Header ---
    Color hdr_bg = dd->header_pressed ? dd->pressed_color : dd->header_hovered ? dd->hover_color : dd->bg_color;

    f32 bw = (f32)dd->border_weight;
    DrawRectangle((i32)dd->position.x, (i32)dd->position.y, (i32)dd->width, (i32)dd->header_height, dd->border_color);
    DrawRectangle((i32)(dd->position.x + bw), (i32)(dd->position.y + bw), (i32)(dd->width - bw * 2), (i32)(dd->header_height - bw * 2), hdr_bg);

    // Arrow — centred in a square of side header_height
    Vector2 arrow_center = {
        dd->position.x + dd->header_height * 0.5f,
        dd->position.y + dd->header_height * 0.5f,
    };
    f32 arrow_s = dd->header_height * 0.22f;
    if (dd->open) {
        __drawArrowDown(arrow_center, arrow_s, dd->arrow_color);
    } else {
        __drawArrowRight(arrow_center, arrow_s, dd->arrow_color);
    }

    // Label
    if (dd->label) {
        f32 text_x = dd->position.x + dd->header_height + 6.0f;
        f32 text_y = dd->position.y + dd->header_height * 0.5f - dd->font_size * 0.5f;
        DrawText(dd->label, (i32)text_x, (i32)text_y, (i32)dd->font_size, dd->text_color);
    }

    // --- List (only when open) ---
    if (dd->open) {
        f32 list_height = __MIN__(dd->total_items_height, dd->list_max_height);
        f32 list_x = dd->position.x;
        f32 list_y = dd->position.y + dd->header_height;

        // Border + background
        DrawRectangle((i32)list_x, (i32)list_y, (i32)dd->width, (i32)list_height, dd->border_color);
        DrawRectangle((i32)(list_x + bw), (i32)(list_y + bw), (i32)(dd->width - bw * 2), (i32)(list_height - bw * 2), dd->list_bg_color);

        f32 inner_x = list_x + bw;
        f32 inner_y = list_y + bw;
        f32 inner_w = dd->width - bw * 2;
        f32 inner_h = list_height - bw * 2;

        BeginScissorMode((i32)inner_x, (i32)inner_y, (i32)inner_w, (i32)inner_h);

        f32 y = inner_y - dd->scroll_offset;
        for (usize i = 0; i < dd->item_count; i++) {
            DropdownItem* item = &dd->items[i];
            bool hovered = (dd->hovered_item == (i32)i);
            Rectangle bounds = {inner_x, y, inner_w, item->height};

            if (hovered) {
                DrawRectangleRec(bounds, dd->item_hover_color);
            }
            if (item->render_item) {
                item->render_item(bounds, item->user_data, hovered);
            }
            y += item->height;
        }

        EndScissorMode();

        // Scrollbar — shown only when content overflows
        if (dd->total_items_height > inner_h) {
            constexpr f32 SCROLLBAR_W = 5.0f;
            constexpr f32 SCROLLBAR_MIN_H = 18.0f;
            f32 sb_x = inner_x + inner_w - SCROLLBAR_W;
            f32 thumb_h = __MAX__(SCROLLBAR_MIN_H, inner_h * (inner_h / dd->total_items_height));
            f32 max_scroll = dd->total_items_height - inner_h;
            f32 thumb_y = inner_y + (dd->scroll_offset / max_scroll) * (inner_h - thumb_h);

            DrawRectangle((i32)sb_x, (i32)inner_y, (i32)SCROLLBAR_W, (i32)inner_h, (Color){60, 60, 60, 180});
            DrawRectangle((i32)sb_x, (i32)thumb_y, (i32)SCROLLBAR_W, (i32)thumb_h, (Color){160, 160, 160, 200});
        }
    }

    // --- Ghost + drop indicator (while dragging) ---
    if (dd->drag_active) {
        DropdownItem* item = &dd->items[dd->drag_item_idx];
        if (item->render_ghost) {
            item->render_ghost(dd->drag_pos, item->user_data);
        }

        // Check validity and draw indicator circle with check / X
        bool valid = item->can_drop && item->can_drop(dd->drag_pos, item->user_data);

        constexpr f32 INDICATOR_R = 11.0f;
        Vector2 ind = {dd->drag_pos.x + 22.0f, dd->drag_pos.y - 22.0f};

        if (valid) {
            DrawCircleV(ind, INDICATOR_R, (Color){40, 190, 70, 220});
            // Checkmark: two segments forming a √
            DrawLineEx((Vector2){ind.x - 5.0f, ind.y + 0.5f}, (Vector2){ind.x - 1.5f, ind.y + 4.5f}, 2.0f, WHITE);
            DrawLineEx((Vector2){ind.x - 1.5f, ind.y + 4.5f}, (Vector2){ind.x + 5.0f, ind.y - 4.0f}, 2.0f, WHITE);
        } else {
            DrawCircleV(ind, INDICATOR_R, (Color){200, 50, 50, 220});
            // X: two crossing lines
            DrawLineEx((Vector2){ind.x - 4.5f, ind.y - 4.5f}, (Vector2){ind.x + 4.5f, ind.y + 4.5f}, 2.0f, WHITE);
            DrawLineEx((Vector2){ind.x + 4.5f, ind.y - 4.5f}, (Vector2){ind.x - 4.5f, ind.y + 4.5f}, 2.0f, WHITE);
        }
    }
}
