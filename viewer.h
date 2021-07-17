
#pragma once

typedef const char error;

typedef void viewert;

error *viewer_init(viewert **pobj);
void viewer_draw_start(viewert *obj);
void viewer_draw_end(viewert *obj);
void viewer_draw_estimate(viewert *obj, int pos, estimator_output *eo);
