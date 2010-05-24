/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
void* aps_ipl_md_new_object(void* owner);

void aps_ipl_md_set_parameters(void* object, float alpha, int picture_threshold, int momentum_threshold, int draw_all, int median);

void* aps_ipl_md_init(void* object);

void aps_ipl_md_set_next_frame(void* object, IplImage* input);

void aps_ipl_md_free_object(void* object);

void aps_ipl_md_get_moving_average(void* object, IplImage* image);

void aps_ipl_md_get_difference(void* object, IplImage *image);

void aps_ipl_md_get_gray_image(void* object, IplImage* image);

float aps_ipl_md_get_momentum(void* object);

int aps_ipl_md_get_objects(void* object);
