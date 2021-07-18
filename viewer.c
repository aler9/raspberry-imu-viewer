
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <bcm_host.h>
#include <math.h>
#include <stdlib.h>

#include "sensor-imu/orientation/est.h"
#include "viewer.h"

#define ROWS 2
#define COLS 3

static const GLbyte vertex_ptr[] = {
    // face 1
    -5,
    -2,
    10,
    5,
    -2,
    10,
    -5,
    2,
    10,
    5,
    2,
    10,
    // face 2
    -5,
    -2,
    -10,
    -5,
    2,
    -10,
    5,
    -2,
    -10,
    5,
    2,
    -10,
    // face 3
    -5,
    -2,
    10,
    -5,
    2,
    10,
    -5,
    -2,
    -10,
    -5,
    2,
    -10,
    // face 4
    5,
    -2,
    -10,
    5,
    2,
    -10,
    5,
    -2,
    10,
    5,
    2,
    10,
    // face 5
    -5,
    2,
    10,
    5,
    2,
    10,
    -5,
    2,
    -10,
    5,
    2,
    -10,
    // face 6
    -5,
    -2,
    10,
    -5,
    -2,
    -10,
    5,
    -2,
    10,
    5,
    -2,
    -10,
};

static const GLfloat color_ptr[] = {
    1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
};

typedef struct {
    EGLDisplay display;
    EGLSurface surface;
    EGL_DISPMANX_WINDOW_T native_window;
    uint32_t screen_width;
    uint32_t screen_height;
} _objt;

error *viewer_init(viewert **pobj) {
    _objt *_obj = malloc(sizeof(_objt));

    bcm_host_init();

    _obj->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (_obj->display == EGL_NO_DISPLAY) {
        free(_obj);
        return "eglGetDisplay() failed";
    }

    EGLBoolean glres = eglInitialize(_obj->display, NULL, NULL);
    if (glres == EGL_FALSE) {
        free(_obj);
        return "eglInitialize() failed";
    }

    EGLint egl_attributes[] = {
        EGL_RED_SIZE,     8,
        EGL_GREEN_SIZE,   8,
        EGL_BLUE_SIZE,    8,
        EGL_ALPHA_SIZE,   8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE,
    };

    EGLConfig config;
    EGLint num_config;
    glres =
        eglChooseConfig(_obj->display, egl_attributes, &config, 1, &num_config);
    if (glres == EGL_FALSE) {
        free(_obj);
        return "eglChooseConfig() failed";
    }

    EGLContext context =
        eglCreateContext(_obj->display, config, EGL_NO_CONTEXT, NULL);
    if (context == EGL_NO_CONTEXT) {
        free(_obj);
        return "eglCreateContext() failed";
    }

    int32_t gres =
        graphics_get_display_size(0, &_obj->screen_width, &_obj->screen_height);
    if (gres < 0) {
        free(_obj);
        return "graphics_get_display_size() failed";
    }

    VC_RECT_T dst_rect = {
        .x = 0,
        .y = 0,
        .width = _obj->screen_width,
        .height = _obj->screen_height,
    };

    VC_RECT_T src_rect = {
        .x = 0,
        .y = 0,
        .width = _obj->screen_width << 16,
        .height = _obj->screen_height << 16,
    };

    DISPMANX_DISPLAY_HANDLE_T dmx_display = vc_dispmanx_display_open(0);
    DISPMANX_UPDATE_HANDLE_T dmx_update = vc_dispmanx_update_start(0);
    DISPMANX_ELEMENT_HANDLE_T dmx_element =
        vc_dispmanx_element_add(dmx_update, dmx_display, 0, &dst_rect, 0,
                                &src_rect, DISPMANX_PROTECTION_NONE, 0, 0, 0);
    vc_dispmanx_update_submit_sync(dmx_update);

    _obj->native_window.element = dmx_element;
    _obj->native_window.width = _obj->screen_width;
    _obj->native_window.height = _obj->screen_height;

    _obj->surface = eglCreateWindowSurface(_obj->display, config,
                                           &_obj->native_window, NULL);
    if (_obj->surface == EGL_NO_SURFACE) {
        free(_obj);
        return "eglCreateWindowSurface() failed";
    }

    glres =
        eglMakeCurrent(_obj->display, _obj->surface, _obj->surface, context);
    if (glres == EGL_FALSE) {
        free(_obj);
        return "eglMakeCurrent() failed";
    }

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
    float hwd = hht * (float)_obj->screen_width / (float)_obj->screen_height;
    glFrustumf(-hwd, hwd, -hht, hht, nearp, farp);

    glMatrixMode(GL_MODELVIEW);

    *pobj = _obj;
    return NULL;
}

void viewer_draw_start(__attribute__((unused)) viewert *obj) {
    glClear(GL_COLOR_BUFFER_BIT);
}

void viewer_draw_end(viewert *obj) {
    _objt *_obj = (_objt *)obj;
    eglSwapBuffers(_obj->display, _obj->surface);
}

void viewer_draw_estimate(viewert *obj, int pos, estimator_output *eo) {
    _objt *_obj = (_objt *)obj;

    int row = (pos / COLS);
    int col = (pos % COLS);

    glViewport((GLsizei)_obj->screen_width / COLS * col,
               (GLsizei)_obj->screen_height / ROWS * (ROWS - row - 1),
               (GLsizei)_obj->screen_width / COLS,
               (GLsizei)_obj->screen_height / ROWS);

    glLoadIdentity();
    glTranslatef(0, 0, -40.0f);
    glRotatef(eo->pitch, 1.0f, 0.f, 0.0f);
    glRotatef(eo->yaw, 0.0f, 1.0f, 0.0f);
    glRotatef(-eo->roll, 0.0f, 0.0f, 1.0f);

    for (int face = 0; face < 6; face++) {
        glColor4f(color_ptr[face * 3 + 0], color_ptr[face * 3 + 1],
                  color_ptr[face * 3 + 2], 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, face * 4, 4);
    }
}
