#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <common.h>
#include <raylib.h>
#include <ui/ui.h>

constexpr const u8 CHECKBOX_ID[] = "_ui_checkbox";
constexpr usize CHECKBOX_ID_LEN = sizeof(CHECKBOX_ID);

typedef enum {
    CheckboxState_Normal,
    CheckboxState_Hover,
    CheckboxState_Pressed,
} CheckboxState;

typedef struct {
    u8            _id[CHECKBOX_ID_LEN];
    Rectangle     bounds;
    CheckboxState state;
    bool          checked;

    Color checked_color;
    Color unchecked_color;
    Color border_color;
    u32   border_weight;

    void (*on_change)(bool checked);
} UiCheckbox;

UiWidget uiCheckboxCreate(u32 x, u32 y, u32 size);

void uiCheckboxSetChecked(UiWidget* self, bool checked);
void uiCheckboxSetCheckedColor(UiWidget* self, Color color);
void uiCheckboxSetUncheckedColor(UiWidget* self, Color color);
void uiCheckboxSetBorderColor(UiWidget* self, Color color);
void uiCheckboxSetBorderWeight(UiWidget* self, u32 weight);
void uiCheckboxSetOnChange(UiWidget* self, void (*on_change)(bool checked));
bool uiCheckboxIsChecked(UiWidget* self);

void uiCheckboxUpdate(void* self, Vector2 offset);
void uiCheckboxRender(void* self);

#endif // CHECKBOX_H
