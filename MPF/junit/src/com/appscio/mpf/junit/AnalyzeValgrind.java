/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 ******************************************************************************/
package com.appscio.mpf.junit;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import junit.framework.AssertionFailedError;
import freemarker.template.Configuration;
import freemarker.template.DefaultObjectWrapper;
import freemarker.template.Template;

public class AnalyzeValgrind implements Analyzer {

    private static final String NONE = "-";
	protected Map<String, Map<?,?>> model = new HashMap<String, Map<?,?>>();
    protected Map<String, String> stats = new HashMap<String, String>();
    protected List<String> tests = new ArrayList<String>();
	protected List<Map<String, Object>> details = new ArrayList<Map<String, Object>>();

    protected int lostBytes = 0, testCount = 0, possiblyLostBytes = 0;
    protected int totalErrors = 0, totalWarnings = 0;

    public String getName() {
    	return "valgrind";
    }

    public static void main(String args[]) throws Exception {
    	new AnalyzeValgrind().report(new File("junit_results"));
    }

    public void report(File dir) throws Exception {
    	File root = new File("junit_results");
    	List<File> files = new ArrayList<File>();
    	Set<String> set = new HashSet<String>();
    	findResults(root, files);
    	// Redo analysis on existing results data.  First create a list of test names.
    	for (File f: files) {
    		if (f.isDirectory())
    			continue;
    		String name = f.getPath();
    		if (!name.endsWith(".err") && !name.endsWith(".out"))
    			continue;
    		int index = name.lastIndexOf(".");
    		set.add(name.substring(0, index));
    	}

    	List<String> names = Arrays.asList(set.toArray(new String[0]));
    	// Now sort the names, and then re-analyze.
        Collections.sort(names, new Comparator<String>() {
            @Override
            public int compare(String n1, String n2) {
            	File f1 = new File(n1);
            	File f2 = new File(n2);
            	if (f1.getParentFile().equals(f2.getParentFile())) {
                    String s1[] = f1.getName().split("-");
                    String s2[] = f2.getName().split("-");
                    try {
                        if (s1.length == s2.length) {
                            if (s1.length >= 2 && s2.length >= 2) {
                                return Integer.valueOf(s1[1]).compareTo(Integer.valueOf(s2[1]));
                            } else {
                                return n1.compareTo(n2);
                            }
                        }
                    } catch (Exception x) {
                    }
            	}
                return f1.compareTo(f2);
            }
        });
        // Iterate the tests, re-running the report.
        for (String test: names) {
        	try {
        		analyze(test, new File(test + ".out"), new File(test + ".err"));
        	} catch (AssertionFailedError ax) {
        		// Ignore.
        	}
        }
    	generateReport(new File(dir + "/html"));
    }

    public static void findResults(File dir, List<File> list) {
    	File[] files = dir.listFiles();
    	for (File f: files) {
    		if (f.isDirectory()) {
    			findResults(f, list);
    		} else {
    			list.add(f);
    		}
    	}
    }

    public void generateReport(File dir) throws Exception {
        File fhtml = new File(dir, getName() + "-results.html");
        fhtml.getParentFile().mkdirs();
        OutputStream out = new FileOutputStream(fhtml);

        stats.put("lostBytes", "" + lostBytes);
        stats.put("possiblyLostBytes", "" + possiblyLostBytes);
        stats.put("testCount", "" + testCount);
        stats.put("errors", "" + totalErrors);
        stats.put("warnings", "" + totalWarnings);

        // Scan for binary data in .out and .err files..
        String scan = "find " + dir.getParent() + " -type f -exec file {} ;";
        System.out.println("scan=" + scan);
        final Process p = Runtime.getRuntime().exec(scan);
        StringBuffer buf = new StringBuffer();
        Thread dout = PluginSuite.dump(p.getInputStream(), System.out, buf);
        Thread derr = PluginSuite.dump(p.getErrorStream(), null, null);
        dout.join();
        derr.join();


        // Get or create a template
        Configuration cfg = new Configuration();
        cfg.setClassForTemplateLoading(getClass(), "");
        cfg.setObjectWrapper(new DefaultObjectWrapper());
        Template template = cfg.getTemplate("valgrind.html.ftl");
        Map root = new HashMap();
        root.put("tests", tests);
        root.put("model", model);
        root.put("stats", stats);
        root.put("details", details);
        root.put("date", new Date());
        root.put("files", buf.toString().split("\n"));
        template.process(root, new OutputStreamWriter(out));
    }

    @Override
    public void analyze(String test, File out, File err) throws Exception {
    	testCount++;
    	Map<String, Object> map = new HashMap<String, Object>();
    	Map<String, Object> props = new HashMap<String, Object>();
    	props.put("errors", 0);
    	props.put("warnings", 0);
    	props.put("lostBytes", 0);
    	props.put("possiblyLostBytes", 0);
    	props.put("test", test);
    	details.add(props);
    	tests.add(test);
    	model.put(test, map);
        System.out.println("analyzing " + err);
        map.put("stdout", out.getPath());
        map.put("stderr", err.getPath());
    	map.put("lost", NONE);
    	map.put("possibly", NONE);
    	map.put("errors", NONE);
    	map.put("warnings", NONE);
        if (!err.exists()) {
        	return;
        }
        FileReader fr = new FileReader(err);
        BufferedReader br = new BufferedReader(fr);
        int warn = 0;
        String line = br.readLine();
        String errors = null;
        String lost = null;
        String possibly = null;
        String warnings = null;
        // TODO: use XML output from valgrind.
        int errs = 0;
        while (line != null) {
            if (line.contains("ERROR SUMMARY")) {
                String toks[] = line.split("\\s+");
                errs = Integer.parseInt(toks[3].replaceAll(",",""));
                if (errs > 0) {
                    errors = line;
                }
                props.put("errors", errs);
            } else if (line.contains("definitely lost:")) {
            	String toks[] = line.split("\\s+");
            	int lb = Integer.parseInt(toks[3].replaceAll(",",""));
            	lostBytes += lb;
            	props.put("lostBytes", lb);
                lost = line;
            } else if (line.contains("possibly lost:")) {
            	String toks[] = line.split("\\s+");
            	int pb =Integer.parseInt(toks[3].replaceAll(",",""));
            	possiblyLostBytes += pb;
                possibly = line;
                props.put("possiblyLostBytes", pb);
            } else if (line.contains("WARNING")) {
            	if (warnings == null)
            		warnings = "";
            	warnings += line + "\n";
            	warn++;
            }
            line = br.readLine();
        }
        totalErrors += errs;
    	totalWarnings += warn;
        if (lost != null) {
            System.out.println("valgrind: " + test + ": " + lost);
            map.put("lost", lost);
        } else {
        	map.put("lost", "0");
        }
        if (possibly != null) {
        	map.put("possibly", possibly);
        } else {
        	map.put("possibly", "0");
        }
        if (errors != null) {
            System.out.println("valgrind: " + test + ": " + errors);
            map.put("errors", errors);
        } else {
        	map.put("errors", "0");
        }
        if (warnings != null) {
        	System.out.println("valgrind: " + test + ": " + warnings);
        	map.put("warnings", warnings);
        } else {
        	map.put("warnings", "0");
        }
    	props.put("warnings", warn);
        if (errors != null) {
        	// Tell JUnit of our pain.
        	throw new AssertionFailedError(errors);
        }
    }
}

