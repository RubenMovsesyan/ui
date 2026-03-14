#include "image_overlay.h"
#include <rlog.h>
#include <string.h>
#include <ui/widgets/image_view.h>

constexpr f32 DEFAULT_RADIUS = 8.0f;
constexpr f32 DEFAULT_BORDER_WIDTH = 2.0f;
constexpr Color DEFAULT_MARKER_COLOR = (Color){.r = 255, .g = 80, .b = 80, .a = 200};
constexpr Color DEFAULT_BORDER_COLOR = (Color){.r = 255, .g = 255, .b = 255, .a = 255};
constexpr Color DEFAULT_LABEL_COLOR = (Color){.r = 255, .g = 255, .b = 255, .a = 255};
constexpr u32 DEFAULT_LABEL_FONT_SIZE = 12;
constexpr usize INITIAL_LOCATIONS_CAP = 8;

// Replicates the render_bounds calculation from image_view.c so the overlay
// is positioned identically to the rendered image.
static Rectangle __computeImageRenderBounds(UiImageView* iv) {
    Rectangle render_bounds = iv->bounds;
    if (iv->scale_by_height) {
        f32 aspect_ratio = (f32)iv->texture.width / (f32)iv->texture.height;
        render_bounds.width = aspect_ratio * render_bounds.height;
    } else {
        f32 aspect_ratio = (f32)iv->texture.height / (f32)iv->texture.width;
        render_bounds.height = aspect_ratio * render_bounds.width;
    }
    return render_bounds;
}

UiWidget uiImageOverlayCreate(Arena* arena, UiWidget* attached_image_view) {
    UiImageOverlay* overlay = (UiImageOverlay*)arenaAlloc(arena, sizeof(UiImageOverlay));
    *overlay = (UiImageOverlay){
        .__arena = arena,
        .attached_image_view = attached_image_view,
        .locations = (OverlayLocation*)arenaAlloc(arena, INITIAL_LOCATIONS_CAP * sizeof(OverlayLocation)),
        .num_locations = 0,
        .locations_cap = INITIAL_LOCATIONS_CAP,
        .default_radius = DEFAULT_RADIUS,
        .default_color = DEFAULT_MARKER_COLOR,
        .default_border_color = DEFAULT_BORDER_COLOR,
        .default_border_width = DEFAULT_BORDER_WIDTH,
        .label_color = DEFAULT_LABEL_COLOR,
        .label_font_size = DEFAULT_LABEL_FONT_SIZE,
    };

    memcpy(&overlay->_id, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);

    return (UiWidget){
        ._widget = overlay,
        .update = uiImageOverlayUpdate,
        .render = uiImageOverlayRender,
    };
}

usize uiImageOverlayAddLocation(UiWidget* self, f32 x, f32 y) {
    __CHECK_WIDGET__(UiImageOverlay, self->_widget, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);
    UiImageOverlay* overlay = (UiImageOverlay*)self->_widget;

    if (overlay->num_locations >= overlay->locations_cap) {
        usize new_cap = overlay->locations_cap * 2;
        overlay->locations = (OverlayLocation*)arenaRealloc(
            overlay->__arena,
            (char*)overlay->locations,
            overlay->locations_cap * sizeof(OverlayLocation),
            new_cap * sizeof(OverlayLocation)
        );
        overlay->locations_cap = new_cap;
    }

    usize index = overlay->num_locations++;
    overlay->locations[index] = (OverlayLocation){
        .x = x,
        .y = y,
        .radius = 0.0f, // 0 means use default at render time
        .color = overlay->default_color,
        .border_color = overlay->default_border_color,
        .border_width = overlay->default_border_width,
        .label = nullptr,
    };
    return index;
}

void uiImageOverlayClearLocations(UiWidget* self) {
    __CHECK_WIDGET__(UiImageOverlay, self->_widget, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);
    UiImageOverlay* overlay = (UiImageOverlay*)self->_widget;
    overlay->num_locations = 0;
}

void uiImageOverlaySetLocationRadius(UiWidget* self, usize index, f32 radius) {
    __CHECK_WIDGET__(UiImageOverlay, self->_widget, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);
    UiImageOverlay* overlay = (UiImageOverlay*)self->_widget;
    if (index >= overlay->num_locations) return;
    overlay->locations[index].radius = radius;
}

void uiImageOverlaySetLocationColor(UiWidget* self, usize index, Color color) {
    __CHECK_WIDGET__(UiImageOverlay, self->_widget, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);
    UiImageOverlay* overlay = (UiImageOverlay*)self->_widget;
    if (index >= overlay->num_locations) return;
    overlay->locations[index].color = color;
}

void uiImageOverlaySetLocationBorderColor(UiWidget* self, usize index, Color color) {
    __CHECK_WIDGET__(UiImageOverlay, self->_widget, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);
    UiImageOverlay* overlay = (UiImageOverlay*)self->_widget;
    if (index >= overlay->num_locations) return;
    overlay->locations[index].border_color = color;
}

void uiImageOverlaySetLocationBorderWidth(UiWidget* self, usize index, f32 width) {
    __CHECK_WIDGET__(UiImageOverlay, self->_widget, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);
    UiImageOverlay* overlay = (UiImageOverlay*)self->_widget;
    if (index >= overlay->num_locations) return;
    overlay->locations[index].border_width = width;
}

void uiImageOverlaySetLocationLabel(UiWidget* self, usize index, char* label) {
    __CHECK_WIDGET__(UiImageOverlay, self->_widget, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);
    UiImageOverlay* overlay = (UiImageOverlay*)self->_widget;
    if (index >= overlay->num_locations) return;
    overlay->locations[index].label = label;
}

void uiImageOverlaySetDefaultRadius(UiWidget* self, f32 radius) {
    __CHECK_WIDGET__(UiImageOverlay, self->_widget, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);
    ((UiImageOverlay*)self->_widget)->default_radius = radius;
}

void uiImageOverlaySetDefaultColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiImageOverlay, self->_widget, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);
    ((UiImageOverlay*)self->_widget)->default_color = color;
}

void uiImageOverlaySetDefaultBorderColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiImageOverlay, self->_widget, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);
    ((UiImageOverlay*)self->_widget)->default_border_color = color;
}

void uiImageOverlaySetDefaultBorderWidth(UiWidget* self, f32 width) {
    __CHECK_WIDGET__(UiImageOverlay, self->_widget, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);
    ((UiImageOverlay*)self->_widget)->default_border_width = width;
}

void uiImageOverlaySetLabelColor(UiWidget* self, Color color) {
    __CHECK_WIDGET__(UiImageOverlay, self->_widget, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);
    ((UiImageOverlay*)self->_widget)->label_color = color;
}

void uiImageOverlaySetLabelFontSize(UiWidget* self, u32 font_size) {
    __CHECK_WIDGET__(UiImageOverlay, self->_widget, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);
    ((UiImageOverlay*)self->_widget)->label_font_size = font_size;
}

void uiImageOverlayUpdate(void* self, Vector2 offset) {
    __CHECK_WIDGET__(UiImageOverlay, self, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);
    (void)offset;
}

void uiImageOverlayRender(void* self) {
    __CHECK_WIDGET__(UiImageOverlay, self, IMAGE_OVERLAY_ID, IMAGE_OVERLAY_ID_LEN);
    UiImageOverlay* overlay = (UiImageOverlay*)self;

    if (!overlay->attached_image_view || overlay->num_locations == 0) return;

    UiImageView* iv = (UiImageView*)overlay->attached_image_view->_widget;
    if (iv->texture.width == 0 || iv->texture.height == 0) return;

    Rectangle rb = __computeImageRenderBounds(iv);
    f32 visible_w = (f32)iv->texture.width / iv->zoom;
    f32 visible_h = (f32)iv->texture.height / iv->zoom;

    BeginScissorMode((i32)rb.x, (i32)rb.y, (i32)rb.width, (i32)rb.height);

    for (usize i = 0; i < overlay->num_locations; i++) {
        OverlayLocation* loc = &overlay->locations[i];

        // Convert image pixel coords → screen coords
        f32 sx = rb.x + (loc->x - iv->pan.x) * rb.width / visible_w;
        f32 sy = rb.y + (loc->y - iv->pan.y) * rb.height / visible_h;

        f32 radius = loc->radius > 0.0f ? loc->radius : overlay->default_radius;

        // Border ring drawn as a slightly larger circle underneath
        if (loc->border_width > 0.0f) {
            DrawCircleV((Vector2){sx, sy}, radius + loc->border_width, loc->border_color);
        }

        DrawCircleV((Vector2){sx, sy}, radius, loc->color);

        if (loc->label) {
            DrawText(
                loc->label,
                (i32)(sx + radius + 4),
                (i32)(sy - (f32)overlay->label_font_size * 0.5f),
                overlay->label_font_size,
                overlay->label_color
            );
        }
    }

    EndScissorMode();
}
