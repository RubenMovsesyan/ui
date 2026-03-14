#ifndef PANEL_H
#define PANEL_H

#include <common.h>
#include <ui/ui.h>

constexpr const u8 PANEL_ID[] = "_panel";
constexpr usize PANEL_ID_LEN = sizeof(PANEL_ID);

typedef struct {
        u8 _id[PANEL_ID_LEN];
        Arena* __arena;

        UiWidget* widgets;
        usize widget_count;
        usize widget_cap;
        bool is_visible;

        Rectangle bounds;
        RenderTexture2D texture;

        Color background_color;
} UiPanel;

UiWidget uiPanelCreate(Arena* arena, u32 x, u32 y, u32 width, u32 height);

// Configuration functions
UiWidget* uiPanelAddWidget(UiWidget* self, UiWidget new_widget);
void uiPanelSetVisible(UiWidget* self, bool visible);
void uiPanelSetPosition(UiWidget* self, Vector2 position);
void uiPanelSetDimensions(UiWidget* self, Vector2 dims);
void uiPanelSetBackgroundColor(UiWidget* self, Color color);

// Ui Trait functions
void uiPanelUpdate(void* self, Vector2 offset);
void uiPanelRender(void* self);

#endif // PANEL_H
