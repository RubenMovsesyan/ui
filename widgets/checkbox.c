#include "checkbox.h"
#include "ui/ui.h"
#include <raylib.h>
#include <raymath.h>
#include <rlog.h>
#include <stdlib.h>
#include <string.h>

constexpr Color DEFAULT_CHECKED_COLOR   = WHITE;
constexpr Color DEFAULT_UNCHECKED_COLOR = {60, 60, 60, 255};
constexpr Color DEFAULT_CHECKBOX_BORDER_COLOR = GRAY;
constexpr u32   DEFAULT_CHECKBOX_BORDER_WEIGHT = 2;

static void uiCheckboxFreeWidget(void* self) { free(self); }

UiWidget uiCheckboxCreate(u32 x, u32 y, u32 size) {
    UiCheckbox* cb = (UiCheckbox*)malloc(sizeof(UiCheckbox));
    *cb = (UiCheckbox){
        .bounds          = (Rectangle){(f32)x, (f32)y, (f32)size, (f32)size},
        .state           = CheckboxState_Normal,
        .checked         = false,
        .checked_color   = DEFAULT_CHECKED_COLOR,
        .unchecked_color = DEFAULT_UNCHECKED_COLOR,
        .border_color    = DEFAULT_CHECKBOX_BORDER_COLOR,
        .border_weight   = DEFAULT_CHECKBOX_BORDER_WEIGHT,
        .on_change       = nullptr,
    };
    memcpy(&cb->_id, CHECKBOX_ID, CHECKBOX_ID_LEN);
    return (UiWidget){
        ._widget     = cb,
        .update      = uiCheckboxUpdate,
        .render      = uiCheckboxRender,
        .free_widget = uiCheckboxFreeWidget,
    };
}

void uiCheckboxSetChecked(UiWidget* self, bool checked) {
    __CHECK_WIDGET__(UiCheckbox, self->_widget, CHECKBOX_ID, CHECKBOX_ID_LEN);
    UiCheckbox* cb = (UiCheckbox*)(self->_widget);
    cb->checked = checked;
}

void uiCheckboxSetCheckedColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiCheckbox, self->_widget, CHECKBOX_ID, CHECKBOX_ID_LEN);
    UiCheckbox* cb = (UiCheckbox*)(self->_widget);
    cb->checked_color = color;
}

void uiCheckboxSetUncheckedColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiCheckbox, self->_widget, CHECKBOX_ID, CHECKBOX_ID_LEN);
    UiCheckbox* cb = (UiCheckbox*)(self->_widget);
    cb->unchecked_color = color;
}

void uiCheckboxSetBorderColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiCheckbox, self->_widget, CHECKBOX_ID, CHECKBOX_ID_LEN);
    UiCheckbox* cb = (UiCheckbox*)(self->_widget);
    cb->border_color = color;
}

void uiCheckboxSetBorderWeight(UiWidget* self, u32 weight) {
    __CHECK_WIDGET__(UiCheckbox, self->_widget, CHECKBOX_ID, CHECKBOX_ID_LEN);
    UiCheckbox* cb = (UiCheckbox*)(self->_widget);
    cb->border_weight = weight;
}

void uiCheckboxSetOnChange(UiWidget* self, void (*on_change)(bool checked)) {
    __CHECK_WIDGET__(UiCheckbox, self->_widget, CHECKBOX_ID, CHECKBOX_ID_LEN);
    UiCheckbox* cb = (UiCheckbox*)(self->_widget);
    cb->on_change = on_change;
}

bool uiCheckboxIsChecked(UiWidget* self) {
    __CHECK_WIDGET__(UiCheckbox, self->_widget, CHECKBOX_ID, CHECKBOX_ID_LEN);
    UiCheckbox* cb = (UiCheckbox*)(self->_widget);
    return cb->checked;
}

void uiCheckboxUpdate(void* self, Vector2 offset) {
    __CHECK_WIDGET__(UiCheckbox, self, CHECKBOX_ID, CHECKBOX_ID_LEN);
    UiCheckbox* cb = (UiCheckbox*)self;

    Vector2 mouse_pos = Vector2Subtract(GetMousePosition(), offset);
    if (CheckCollisionPointRec(mouse_pos, cb->bounds)) {
        bool mouse_down     = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        bool mouse_released = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

        if (mouse_down) {
            cb->state = CheckboxState_Pressed;
        } else {
            cb->state = CheckboxState_Hover;
        }

        if (mouse_released) {
            cb->checked = !cb->checked;
            if (cb->on_change) {
                cb->on_change(cb->checked);
            }
        }
    } else {
        cb->state = CheckboxState_Normal;
    }
}

void uiCheckboxRender(void* self) {
    __CHECK_WIDGET__(UiCheckbox, self, CHECKBOX_ID, CHECKBOX_ID_LEN);
    UiCheckbox* cb = (UiCheckbox*)self;

    Color fill = cb->checked ? cb->checked_color : cb->unchecked_color;

    // Draw border by drawing full rect in border color, then inset rect in fill color
    DrawRectangleRec(cb->bounds, cb->border_color);
    DrawRectangle(
        (i32)(cb->bounds.x + cb->border_weight),
        (i32)(cb->bounds.y + cb->border_weight),
        (i32)(cb->bounds.width  - cb->border_weight * 2),
        (i32)(cb->bounds.height - cb->border_weight * 2),
        fill
    );
}
