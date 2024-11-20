#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define BUF_LEN 80
#define CDEV_NAME "/dev/chardev"

char buf[BUF_LEN + 1];

int main()
{
	int fd, bytes_read;

	fd = open(CDEV_NAME, 0, O_RDONLY);

	if (fd < 0)
	{
		perror("Open");
	}

	bytes_read = read(fd, buf, BUF_LEN);

	if (bytes_read < 0)
	{
		perror("Read");
		exit(bytes_read);
	}

	printf("Read: %s from /dev/chardev\n", buf);
}
