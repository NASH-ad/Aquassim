// Main header file for the AquaSim project

#ifndef AQUASSIM_H
#define AQUASSIM_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>

    #define LOG(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

#endif // AQUASSIM_H