/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/**
 * @short_description: A more complex example showing how to use named pipes together
 * with a "shovel" process who's role is to shovel data from input to output.
 */

#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char** argv) {
    printf("test-pipe\n");
    char* pipew = "pipew";
    char* piper = "piper";
    mkfifo(pipew, S_IRWXU);
    mkfifo(piper, S_IRWXU);

    // Shovel
    char* process = "cat < pipew > piper &";
    printf("opening shovel process: %s\n", process);
    system(process);

    printf("opening pipew\n");
    FILE* pw = fopen(pipew, "w");

    printf("opening piper\n");
    FILE* pr = fopen(piper, "r");

    printf("writing pipew\n");
    char *send = "sending data";
    int len = fwrite(send, 1, strlen(send), pw);
    fflush(pw);
    printf("len=%d\n", len);

    char buf[100];
    printf("reading piper\n");
    len = fread(buf, 1, len, pr);
    buf[len] = 0;
    printf("pr read(%d): %s\n", len, buf);

    return 0;
}
