/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 ******************************************************************************/
package com.appscio.mpf.junit;

import java.io.File;
import java.io.OutputStream;

public interface Analyzer {

    public void analyze(String test, File output, File errors) throws Exception;

    public void generateReport(File dir) throws Exception;

    public String getName();
}
