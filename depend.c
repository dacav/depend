#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>

/* simple program that keeps running until a given process exits */

int main(int argc, char *argv[]) {
	int kq;
	struct kevent events[1];

	if (argc != 2) {
		fprintf(stderr, "usage: %s pid\n", argv[0]);
		exit(1);
	}

	if ((kq = kqueue()) == -1) err(1, "kqueue");

	struct kevent change = { // event to watch for
		.ident = fd,
		.filter = EVFILT_PROC,
		.flags = EV_ADD | EV_CLEAR,
		.fflags = NOTE_EXIT,
		.data = 0,
		.udata = NULL
	};

	if (pledge("stdio", NULL) == -1) err(1, "pledge");

	for (;;) {
		int n;
		/* register changes and wait for events */
		if ((n = kevent(kq, &change, file_count, events, file_count, NULL)) == -1)
			err(1, "kevent wait");
		if (n > 0) {
			for (int i = 0; i < n; i++) {
				if (events[i].flags & EV_ERROR)
					errx(1, "event error: %s", strerror(events[i].data));
				if (events[i].fflags & NOTE_EXIT)
					exit(0);
			}
		}
	}
}
