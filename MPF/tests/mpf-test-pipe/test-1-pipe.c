/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/**
 * @short_description: A simple example showing how to use named pipes to communicate
 * with subprocesses (which block until the other end of the pipe is open).
 */
#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char** argv) {
    printf("test-pipe\n");
    char* pipew = "pipew";
    char* piper = "piper";
    mkfifo(pipew, S_IRWXU);
    mkfifo(piper, S_IRWXU);
    system("echo piper > piper &");
    FILE* p1 = fopen(piper, "r");

    char buf[100];
    int size = fread(buf, 1, 5, p1);
    buf[size] = 0;
    printf("p1 read: %s\n", buf);

    system("cat -v pipew &");
    FILE* p2 = fopen(pipew, "w");
    fwrite("pipew\n", 1, 6, p2);
    return 0;
}
