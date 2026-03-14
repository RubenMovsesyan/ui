#ifndef IMAGE_VIEW_H
#define IMAGE_VIEW_H

#include <allocators/arena.h>
#include <common.h>
#include <raylib.h>
#include <ui/ui.h>

constexpr const u8 IMAGE_VIEW_ID[] = "_image_view";
constexpr usize IMAGE_VIEW_ID_LEN = sizeof(IMAGE_VIEW_ID);

typedef struct {
        u8 _id[IMAGE_VIEW_ID_LEN];

        Texture2D texture;
        Rectangle bounds;
        bool scale_by_height;
        f32 zoom;
        f32 max_zoom;
        Vector2 pan;
        bool is_dragging;
} UiImageView;

UiWidget uiImageViewCreate(Arena* arena, u32 x, u32 y, u32 width, u32 height, Texture2D texture);

// Configuration functions
void uiImageViewSetPosition(UiWidget* self, Vector2 pos);
void uiImageViewSetDimension(UiWidget* self, Vector2 pos);
void uiImageViewSetTexture(UiWidget* self, Texture2D texture);
void uiImageViewSetScaleByHeight(UiWidget* self, bool scale_by_height);
void uiImageViewSetMaxZoom(UiWidget* self, f32 max_zoom);

// Ui Trait functions
void uiImageViewUpdate(void* self, Vector2 offset);
void uiImageViewRender(void* self);

#endif // IMAGE_VIEW_H
