

typedef struct opts_holder_t {
	int encode;
	int decode;
	int recursion;
	int test = 0;

	char input[1024];
	char output[1024];
	char xxteasign[1024];
	char xxteakey[1024];
	char file_format[20];
}opts_holder;

extern const char * progname;

typedef int(*func)(const char* input, const char*output, const char* sign, const char* key);

int getopt(int nargc, char * const nargv[], const char *ostr);

int write_file(const char* sign, const char* content, int len, const char* file);

int read_file(const char* inputf, char **out);

int encode_file(const char* input, const char*output, const char* sign, const char* key);

int decode_file(const char* input, const char*output, const char* sign, const char* key);

void get_ext(const char *file_name, char* extension);

void mkdir_p(char *dir);

void list_files(const char * inputDir, const char * outputDir, func fn, opts_holder *opts);

const char *_getprogname(void);

int getopt(int nargc, char * const nargv[], const char *ostr);

void parse_opts(opts_holder *cf, int argc, char **argv);

void usage();

