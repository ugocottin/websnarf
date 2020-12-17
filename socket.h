//
// Created by Ugo Cottin on 21/11/2020.
//

#ifndef WEBSNARF_SOCKET_H
#define WEBSNARF_SOCKET_H

#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

#include "websnarf.h"

typedef struct s_socket {
    int socket;
    int proto;
    int port;
} s_socket;

s_socket createSocket(websnarf snarf, int proto, int af);

#endif //WEBSNARF_SOCKET_H
