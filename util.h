#ifndef UTIL_H
#define UTIL_H

void
die(const char *const msg) {
	perror(msg);
	exit(1);
}

#endif