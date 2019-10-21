#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sysexits.h>
#include <unistd.h>

#define USAGE \
    "Usage: %1$s [-s <signal>] -- command [args...]\n" \
    "       %1$s [-s <signal>] -p <pid>\n"

typedef struct options {
    pid_t pid;
    int signum;
} options_t;

static int parse_int(const char *str, int *result, int min, int max)
{
    char *endptr;
    long value;

    errno = 0;
    value = strtoul(str, &endptr, 10);
    if (errno)
        return -1;

    if (*endptr != '\0') {
        errno = EINVAL;
        return -1;
    }

    if (value < min || value > max) {
        errno = ERANGE;
        return -1;
    }

    *result = (pid_t)value;
    return 0;
}

static int parse_signal(const char *str, int *signum)
{
    if (isdigit(str[0]))
        return parse_int(str, signum, 0, INT_MAX);

    if (strncmp(str, "SIG", 3) == 0)
        str += 3;

    /* TODO - figure out if there's a reverse strsignal(3) */
    if (strcmp(str, "HUP") == 0)
        *signum = 1;
    else if (strcmp(str, "INT") == 0)
        *signum = 2;
    else if (strcmp(str, "QUIT") == 0)
        *signum = 3;
    else if (strcmp(str, "ILL") == 0)
        *signum = 4;
    else if (strcmp(str, "ABRT") == 0)
        *signum = 6;
    else if (strcmp(str, "FPE") == 0)
        *signum = 8;
    else if (strcmp(str, "KILL") == 0)
        *signum = 9;
    else if (strcmp(str, "SEGV") == 0)
        *signum = 11;
    else if (strcmp(str, "PIPE") == 0)
        *signum = 13;
    else if (strcmp(str, "ALRM") == 0)
        *signum = 14;
    else if (strcmp(str, "TERM") == 0)
        *signum = 15;
    else if (strcmp(str, "USR1") == 0)
        *signum = 30;
    else if (strcmp(str, "USR2") == 0)
        *signum = 31;
    else if (strcmp(str, "CHLD") == 0)
        *signum = 20;
    else if (strcmp(str, "CONT") == 0)
        *signum = 19;
    else if (strcmp(str, "STOP") == 0)
        *signum = 17;
    else if (strcmp(str, "TSTP") == 0)
        *signum = 18;
    else if (strcmp(str, "TTIN") == 0)
        *signum = 21;
    else if (strcmp(str, "TTOU") == 0)
        *signum = 22;
    else {
        warnx("Unsupported signal \"%s\"", str);
        return -1;
    }
    return 0;
}

static int read_options(int argc, char **argv, options_t *options)
{
    int opt;

    while (opt = getopt(argc, argv, "p:s:"), opt != -1)
        switch (opt) {
        case 'p':
            if (parse_int(optarg, &options->pid, 1, INT_MAX) != 0) {
                warn("Invalid pid \"%s\"", optarg);
                return -1;
            }
            break;
        case 's':
            if (parse_signal(optarg, &options->signum) != 0) {
                warn("invalid sinal \"%s\"", optarg);
                return -1;
            }
            break;
        default:
            warnx("Invalid option '%c'", opt);
            return -1;
        }

    return 0;
}

int main(int argc, char **argv)
{
    options_t options;

    if (read_options(argc, argv, &options) == -1) {
        fprintf(stderr, USAGE, basename(argv[0]));
        exit(EX_USAGE);
    }


    exit(EXIT_SUCCESS);
}
