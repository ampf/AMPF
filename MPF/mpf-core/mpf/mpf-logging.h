/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
// TODO: Appscio Header

// A simple logging system based on Log4j.

#if !defined(__MPF_LOGGING_H)
#define __MPF_LOGGING_H

#include <stdarg.h>
#include <stdio.h>

typedef int boolean;
typedef enum _MPF_LOG_LEVEL {MPF_LEVEL_DEBUG, MPF_LEVEL_INFO, MPF_LEVEL_WARN, MPF_LEVEL_ERROR, MPF_LEVEL_ALWAYS} MpfLogLevel;

#define MPF_LOG_DEBUG(logger, args...) (logger)->append(logger, MPF_LEVEL_DEBUG, args)
#define MPF_LOG_INFO(logger, args...)  (logger)->append(logger, MPF_LEVEL_INFO, args)
#define MPF_LOG_WARN(logger, args...)  (logger)->append(logger, MPF_LEVEL_WARN, args)
#define MPF_LOG_ERROR(logger, args...) (logger)->append(logger, MPF_LEVEL_ERROR, args)
#define MPF_LOG_ALWAYS(logger, args...) (logger)->append(logger, MPF_LEVEL_ALWAYS, args)
#define MPF_LOG_ENABLED(logger, LEVEL) ((logger)->level <= LEVEL)

// The following macros simplify logging from an MPF component.  Requires mpf_private to contain a logger.
#define MPF_PRIVATE_DEBUG(args...) MPF_LOG_DEBUG(&mpf_private.logger, args)
#define MPF_PRIVATE_INFO(args...)  MPF_LOG_INFO(&mpf_private.logger, args)
#define MPF_PRIVATE_WARN(args...)  MPF_LOG_WARN(&mpf_private.logger, args)
#define MPF_PRIVATE_ERROR(args...) MPF_LOG_ERROR(&mpf_private.logger, args)
#define MPF_PRIVATE_ALWAYS(args...) MPF_LOG_ALWAYS(&mpf_private.logger, args)
#define MPF_PRIVATE_ENABLED(LEVEL) MPF_LOG_ENABLED(&mpf_private.logger, LEVEL)

// Forward Declaration.
struct _MpfLogger;

// Analogous to an Appender.  Use like printf, but pass in a logger object and level.
typedef void (*MpfAppender)(const struct _MpfLogger* logger, MpfLogLevel level, char* fmt, ...);

// Analogous to a Logger.  Contains log level, name, and a parent pointer.
typedef struct _MpfLogger {
    MpfLogLevel level;
    char* name;
    struct _MpfLogger *parent;
    MpfAppender append;
} MpfLogger;


void mpf_logger_init(MpfLogger* logger, MpfLogLevel level, char* name);

void mpf_logger_stdout_appender(const MpfLogger* logger, MpfLogLevel level, char* fmt, ...);

void mpf_logger_null_appender(const MpfLogger* logger, MpfLogLevel level, char* fmt, ...);

#endif
