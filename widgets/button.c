#include "button.h"
#include "ui/ui.h"
#include <raylib.h>
#include <raymath.h>
#include <rlog.h>
#include <stdlib.h>
#include <string.h>

constexpr u32 DEFAULT_FONT_SIZE = 14;
constexpr u32 DEFAULT_BORDER_WEIGHT = 5;
constexpr Color DEFAULT_BORDER_COLOR = BLACK;
constexpr Color DEFAULT_BUTTON_COLOR = WHITE;
constexpr Color DEFAULT_TEXT_COLOR = BLACK;
constexpr Color DEFAULT_HOVER_TINT = LIGHTGRAY;
constexpr Color DEFAULT_PRESSED_TINT = DARKGRAY;

static void uiButtonFreeWidget(void* self) { free(self); }

UiWidget uiButtonCreate(u32 x, u32 y, u32 width, u32 height) {
    UiButton* button = (UiButton*)malloc(sizeof(UiButton));
    *button = (UiButton){
        .button_rect =
            (Rectangle){
                .x = x,
                .y = y,
                .width = width,
                .height = height,
            },
        .callback = nullptr,
        .text = nullptr,
        .state = ButtonState_Unpressed,
        .fontsize = DEFAULT_FONT_SIZE,
        .border_weight = DEFAULT_BORDER_WEIGHT,
        .button_color = DEFAULT_BUTTON_COLOR,
        .border_color = DEFAULT_BORDER_COLOR,
        .text_color = DEFAULT_TEXT_COLOR,
        .hover_tint = DEFAULT_HOVER_TINT,
        .pressed_tint = DEFAULT_PRESSED_TINT,
    };

    // Copy the ID for verification
    memcpy(&button->_id, BUTTON_ID, BUTTON_ID_LEN);

    return (UiWidget){
        ._widget = button,
        .update = uiButtonUpdate,
        .render = uiButtonRender,
        .free_widget = uiButtonFreeWidget,
    };
}

// Configuration functions
void uiButtonSetCallback(UiWidget* self, void (*callback)(void)) {
    __CHECK_WIDGET__(UiButton, self->_widget, BUTTON_ID, BUTTON_ID_LEN);
    UiButton* button = (UiButton*)(self->_widget);
    button->callback = callback;
    RLOG(LL_DEBUG, "Button callback set");
}

void uiButtonSetPosition(UiWidget* self, Vector2 pos) {
    __CHECK_WIDGET__(UiButton, self->_widget, BUTTON_ID, BUTTON_ID_LEN);
    UiButton* button = (UiButton*)(self->_widget);
    button->button_rect.x = pos.x;
    button->button_rect.y = pos.y;
    RLOG(LL_DEBUG, "Button position set to (%f, %f)", pos.x, pos.y);
}

void uiButtonSetDimensions(UiWidget* self, Vector2 dims) {
    __CHECK_WIDGET__(UiButton, self->_widget, BUTTON_ID, BUTTON_ID_LEN);
    UiButton* button = (UiButton*)(self->_widget);
    button->button_rect.width = dims.x;
    button->button_rect.height = dims.y;
    RLOG(LL_DEBUG, "Button dimensions set to (%f, %f)", dims.x, dims.y);
}

void uiButtonSetText(UiWidget* self, char* text) {
    __CHECK_WIDGET__(UiButton, self->_widget, BUTTON_ID, BUTTON_ID_LEN);
    UiButton* button = (UiButton*)(self->_widget);
    button->text = text;
    RLOG(LL_DEBUG, "Button text set to '%s'", text);
}

void uiButtonSetFontSize(UiWidget* self, u32 fontsize) {
    __CHECK_WIDGET__(UiButton, self->_widget, BUTTON_ID, BUTTON_ID_LEN);
    UiButton* button = (UiButton*)(self->_widget);
    button->fontsize = fontsize;
    RLOG(LL_DEBUG, "Button font size set to %d", fontsize);
}

void uiButtonSetBorderWeight(UiWidget* self, u32 weight) {
    __CHECK_WIDGET__(UiButton, self->_widget, BUTTON_ID, BUTTON_ID_LEN);
    UiButton* button = (UiButton*)(self->_widget);
    button->border_weight = weight;
    RLOG(LL_DEBUG, "Button border weight set to %d", weight);
}

void uiButtonSetButtonColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiButton, self->_widget, BUTTON_ID, BUTTON_ID_LEN);
    UiButton* button = (UiButton*)(self->_widget);
    button->button_color = color;
    RLOG(LL_DEBUG, "Button color set to (%d, %d, %d, %d)", color.r, color.g, color.b, color.a);
}

void uiButtonSetBorderColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiButton, self->_widget, BUTTON_ID, BUTTON_ID_LEN);
    UiButton* button = (UiButton*)(self->_widget);
    button->border_color = color;
    RLOG(LL_DEBUG, "Button border color set to (%d, %d, %d, %d)", color.r, color.g, color.b, color.a);
}

void uiButtonSetTextColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiButton, self->_widget, BUTTON_ID, BUTTON_ID_LEN);
    UiButton* button = (UiButton*)(self->_widget);
    button->text_color = color;
    RLOG(LL_DEBUG, "Button text color set to (%d, %d, %d, %d)", color.r, color.g, color.b, color.a);
}

void uiButtonSetHoverTint(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiButton, self->_widget, BUTTON_ID, BUTTON_ID_LEN);
    UiButton* button = (UiButton*)(self->_widget);
    button->hover_tint = color;
    RLOG(LL_DEBUG, "Button hover tint set to (%d, %d, %d, %d)", color.r, color.g, color.b, color.a);
}

void uiButtonSetPressedTint(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiButton, self->_widget, BUTTON_ID, BUTTON_ID_LEN);
    UiButton* button = (UiButton*)(self->_widget);
    button->pressed_tint = color;
    RLOG(LL_DEBUG, "Button pressed tint set to (%d, %d, %d, %d)", color.r, color.g, color.b, color.a);
}

// Trait functions
void uiButtonUpdate(void* self, Vector2 offset) {
    __CHECK_WIDGET__(UiButton, self, BUTTON_ID, BUTTON_ID_LEN);
    UiButton* button = (UiButton*)self;

    Vector2 mouse_pos = Vector2Subtract(GetMousePosition(), offset);
    if (CheckCollisionPointRec(mouse_pos, button->button_rect)) {
        bool mouse_down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        bool mouse_released = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

        if (mouse_down) {
            button->state = ButtonState_Pressed;
        } else {
            button->state = ButtonState_Hover;
        }

        if (mouse_released) {
            if (button->callback) {
                button->callback();
            }
        }
    } else {
        button->state = ButtonState_Unpressed;
    }
}

void uiButtonRender(void* self) {
    __CHECK_WIDGET__(UiButton, self, BUTTON_ID, BUTTON_ID_LEN);
    UiButton* button = (UiButton*)self;

    Color button_col = button->button_color;
    Color border_col = button->border_color;
    Color text_col = button->text_color;

    // Set the colors based on the button state
    if (button->state == ButtonState_Pressed) {
        button_col = ColorTint(button_col, button->pressed_tint);
        border_col = ColorTint(border_col, button->pressed_tint);
        text_col = ColorTint(text_col, button->pressed_tint);
    } else if (button->state == ButtonState_Hover) {
        button_col = ColorTint(button_col, button->hover_tint);
        border_col = ColorTint(border_col, button->hover_tint);
        text_col = ColorTint(text_col, button->hover_tint);
    }

    // Draw the border
    DrawRectangleRec(button->button_rect, border_col);
    // Draw the button
    DrawRectangle(
        button->button_rect.x + button->border_weight, button->button_rect.y + button->border_weight,
        button->button_rect.width - (button->border_weight * 2), button->button_rect.height - (button->border_weight * 2), button_col
    );

    if (button->text) {
        u32 text_width = MeasureText(button->text, button->fontsize);
        DrawText(
            button->text, button->button_rect.x + button->button_rect.width / 2 - (text_width / 2.0f),
            button->button_rect.y + button->button_rect.height / 2 - (button->fontsize / 2.0f), button->fontsize, text_col
        );
    }
}
