/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 ******************************************************************************/
import java.util.Arrays;
import java.io.*;
public class Test {
    public static void main(String args[]) throws Exception {
        System.out.println("usage: Test read write");
        System.out.println("Test " + Arrays.asList(args) + " param=" + System.getProperty("param"));
        FileInputStream fis = new FileInputStream(args[0]);
        FileOutputStream fos = new FileOutputStream(args[1]);
        int ch;
        while ((ch = fis.read()) >= 0) {
            System.out.print("+" + (char)ch);
            fos.write(Character.toUpperCase(ch));
        }
    }
}
