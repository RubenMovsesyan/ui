#include "label.h"
#include "ui/ui.h"
#include <raylib.h>
#include <rlog.h>
#include <stdlib.h>
#include <string.h>

constexpr u32 DEFAULT_LABEL_FONT_SIZE = 14;
constexpr Color DEFAULT_LABEL_COLOR = WHITE;

static void uiLabelFreeWidget(void* self) { free(self); }

UiWidget uiLabelCreate(u32 x, u32 y) {
    UiLabel* label = (UiLabel*)malloc(sizeof(UiLabel));
    *label = (UiLabel){
        .x        = (f32)x,
        .y        = (f32)y,
        .text     = nullptr,
        .fontsize = DEFAULT_LABEL_FONT_SIZE,
        .color    = DEFAULT_LABEL_COLOR,
    };
    memcpy(&label->_id, LABEL_ID, LABEL_ID_LEN);
    return (UiWidget){
        ._widget     = label,
        .update      = uiLabelUpdate,
        .render      = uiLabelRender,
        .free_widget = uiLabelFreeWidget,
    };
}

void uiLabelSetText(UiWidget* self, char* text) {
    __CHECK_WIDGET__(UiLabel, self->_widget, LABEL_ID, LABEL_ID_LEN);
    UiLabel* label = (UiLabel*)(self->_widget);
    label->text = text;
}

void uiLabelSetFontSize(UiWidget* self, u32 fontsize) {
    __CHECK_WIDGET__(UiLabel, self->_widget, LABEL_ID, LABEL_ID_LEN);
    UiLabel* label = (UiLabel*)(self->_widget);
    label->fontsize = fontsize;
}

void uiLabelSetColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiLabel, self->_widget, LABEL_ID, LABEL_ID_LEN);
    UiLabel* label = (UiLabel*)(self->_widget);
    label->color = color;
}

void uiLabelSetPosition(UiWidget* self, Vector2 pos) {
    __CHECK_WIDGET__(UiLabel, self->_widget, LABEL_ID, LABEL_ID_LEN);
    UiLabel* label = (UiLabel*)(self->_widget);
    label->x = pos.x;
    label->y = pos.y;
}

void uiLabelUpdate(void* self, Vector2 offset) {
    (void)self;
    (void)offset;
}

void uiLabelRender(void* self) {
    __CHECK_WIDGET__(UiLabel, self, LABEL_ID, LABEL_ID_LEN);
    UiLabel* label = (UiLabel*)self;
    if (label->text != nullptr) {
        DrawText(label->text, (i32)label->x, (i32)label->y, (i32)label->fontsize, label->color);
    }
}
