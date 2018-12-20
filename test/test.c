#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int readcontent(FILE *fp) {
	char buf[4096];
	int rslt = fscanf(fp, "%s", buf);
	if (!rslt) {
		fprintf(stderr, "read error\n");
	}
	printf("== contents of fd: %d ==\n%s\n", fileno(fp), buf);
	return 0;
}

int main() {
	FILE *fp0, *fp1, *fp2;

	fp0 = fopen("./sample0.txt", "r");
	fp1 = fopen("./sample1.txt", "r");
	fp2 = fopen("./sample0.txt", "r");

	readcontent(fp0);
	readcontent(fp1);
	readcontent(fp2);

	return 0;
}
