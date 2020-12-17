//
// Created by Ugo Cottin on 21/11/2020.
//

#include <errno.h>
#include "main.h"

int main(int argc, char** argv) {

    websnarf snarf;
    snarf.logfile = NULL;
    snarf.port = 80;
    snarf.timeout = 0;
    snarf.maxsize = 4096;
    snarf.debug = 0;
    snarf.apache = 0;
    snarf.resolve = 0;

    for(int i = 1; i < argc; i++) {

        char* arg = argv[i];

        if (strcmp(arg, "--help") == 0) {
            printf("usage: %s [options]\n", argv[0]);
            printf("--timeout <n>   wait at most <n> seconds on a read (default 0 = no timeout)\n"
                   "--log FILE      append output to FILE (default stdout only)\n"
                   "--port <n>      listen on TCP port <n> (default 80)\n"
                   "--max <n>       save at most <n> chars of request (default 4096 bytes)\n"
                   "--save DIR      save all incoming data into DIR/ipaddress\n"
                   "--debug         turn on a bit of debugging\n"
                   "--apache        logs are in Apache style (non implemented)\n"
                   "--resolve       resolve ip hostname of incoming request\n"
                   "--version       show version info\n"
                   "\n"
                   "--help          show this listing");
        } else if (strcmp(arg, "--log") == 0) {
            snarf.logfile = argv[i + 1];
            snarf.file = fopen(snarf.logfile, "w+");

            if (snarf.file == NULL) {
                printf("Error: cannot create logfile %s: %s\n", snarf.logfile, strerror(errno));
                exit(EXIT_FAILURE);
            }

            i++;
        } else if (strcmp(arg, "--port") == 0) {
            snarf.port = (int) strtol(argv[i + 1], NULL, 10);
            i++;
        } else if (strcmp(arg, "--timeout") == 0) {
            snarf.timeout = (int) strtol(argv[i + 1], NULL, 10);
            i++;
        } else if (strcmp(arg, "--max") == 0) {
            snarf.maxsize = (int) strtol(argv[i + 1], NULL, 10);
            i++;
        } else if (strcmp(arg, "--save") == 0) {
            snarf.save_dir = argv[i + 1];
            i++;
        } else if (strcmp(arg, "--debug") == 0) {
            snarf.debug = 1;
        } else if (strcmp(arg, "--apache") == 0) {
            snarf.apache = 1;
        } else if (strcmp(arg, "--resolve") == 0) {
            snarf.resolve = 1;
        }else if (strcmp(arg, "--version") == 0) {
            printf("Websnarf version %s\n", VERSION);
            exit(EXIT_SUCCESS);
        } else {
            printf("%s in an invalid option\n", arg);
            exit(EXIT_FAILURE);
        }
    }

    server _server;
    _server.socket = createSocket(snarf, SOCK_STREAM);
    listen(_server.socket.socket, 5);

    run(snarf, _server);

    return EXIT_SUCCESS;
}
