#ifndef CANVAS_H
#define CANVAS_H

#include <common.h>
#include <ui/ui.h>

constexpr const u8 CANVAS_ID[] = "_canvas";
constexpr usize CANVAS_ID_LEN = sizeof(CANVAS_ID);

typedef struct {
        u8 _id[CANVAS_ID_LEN];
        Rectangle bounds;
        RenderTexture2D texture;
        Color background_color;
        f32 scale;  // pixels per world unit
        void (*draw)(RenderTexture2D texture, void* user_data);
        void* user_data;
} UiCanvas;

UiWidget uiCanvasCreate(u32 x, u32 y, u32 width, u32 height);

// Configuration functions
void uiCanvasSetDrawCallback(UiWidget* self, void (*draw)(RenderTexture2D, void*), void* user_data);
void uiCanvasSetPosition(UiWidget* self, Vector2 pos);
void uiCanvasSetDimensions(UiWidget* self, Vector2 dims);
void uiCanvasSetBackgroundColor(UiWidget* self, Color color);
void uiCanvasSetScale(UiWidget* self, f32 scale);

// Ui Trait functions
void uiCanvasUpdate(void* self, Vector2 offset);
void uiCanvasRender(void* self);

#endif // CANVAS_H
