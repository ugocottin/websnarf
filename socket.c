//
// Created by Ugo Cottin on 21/11/2020.
//

#include "socket.h"

s_socket createSocket(websnarf snarf, int proto) {

    int _sock;
    int _bind;
    struct sockaddr_in address;
    struct s_socket _socket;

    if (proto != SOCK_STREAM && proto != SOCK_DGRAM) {
        perror("Invalid protocol");
        exit(EXIT_FAILURE);
    }

    if (proto == SOCK_STREAM) {
        _sock = socket(AF_INET, proto, IPPROTO_TCP);
    } else {
        _sock = socket(AF_INET, proto, IPPROTO_UDP);
    }

    if (_sock < 0) {
        perror ("Error while opening socket");
    }

    int optval = 1;
    setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    address.sin_family = AF_INET;
    address.sin_port = htons(snarf.port);
    address.sin_addr.s_addr = INADDR_ANY;

    if (snarf.timeout > 0) {
        struct timeval timeout;
        timeout.tv_sec = snarf.timeout;
        timeout.tv_usec = 0;

        if (setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
            perror("Unable to set timeout on socket\n");
    }

    _bind = bind(_sock, (struct sockaddr *) &address, sizeof address);
    if (_bind < 0) {
        perror("Error while binding to port");
        exit(EXIT_FAILURE);
    }

    _socket.socket = _sock;
    _socket.proto = proto;
    _socket.port = snarf.port;

    return _socket;
}
