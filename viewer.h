
#pragma once

typedef void visualizert;

error* visualizer_init(visualizert** pobj);
void visualizer_draw_start(visualizert* obj);
void visualizer_draw_end(visualizert* obj);
void visualizer_draw_estimate(visualizert* obj, int pos, estimator_output* eo);
