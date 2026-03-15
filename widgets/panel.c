#include "panel.h"
#include "raylib.h"
#include "raymath.h"
#include "ui/ui.h"
#include <rlog.h>
#include <stdlib.h>
#include <string.h>

constexpr usize DEFAULT_WIDGET_CAP = 10;
constexpr Color DEFAULT_BACKGROUND_COLOR = RAYWHITE;

static void uiPanelFreeWidget(void* self) {
    UiPanel* panel = (UiPanel*)self;
    for (usize i = 0; i < panel->widget_count; i++) {
        panel->widgets[i].free_widget(panel->widgets[i]._widget);
    }
    free(panel->widgets);
    UnloadRenderTexture(panel->texture);
    free(panel);
}

UiWidget uiPanelCreate(u32 x, u32 y, u32 width, u32 height) {
    UiPanel* panel = (UiPanel*)malloc(sizeof(UiPanel));
    *panel = (UiPanel){
        .bounds =
            (Rectangle){
                .x = x,
                .y = y,
                .width = width,
                .height = height,
            },
        .widgets = (UiWidget*)malloc(DEFAULT_WIDGET_CAP * sizeof(UiWidget)),
        .widget_count = 0,
        .widget_cap = DEFAULT_WIDGET_CAP,
        .background_color = DEFAULT_BACKGROUND_COLOR,
        .texture = LoadRenderTexture(width, height),
        .is_visible = true,
    };

    memcpy(&panel->_id, PANEL_ID, PANEL_ID_LEN);

    return (UiWidget){
        ._widget = panel,
        .update = uiPanelUpdate,
        .render = uiPanelRender,
        .free_widget = uiPanelFreeWidget,
    };
}

// Configuration functions
UiWidget* uiPanelAddWidget(UiWidget* self, UiWidget new_widget) {
    __CHECK_WIDGET__(UiPanel, self->_widget, PANEL_ID, PANEL_ID_LEN);
    UiPanel* panel = (UiPanel*)self->_widget;

    if (panel->widget_count >= panel->widget_cap) {
        panel->widget_cap *= 2;
        panel->widgets = (UiWidget*)realloc(panel->widgets, panel->widget_cap * sizeof(UiWidget));
    }

    panel->widgets[panel->widget_count++] = new_widget;
    return &panel->widgets[panel->widget_count - 1];
}

void uiPanelSetVisible(UiWidget* self, bool visible) {
    __CHECK_WIDGET__(UiPanel, self->_widget, PANEL_ID, PANEL_ID_LEN);
    UiPanel* panel = (UiPanel*)self->_widget;
    panel->is_visible = visible;
}

void uiPanelSetPosition(UiWidget* self, Vector2 position) {
    __CHECK_WIDGET__(UiPanel, self->_widget, PANEL_ID, PANEL_ID_LEN);
    UiPanel* panel = (UiPanel*)self->_widget;
    panel->bounds.x = position.x;
    panel->bounds.y = position.y;
}

void uiPanelSetDimensions(UiWidget* self, Vector2 dims) {
    __CHECK_WIDGET__(UiPanel, self->_widget, PANEL_ID, PANEL_ID_LEN);
    UiPanel* panel = (UiPanel*)self->_widget;
    panel->bounds.width = dims.x;
    panel->bounds.height = dims.y;
    UnloadRenderTexture(panel->texture);
    panel->texture = LoadRenderTexture(dims.x, dims.y);
}

void uiPanelSetBackgroundColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiPanel, self->_widget, PANEL_ID, PANEL_ID_LEN);
    UiPanel* panel = (UiPanel*)self->_widget;
    panel->background_color = color;
}

// Ui Trait functions
void uiPanelUpdate(void* self, Vector2 offset) {
    __CHECK_WIDGET__(UiPanel, self, PANEL_ID, PANEL_ID_LEN);
    UiPanel* panel = (UiPanel*)self;

    // No need to waste resources on updating the panel if it isn't visible
    if (!panel->is_visible) {
        return;
    }

    Vector2 mouse_pos = Vector2Subtract(GetMousePosition(), offset);
    if (CheckCollisionPointRec(mouse_pos, panel->bounds)) {
        for (u32 i = 0; i < panel->widget_count; ++i) {
            panel->widgets[i].update(panel->widgets[i]._widget, Vector2Subtract((Vector2){panel->bounds.x, panel->bounds.y}, offset));
        }
    }
}

void uiPanelRender(void* self) {
    __CHECK_WIDGET__(UiPanel, self, PANEL_ID, PANEL_ID_LEN);
    UiPanel* panel = (UiPanel*)self;

    if (!panel->is_visible) {
        return;
    }

    BeginTextureMode(panel->texture);
    DrawRectangle(0, 0, panel->texture.texture.width, panel->texture.texture.height, panel->background_color);
    for (u32 i = 0; i < panel->widget_count; ++i) {
        panel->widgets[i].render(panel->widgets[i]._widget);
    }
    EndTextureMode();

    Rectangle source_rect = {
        .x = 0,
        .y = 0,
        .width = panel->texture.texture.width,
        .height = -panel->texture.texture.height,
    };

    Rectangle dest_rect = {
        .x = panel->bounds.x,
        .y = panel->bounds.y,
        .width = panel->bounds.width,
        .height = panel->bounds.height,
    };

    DrawTexturePro(panel->texture.texture, source_rect, dest_rect, (Vector2){0, 0}, 0.0f, WHITE);
}
