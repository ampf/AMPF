/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <mpf/mpf-logging.h>
#include <mpf/mpf-iplimage.h>
#include <cv.h>

#include <aps-opencv-md.h>

#include <mpf/mpf-rdf.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-opencv-md"
#define COMPONENT_DESC "Motion detector for OpenCV IplImages"
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
	int frame;
	int loglevel;
	MpfLogger logger;
	void* md;
	int visualization;
};

struct component_private *private;

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */
component_class_init() {

	mpf_add_param_int("loglevel", "Log Level",
			"Logging level (0-3 = DEBUG .. ERROR)", MPF_LEVEL_DEBUG,
			MPF_LEVEL_ERROR, MPF_LEVEL_WARN);

	mpf_add_param_int("mpf-debug", "Mpf Debug", "Debug MPF component", 0, 1, 0);

	mpf_rdf_add_output("interest");

	mpf_iplimage_add_input("input", MPF_IPLIMAGE_FORMAT_ANY);
	mpf_iplimage_add_output("output", MPF_IPLIMAGE_FORMAT_ANY);

	mpf_add_param_float("alpha", "Alpha", "Moving average fraction of new image", 0, 1, 0.1);
	mpf_add_param_int("median", "Median", "Median filter width applied to difference", 1, 101, 15);
	mpf_add_param_int("picture-threshold", "Picture Threshold", "Detector gray-scale to binary threshold for frame differencing", 0, 255, 70);
	mpf_add_param_int("momentum-threshold", "Momentum Threshold", "Detector threshold for object motion", 0, INT_MAX, 10000);
	mpf_add_param_enum("draw-all", "Draw All", "Draw all regions where motion is detected", "false",
	  "false", "Do not draw",
	  "true", "Do draw", NULL);

	mpf_add_param_enum("visualization", "Visualization", "Show intermediate video visualizations", "final",
		"final", "Final image with markup",
		"gray-scale", "Gray Scale",
		"difference", "Difference",
		"moving-average", "Moving Average",
		NULL);

	/*
	MpfIplImageFormatList format = mpf_iplimage_formatlist(CV_8UC3);

	// Add an IplImage input
	mpf_iplimage_add_input("input", format);

	// And an output.
	mpf_iplimage_add_output("output", format);
	*/
}

/* Component instance initialization and parameter handling. */
component_setup() {
	private = &mpf_private;
	// Initialize logging.
	mpf_private.loglevel = mpf_param_get_int("loglevel");
	mpf_logger_init(&mpf_private.logger, mpf_private.loglevel,
			gst_element_get_name(GST_ELEMENT(component)));

	if (mpf_param_get_int("mpf-debug"))
        mpf_component_get_curcomponent()->flags = MPF_DEBUG;

	mpf_private.md = aps_ipl_md_new_object(NULL);
	aps_ipl_md_set_parameters(mpf_private.md, mpf_param_get_float("alpha"), mpf_param_get_int("picture-threshold"),
		mpf_param_get_int("momentum-threshold"), mpf_param_get_enum("draw-all"), mpf_param_get_int("median"));
	aps_ipl_md_init(mpf_private.md);

	mpf_private.visualization = mpf_param_get_enum("visualization");
}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {

	/* Get the input */
	IplImage *input = mpf_iplimage_pull("input");

	MPF_PRIVATE_DEBUG("input depth=%d, nChannels=%d\n", mpf_private.frame, input->depth, input->nChannels);

	aps_ipl_md_set_next_frame(mpf_private.md, input);

	// cvCvtColor(aps_ipl_md_get_gray_image(mpf_private.md), input, CV_GRAY2RGB);

	int visualization = mpf_private.visualization;


	// Override visualization?
	if (visualization == 1) {
	    aps_ipl_md_get_gray_image(mpf_private.md, input);
	} else if (visualization == 2) {
	    aps_ipl_md_get_difference(mpf_private.md, input);
	} else if (visualization == 3) {
	    aps_ipl_md_get_moving_average(mpf_private.md, input);
	}

	MPF_PRIVATE_INFO("frame=%d, objects=%d, momentum=%g\n", mpf_private.frame, aps_ipl_md_get_objects(mpf_private.md), aps_ipl_md_get_momentum(mpf_private.md));

	mpf_iplimage_push("output", input);

	GrdfGraph *graph = mpf_rdf_new();
	GrdfNode *node = grdf_node_anon_new(graph, "interest");
	grdf_stmt_new_nuu(graph, node, "dcterms:type","urn:rdf:appscio.com/ver_1.0/toi");
	grdf_stmt_new_nui(graph, node, "urn:rdf:appscio.com/ver_1.0/toi/currentInterestLevel", aps_ipl_md_get_objects(mpf_private.md));
	grdf_stmt_new_nui(graph, node, "urn:rdf:appscio.com/ver_1.0/md/objects", aps_ipl_md_get_objects(mpf_private.md));
	grdf_stmt_new_nuf(graph, node, "urn:rdf:appscio.com/ver_1.0/md/momentum", aps_ipl_md_get_momentum(mpf_private.md));

	mpf_rdf_push("interest", graph);
	mpf_private.frame++;

	return MPF_GOOD;
}

/* Component instance finalization. */
component_teardown() {
}

