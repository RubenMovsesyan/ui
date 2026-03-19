#ifndef BUILD_H
#error "Include build.h before ui_lib/build_lib.h"
#endif

#ifndef UI_LIB_BUILD_H
#define UI_LIB_BUILD_H

#include <string.h>

// Extract the directory containing this header at preprocessing time.
// __FILE__ expands to the path the compiler used to open this file,
// e.g. "vendor/ui/build_lib.h" -> "vendor/ui"
static inline char* _ui_lib_dir(void) {
    static char dir[4096];
    static int done = 0;
    if (!done) {
        const char* f = __FILE__;
        const char* slash = strrchr(f, '/');
        if (slash) {
            size_t n = (size_t)(slash - f);
            memcpy(dir, f, n);
            dir[n] = '\0';
        } else {
            // Header is in the same directory as the including file
            dir[0] = '.';
            dir[1] = '\0';
        }
        done = 1;
    }
    return dir;
}

// Returns the parent directory of the library root.
// e.g. "vendor/ui" -> "vendor"
// Headers are included as <ui/ui.h>, so the parent must be on the include path.
static inline char* _ui_lib_parent_dir(void) {
    static char parent[4096];
    static int done = 0;
    if (!done) {
        const char* d = _ui_lib_dir();
        const char* slash = strrchr(d, '/');
        if (slash) {
            size_t n = (size_t)(slash - d);
            memcpy(parent, d, n);
            parent[n] = '\0';
        } else {
            parent[0] = '.';
            parent[1] = '\0';
        }
        done = 1;
    }
    return parent;
}

// Build a path string: <library_root>/<file>, allocated from the arena.
static inline char* _ui_lib_path(const char* file) {
    char* d = _ui_lib_dir();
    size_t n = strlen(d) + 1 + strlen(file) + 1;
    char* buf = arenaAlloc(n);
    snprintf(buf, n, "%s/%s", d, file);
    return buf;
}

// Register all ui_lib sources and include paths into the current build step.
// External dependencies (raylib, GLFW, platform frameworks) must be added
// by the caller — this function only adds what lives in this library.
static inline void ui_lib_add_to_build(Build* b) {
    buildAddInclude(b, newDirectInclude(_ui_lib_parent_dir()));
    buildAddObject(b, newObject(_ui_lib_path("ui.c")));
    buildAddObject(b, newObject(_ui_lib_path("widgets/button.c")));
    buildAddObject(b, newObject(_ui_lib_path("widgets/slider.c")));
    buildAddObject(b, newObject(_ui_lib_path("widgets/panel.c")));
    buildAddObject(b, newObject(_ui_lib_path("widgets/image_view.c")));
    buildAddObject(b, newObject(_ui_lib_path("widgets/image_overlay.c")));
    buildAddObject(b, newObject(_ui_lib_path("widgets/scrollable_list.c")));
    buildAddObject(b, newObject(_ui_lib_path("widgets/canvas.c")));
    buildAddObject(b, newObject(_ui_lib_path("widgets/label.c")));
    buildAddObject(b, newObject(_ui_lib_path("widgets/checkbox.c")));
    buildAddObject(b, newObject(_ui_lib_path("widgets/dropdown.c")));
}

#endif // UI_LIB_BUILD_H
