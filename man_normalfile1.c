#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

typedef struct{
	char name[4];
	int age;
} people;

main (int argc, char *argv[])
{
	int fd,i;
	people *p_map;
	char temp;
	
	if (argc != 2) {
		fprintf(stderr, "please give me a file name like this: %s [file name] \n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	fd = open(argv[1], O_CREAT|O_RDWR|O_TRUNC,00777);	
	if (fd == -1)
		handle_error("open");
	lseek(fd, sizeof(people)*5-1, SEEK_SET);
	write(fd, "", 1);
	p_map = (people*) mmap (NULL, sizeof(people)*10,PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	temp = 'a';
	for(i=0; i<10; i++)
	{
		temp++;
		memcpy((*(p_map+i)).name, &temp,2);
		(*(p_map+i)).age = 20 + i;
	}
	
	printf("initialize over \n");
	sleep(10);

	munmap(p_map, sizeof(people)*10);
	printf("umap ok \n");
}

		
