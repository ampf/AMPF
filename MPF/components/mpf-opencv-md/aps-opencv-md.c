/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
// MotionDetection.cpp : Defines the entry point for the console application.
//


// Contourold.cpp : Defines the entry point for the console application.
//
// #include "stdafx.h"

#include "stdlib.h"
#include "stdio.h"

// OpenCV includes.
#include "cv.h"
#include "highgui.h"
#pragma comment(lib,"cv.lib")
#pragma comment(lib,"cxcore.lib")
#pragma comment(lib,"highgui.lib")

typedef struct _State {
	IplImage *movingAverage, *grayImage, *motionHistory;
	IplImage *difference, *temp, *temp2;
	int first;
	float alpha;
	int median;
	int picture_threshold;
	int momentum_threshold;
	int objects;
	float momentum;
	CvSeq* saved;
	int draw_all;
} State;


void* aps_ipl_md_new_object(void* owner) {
	State *state = calloc(sizeof(State), 1);
	state->first = 1;
	state->alpha = 0.1;
	state->momentum_threshold = 20000;
}

void aps_ipl_md_set_parameters(void* object, float alpha, int picture_threshold, int momentum_threshold, int draw_all, int median) {
	State *state = (State*)object;
	state->alpha = alpha;
	state->picture_threshold = picture_threshold;
	state->momentum_threshold = momentum_threshold;
	state->draw_all = draw_all;
	state->median = median;
}

void* aps_ipl_md_init(void* object) {
	State *state = (State*)object;
}

void* clone(void* source, size_t size) {
	void* result = calloc(size, 1);
	memcpy(result, source, size);
	return clone;
}

#define CLONE(TYPE, object) (TYPE*)clone(object, sizeof(TYPE))

CvRect* find_closest_rect(CvSeq *prev, CvRect *crect) {
	if (!prev)
		return NULL;
	CvRect *closest = NULL;
	int best = 0;
	int x2 = crect->x;
	int y2 = crect->y;
	int w2 = crect->width;
	int h2 = crect->height;
	int i;
	for (i=0; i<prev->total; i++) {
		CvRect *prect = CV_GET_SEQ_ELEM(CvRect, prev, i);
		//
		// Enforce overlap, (x2 < x1+w1 && x2+w2 > x1) && (y2 < y1+h1 && y2+h2 > y1)
		//   x1
		//   +----w1---+
 		// y1|      x2 |
		//   h1   y2+----w2-+
		//   +------|--+    h2
		//          +-------+
		//
		int x1 = prect->x;
		int y1 = prect->y;
		int w1 = prect->width;
		int h1 = prect->height;
		if (x2 < x1+w1 && x2+w2 > x1 && y2 < y1+h1 && y2+h2 > y1) {
			int dist = abs(x2-x1) + abs(y2-y1) + abs(w2-w1) + abs(h2-h1);
			if ((best == 0 && dist < 20) || dist < best) {
				best = dist;
				closest = prect;
			}
		}
	}
	if (closest) {
		// printf("closest to: %d,%d,%d,%d is %d,%d,%d,%d\n", crect->x, crect->y, crect->width,crect->height, closest->x, closest->y, closest->width, closest->height);
	}
	return closest;
}

void aps_ipl_md_set_next_frame(void* object, IplImage* input) {

	State *state = (State*)object;
	//Size of the image.
	CvSize imgSize;
	imgSize.width = input->width;
	imgSize.height = input->height;

	//Points for the edges of the rectangle.
	CvPoint pt1, pt2;

	//Create a font object.
	CvFont font;

	//Capture the movie frame by frame.
	int numPeople = 0;

	//Buffer to save the number of people when converting the integer
	//to a string.
	char wow[65];

	if (0) {
		// Predator video HUD mask.
		CvPoint mask[] = {
			{110,190},{166,333},
			{570,350},{688,381},
			{608,383},{688,444},
			{570,79}, {688,126},
			{263,70}, {464,99},
			{40,147}, {100,172},
			{343,16}, {385,34},
		};
		int i = 0;
		for (i=0; i<sizeof(mask)/sizeof(CvPoint); i+=2) {
			cvRectangle(input, mask[i], mask[i+1], CV_RGB(255, 255, 0), CV_FILLED, 8, 0);
		}
	}

	//If this is the first time, initialize the images.
	if (state->first) {
		state->grayImage = cvCreateImage(imgSize, IPL_DEPTH_8U, 1);
		state->movingAverage = cvCreateImage(imgSize, IPL_DEPTH_32F, 3);
		state->motionHistory = cvCreateImage(imgSize, IPL_DEPTH_8U, 3);
		state->difference = cvCloneImage(input);
		state->temp = cvCloneImage(input);
		state->temp2 = cvCloneImage(input);
		cvConvertScale(input, state->movingAverage, 1.0, 0.0);
		state->first = 0;
	} else {
		//else, make a running average of the motion.
		cvRunningAvg(input, state->movingAverage, state->alpha, NULL);
	}

	//Convert the scale of the moving average.
	cvConvertScale(state->movingAverage, state->temp, 1.0, 0.0);

	//Minus the current frame from the moving average.
	cvAbsDiff(input, state->temp, state->temp2);

	cvSmooth(state->temp2, state->difference, CV_MEDIAN, state->median, state->median, 0, 0);

	//Convert the image to grayscale.
	cvCvtColor(state->difference, state->grayImage, CV_RGB2GRAY);

	//Convert the image to black and white.
	cvThreshold(state->grayImage, state->grayImage, state->picture_threshold, 255, CV_THRESH_BINARY);

	//Dilate and erode to get people blobs
	cvDilate(state->grayImage, state->grayImage, 0, 18);
	cvErode(state->grayImage, state->grayImage, 0, 10);

	//Find the contours of the moving images in the frame.
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;
	cvFindContours(state->grayImage, storage, &contour, sizeof(CvContour),
			CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

	state->objects = 0;
	state->momentum = 0;

	// TODO: leaks.
	CvSeq *saved = cvCreateSeq(CV_SEQ_ELTYPE_GENERIC, sizeof(CvSeq), sizeof(CvRect), cvCreateMemStorage(0));

	//Process each moving contour in the current frame...
	for (; contour != 0; contour = contour->h_next) {
		//Get a bounding rectangle around the moving object.
		CvRect bndRect = cvBoundingRect(contour, 0);

		// Have to clone to put in sequence, and no simpler way to do it?
		cvSeqPush(saved, &bndRect);

		// Find previous closest contour. TODO: save rectangles, more efficient.
		CvRect *prev = find_closest_rect(state->saved, &bndRect);

		int velocity = 0;
		int momentum = 0;
		if (prev) {
			int x1 = bndRect.x, y1 = bndRect.y, w1 = bndRect.width, h1 = bndRect.height;
			int x2 = prev->x, y2 = prev->y, w2 = prev->width, h2 = prev->height;
			int x1avg = x1+w1/2;
			int x2avg = x2+w2/2;
			int y1avg = y1+h1/2;
			int y2avg = y2+h2/2;
			int dx = x2avg-x1avg;
			int dy = y2avg-y1avg;

			// Compute velocity:
			velocity = sqrt((double)(dx*dx+dy*dy));
			// And momentum.
			momentum = velocity*w1*h1;
			// printf("objects=%d: velocity=%d, momentum=%d\n", state->objects, velocity, momentum);
		}

		pt1.x = bndRect.x;
		pt1.y = bndRect.y;
		pt2.x = bndRect.x + bndRect.width;
		pt2.y = bndRect.y + bndRect.height;
		CvPoint c;
		c.x = bndRect.x + bndRect.width/2;
		c.y = bndRect.y + bndRect.height/2;

		state->momentum += momentum;
		if (state->draw_all || momentum > state->momentum_threshold) {
			//Draw the bounding rectangle around the moving object.
			cvRectangle(input, pt1, pt2, CV_RGB(255, 255, 0), 2, 8, 0);
			cvCircle(input, c, 4, CV_RGB(255, 255, 0), -1, CV_AA, 0);
			if (momentum > state->momentum_threshold) {
				state->objects++;
			}
		}

	}
	// Save contour for next cycle.  TODO: hold onto objects for a while.
	state->saved = saved;

	// Write the number of objects  counted at the top of the output frame.
	double ratio = (double)640/input->width;
	double font_scale = 0.8/ratio;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, font_scale, font_scale, 0, 2/ratio, 8);
	snprintf(wow, 40, "objects=%i momentum=%d", state->objects, (int)state->momentum);
	cvPutText(input, wow, cvPoint(20/ratio, 40/ratio),
			&font, CV_RGB(255, 255, 0));

}

void aps_ipl_md_free_object(void* object) {

	State *state = (State*)object;

	// Destroy the image, movies, and window.
	cvReleaseImage(&state->temp);
	cvReleaseImage(&state->difference);
	cvReleaseImage(&state->grayImage);
	cvReleaseImage(&state->movingAverage);

}

void aps_ipl_md_get_moving_average(void* object, IplImage* image) {
	State *state = (State*)object;
	cvConvertScale(state->movingAverage, image, 1.0, 0.0);
}

void aps_ipl_md_get_difference(void* object, IplImage* image) {
	State *state = (State*)object;
	cvConvertImage(state->difference, image, 0);
}

void aps_ipl_md_get_gray_image(void* object, IplImage* image) {
	State *state = (State*)object;
	cvConvertImage(state->grayImage, image, 0);
}

int aps_ipl_md_get_objects(void* object) {
	State *state = (State*)object;
	return state->objects;
}

float aps_ipl_md_get_momentum(void* object) {
	State *state = (State*)object;
	return state->momentum;
}
