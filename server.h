//
// Created by Ugo Cottin on 21/11/2020.
//

#ifndef WEBSNARF_SERVER_H
#define WEBSNARF_SERVER_H

#include <unistd.h>
#include <string.h>
#include "socket.h"
#include "websnarf.h"

typedef struct s_server {
    s_socket socket;
} server;

void run(websnarf snarf, server serv);

#endif //WEBSNARF_SERVER_H
