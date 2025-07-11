#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

	static char *input_files[64];
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
			static char **current_infile = input_files;
			*current_infile = argv[i];
			current_infile++;
		}
	}

	if (input_files[0] == NULL) {
		printf("u didn't put any input files, there, bud...\n");
		exit(EXIT_FAILURE);
	} else if (output_filename == NULL) {
		printf("u know u need to put an output file... right?\n");
		exit(EXIT_FAILURE);
	}

	/* Generate header protection string (capslock filename) */
	char *header_protection = strdup(output_filename);
	for (int i = 0; header_protection[i] != '\0'; i++) {
		char c = header_protection[i];
		c &= 0b11011111; /* make uppercase */
		if (c < 'A' || c > 'Z') {
			header_protection[i] = '_'; /* set non-alphabetical characters to '_' */
			continue;
		}
		header_protection[i] = c;
	}

	/* Generate header file */
	FILE *output_fp = fopen(output_filename, "w");
	fprintf(output_fp, "#ifndef %s\n#define %s\n\n", header_protection, header_protection);

	for (int i = 0; input_files[i] != NULL; i++)
	{
		/* Make 'strname' appropriate for a c variable name */
		char *lower_fname = strdup(input_files[i]);
		for (char *c = lower_fname; *c != '\0'; c++)
		{
			if (*c == '.') {
				*c = '\0';
				break;
			} else if ((*c | 0x20) < 'a' || (*c | 0x20) > 'z') {
				*c = '_';
				continue;
			}

			*c |= 0x20;
		}

		char *var_name = malloc(sizeof("sth_" "_src") + strlen(lower_fname) - 1);
		strncpy(var_name, "sth_", 4);
		strncpy(&var_name[4], lower_fname, strlen(lower_fname));
		strncpy(&var_name[4 + strlen(lower_fname)], "_src", 5);

		/* Write variable name to output file */
		fprintf(output_fp, "static const char *%s = \"\\\n", var_name);
		free(var_name);
		free(lower_fname);

		/* Open file */
		FILE *input_fp = fopen(input_files[i], "r");

		/* Copy contents of input file into output file line by line */
		char *line = NULL;
		size_t len = 0;
		int getline_result;
		errno = 0;
		while ((getline_result = getline(&line, &len, input_fp)) > 0) {
			line[getline_result - 1] = '\0';
			fprintf(output_fp, line);
			fwrite("\\n\\\n", sizeof(char), 4, output_fp);
			free(line);
			line = NULL;
		}

		fclose(input_fp);
		if (errno > 0) {
			fprintf(stderr, "something went wrong\n");
			exit(EXIT_FAILURE);
		}

		/* Close string definition */
		fprintf(output_fp, "\";\n\n");
	}

	fprintf(output_fp, "#endif");
	fclose(output_fp);

	return 0;
}
