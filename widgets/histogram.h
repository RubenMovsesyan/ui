#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <allocators/arena.h>
#include <common.h>
#include <image_io/image_manip.h>
#include <raylib.h>
#include <ui/ui.h>

constexpr const u8  HISTOGRAM_WIDGET_ID[]  = "_ui_histogram";
constexpr usize     HISTOGRAM_WIDGET_ID_LEN = sizeof(HISTOGRAM_WIDGET_ID);

typedef enum {
    HistogramChannel_RGB,       // overlaid R/G/B filled curves
    HistogramChannel_Luminance, // luminance-only curve
    HistogramChannel_All,       // R/G/B + luminance outline on top
} HistogramChannel;

typedef struct {
    u8 _id[HISTOGRAM_WIDGET_ID_LEN];

    Rectangle bounds;

    Histogram data;  // CPU-side copy, updated via uiHistogramSetData()
    u32       peak;  // cached max bin value across all channels (for normalization)

    HistogramChannel channel;
    Color            background_color;
    Color            r_color;
    Color            g_color;
    Color            b_color;
    Color            lum_color;
    u8               fill_alpha;  // alpha for filled channel areas
} UiHistogram;

UiWidget uiHistogramCreate(Arena* arena, u32 x, u32 y, u32 width, u32 height);

// Update the histogram data and recalculate the peak bin for normalization
void uiHistogramSetData(UiWidget* self, Histogram data);
void uiHistogramSetChannel(UiWidget* self, HistogramChannel channel);
void uiHistogramSetPosition(UiWidget* self, Vector2 pos);
void uiHistogramSetDimensions(UiWidget* self, Vector2 dims);

// Trait functions (registered in UiWidget vtable)
void uiHistogramUpdate(void* self, Vector2 offset);
void uiHistogramRender(void* self);

#endif // HISTOGRAM_H
