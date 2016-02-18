#include <stdlib.h>
#include <stdio.h>


FILE *open_or_exit(const char *fname, const char *mode)
{
	FILE *f = fopen(fname, mode);
	if (f == NULL) {
		perror(fname);
		exit(EXIT_FAILURE);
	}
	return f;
}

int main(int argc, char *argv[])
{
	if (argc <= 3) {
		fprintf(stderr, "Usage: %s <in> <out> <varname...>\n", argv[0]);
		return EXIT_FAILURE;
	}

	const char *inFile = argv[1];
	const char *outFile = argv[2];
	const char *varname = argv[argc-1];

	FILE *in = open_or_exit(inFile, "r");
	FILE *out = open_or_exit(outFile,"w");

	// add includes
	fprintf(out, "#include <string>\n");

	// open namespaces
	for (int i = 3; i < argc - 1; ++i) {
		fprintf(out, "namespace %s {", argv[i]);
	}
	fprintf(out, "\n");

	// create variable
	fprintf(out, "extern const std::string %s = {\n", varname);

	unsigned char buf[1024];
	size_t nread = 0;
	size_t linecount = 0;
	do {
		nread = fread(buf, 1, sizeof(buf), in);
		for (size_t i = 0; i < nread; i++) {
			fprintf(out, "0x%02x, ", buf[i]);
			if (++linecount == 10) { fprintf(out, "\n"); linecount = 0; }
		}
	} while (nread > 0);
	if (linecount > 0) fprintf(out, "\n");

	fprintf(out, "};\n");
	//fprintf(out, "const std::size_t %s_len = sizeof(%s);\n\n", varname, varname);

	// close namespaces
	for (int i = 3; i < argc - 1; ++i) {
		fprintf(out, "}");
	}
	fprintf(out, "\n");

	// cleanup
	fclose(in);
	fclose(out);

	return EXIT_SUCCESS;
}
