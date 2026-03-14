# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

This is a C UI library extracted from the AstroPhoto project, intended for use as a git submodule across multiple projects. It depends on **Raylib** (rendering) and **GLFW** (windowing). It targets C23.

## Structure

```
ui.h / ui.c          Core widget system: UiWidget (polymorphic via update/render fn pointers),
                     UiWindow (GLFW window + arena allocator + 2D camera)
widgets/             Seven widget implementations (each is a self-contained .h/.c pair)
```

### Widget inventory
- `button` — stateful button with hover/press callbacks
- `slider` — horizontal draggable slider with `on_change(f32)` callback
- `panel` — container widget; renders children to a texture and composites to screen
- `histogram` — display-only RGB/luminance histogram visualization
- `image_view` — image viewer with scroll-to-zoom and middle-mouse pan
- `image_overlay` — marker overlays attached to an `image_view`, clips to image bounds
- `scrollable_list` — multi-select scrollable list with Ctrl+click support

## Architecture

- **Polymorphism via fn pointers**: `UiWidget` holds `update()` and `render()` function pointers; concrete widget structs embed a `UiWidget` as their first member and are cast from it.
- **Arena allocator**: `UiWindow` owns an arena; widget state is allocated from it.
- **Texture-based compositing**: `panel` and `scrollable_list` render children to a `RenderTexture2D` for efficiency, then blit to screen.
- **Dynamic capacity arrays**: widgets that hold collections (panel children, overlay markers, list items) start small and double capacity on overflow.

## Build system (planned)

This library is intended to be self-describing for the `build.h`-based build system. The convention under consideration: a `build_lib.h` in this repo will expose a single function (e.g., `ui_lib_add_to_build(Build *b)`) that registers all source files, include paths, and required flags into the caller's build. Top-level `build.c` files in consuming projects will `#include` it and call it — no separate sub-build process needed.

The sources that need to be compiled are:
- `ui.c`
- `widgets/button.c`
- `widgets/slider.c`
- `widgets/panel.c`
- `widgets/histogram.c`
- `widgets/image_view.c`
- `widgets/image_overlay.c`
- `widgets/scrollable_list.c`

External link dependencies (caller's build.c must handle): Raylib, GLFW, and platform frameworks (Cocoa/Metal on macOS, X11/OpenGL on Linux).
