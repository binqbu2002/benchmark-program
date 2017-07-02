#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // for memcpy
#include <strings.h>
#include <sys/mman.h>  // for mmap function
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define PERMS 0600

int main (int argc, char * argv[])
{
	int src, dst;
	void * sm, * dm;
	struct stat statbuf;

	if (argc != 3)
	{
		fprintf(stderr, "Usag: %s \n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if ((src = open (argv[1], O_RDONLY)) < 0)
	{
		perror("open source");
		exit(EXIT_FAILURE);
	}

	/* include the read open */
	if (( dst = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, PERMS)) < 0)
	{
		perror("open target");
		exit(EXIT_FAILURE);
	}

	if(fstat(src, & statbuf) < 0)
	{
		perror("fstat source");
		exit(EXIT_FAILURE);
	}
	/* mmap could not be used for extending file length, so we should prepare an extended stat buffer size */
	
	if (lseek(dst, statbuf.st_size -1, SEEK_SET) < 0)
	{
		perror("lseek target");
		exit(EXIT_FAILURE);
	}
	if(write(dst, &statbuf, 1) != 1)
	{
		perror("write target");
		exit(EXIT_FAILURE);
	}

	/* if you read an read-only file, please mark the mark the make as MAP_PRIVATE */
	sm = mmap (0, (size_t)statbuf.st_size, PROT_READ, MAP_PRIVATE | MAP_NORESERVE, src, 0);
	if (MAP_FAILED == sm)
	{
		perror("mmap source");
		exit (EXIT_FAILURE);
	}

	dm = mmap (0, (size_t)statbuf.st_size, PROT_WRITE, MAP_SHARED, dst, 0);
	if (MAP_FAILED == dm)
	{
		perror("mmap target");
		exit(EXIT_FAILURE);
	}

	memcpy(dm, sm, (size_t)statbuf.st_size);	
	close(src);
	close(dst);
	return 0;

}
