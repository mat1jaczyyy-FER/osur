/*! Hello world program */

#include <stdio.h>
#include <api/prog_info.h>

int fs_test()
{
	printf("Example program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__,
		 fs_test_PROG_HELP);

	int fd = open("file:fs_test", O_CREAT | O_WRONLY, 0);
	int fd2 = open("file:fs_test2", O_CREAT | O_WRONLY, 0);

	write(fd, "neki tekst", 11);
	write(fd2, "neki drugi tekst", 17);

	close(fd);
	close(fd2);

	fd = open("file:fs_test", O_RDWR, 0);
	fd2 = open("file:fs_test2", O_RDONLY, 0);

	char buff[17];

	read(fd, buff, 11);
	printf("fs_test=%s\n", buff);

	read(fd2, buff, 17);
	close(fd);
	
	printf("fs_test2=%s\n", buff);
	close(fd2);

	return 0;
}
