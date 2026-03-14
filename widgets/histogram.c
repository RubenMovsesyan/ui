#include "histogram.h"
#include <raylib.h>
#include <rlog.h>
#include <string.h>

constexpr Color DEFAULT_HISTOGRAM_BG    = {20, 20, 20, 200};
constexpr Color DEFAULT_HISTOGRAM_R     = {220, 60, 60, 255};
constexpr Color DEFAULT_HISTOGRAM_G     = {60, 200, 60, 255};
constexpr Color DEFAULT_HISTOGRAM_B     = {60, 100, 220, 255};
constexpr Color DEFAULT_HISTOGRAM_LUM   = {200, 200, 200, 255};
constexpr u8    DEFAULT_HISTOGRAM_ALPHA = 80;

// ============================================================================
// Helpers
// ============================================================================

static u32 calcPeak(const Histogram* data, HistogramChannel channel) {
    u32 peak = 1;  // avoid division by zero
    if (channel == HistogramChannel_RGB || channel == HistogramChannel_All) {
        for (i32 i = 0; i < HISTOGRAM_BINS; i++) {
            if (data->r[i] > peak) peak = data->r[i];
            if (data->g[i] > peak) peak = data->g[i];
            if (data->b[i] > peak) peak = data->b[i];
        }
    }
    if (channel == HistogramChannel_Luminance || channel == HistogramChannel_All) {
        for (i32 i = 0; i < HISTOGRAM_BINS; i++) {
            if (data->luminance[i] > peak) peak = data->luminance[i];
        }
    }
    return peak;
}

// Draw one channel as a filled bar chart.
// Each of the 256 bins maps to a proportional-width column drawn from the bottom.
static void drawChannel(const u32* bins, f32 x, f32 y, f32 w, f32 h, u32 peak, Color color) {
    f32 bin_w = w / (f32)HISTOGRAM_BINS;
    Color fill = color;
    fill.a     = DEFAULT_HISTOGRAM_ALPHA;

    for (i32 i = 0; i < HISTOGRAM_BINS; i++) {
        f32 normalized = (f32)bins[i] / (f32)peak;
        f32 bar_h      = normalized * h;
        f32 bar_x      = x + (f32)i * bin_w;
        f32 bar_y      = y + h - bar_h;

        DrawRectangleRec((Rectangle){bar_x, bar_y, bin_w + 0.5f, bar_h}, fill);
    }
}

// Draw a channel as a line outline only (for luminance overlay on top of RGB)
static void drawChannelOutline(const u32* bins, f32 x, f32 y, f32 w, f32 h, u32 peak, Color color) {
    f32 bin_w = w / (f32)HISTOGRAM_BINS;

    for (i32 i = 0; i + 1 < HISTOGRAM_BINS; i++) {
        f32 n0 = (f32)bins[i]     / (f32)peak;
        f32 n1 = (f32)bins[i + 1] / (f32)peak;
        f32 x0 = x + (f32)i * bin_w + bin_w * 0.5f;
        f32 x1 = x + (f32)(i + 1) * bin_w + bin_w * 0.5f;
        f32 y0 = y + h - n0 * h;
        f32 y1 = y + h - n1 * h;

        DrawLineEx((Vector2){x0, y0}, (Vector2){x1, y1}, 1.5f, color);
    }
}

// ============================================================================
// Widget API
// ============================================================================

UiWidget uiHistogramCreate(Arena* arena, u32 x, u32 y, u32 width, u32 height) {
    UiHistogram* hist = (UiHistogram*)arenaAlloc(arena, sizeof(UiHistogram));
    *hist = (UiHistogram){
        .bounds           = (Rectangle){(f32)x, (f32)y, (f32)width, (f32)height},
        .data             = {0},
        .peak             = 1,
        .channel          = HistogramChannel_RGB,
        .background_color = DEFAULT_HISTOGRAM_BG,
        .r_color          = DEFAULT_HISTOGRAM_R,
        .g_color          = DEFAULT_HISTOGRAM_G,
        .b_color          = DEFAULT_HISTOGRAM_B,
        .lum_color        = DEFAULT_HISTOGRAM_LUM,
        .fill_alpha       = DEFAULT_HISTOGRAM_ALPHA,
    };
    memcpy(&hist->_id, HISTOGRAM_WIDGET_ID, HISTOGRAM_WIDGET_ID_LEN);

    return (UiWidget){
        ._widget = hist,
        .update  = uiHistogramUpdate,
        .render  = uiHistogramRender,
    };
}

void uiHistogramSetData(UiWidget* self, Histogram data) {
    __CHECK_WIDGET__(UiHistogram, self->_widget, HISTOGRAM_WIDGET_ID, HISTOGRAM_WIDGET_ID_LEN);
    UiHistogram* hist = (UiHistogram*)self->_widget;
    hist->data = data;
    hist->peak = calcPeak(&data, hist->channel);
}

void uiHistogramSetChannel(UiWidget* self, HistogramChannel channel) {
    __CHECK_WIDGET__(UiHistogram, self->_widget, HISTOGRAM_WIDGET_ID, HISTOGRAM_WIDGET_ID_LEN);
    UiHistogram* hist = (UiHistogram*)self->_widget;
    hist->channel = channel;
    hist->peak    = calcPeak(&hist->data, channel);
}

void uiHistogramSetPosition(UiWidget* self, Vector2 pos) {
    __CHECK_WIDGET__(UiHistogram, self->_widget, HISTOGRAM_WIDGET_ID, HISTOGRAM_WIDGET_ID_LEN);
    UiHistogram* hist = (UiHistogram*)self->_widget;
    hist->bounds.x = pos.x;
    hist->bounds.y = pos.y;
}

void uiHistogramSetDimensions(UiWidget* self, Vector2 dims) {
    __CHECK_WIDGET__(UiHistogram, self->_widget, HISTOGRAM_WIDGET_ID, HISTOGRAM_WIDGET_ID_LEN);
    UiHistogram* hist = (UiHistogram*)self->_widget;
    hist->bounds.width  = dims.x;
    hist->bounds.height = dims.y;
}

// ============================================================================
// Trait functions
// ============================================================================

void uiHistogramUpdate(void* self, Vector2 offset) {
    (void)self;
    (void)offset;
    // No interaction — display only
}

void uiHistogramRender(void* self) {
    __CHECK_WIDGET__(UiHistogram, self, HISTOGRAM_WIDGET_ID, HISTOGRAM_WIDGET_ID_LEN);
    UiHistogram* hist = (UiHistogram*)self;

    f32 x = hist->bounds.x;
    f32 y = hist->bounds.y;
    f32 w = hist->bounds.width;
    f32 h = hist->bounds.height;

    // Background
    DrawRectangleRec(hist->bounds, hist->background_color);

    if (hist->peak <= 1)
        return;  // no data yet

    switch (hist->channel) {
        case HistogramChannel_RGB:
            drawChannel(hist->data.r, x, y, w, h, hist->peak, hist->r_color);
            drawChannel(hist->data.g, x, y, w, h, hist->peak, hist->g_color);
            drawChannel(hist->data.b, x, y, w, h, hist->peak, hist->b_color);
            break;

        case HistogramChannel_Luminance:
            drawChannel(hist->data.luminance, x, y, w, h, hist->peak, hist->lum_color);
            break;

        case HistogramChannel_All:
            drawChannel(hist->data.r, x, y, w, h, hist->peak, hist->r_color);
            drawChannel(hist->data.g, x, y, w, h, hist->peak, hist->g_color);
            drawChannel(hist->data.b, x, y, w, h, hist->peak, hist->b_color);
            drawChannelOutline(hist->data.luminance, x, y, w, h, hist->peak, hist->lum_color);
            break;
    }

    // Border
    DrawRectangleLinesEx(hist->bounds, 1.0f, (Color){80, 80, 80, 255});
}
