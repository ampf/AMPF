#!/usr/bin/perl

use File::Basename;

$file = $ARGV[0];

$path = dirname $file;
$name = basename $file;
$old = "${path}/.${name}.old";

# Avoid double processing.

die $0, ": old file ${old} already exists, please remove it and make sure you are not double-processing a file\n" if -e $old;

$out = $ARGV[1]? $ARGV[1]: "${path}/${name}.new";

open (IN, "< ${file}");
open (OUT, "> ${out}");

$text = 0;
$first_header = 1;
$header = "";
while (<IN>) {
    if ($first_header && /\/\*/) {
        $in_header = 1;
    }
    if ($in_header) {
        $header .= $_;
        $appscio |= $_ =~ /copyright.*appscio/i || /appscio customer license/i;
    } else {
        if (!$text && /\S+/) {
            $text = 1;
            $first_header = 0;
            # If not an Appscio Copyright, preserve the header.
            if (!$appscio) {
                print OUT $header;
            }
            # Print single blank line to help with copyright wizard.
            print OUT "\n";
        }
        if ($text) { print OUT; }
    }
    if ($first_header && /\*\//) {
        $in_header = 0;
        $first_header = 0;
    }
}
if (@ARGV == 1) {
    rename($file, $old);
    rename($out, $file);
    print "FILE=${file}\n  OLD -> ${old}\n";
}