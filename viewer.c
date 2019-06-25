
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <bcm_host.h>
#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "est.h"
#include "viewer.h"

#define VISUALIZER_ROWS 2
#define VISUALIZER_COLS 3

const GLbyte vertex_ptr[6 * 4 * 3] = {
    // face 1
   -5, -2,  10,
   5,  -2,  10,
   -5, 2,  10,
   5,  2,  10,
   // face 2
   -5, -2, -10,
   -5, 2, -10,
   5, -2, -10,
   5,  2, -10,
   // face 3
   -5, -2,  10,
   -5,  2,  10,
   -5, -2, -10,
   -5,  2, -10,
   // face 4
   5, -2, -10,
   5,  2, -10,
   5, -2,  10,
   5,  2,  10,
   // face 5
   -5,  2,  10,
   5,  2,  10,
   -5,  2, -10,
   5,  2, -10,
   // face 6
   -5, -2,  10,
   -5, -2, -10,
   5, -2,  10,
   5, -2, -10,
};

const GLfloat color_ptr[6 * 3] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f,
};

typedef struct {
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
    EGL_DISPMANX_WINDOW_T native_window;
    uint32_t screen_width;
    uint32_t screen_height;
} _visualizer;

_visualizer _vp, *_v = &_vp;

void visualizer_init() {
    memset(_v, 0, sizeof(_visualizer));

    bcm_host_init();

    _v->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    assert(_v->display != EGL_NO_DISPLAY);

    EGLBoolean glres = eglInitialize(_v->display, NULL, NULL);
    assert(glres != EGL_FALSE);

    const EGLint egl_attributes[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE,
    };

    EGLConfig config;
    EGLint num_config;
    glres = eglChooseConfig(_v->display, egl_attributes, &config, 1, &num_config);
    assert(glres != EGL_FALSE);

    _v->context = eglCreateContext(_v->display, config, EGL_NO_CONTEXT, NULL);
    assert(_v->context != EGL_NO_CONTEXT);

    int32_t gres = graphics_get_display_size(0, &_v->screen_width, &_v->screen_height);
    assert(gres >= 0);

    VC_RECT_T dst_rect;
    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width = _v->screen_width;
    dst_rect.height = _v->screen_height;

    VC_RECT_T src_rect;
    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.width = _v->screen_width << 16;
    src_rect.height = _v->screen_height << 16;

    DISPMANX_DISPLAY_HANDLE_T dmx_display = vc_dispmanx_display_open(0);
    DISPMANX_UPDATE_HANDLE_T dmx_update = vc_dispmanx_update_start(0);
    DISPMANX_ELEMENT_HANDLE_T dmx_element = vc_dispmanx_element_add(
        dmx_update, dmx_display, 0, &dst_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE, 0, 0, 0);
    vc_dispmanx_update_submit_sync(dmx_update);

    _v->native_window.element = dmx_element;
    _v->native_window.width = _v->screen_width;
    _v->native_window.height = _v->screen_height;

    _v->surface = eglCreateWindowSurface(_v->display, config, &_v->native_window, NULL);
    assert(_v->surface != EGL_NO_SURFACE);

    glres = eglMakeCurrent(_v->display, _v->surface, _v->surface, _v->context);
    assert(glres != EGL_FALSE);

    glClearColor(0.25f, 0.35f, 0.45f, 1.0f);

    glEnable(GL_CULL_FACE);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_BYTE, 0, vertex_ptr);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float nearp = 1.0f;
    float farp = 500.0f;
    float hht = nearp * (float)tan(45.0 / 2.0 / 180.0 * M_PI);
    float hwd = hht * (float)_v->screen_width / (float)_v->screen_height;
    glFrustumf(-hwd, hwd, -hht, hht, nearp, farp);

    glMatrixMode(GL_MODELVIEW);
}

void visualizer_draw_start() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void visualizer_draw_end() {
    eglSwapBuffers(_v->display, _v->surface);
}

void visualizer_draw_estimate(int pos, estimator_output* eo) {
    int row = (pos / VISUALIZER_COLS);
    int col = (pos % VISUALIZER_COLS);

    glViewport((GLsizei)_v->screen_width/VISUALIZER_COLS * col,
        (GLsizei)_v->screen_height/VISUALIZER_ROWS * (VISUALIZER_ROWS - row - 1),
        (GLsizei)_v->screen_width/VISUALIZER_COLS,
        (GLsizei)_v->screen_height/VISUALIZER_ROWS);

    glLoadIdentity();
    glTranslatef(0, 0, -40.0f);
    glRotatef(eo->pitch * (180.0f / M_PI), 1.0f, 0.f, 0.0f);
    glRotatef(eo->yaw * (180.0f / M_PI), 0.0f, 1.0f, 0.0f);
    glRotatef(-eo->roll * (180.0f / M_PI), 0.0f, 0.0f, 1.0f);

    for(int face = 0; face < 6; face++) {
        glColor4f(color_ptr[face * 3 + 0], color_ptr[face * 3 + 1],
            color_ptr[face * 3 + 2], 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, face * 4, 4);
    }
}