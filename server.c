//
// Created by Ugo Cottin on 21/11/2020.
//

#include <arpa/inet.h>
#include <time.h>
#include <netdb.h>
#include <libc.h>
#include <errno.h>
#include "server.h"

char* getTimeStamp(time_t *time) {
    char *timestamp = malloc(22);
    struct tm tm = *localtime(time);
    snprintf(timestamp, 22, "[%04d/%02d/%02d %02d:%02d:%02d]", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    return timestamp;
}

void trace(char* message) {
    time_t t = time(NULL);
    char* timestamp = getTimeStamp(&t);
    printf("%s %s\n", timestamp, message);
    free(timestamp);
}

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
            if (snarf.debug) {
                char message[1024];
                snprintf(message, sizeof message, "Unable to get hostname of %s", address);
                trace(message);
            }
            free(node);
        } else {
            return node;
        }
    }

    return address;
}

char* createFileName(time_t *time) {
    char *timestamp = malloc(20);
    struct tm tm = *localtime(time);
    snprintf(timestamp, 20, "%04d_%02d_%02d_%02d_%02d_%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    return timestamp;
}

void trace2(char* message, char* out, ssize_t len) {
    time_t t = time(NULL);
    char* timestamp = getTimeStamp(&t);
    snprintf(out, len, "%s %s", timestamp, message);
    free(timestamp);
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
            char message[1024];
            snprintf(message, sizeof message, "accepting connection from %s", address);
            trace(message);
        }

        if (new_sock_fd < 0) {
            if (snarf.debug) {
                trace("Error while accepting new connection");
                perror("");
            }
            exit(EXIT_FAILURE);
        }

        if (snarf.timeout > 0) {
            struct timeval timeout;
            timeout.tv_sec = snarf.timeout;
            timeout.tv_usec = 0;

            if (setsockopt(new_sock_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
                if (snarf.debug) {
                    trace("Unable to set timeout on socket");
                    perror("");
                }
        }

        if (fork() == 0) {
            if (snarf.debug) {
                char message[1024];
                snprintf(message, sizeof message, "Create new worker with pid %d", getpid());
                trace(message);
            }

            char *buffer = malloc(snarf.maxsize);
            memset(buffer, '\0', snarf.maxsize);

            if (snarf.debug) {
                trace("Worker ready to read");
            }

            size = read(new_sock_fd, buffer, snarf.maxsize);

            if (snarf.debug) {
                char message[1024];
                snprintf(message, sizeof message, "Worker got read %zd bytes", size);
                trace(message);
            }

            if (snarf.debug) trace("Worker now process request");

            char log[2048];
            time_t t = time(NULL);
            snprintf(log, sizeof log, "[%s:%d -> :%d] %s", address, addr_in->sin_port, snarf.port, buffer);

            if (snarf.file) {
                if (snarf.debug) trace("Worker writing to log file");
                char content[snarf.maxsize + 30];
                trace2(log, content, sizeof content);
                fputs(content, snarf.file);
            } else {
                if (snarf.debug) trace("Worker printing to stdout");
                trace(log);
            }

            if (strlen(snarf.save_dir) > 0) {
                // Si on a précisé un répertoire de sauvegarde
                struct stat sb;
                char folder[2048];
                snprintf(folder, sizeof folder, "%s/%s", snarf.save_dir, address);

                int access = 1;
                // On regarde si on peut accéder au dossier de sauvegarde
                if (stat(folder, &sb) != 0 || !S_ISDIR(sb.st_mode)) {
                    // Si non, on tente de le créer
                    if (snarf.debug) trace("Worker unable to access save dir, trying to create it");
                    if (mkdir(folder, S_IRWXU | S_IRWXG) < 0 && errno != EEXIST) {
                        // Si mkdir renvoie une erreur différente de EEXIST
                        if (snarf.debug) trace("Worker unable to create save dir, nothing will be saved");
                        access = 0;
                    }
                }

                if (access) {
                    char absolute[4096];
                    char *filename = createFileName(&t);
                    snprintf(absolute, sizeof absolute, "%s/%s", folder, filename);
                    free(filename);
                    FILE *file = fopen(absolute, "w+");
                    if (file == NULL) {
                        trace("unable to open file");
                    } else {
                        trace("Worker write incoming data to file");
                        if(fputs(buffer, file) < 0) {
                            trace("unable to write to file");
                        }
                        fclose(file);
                    }
                }
            }

            if (snarf.debug) {
                if (snarf.debug) trace("Worker finish processing request, now closing the connection\n");
            }

            close(new_sock_fd);
            close(serv.socket.socket);
            exit(EXIT_SUCCESS);
        }
    }
}