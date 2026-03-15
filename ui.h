#ifndef UI_H
#define UI_H

#include <GLFW/glfw3.h>
#include <common.h>
#include <raylib.h>
#include <stdlib.h>

#define __CHECK_WIDGET__(type, self, ID, ID_LEN)                                                                                                     \
    do {                                                                                                                                             \
        if (!self) {                                                                                                                                 \
            RLOG(LL_FATAL, "%s widget is NULL", __STRINGIFY__(type));                                                                                \
        }                                                                                                                                            \
        if (memcmp(&((type*)self)->_id, ID, ID_LEN) != 0) {                                                                                          \
            RLOG(LL_FATAL, "%s ID mismatch", __STRINGIFY__(type));                                                                                   \
        }                                                                                                                                            \
    } while (0)

typedef struct {
        void (*update)(void* /* self pointer */, Vector2 offset);
        void (*render)(void* /* self pointer */);
        void (*free_widget)(void* self);

        void* _widget;
} UiWidget;

typedef struct {
        GLFWwindow* _glfw_window;

        UiWidget* widgets;
        usize widget_count;
        usize widget_cap;

        Camera2D _2D_camaera;
} UiWindow;

UiWindow uiCreateWindow(u32 width, u32 height, const char* title);
void uiFree(UiWindow* window);

UiWidget* uiAddWidget(UiWindow* window, UiWidget widget);

void uiUpdate(UiWindow* window);
void uiRender(UiWindow* window);

#endif // UI_H
