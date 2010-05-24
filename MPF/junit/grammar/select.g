// Grammar for processing test-filter selection expressions such as:
//
//   select="(resource.output=events.rdf | resource.output=tracks.rdf) & resource.ximagesink=true"
//                (res=res|res=res)&res=res

meta-data = "transparent=";

$expr: $or ('|' $or)*;
$or: $and ('&' $and)*;
$and: $not? ($eq | '(' $expr ')');
$not: '!';
$eq: $res '=' $res;
$res: ['a'-'z' '.' '-' '_' '0'-'9']+;

expression="res1=val1&res2=val2";

skip = " \t\n\r\f";
backtrack = true;

logparse = false;
dumpgrammar = true;
dumpparsertree = true;

action {
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
    }
}