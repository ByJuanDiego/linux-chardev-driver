#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#define CDEV_NAME "/dev/chardev"
#define BUF_LEN 80

char buf[BUF_LEN];

int main(int argc, char *argv[])
{
	int fd, bytes_written;

	strcpy(buf, argv[1]);

	fd = open(CDEV_NAME, O_CREAT | O_WRONLY, 0777);

	if (fd < 0)
	{
		perror("Open");
		exit(fd);
	}

	bytes_written = write(fd, buf, strlen(buf));

	if (bytes_written < 0)
	{
		perror("Write");
		exit(bytes_written);
	}

	printf("Wrote: '%s' to /dev/chardev\n", buf);
}
