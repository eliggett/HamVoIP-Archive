//	Trivial Watchdog 
//
//	KB4FXC, 09/01/2014
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>

int fd = -1;

int main(int argc, char *argv[])
{

	fd = open("/dev/watchdog", O_WRONLY);

	if (fd == -1) {
		fprintf(stderr, "Watchdog device not enabled.\n");
		fflush(stderr);
		exit(-1);
	}
	
	for (;;) {
		write (fd, "\n", 1);
		sleep(10);
	}

	// Should never get here!

	close(fd);
	exit (0);
}

