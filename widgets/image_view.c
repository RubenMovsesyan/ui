#include "image_view.h"
#include "raylib.h"
#include "raymath.h"
#include <rlog.h>
#include <stdlib.h>
#include <string.h>

constexpr f32 DEFAULT_MAX_ZOOM = 50.0f;


static void uiImageViewFreeWidget(void* self) { free(self); }

UiWidget uiImageViewCreate(u32 x, u32 y, u32 width, u32 height, Texture2D texture) {
    UiImageView* image_view = (UiImageView*)malloc(sizeof(UiImageView));
    *image_view = (UiImageView){
        .bounds =
            {
                .x = x,
                .y = y,
                .width = width,
                .height = height,
            },
        .texture = texture,
        .scale_by_height = true,
        .zoom = 1.0f,
        .max_zoom = DEFAULT_MAX_ZOOM,
        .pan = (Vector2){0.0f, 0.0f},
        .is_dragging = false,
    };

    // Copy the ID for verification
    memcpy(&image_view->_id, IMAGE_VIEW_ID, IMAGE_VIEW_ID_LEN);

    return (UiWidget){
        ._widget = image_view,
        .update = uiImageViewUpdate,
        .render = uiImageViewRender,
        .free_widget = uiImageViewFreeWidget,
    };
}

// Configuration functions
void uiImageViewSetPosition(UiWidget* self, Vector2 pos) {
    __CHECK_WIDGET__(UiImageView, self->_widget, IMAGE_VIEW_ID, IMAGE_VIEW_ID_LEN);
    UiImageView* image_view = (UiImageView*)self->_widget;
    image_view->bounds.x = pos.x;
    image_view->bounds.y = pos.y;
}

void uiImageViewSetDimension(UiWidget* self, Vector2 dim) {
    __CHECK_WIDGET__(UiImageView, self->_widget, IMAGE_VIEW_ID, IMAGE_VIEW_ID_LEN);
    UiImageView* image_view = (UiImageView*)self->_widget;
    image_view->bounds.width = dim.x;
    image_view->bounds.height = dim.y;
}

void uiImageViewSetScaleByHeight(UiWidget* self, bool scale_by_height) {
    __CHECK_WIDGET__(UiImageView, self->_widget, IMAGE_VIEW_ID, IMAGE_VIEW_ID_LEN);
    UiImageView* image_view = (UiImageView*)self->_widget;
    image_view->scale_by_height = scale_by_height;
}

void uiImageViewSetTexture(UiWidget* self, Texture2D texture) {
    __CHECK_WIDGET__(UiImageView, self->_widget, IMAGE_VIEW_ID, IMAGE_VIEW_ID_LEN);
    UiImageView* image_view = (UiImageView*)self->_widget;
    UnloadTexture(image_view->texture);
    image_view->texture = texture;
}

void uiImageViewSetMaxZoom(UiWidget* self, f32 max_zoom) {
    __CHECK_WIDGET__(UiImageView, self->_widget, IMAGE_VIEW_ID, IMAGE_VIEW_ID_LEN);
    UiImageView* image_view = (UiImageView*)self->_widget;
    image_view->max_zoom = max_zoom;
}

// Trait functions
void uiImageViewUpdate(void* self, Vector2 offset) {
    __CHECK_WIDGET__(UiImageView, self, IMAGE_VIEW_ID, IMAGE_VIEW_ID_LEN);
    UiImageView* image_view = (UiImageView*)self;

    Vector2 mouse_pos = Vector2Subtract(GetMousePosition(), offset);
    if (CheckCollisionPointRec(mouse_pos, image_view->bounds)) {
        f32 scroll = GetMouseWheelMove();

        if (scroll != 0.0f) {
            Vector2 local_mouse = {
                (mouse_pos.x - image_view->bounds.x) / image_view->bounds.width,
                (mouse_pos.y - image_view->bounds.y) / image_view->bounds.height,
            };

            f32 old_src_w = image_view->texture.width / image_view->zoom;
            f32 old_src_h = image_view->texture.height / image_view->zoom;

            Vector2 tex_point = {
                image_view->pan.x + local_mouse.x * old_src_w,
                image_view->pan.y + local_mouse.y * old_src_h,
            };

            f32 old_zoom = image_view->zoom;
            image_view->zoom += scroll * 0.1f * image_view->zoom;
            image_view->zoom = Clamp(image_view->zoom, 1.0f, image_view->max_zoom);

            f32 new_src_w = image_view->texture.width / image_view->zoom;
            f32 new_src_h = image_view->texture.height / image_view->zoom;

            image_view->pan.x = tex_point.x - local_mouse.x * new_src_w;
            image_view->pan.y = tex_point.y - local_mouse.y * new_src_h;

            image_view->pan.x = Clamp(image_view->pan.x, 0.0f, image_view->texture.width - new_src_w);
            image_view->pan.y = Clamp(image_view->pan.y, 0.0f, image_view->texture.height - new_src_h);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            image_view->is_dragging = true;
        } else {
            image_view->is_dragging = false;
        }

        if (image_view->is_dragging) {
            Vector2 mouse_delta = GetMouseDelta();

            image_view->pan.x -= mouse_delta.x / (image_view->zoom / 2.0f);
            image_view->pan.y -= mouse_delta.y / (image_view->zoom / 2.0f);

            f32 src_w = image_view->texture.width / image_view->zoom;
            f32 src_h = image_view->texture.height / image_view->zoom;

            image_view->pan.x = Clamp(image_view->pan.x, 0.0f, image_view->texture.width - src_w);
            image_view->pan.y = Clamp(image_view->pan.y, 0.0f, image_view->texture.height - src_h);
        }
    }
}

void uiImageViewRender(void* self) {
    __CHECK_WIDGET__(UiImageView, self, IMAGE_VIEW_ID, IMAGE_VIEW_ID_LEN);
    UiImageView* image_view = (UiImageView*)self;
    f32 zoom = image_view->zoom;

    Rectangle render_bounds = image_view->bounds;
    if (image_view->scale_by_height) {
        f32 aspect_ratio = (f32)image_view->texture.width / (f32)image_view->texture.height;
        render_bounds.width = aspect_ratio * render_bounds.height;
    } else {
        f32 aspect_ratio = (f32)image_view->texture.height / (f32)image_view->texture.width;
        render_bounds.height = aspect_ratio * render_bounds.width;
    }

    Rectangle source = {
        .x = image_view->pan.x,
        .y = image_view->pan.y,
        .width = image_view->texture.width / zoom,
        .height = image_view->texture.height / zoom,
    };

    DrawTexturePro(image_view->texture, source, render_bounds, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}
