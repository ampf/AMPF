/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 ******************************************************************************/
package com.appscio.mpf.junit;

import java.util.HashMap;
import java.util.Map;
import java.util.Stack;

import com.simontuffs.yaccl.RDP;
import com.simontuffs.yaccl.RDP.Fire;
import com.simontuffs.yaccl.RDP.Node;

// Evaluates the node tree parsed out of an eval expession against a set of variable values.
public class SelectEvaluator {

    protected SelectGrammar grammar = new SelectGrammar();
    protected RDP rdp;
    {
        rdp = grammar.parser();
    }

    public static void main(String args[]) throws Exception {
        // (resource.input=predator-1.mpg | resource.output=events.rdf) & resource.timeout=10 | foo=bar
        Map<String, String> vars = new HashMap<String, String>();
        vars.put("resource.input", "predator-1.mpg");
        vars.put("resource.output", "events.rdf");
        vars.put("resource.timeout", "10");
        vars.put("foo", "bar1");
        String eval = "(resource.input=predator-1.mpg & resource.output=events.rdf)";
        SelectEvaluator se = new SelectEvaluator();
        try {
            se.parse(eval);
            boolean result = se.evaluate(vars);
            System.out.println("eval=" + eval);
            System.out.println("vars=" + vars);
            System.out.println("result=" + result);
        } catch (Exception x) {
            System.out.println(x);
        }
    }

    public void parse(String eval) {
        eval = eval.trim();
        grammar.parse(rdp, eval, null, null);
        // rdp.root.dump();
    }

    public boolean evaluate(final Map<String, String> vars) throws RuntimeException {
        final Stack<Boolean> stack = new Stack<Boolean>();
        rdp.root.traverse(new Fire() {
            @Override
            public void fire(Node node) {
                // System.out.print(node.term.name + " ");
                if (node.term == null || node.term.name == null)
                    return;
                if (node.term.name.equals("$eq")) {
                    // System.out.print(node.flatten());
                    String left = node.childAt(0).flatten();
                    String right = node.childAt(2).flatten();
                    String val = (String)vars.get(left);
                    if (val == null) {
                        // Treat as a false match.
                        stack.push(false);
                    } else {
                        stack.push(val.equals(right));
                    }
                } else if (node.term.name.equals("$expr")) {
                    boolean result = false;
                    int n = (node.children.size()+1)/2;
                    for (int i=0; i<n; i++) {
                        result |= stack.pop();
                    }
                    stack.push(result);
                } else if (node.term.name.equals("$or")) {
                    boolean result = true;
                    int n = (node.children.size()+1)/2;
                    for (int i=0; i<n; i++) {
                        result &= stack.pop();
                    }
                    stack.push(result);
                } else if (node.term.name.equals("$and")) {
                    if (node.stringAt(0).equals("!")) {
                        stack.push(!stack.pop());
                    }
                } else if (node.term.name.equals("root")) {
                    // System.out.print(" result ");
                }
                // System.out.println(" -> " + stack);
            }
        }, false);
        return stack.isEmpty() || stack.peek();
    }
}
