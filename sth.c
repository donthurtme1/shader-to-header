#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

#define MAX_INPUT_SHADERS 32
#define USAGE \
"Usage\e[37m:      \e[32mstch \e[35m-o \e[0;97m[\e[3;37mheader.h \e[0;97m] \
[\e[3;37mfile1\e[37m \e[37mfile2\e[37m ... \e[37mfileN\e[0;97m]\e[0m\n\
Example\e[37m:    \e[32mstch \e[35m-o \e[3;37mshaders.h vertex.glsl \
fragment.glsl\e[0m\n"

static char *output, *input[MAX_INPUT_SHADERS];
static FILE *output_fp, *input_fp;

void
cleanup(void) {
	int i;

	if (output_fp != NULL) {
		fclose(output_fp);
		output_fp = NULL;
	}
	if (input_fp != NULL) {
		fclose(input_fp);
		input_fp = NULL;
	}

	if (output != NULL) {
		free(output);
		output = NULL;
	}
	for (i = 0; i < MAX_INPUT_SHADERS; i++) {
		if (input[i] != NULL) {
			free(input[i]);
			input[i] = NULL;
		}
	}
}

void
parsearg(int *idx, char **argv) {
	/* Arguments:
	 *   -o filename.h  ->  output header file */
	if (argv[0][1] == 'o') {
		if (argv[0][2] != '\0') {
			output = strdup(&argv[0][2]);
			return;
		}

		(*idx)++;
		if (argv[1] == NULL) {
			printf("asdf\n");
			printf(USAGE);
			exit(EXIT_FAILURE);
		}
		output = strdup(argv[1]);
		return;
	} else if (argv[0][1] == 'h') {
		printf("Help (todo).\n");
	} else {
		printf(USAGE);
		exit(EXIT_FAILURE);
	}
}

int
main(int argc, char *argv[]) {
	int i, arg_idx = 0, inputstr_idx = 0;

	if (argc < 2) {
		printf("\e[31mError\e[37m: \e[0mMissing \e[1moutput\e[0m file.\n\
\e[31mError\e[37m: \e[0mMissing \e[1minput\e[0m files.\n");
		printf(USAGE);
		exit(EXIT_FAILURE);
	}
	while (argv[++arg_idx] != NULL) {
		if (argv[arg_idx][0] == '-') {
			parsearg(&arg_idx, &argv[arg_idx]);
		} else {
			input[inputstr_idx] = strdup(argv[arg_idx]);
			inputstr_idx++;
		}
	}

	if (atexit(&cleanup) < 0)
		die("atexit");

	if (output == NULL || input[0] == NULL) {
		if (output == NULL)
			printf("\e[31mError\e[37m: \e[0mMissing \e[1moutput\e[0m file.\n");
		if (input[0] == NULL)
			printf("\e[31mError\e[37m: \e[0mMissing \e[1minput\e[0m files.\n");
		printf(USAGE);
		exit(EXIT_FAILURE);
	}

	/* Generate all-caps header file name */
	char *headername = strdup(output);
	for (i = 0; headername[i] != '\0'; i++) {
		char c = headername[i];
		c &= 0b11011111;
		if (c < 'A' || c > 'Z') {
			headername[i] = '_'; /* set non-alphabetical characters to '_' */
			continue;
		}
		headername[i] &= 0b11011111; /* make uppercase */
	}

	output_fp = fopen(output, "w");
	fprintf(output_fp, "#ifndef %s\n#define %s\n\n", headername, headername);

	for (i = 0; input[i] != NULL; i++) {
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

		printf("Converting \e[1m%s\e[0m...\n", input[i]);
		input_fp = fopen(input[i], "r");
		if (input_fp == NULL) {
			die("fopen");
		}

		fprintf(output_fp, "const char *const %s = \"\\\n", strname);

		char *curline = NULL;
		size_t size = 0;
		int len = 0;
		while ((len = getline(&curline, &size, input_fp)) > 0) {
			/*if (curline[len - 1] != '\n') {
				fprintf(output_fp, "%s\";\n\n", curline);
				break;
			}*/
			curline[len - 1] = '\0';
			fprintf(output_fp, "%s\\n\\\n", curline);
		}
		fprintf(output_fp, "\";\n\n");

		fclose(input_fp);
		input_fp = NULL;
	}

	fprintf(output_fp, "#endif");
	output_fp = NULL;
	
	return 0;
}
