#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

#define MAX_INPUT_SHADERS 32
#define USAGE "\
	Usage\e[37m:      \e[32msth \e[35m-o \e[0;97m[\e[3;37mheader.h\e[0;97m] [\e[3;37mfile1\e[37m \e[37mfile2\e[37m ... \e[37mfileN\e[0;97m]\e[0m\n \
	Example\e[37m:    \e[32msth \e[35m-o \e[3;37mshaders.h vertex.glsl fragment.glsl\e[0m\n"

int
main(int argc, char *argv[])
{
	/* Handle arguments */
	if (argv[0] == NULL) {
		printf("just what do u think ur doing??????\n");
		exit(EXIT_FAILURE);
	}

	static FILE *input_files[64];
	char *output_filename = NULL;
	for (int i = 1; argv[i] != NULL; i++)
	{
		if (strcmp(argv[i], "-o") == 0)
		{
			i++;
			if (argv[i] == NULL) {
				fprintf(stderr, "you can't just put '-o', you need to put the name of the output file, silly\n");
				exit(EXIT_FAILURE);
			}
			output_filename = argv[i];
		}
		else
		{
			static FILE **current_infile = input_files;
			*current_infile = fopen(argv[i], "r");
			current_infile++;
		}
	}

	if (input_files[0] == NULL) {
		printf("u didn't put any input files, there, bud...\n");
		exit(EXIT_FAILURE);
	}

	/* Generate header protection string (capslock filename) */
	char *headername = strdup(output);
	for (int i = 0; headername[i] != '\0'; i++) {
		char c = headername[i];
		c &= 0b11011111;
		if (c < 'A' || c > 'Z') {
			headername[i] = '_'; /* set non-alphabetical characters to '_' */
			continue;
		}
		headername[i] &= 0b11011111; /* make uppercase */
	}

	/* Generate header file */
	FILE *output_fp = fopen(output, "w");
	fprintf(output_fp, "#ifndef %s\n#define %s\n\n", headername, headername);

	for (int i = 0; input_files[i] != NULL; i++)
	{
		/* Make 'strname' appropriate for a c variable name */
		char *strname = strdup(input[i]);
		for (int j = 0; strname[j] != '\0'; j++) {
			char c = strname[j];
			c &= 0b11011111;
			if (c < 'A' || c > 'Z') {
				strname[j] = '_'; /* set non-alphabetical characters to '_' */
				continue;
			}
			strname[i] |= 0b00100000; /* make lowercase */
		}
	}

	fprintf(output_fp, "#endif");
	fclose(output_fp);

	/* Close all input files */
	for (int i = 0; input_files[i] != NULL; i++)
		fclose(input_files[i]);
	
	return 0;
}
