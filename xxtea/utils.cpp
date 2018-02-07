#include "xxtea.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <direct.h>
#include "utils.h"


int write_file(const char* sign, const char* content, int len, const char* file) {
	int sign_len = 0;
	FILE *f = fopen(file, "wb");
	if (!f) {
		printf("打开文件:%s 失败![%s]\n", file, __FUNCTION__);
		return -1;
	}

	//当解码的时候就不需要文件头，入入空指针即可
	sign_len = sign == NULL ? 0 : strlen(sign);
	if (sign_len > 0)
		fwrite(sign, 1, sign_len, f);

	fwrite(content, 1, len, f);

	fclose(f);
	return 0;
}

int read_file(const char* inputf, char **out) {
	char *p = NULL;
	int total = 0;
	char temp[8096];

	FILE *f = fopen(inputf, "rb");
	if (!f) {
		printf("打开文件:%s 失败![%s]\n", inputf, __FUNCTION__);
		return -1;
	}

	p = (char*)malloc(1);
	while (!feof(f)) {
		int n = fread(temp, 1, sizeof(temp), f);
		total += n;
		p = (char*)realloc(p, total + 1);
		memcpy(p + total - n, temp, n);
	}

	fclose(f);

	*out = p;
	return total;
}

int encode_file(const char* input, const char*output, const char* sign, const char* key) {
	char *in_content = NULL;
	int in_total = 0;
	char *encode_result = NULL;
	xxtea_long result_size = 0;

	//读取文件流
	if ((in_total = read_file(input, &in_content)) == -1) {
		return -1;
	}

	//加密
	encode_result = (char*)xxtea_encrypt((unsigned char*)in_content, in_total,
		(unsigned char*)key, strlen(key), &result_size);

	//写入文件流
	if (write_file(sign, encode_result, result_size, output)) {
		free(encode_result);
		return -2;
	}

	//完工
	if (encode_result) {
		free(encode_result);
		encode_result = NULL;
	}
	return 0;
}

int decode_file(const char* input, const char*output, const char* sign, const char* key) {
	int sign_len = 0;
	char *in_content = NULL;
	int in_total = 0;
	char *encode_result = NULL;
	xxtea_long result_size = 0;

	//读取文件流
	if ((in_total = read_file(input, &in_content)) == -1) {
		return -1;
	}

	//验证一下文件头
	sign_len = strlen(sign);
	if (sign_len && strncmp(in_content, sign, sign_len) != 0) {
		return -2;
	}

	//解密, 忽略文件头
	encode_result = (char*)xxtea_decrypt((unsigned char*)in_content + sign_len, in_total - sign_len,
		(unsigned char*)key, strlen(key), &result_size);

	//写入文件流
	if (write_file(NULL, encode_result, result_size, output)) {
		free(encode_result);
		return -3;
	}

	//完工
	if (encode_result) {
		free(encode_result);
		encode_result = NULL;
	}
	return 0;
}

void get_ext(const char *file_name, char* extension)
{
	int i = 0, length;
	length = strlen(file_name);
	for (i = length - 1; i >= 0; i--)
	{
		if (file_name[i] == '.')
			break;
	}
	if (i < length)
		strcpy(extension, file_name + i + 1);
	else
		strcpy(extension, "\0");
}

//创建多级目录
void mkdir_p(char *dir)
{
	int i, len;
	char str[512];
	strncpy(str, dir, 512);
	len = strlen(str);
	for (i = 0; i < len; i++)
	{
		char sep = str[i];
		if (sep == '/' || sep == '\\')
		{
			str[i] = '\0';
			if (access(str, 0) != 0)
			{
				mkdir(str);
			}
			str[i] = sep;
		}
	}
	if (len > 0 && access(str, 0) != 0)
	{
		mkdir(str);
	}
	return;
}

void list_files(const char * inputDir, const char * outputDir, func fn, opts_holder *opts)
{
	char input_new_dir[1000];
	char target_new_dir[1000];
	char target[1000];
	char source[1000];
	char ext[10] = "*";
	int fmt_len = 0;

	strcpy(input_new_dir, inputDir);
	strcat(input_new_dir, "\\");
	strcat(input_new_dir, ext);    // 在目录后面加上"*"进行第一次搜索
	strcpy(target_new_dir, outputDir);
	strcat(target_new_dir, "\\");

	intptr_t handle;
	_finddata_t find_data;

	handle = _findfirst(input_new_dir, &find_data);
	if (handle == -1)        // 检查是否成功
		return;

	mkdir_p(target_new_dir);

	do
	{
		if (find_data.attrib & _A_SUBDIR)
		{
			if (strcmp(find_data.name, ".") == 0 || strcmp(find_data.name, "..") == 0)
				continue;

			//printf("%s\t<dir>\n", find_data.name);

			// 在目录后面加上"\\"和搜索到的目录名进行下一次搜索
			strcpy(input_new_dir, inputDir);
			strcat(input_new_dir, "\\");
			strcat(input_new_dir, find_data.name);
			// 生成目标目录
			strcpy(target_new_dir, outputDir);
			strcat(target_new_dir, "\\");
			strcat(target_new_dir, find_data.name);
			mkdir_p(target_new_dir);
			list_files(input_new_dir, target_new_dir, fn, opts);
		}
		else
		{
			char file_ext[20] = "";
			get_ext(find_data.name, file_ext);
			fmt_len = strlen(file_ext);
			if (stricmp(file_ext, opts->file_format) != 0 || fmt_len == 0) {
				continue;
			}
			memset(target, 0, sizeof(target));
			memset(source, 0, sizeof(source));
			printf("%s \t %d bytes.\n", find_data.name, find_data.size);
			strcpy(source, inputDir);
			strcat(source, "\\");
			strcat(source, find_data.name);
			strcpy(target, outputDir);
			strcat(target, "\\");
			strcat(target, find_data.name);
			fn(source, target, opts->xxteasign, opts->xxteakey);

		}
	} while (_findnext(handle, &find_data) == 0);

	_findclose(handle);    // 关闭搜索句柄
}

char    *optarg;                /* argument associated with option */
int     opterr = 1,             /* if error message should be printed */
optind = 1,             /* index into parent argv vector */
optopt,                 /* character checked for validity */
optreset;               /* reset getopt */

#define BADCH   (int)'?'
#define BADARG  (int)':'
#define EMSG    ""

const char *_getprogname(void) {
	return progname;
}

/*
* getopt --
*      Parse argc/argv argument vector.
*/
int getopt(int nargc, char * const nargv[], const char *ostr)
{
	static char *place = EMSG;              /* option letter processing */
	char *oli;                              /* option letter list index */

	if (optreset || *place == 0) {          /* update scanning pointer */
		optreset = 0;
		place = nargv[optind];
		if (optind >= nargc || *place++ != '-') {
			/* Argument is absent or is not an option */
			place = EMSG;
			return (-1);
		}
		optopt = *place++;
		if (optopt == '-' && *place == 0) {
			/* "--" => end of options */
			++optind;
			place = EMSG;
			return (-1);
		}
		if (optopt == 0) {
			/* Solitary '-', treat as a '-' option
			if the program (eg su) is looking for it. */
			place = EMSG;
			if (strchr(ostr, '-') == NULL)
				return (-1);
			optopt = '-';
		}
	}
	else
		optopt = *place++;

	/* See if option letter is one the caller wanted... */
	if (optopt == ':' || (oli = (char*)strchr(ostr, optopt)) == NULL) {
		if (*place == 0)
			++optind;
		if (opterr && *ostr != ':')
			(void)fprintf(stderr,
				"%s: illegal option -- %c\n", _getprogname(),
				optopt);
		return (BADCH);
	}

	/* Does this option need an argument? */
	if (oli[1] != ':') {
		/* don't need argument */
		optarg = NULL;
		if (*place == 0)
			++optind;
	}
	else {
		/* Option-argument is either the rest of this argument or the
		entire next argument. */
		if (*place)
			optarg = place;
		else if (nargc > ++optind)
			optarg = nargv[optind];
		else {
			/* option-argument absent */
			place = EMSG;
			if (*ostr == ':')
				return (BADARG);
			if (opterr)
				(void)fprintf(stderr,
					"%s: option requires an argument -- %c\n",
					_getprogname(), optopt);
			return (BADCH);
		}
		place = EMSG;
		++optind;
	}
	return (optopt);                        /* return option letter */
}

void parse_opts(opts_holder *cf, int argc, char **argv) {
	int opt;
	int tmp;
	progname = argv[0];

	memset(cf, 0, sizeof(*cf));

#ifdef ENABLE_TEST
	while (-1 != (opt = getopt(argc, argv, "i:o:s:k:f:aedhrt"))) {
#else
	while (-1 != (opt = getopt(argc, argv, "i:o:s:k:f:edhr"))) {
#endif
		switch (opt) {
		case 'e':	cf->encode = 1;	break;
		case 'd':	cf->decode = 1;	break;
		case 'r':	cf->recursion = 1;	break;
#ifdef ENABLE_TEST
		case 't':	cf->test = 1;	break;
#endif

		case 'i':	strcpy(cf->input, optarg);	break;
		case 'o':	strcpy(cf->output, optarg);	break;
		case 's':	strcpy(cf->xxteasign, optarg);	break;
		case 'k':	strcpy(cf->xxteakey, optarg);	break;
		case 'f':	strcpy(cf->file_format, optarg);	break;

		default:
			usage();
		}
	}

	//解密及加密、根据文件头自动决定模式等不能同时存在
	tmp = cf->encode + cf->decode + cf->test;

	if (tmp > 1 || tmp == 0)
		usage();

	//输入和输出都不可以为空
	if (strlen(cf->input) == 0 || strlen(cf->output) == 0)
		usage();
	}
