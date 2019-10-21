#include <err.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sysexits.h>
#include <unistd.h>

#define USAGE \
    "Usage: %1$s [options] -- command [args...]\n" \
    "       %1$s -p pid\n"

typedef struct options {
    pid_t pid;
} options_t;

static int parse_pid(const char *str, pid_t *result)
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

    if (value < 0) {
        errno = ERANGE;
        return -1;
    }

    *result = (pid_t)value;
    return 0;
}

static int read_options(int argc, char **argv, options_t *options)
{
    int opt;

    while (opt = getopt(argc, argv, "p:"), opt != -1)
        switch (opt) {
        case 'p':
            if (parse_pid(optarg, &options->pid) != 0) {
                warn("Invalid pid \"%s\"", optarg);
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
