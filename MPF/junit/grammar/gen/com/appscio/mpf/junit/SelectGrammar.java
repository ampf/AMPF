/*
 * YACCL parser generated from 'select.g' Tue Dec 22 12:47:01 PST 2009
 * YACCL is Copyright 2003, P. Simon Tuffs. All Rights Reserved.
 * Contact simon@simontuffs.com for more information about YACCL.
 * Redistributions of this source code must retain the above copyright notice.
 */

package com.appscio.mpf.junit;
import com.simontuffs.yaccl.RDP;
import com.simontuffs.yaccl.RDP.*;
public class SelectGrammar {
    public static void main(String args[]) throws Exception {
        java.io.FileReader reader = new java.io.FileReader(args[0]);
        SelectGrammar grammar = new SelectGrammar();
        RDP rdp = grammar.parser();
        String eval = rdp.load(reader).trim();
        grammar.parse(rdp, eval, null, null);
        System.out.println("Dumping parse-tree");
        rdp.root.dump();
    }
 
    
    // Dumping 'select.g' productions...
    // $expr: $or ('|' $or)*;
    // $or: $and ('&' $and)*;
    // $and: ($not)? ($eq | '(' $expr ')');
    // $not: '!';
    // $eq: $res '=' $res;
    // $res: ['a'-'z' '.' '-' '_' '0'-'9']+;
    
    // Dumping 'select.g' controls...
    // skip = " \t\n\r\f";
    // backtrack = true;
    
    
    public RDP parser() {
        RDP parser = new RDP();
        // Terminals.
        Terminal $$expr = parser.new NonTerminal("$expr");
        Terminal $$or = parser.new NonTerminal("$or");
        Terminal $$and = parser.new NonTerminal("$and");
        Terminal $$not = parser.new NonTerminal("$not");
        Terminal $$eq = parser.new NonTerminal("$eq");
        Terminal $$res = parser.new NonTerminal("$res");
    
        // Productions.
        $$expr.match($$or.and(parser.terminal("|").and($$or).star()));
        $$or.match($$and.and(parser.terminal("&").and($$and).star()));
        $$and.match($$not.query().and($$eq.or(parser.terminal("(").and($$expr)
            .and(")"))));
        $$not.match(parser.terminal("!"));
        $$eq.match($$res.and("=").and($$res));
        $$res.match(parser.new Chars().include('a','z').include('.').include('-')
            .include('_').include('0','9').plus());
    
        // Controls.
        parser.skip(" \t\n\r\f");
        parser.useBacktrack = true;
    
        // Transparent tokens.
    
        return parser;
    }
    
    public boolean parse(RDP parser, String expression, String production, Object actions) {
        // Parse.
        parser.actions = actions;
        if (production == null) production = (String)parser.productionNames.get(0);
        Terminal term = (Terminal)parser.productionMap.get(production);
        parser.expression(expression);
        return parser.parse(term);
    }
}
