/*
 -------------------------------------
 File:    filecopy.c
 Project: a01
 file description
 -------------------------------------
 Author:  Mila Cvetanovska
 ID:      210311400
 Email:   cvet1400@mylaurier.ca
 Version  2023-09-21
 -------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>

int main(int argc, char *argv[]) {

	const char *content;
	content = "This is an input text";

	if (argc == 1) {
		printf("Insufficient parameters \n");
		return 1;
	} else {

		const char *inputFileName = argv[1];
		const char *outputFileName = argv[2];

		size_t size = strlen(content);
		int fd = creat(inputFileName, O_RDWR);

		if (fd == -1) {
			printf("Error: failed to create file");
		} else {

			write(fd, content, size);
			FILE *meminfo_file = fopen(inputFileName, "r");

			if (argc == 1) {
				printf("Insufficient parameters \n");
				return 1;
			} else {
				int outFile = creat(outputFileName, O_RDWR);

				char line[256];
				while (fgets(line, sizeof(line), meminfo_file) != NULL) {
					size = strlen(line);
					// Write the line to the output file
					write(outFile, line, size);
				}
				printf(
						"The contents of file %s have been successfully copied into the %s \n",
						inputFileName, outputFileName);
				close(outFile);
			}
		}
		close(fd);
	}
}

