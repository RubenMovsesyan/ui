#include "canvas.h"
#include "raylib.h"
#include "rlgl.h"
#include "ui/ui.h"
#include <rlog.h>
#include <stdlib.h>
#include <string.h>

static void uiCanvasFreeWidget(void* self) {
    UiCanvas* canvas = (UiCanvas*)self;
    UnloadRenderTexture(canvas->texture);
    free(canvas);
}

UiWidget uiCanvasCreate(u32 x, u32 y, u32 width, u32 height) {
    UiCanvas* canvas = (UiCanvas*)malloc(sizeof(UiCanvas));
    *canvas = (UiCanvas){
        .bounds =
            (Rectangle){
                .x = x,
                .y = y,
                .width = width,
                .height = height,
            },
        .texture = LoadRenderTexture(width, height),
        .background_color = BLACK,
        .scale = 1.0f,
        .draw = NULL,
        .user_data = NULL,
    };

    memcpy(&canvas->_id, CANVAS_ID, CANVAS_ID_LEN);

    return (UiWidget){
        ._widget = canvas,
        .update = uiCanvasUpdate,
        .render = uiCanvasRender,
        .free_widget = uiCanvasFreeWidget,
    };
}

void uiCanvasSetDrawCallback(UiWidget* self, void (*draw)(RenderTexture2D, void*), void* user_data) {
    __CHECK_WIDGET__(UiCanvas, self->_widget, CANVAS_ID, CANVAS_ID_LEN);
    UiCanvas* canvas = (UiCanvas*)self->_widget;
    canvas->draw = draw;
    canvas->user_data = user_data;
}

void uiCanvasSetPosition(UiWidget* self, Vector2 pos) {
    __CHECK_WIDGET__(UiCanvas, self->_widget, CANVAS_ID, CANVAS_ID_LEN);
    UiCanvas* canvas = (UiCanvas*)self->_widget;
    canvas->bounds.x = pos.x;
    canvas->bounds.y = pos.y;
}

void uiCanvasSetDimensions(UiWidget* self, Vector2 dims) {
    __CHECK_WIDGET__(UiCanvas, self->_widget, CANVAS_ID, CANVAS_ID_LEN);
    UiCanvas* canvas = (UiCanvas*)self->_widget;
    canvas->bounds.width = dims.x;
    canvas->bounds.height = dims.y;
    UnloadRenderTexture(canvas->texture);
    canvas->texture = LoadRenderTexture(dims.x, dims.y);
}

void uiCanvasSetBackgroundColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiCanvas, self->_widget, CANVAS_ID, CANVAS_ID_LEN);
    UiCanvas* canvas = (UiCanvas*)self->_widget;
    canvas->background_color = color;
}

void uiCanvasSetScale(UiWidget* self, f32 scale) {
    __CHECK_WIDGET__(UiCanvas, self->_widget, CANVAS_ID, CANVAS_ID_LEN);
    UiCanvas* canvas = (UiCanvas*)self->_widget;
    canvas->scale = scale;
}

void uiCanvasUpdate(void* self, Vector2 offset) {
    (void)self;
    (void)offset;
}

void uiCanvasRender(void* self) {
    __CHECK_WIDGET__(UiCanvas, self, CANVAS_ID, CANVAS_ID_LEN);
    UiCanvas* canvas = (UiCanvas*)self;

    BeginTextureMode(canvas->texture);
    ClearBackground(canvas->background_color);
    if (canvas->draw) {
        f32 hw = canvas->texture.texture.width / 2.0f / canvas->scale;
        f32 hh = canvas->texture.texture.height / 2.0f / canvas->scale;
        rlSetCullFace(RL_CULL_FACE_FRONT);
        rlMatrixMode(RL_PROJECTION);
        rlPushMatrix();
        rlLoadIdentity();
        rlOrtho(-hw, hw, -hh, hh, -1, 1);
        rlMatrixMode(RL_MODELVIEW);
        canvas->draw(canvas->texture, canvas->user_data);
        rlDrawRenderBatchActive(); // flush while our projection is still active
        rlMatrixMode(RL_PROJECTION);
        rlPopMatrix();
        rlMatrixMode(RL_MODELVIEW);
        rlSetCullFace(RL_CULL_FACE_BACK);
    }
    EndTextureMode();

    Rectangle source_rect = {
        .x = 0,
        .y = 0,
        .width = canvas->texture.texture.width,
        .height = -canvas->texture.texture.height,
    };

    Rectangle dest_rect = {
        .x = canvas->bounds.x,
        .y = canvas->bounds.y,
        .width = canvas->bounds.width,
        .height = canvas->bounds.height,
    };

    DrawTexturePro(canvas->texture.texture, source_rect, dest_rect, (Vector2){0, 0}, 0.0f, WHITE);
}
