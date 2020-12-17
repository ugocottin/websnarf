//
// Created by Ugo Cottin on 21/11/2020.
//

#include "socket.h"

s_socket createSocket(websnarf snarf, int proto, int af) {

    int _sock;
    int _bind;
    struct sockaddr_in address;
    struct sockaddr_in6 address6;
    struct s_socket _socket;

    if (proto != SOCK_STREAM && proto != SOCK_DGRAM) {
        perror("Invalid protocol");
        exit(EXIT_FAILURE);
    }

    if (af != AF_INET && af != AF_INET6) {
        perror("Invalid address family");
        exit(EXIT_FAILURE);
    }

    if (proto == SOCK_STREAM) {
        _sock = socket(af, proto, IPPROTO_TCP);
    } else {
        _sock = socket(af, proto, IPPROTO_UDP);
    }

    if (_sock < 0) {
        perror ("Error while opening socket");
    }

    int optval = 1;
    setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    address.sin_family = af;
    address.sin_port = htons(snarf.port);
    address.sin_addr.s_addr = INADDR_ANY;

    address6.sin6_family = AF_INET6;
    address6.sin6_port = htons(snarf.port);
    address6.sin6_addr = in6addr_any;

    if (snarf.timeout > 0) {
        struct timeval timeout;
        timeout.tv_sec = snarf.timeout;
        timeout.tv_usec = 0;

        if (setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
            perror("Unable to set timeout on socket\n");
    }

    if (af == AF_INET) {
        _bind = bind(_sock, (struct sockaddr *) &address, sizeof address);
    } else {
        _bind = bind(_sock, (struct sockaddr *) &address6, sizeof address6);
    }

    if (_bind < 0) {
        perror("Error while binding to port");
        exit(EXIT_FAILURE);
    }

    _socket.socket = _sock;
    _socket.proto = proto;
    _socket.port = snarf.port;

    return _socket;
}
