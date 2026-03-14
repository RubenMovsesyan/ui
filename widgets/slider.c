#include "slider.h"
#include <raylib.h>
#include <raymath.h>
#include <rlog.h>
#include <string.h>

constexpr u32 DEFAULT_FONT_SIZE = 14;
constexpr Color DEFAULT_TRACK_COLOR = {60, 60, 60, 255};
constexpr Color DEFAULT_TRACK_FILL_COLOR = {100, 149, 237, 255};
constexpr Color DEFAULT_HANDLE_COLOR = WHITE;
constexpr Color DEFAULT_HANDLE_HOVER_TINT = LIGHTGRAY;
constexpr Color DEFAULT_HANDLE_DRAG_TINT = DARKGRAY;
constexpr Color DEFAULT_LABEL_COLOR = WHITE;
constexpr f32 DEFAULT_HANDLE_LEAK = 4.0f;
constexpr u32 SLIDER_LABEL_OFFSET = 20;

UiWidget uiSliderCreate(Arena* arena, u32 x, u32 y, u32 width, u32 height, f32 min, f32 max, f32 value) {
    UiSlider* slider = (UiSlider*)arenaAlloc(arena, sizeof(UiSlider));
    *slider = (UiSlider){
        .bounds =
            (Rectangle){
                .x = x,
                .y = y,
                .width = width,
                .height = height,
            },
        .value = value,
        .min = min,
        .max = max,
        .state = SliderState_Idle,
        .is_dragging = false,
        .label = nullptr,
        .fontsize = DEFAULT_FONT_SIZE,
        .track_color = DEFAULT_TRACK_COLOR,
        .track_fill_color = DEFAULT_TRACK_FILL_COLOR,
        .handle_color = DEFAULT_HANDLE_COLOR,
        .handle_hover_tint = DEFAULT_HANDLE_HOVER_TINT,
        .handle_drag_tint = DEFAULT_HANDLE_DRAG_TINT,
        .label_color = DEFAULT_LABEL_COLOR,
        .handle_leak = DEFAULT_HANDLE_LEAK,
        .on_change = nullptr,
    };

    memcpy(&slider->_id, SLIDER_ID, SLIDER_ID_LEN);

    return (UiWidget){
        ._widget = slider,
        .update = uiSliderUpdate,
        .render = uiSliderRender,
    };
}

// ============================================================================
// Configuration functions
// ============================================================================

void uiSliderSetPosition(UiWidget* self, Vector2 pos) {
    __CHECK_WIDGET__(UiSlider, self->_widget, SLIDER_ID, SLIDER_ID_LEN);
    UiSlider* slider = (UiSlider*)self->_widget;
    slider->bounds.x = pos.x;
    slider->bounds.y = pos.y;
}

void uiSliderSetDimensions(UiWidget* self, Vector2 dims) {
    __CHECK_WIDGET__(UiSlider, self->_widget, SLIDER_ID, SLIDER_ID_LEN);
    UiSlider* slider = (UiSlider*)self->_widget;
    slider->bounds.width = dims.x;
    slider->bounds.height = dims.y;
}

void uiSliderSetValue(UiWidget* self, f32 value) {
    __CHECK_WIDGET__(UiSlider, self->_widget, SLIDER_ID, SLIDER_ID_LEN);
    UiSlider* slider = (UiSlider*)self->_widget;
    slider->value = Clamp(value, slider->min, slider->max);
}

void uiSliderSetRange(UiWidget* self, f32 min, f32 max) {
    __CHECK_WIDGET__(UiSlider, self->_widget, SLIDER_ID, SLIDER_ID_LEN);
    UiSlider* slider = (UiSlider*)self->_widget;
    slider->min = min;
    slider->max = max;
    slider->value = Clamp(slider->value, min, max);
}

void uiSliderSetLabel(UiWidget* self, char* label) {
    __CHECK_WIDGET__(UiSlider, self->_widget, SLIDER_ID, SLIDER_ID_LEN);
    UiSlider* slider = (UiSlider*)self->_widget;
    slider->label = label;
}

void uiSliderSetFontSize(UiWidget* self, u32 fontsize) {
    __CHECK_WIDGET__(UiSlider, self->_widget, SLIDER_ID, SLIDER_ID_LEN);
    UiSlider* slider = (UiSlider*)self->_widget;
    slider->fontsize = fontsize;
}

void uiSliderSetTrackColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiSlider, self->_widget, SLIDER_ID, SLIDER_ID_LEN);
    UiSlider* slider = (UiSlider*)self->_widget;
    slider->track_color = color;
}

void uiSliderSetTrackFillColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiSlider, self->_widget, SLIDER_ID, SLIDER_ID_LEN);
    UiSlider* slider = (UiSlider*)self->_widget;
    slider->track_fill_color = color;
}

void uiSliderSetHandleColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiSlider, self->_widget, SLIDER_ID, SLIDER_ID_LEN);
    UiSlider* slider = (UiSlider*)self->_widget;
    slider->handle_color = color;
}

void uiSliderSetHandleLeak(UiWidget* self, f32 leak) {
    __CHECK_WIDGET__(UiSlider, self->_widget, SLIDER_ID, SLIDER_ID_LEN);
    UiSlider* slider = (UiSlider*)self->_widget;
    slider->handle_leak = leak;
}

void uiSliderSetLabelColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiSlider, self->_widget, SLIDER_ID, SLIDER_ID_LEN);
    UiSlider* slider = (UiSlider*)self->_widget;
    slider->label_color = color;
}

void uiSliderSetOnChange(UiWidget* self, void (*on_change)(f32 value)) {
    __CHECK_WIDGET__(UiSlider, self->_widget, SLIDER_ID, SLIDER_ID_LEN);
    UiSlider* slider = (UiSlider*)self->_widget;
    slider->on_change = on_change;
}

// ============================================================================
// Accessors
// ============================================================================

f32 uiSliderGetValue(UiWidget* self) {
    __CHECK_WIDGET__(UiSlider, self->_widget, SLIDER_ID, SLIDER_ID_LEN);
    return ((UiSlider*)self->_widget)->value;
}

// ============================================================================
// Trait functions
// ============================================================================

void uiSliderUpdate(void* self, Vector2 offset) {
    __CHECK_WIDGET__(UiSlider, self, SLIDER_ID, SLIDER_ID_LEN);
    UiSlider* slider = (UiSlider*)self;

    Vector2 mouse_pos = Vector2Subtract(GetMousePosition(), offset);

    f32 filled_ratio = (slider->value - slider->min) / (slider->max - slider->min);
    Vector2 handle_center = {
        .x = slider->bounds.x + (slider->bounds.width * filled_ratio),
        .y = slider->bounds.y + (slider->bounds.height / 2.0f),
    };
    f32 handle_radius = (slider->bounds.height / 2.0f) + (slider->handle_leak / 2.0f);
    bool mouse_down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    if (mouse_down && slider->state == SliderState_Dragging) {
        goto slider_uiSliderUpdate_skip_collision_check;
    }

    // Set the slider state before making modifications
    if (CheckCollisionPointCircle(mouse_pos, handle_center, handle_radius)) {
        if (mouse_down) {
            slider->state = SliderState_Dragging;
        } else {
            slider->state = SliderState_Hover;
        }
    } else {
        slider->state = SliderState_Idle;
    }

slider_uiSliderUpdate_skip_collision_check:

    f32 scale_ratio = (slider->max - slider->min) / (slider->bounds.width);

    if (slider->state == SliderState_Dragging) {
        f32 mouse_delta = GetMouseDelta().x;

        if (mouse_delta != 0.0f) {
            slider->value += (mouse_delta * scale_ratio);
            slider->value = Clamp(slider->value, slider->min, slider->max);

            if (slider->on_change) {
                slider->on_change(slider->value);
            }
        }
    }
}

void uiSliderRender(void* self) {
    __CHECK_WIDGET__(UiSlider, self, SLIDER_ID, SLIDER_ID_LEN);
    UiSlider* slider = (UiSlider*)self;

    f32 filled_ratio = (slider->value - slider->min) / (slider->max - slider->min);

    DrawRectangleRec(slider->bounds, slider->track_color);
    DrawRectangleRec(
        (Rectangle){
            .x = slider->bounds.x,
            .y = slider->bounds.y,
            .width = slider->bounds.width * filled_ratio,
            .height = slider->bounds.height,

        },
        slider->track_fill_color
    );

    Color handle_mod_color;
    switch (slider->state) {
        case SliderState_Hover:
            handle_mod_color = slider->handle_hover_tint;
            break;
        case SliderState_Dragging:
            handle_mod_color = slider->handle_drag_tint;
            break;
        default:
            handle_mod_color = WHITE;
            break;
    }
    // clang-format off
    DrawCircle(
        slider->bounds.x + (slider->bounds.width * filled_ratio),
        slider->bounds.y + (slider->bounds.height / 2.0f),
        (slider->bounds.height / 2.0f) + slider->handle_leak,
        ColorTint(slider->handle_color, handle_mod_color)
    );
    // clang-format on

    if (slider->label != nullptr) {
        DrawText(slider->label, slider->bounds.x, slider->bounds.y - SLIDER_LABEL_OFFSET, slider->fontsize, slider->label_color);
    }
}
