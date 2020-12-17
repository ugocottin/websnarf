//
// Created by Ugo Cottin on 21/11/2020.
//

#include <arpa/inet.h>
#include <time.h>
#include <netdb.h>
#include "server.h"

char *getAddress(struct sockaddr client_address) {

    switch (client_address.sa_family) {
        case AF_INET: {
            char *buf = malloc(INET_ADDRSTRLEN);
            struct sockaddr_in *addr_in = (struct sockaddr_in *) &client_address;
            if (inet_ntop(AF_INET, &addr_in->sin_addr, buf, INET_ADDRSTRLEN) == NULL)
                perror("inet_ntop");
            return buf;
        }

        case AF_INET6: {
            char *buf6 = malloc(INET6_ADDRSTRLEN);
            struct sockaddr_in6 *addr_in = (struct sockaddr_in6 *) &client_address;
            if (inet_ntop(AF_INET6, &addr_in->sin6_addr, buf6, INET6_ADDRSTRLEN) == NULL)
                perror("inet_ntop");
            return buf6;
        }
    }

    return "unknown";
}

char* resolve(struct sockaddr client_address, websnarf snarf) {

    char* address = getAddress(client_address);
    if (snarf.resolve) {
        char *node = malloc(NI_MAXHOST);
        if (getnameinfo(&client_address, sizeof(client_address), node, NI_MAXHOST, NULL, 0, NI_NAMEREQD)) {
            if (snarf.debug) printf("Unable to get hostname of %s\n", address);
            free(node);
        } else {
            return node;
        }
    }

    return address;
}

void run(websnarf snarf, server serv) {

    int new_sock_fd;
    struct sockaddr client_address;
    socklen_t client_address_len = sizeof(client_address);

    ssize_t size;

    printf("# Now listening on port %d\n", snarf.port);

    while (1) {
        new_sock_fd = accept(serv.socket.socket, &client_address, &client_address_len);
        struct sockaddr_in *addr_in = (struct sockaddr_in *) &client_address;
        char *address = resolve(client_address, snarf);

        if (snarf.debug) {
            printf("--> accepted connection from %s\n", address);
        }

        if (new_sock_fd < 0) {
            perror("Error while accepting connection");
            exit(EXIT_FAILURE);
        }

        if (snarf.timeout > 0) {
            struct timeval timeout;
            timeout.tv_sec = snarf.timeout;
            timeout.tv_usec = 0;

            if (setsockopt(new_sock_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
                perror("Unable to set timeout on socket\n");
        }

        if (fork() == 0) {

            char *buffer = malloc(snarf.maxsize);
            memset(buffer, '\0', snarf.maxsize);

            if (snarf.debug) {
                printf("    client ready to read, now reading\n");
            }

            size = read(new_sock_fd, buffer, snarf.maxsize);

            if (size < 0) {
                printf("Error while receiving data\n");
                close(new_sock_fd);
                exit(EXIT_FAILURE);
            }

            if (snarf.debug) {
                printf("    got read %zd bytes\n", size);
            }

            char log[2048];
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            snprintf(log, sizeof log, "[%d/%d/%d %d:%d:%d] [%s:%d -> :%d] %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, address, addr_in->sin_port, snarf.port, buffer);

            if (snarf.file) {
                fputs(log, snarf.file);
            }

            if (!snarf.debug && !snarf.file) {
                printf("%s", log);
            }

            if (snarf.save_dir) {

            }

            // Process des données ici

            if (snarf.debug) {
                printf("    finish processing request, now closing the connection\n");
            }
            close(new_sock_fd);
            close(serv.socket.socket);
            exit(EXIT_SUCCESS);
        }
    }
}