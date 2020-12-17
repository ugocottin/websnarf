//
// Created by Ugo Cottin on 21/11/2020.
//

#ifndef WEBSNARF_WEBSNARF_H
#define WEBSNARF_WEBSNARF_H

#include <stdio.h>

typedef struct s_websnarf {
    char* logfile;
    int port;
    int timeout;
    int maxsize;
    char* save_dir;
    int debug;
    int apache;
    int resolve;
    int v6;
    FILE *file;
} websnarf;

#endif //WEBSNARF_WEBSNARF_H
