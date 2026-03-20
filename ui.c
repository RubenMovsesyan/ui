#include "ui.h"

#include <raylib.h>
#include <rlog.h>
#include <stdlib.h>

constexpr usize MAX_UI_WIDGETS = 100;
constexpr i32 MAX_FPS = 165;

UiWindow uiCreateWindow(u32 width, u32 height, const char* title) {
    InitWindow(width, height, title);
    SetTargetFPS(MAX_FPS);

    UiWindow window = {
        ._glfw_window = (GLFWwindow*)GetWindowHandle(),
        .widget_cap = MAX_UI_WIDGETS,
        .widget_count = 0,
        .widgets = (UiWidget*)malloc(MAX_UI_WIDGETS * sizeof(UiWidget)),
        ._2D_camaera = {
            .zoom = 1.0f,
            .rotation = 0.0f,
            .offset = (Vector2){0.0f, 0.0f},
            .target = (Vector2){0.0f, 0.0f},
        }
    };

    return window;
}

void uiFree(UiWindow* window) {
    for (usize i = 0; i < window->widget_count; i++) {
        UiWidget* w = &window->widgets[i];
        w->free_widget(w->_widget);
    }
    free(window->widgets);
    CloseWindow();
}

UiWidget* uiAddWidget(UiWindow* window, UiWidget widget) {
    if (window->widget_count >= window->widget_cap) {
        window->widget_cap *= 2;
        window->widgets = (UiWidget*)realloc(window->widgets, window->widget_cap * sizeof(UiWidget));
    }

    window->widgets[window->widget_count++] = widget;
    return &window->widgets[window->widget_count - 1];
}

void uiUpdate(UiWindow* window) {
    for (u32 i = 0; i < window->widget_count; i++) {
        UiWidget* current_widget = &window->widgets[i];
        current_widget->update(current_widget->_widget, (Vector2){0.0f, 0.0f});
    }
}

void uiRender(UiWindow* window) {
    BeginMode2D(window->_2D_camaera);
    for (u32 i = 0; i < window->widget_count; i++) {
        UiWidget* current_widget = &window->widgets[i];
        current_widget->render(current_widget->_widget);
    }
    EndMode2D();
}
