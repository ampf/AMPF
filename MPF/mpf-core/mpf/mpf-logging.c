/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
// TODO: Appscio Header

// A simple logging appender implementation.  This can/should be made more sophisticated, allowing the kind of
// control over logging provided by log4j.  This is much better than simple fprintf's for now.

#include <mpf/mpf-logging.h>

void mpf_logger_init(MpfLogger* logger, MpfLogLevel level, char* name) {
    logger->level = level;
    logger->name = name;
    logger->parent = NULL;
    logger->append = mpf_logger_stdout_appender;
}

// Log to stdout, based on log level relative to this components "logging" parameter.
void mpf_logger_stdout_appender(const MpfLogger* logger, MpfLogLevel level, char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    if (level == MPF_LEVEL_ALWAYS || (level >= logger->level && (!logger->parent || level >= logger->parent->level))) {
        printf("%s::%s: ", logger->parent?logger->parent->name: "", logger->name);
        vprintf(fmt, ap);
    }
    va_end(ap);
}

// Eat all log information.
void mpf_logger_null_appender(const MpfLogger* logger, MpfLogLevel level, char* fmt, ...) {
    // NOOP.
}

