#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // for memcpy
#include <strings.h>
#include <sys/mman.h> // for mmap function
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define PERMS 0600

int main (int argc, char * argv[])
{
	/* basic definition for file copy */
	int src, dst;
	struct stat statbuf;
	char * sm, *dm;

	/* guarantee two files' copy */
	if(argc != 3)
	{
		fprintf( stderr, "Usage %s: \n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	/* open src and dst files */
	if ( (src = open(argv[1], O_RDONLY)) < 0)
	{
		perror("source failure");
		exit(EXIT_FAILURE);
	}

	if ( (dst= open(argv[2], O_RDWR | O_CREAT | O_TRUNC, PERMS)) < 0)
	{
		perror("destination failure");
		exit(EXIT_FAILURE);
	}

	/* researve check */
	if (fstat(src, &statbuf) < 0)
	{
		perror("fstat failure");
		exit(EXIT_FAILURE);
	}
	
	if (lseek(dst, statbuf.st_size - 1, SEEK_SET) < 0)
	{
		perror("lseek failure");
		exit(EXIT_FAILURE);
	}

	if (write(dst, &statbuf, 1) != 1)
	{
		perror("write target");
		exit(EXIT_FAILURE);
	}


	/* mmap relationship */
	sm = mmap(0, (size_t) statbuf.st_size, PROT_READ , MAP_PRIVATE | MAP_NORESERVE, src, 0);
	if (MAP_FAILED == sm)
	{
		perror("source map failure");
		exit(EXIT_FAILURE);
	}
	
	dm = mmap(0, (size_t) statbuf.st_size, PROT_WRITE, MAP_SHARED, dst, 0);
	if (MAP_FAILED == dm)
	{
		perror("destination map failure");
		exit(EXIT_FAILURE);
	}

	/* copy the file from source to destination */
	memcpy (dm, sm, (size_t)statbuf.st_size);

	/* the end of the two maps */
	munmap(sm, (size_t) statbuf.st_size);
	munmap(dm, (size_t) statbuf.st_size);
	close(src);
	close(dst);
	return 0;
}
