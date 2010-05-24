#!/bin/sh

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:`pwd`/../mpfcore

pkg-config --exists --print-errors "mpfcore"
