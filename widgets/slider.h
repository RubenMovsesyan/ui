#ifndef SLIDER_H
#define SLIDER_H

#include <allocators/arena.h>
#include <common.h>
#include <raylib.h>
#include <ui/ui.h>

constexpr const u8 SLIDER_ID[] = "_ui_slider";
constexpr usize SLIDER_ID_LEN = sizeof(SLIDER_ID);

typedef enum {
    SliderState_Idle,
    SliderState_Hover,
    SliderState_Dragging,
} SliderState;

typedef struct {
        u8 _id[SLIDER_ID_LEN];

        Rectangle bounds;
        f32 value;
        f32 min;
        f32 max;

        SliderState state;
        bool is_dragging;

        char* label;
        u32 fontsize;

        Color track_color;
        Color track_fill_color;
        Color handle_color;
        Color handle_hover_tint;
        Color handle_drag_tint;
        Color label_color;

        f32 handle_leak;

        void (*on_change)(f32 value);
} UiSlider;

UiWidget uiSliderCreate(Arena* arena, u32 x, u32 y, u32 width, u32 height, f32 min, f32 max, f32 value);

// Configuration functions
void uiSliderSetPosition(UiWidget* self, Vector2 pos);
void uiSliderSetDimensions(UiWidget* self, Vector2 dims);
void uiSliderSetValue(UiWidget* self, f32 value);
void uiSliderSetRange(UiWidget* self, f32 min, f32 max);
void uiSliderSetLabel(UiWidget* self, char* label);
void uiSliderSetFontSize(UiWidget* self, u32 fontsize);
void uiSliderSetTrackColor(UiWidget* self, Color color);
void uiSliderSetTrackFillColor(UiWidget* self, Color color);
void uiSliderSetHandleColor(UiWidget* self, Color color);
void uiSliderSetHandleLeak(UiWidget* self, f32 leak);
void uiSliderSetLabelColor(UiWidget* self, Color color);
void uiSliderSetOnChange(UiWidget* self, void (*on_change)(f32 value));

// Accessors
f32 uiSliderGetValue(UiWidget* self);

// Ui Trait functions
void uiSliderUpdate(void* self, Vector2 offset);
void uiSliderRender(void* self);

#endif // SLIDER_H
