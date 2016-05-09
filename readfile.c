#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){

	FILE *fp;
	fp = fopen("mytest.txt", "r");

	// count bytes of file
	int count = 0;

	while(fgetc(fp) != EOF){
		 count++;
	}
	rewind(fp);

	printf("bytes: %d\n", count);
	char *buffer = malloc(count);
	fread(buffer, count, 1, fp);
	printf("%s\n", buffer);

	return 0;
}
