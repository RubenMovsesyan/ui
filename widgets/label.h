#ifndef LABEL_H
#define LABEL_H

#include <common.h>
#include <raylib.h>
#include <ui/ui.h>

constexpr const u8 LABEL_ID[] = "_ui_label";
constexpr usize LABEL_ID_LEN = sizeof(LABEL_ID);

typedef struct {
    u8    _id[LABEL_ID_LEN];
    f32   x;
    f32   y;
    char* text;
    u32   fontsize;
    Color color;
} UiLabel;

UiWidget uiLabelCreate(u32 x, u32 y);

void uiLabelSetText(UiWidget* self, char* text);
void uiLabelSetFontSize(UiWidget* self, u32 fontsize);
void uiLabelSetColor(UiWidget* self, Color color);
void uiLabelSetPosition(UiWidget* self, Vector2 pos);

void uiLabelUpdate(void* self, Vector2 offset);
void uiLabelRender(void* self);

#endif // LABEL_H
