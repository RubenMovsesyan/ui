#ifndef IMAGE_OVERLAY_H
#define IMAGE_OVERLAY_H

#include <common.h>
#include <raylib.h>
#include <ui/ui.h>
#include <ui/widgets/image_view.h>

constexpr const u8 IMAGE_OVERLAY_ID[] = "_image_overlay";
constexpr usize IMAGE_OVERLAY_ID_LEN = sizeof(IMAGE_OVERLAY_ID);

// A single marker drawn on the overlay, positioned in image pixel coordinates
typedef struct {
        f32 x, y;           // position in image pixel coordinates
        f32 radius;         // screen-pixel radius (0 = use overlay default)
        Color color;        // fill color
        Color border_color; // border ring color
        f32 border_width;   // border ring thickness in screen pixels
        char* label;        // optional text label drawn beside the marker (nullptr = none)
} OverlayLocation;

typedef struct {
        u8 _id[IMAGE_OVERLAY_ID_LEN];

        UiWidget* attached_image_view; // image view this overlay is drawn on top of

        OverlayLocation* locations;
        usize num_locations;
        usize locations_cap;

        // Default style applied when a location is added
        f32 default_radius;
        Color default_color;
        Color default_border_color;
        f32 default_border_width;
        Color label_color;
        u32 label_font_size;
} UiImageOverlay;

// Creates an overlay widget attached to the given image view widget.
// The overlay has no visual until locations are added.
UiWidget uiImageOverlayCreate(UiWidget* attached_image_view);

// Location management
// Returns the index of the newly added location.
usize uiImageOverlayAddLocation(UiWidget* self, f32 x, f32 y);
void uiImageOverlayClearLocations(UiWidget* self);

// Per-location configuration (index must be < num_locations)
void uiImageOverlaySetLocationRadius(UiWidget* self, usize index, f32 radius);
void uiImageOverlaySetLocationColor(UiWidget* self, usize index, Color color);
void uiImageOverlaySetLocationBorderColor(UiWidget* self, usize index, Color color);
void uiImageOverlaySetLocationBorderWidth(UiWidget* self, usize index, f32 width);
void uiImageOverlaySetLocationLabel(UiWidget* self, usize index, char* label);

// Default style — affects locations added after this call
void uiImageOverlaySetDefaultRadius(UiWidget* self, f32 radius);
void uiImageOverlaySetDefaultColor(UiWidget* self, Color color);
void uiImageOverlaySetDefaultBorderColor(UiWidget* self, Color color);
void uiImageOverlaySetDefaultBorderWidth(UiWidget* self, f32 width);
void uiImageOverlaySetLabelColor(UiWidget* self, Color color);
void uiImageOverlaySetLabelFontSize(UiWidget* self, u32 font_size);

// Ui Trait functions
void uiImageOverlayUpdate(void* self, Vector2 offset);
void uiImageOverlayRender(void* self);

#endif // IMAGE_OVERLAY_H
