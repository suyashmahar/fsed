#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int main() {
	printf("Starting test...\n");
	FILE *fd0, *fd1;
	fd0 = fopen("./sample0.txt", "r");
	fd1 = fopen("./sample1.txt", "r");
	printf("fd: %d\n", fileno(fd0));
	printf("fd: %d\n", fileno(fd1));

	printf("Printing stuff from fd%d\n", fileno(fd0));
	char buf[100];
	fscanf(fd0, "%s", buf);
	printf("%s\n", buf);
	return 0;
}
