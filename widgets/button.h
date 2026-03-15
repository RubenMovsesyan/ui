#ifndef BUTTON_H
#define BUTTON_H

#include <common.h>
#include <raylib.h>
#include <ui/ui.h>

constexpr const u8 BUTTON_ID[] = "_ui_button";
constexpr usize BUTTON_ID_LEN = sizeof(BUTTON_ID);

typedef enum {
    ButtonState_Unpressed,
    ButtonState_Hover,
    ButtonState_Pressed,
} ButtonState;

typedef struct {
        u8 _id[BUTTON_ID_LEN];
        Rectangle button_rect;
        u32 border_weight;
        ButtonState state;
        char* text;
        u32 fontsize;

        Color button_color;
        Color border_color;
        Color text_color;

        Color hover_tint;
        Color pressed_tint;

        void (*callback)(void);
} UiButton;

UiWidget uiButtonCreate(u32 x, u32 y, u32 width, u32 height);

// Configuration functions
void uiButtonSetCallback(UiWidget* self, void (*callback)(void));
void uiButtonSetPosition(UiWidget* self, Vector2 pos);
void uiButtonSetDimensions(UiWidget* self, Vector2 dims);
void uiButtonSetText(UiWidget* self, char* text);
void uiButtonSetFontSize(UiWidget* self, u32 fontsize);
void uiButtonSetBorderWeight(UiWidget* self, u32 weight);
void uiButtonSetButtonColor(UiWidget* self, Color color);
void uiButtonSetBorderColor(UiWidget* self, Color color);
void uiButtonSetTextColor(UiWidget* self, Color color);
void uiButtonSetHoverTint(UiWidget* self, Color color);
void uiButtonSetPressedTint(UiWidget* self, Color color);

// Ui Trait functions
void uiButtonUpdate(void* self, Vector2 offset);
void uiButtonRender(void* self);

#endif // BUTTON_H
