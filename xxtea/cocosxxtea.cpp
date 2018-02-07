//----------------------------------------------------
// linbc  2014.11.28
// http://linbc.cnblogs.com/
// github: https://github.com/linbc/cocos2dx_xxtea_asset

#include "xxtea.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include "utils.h"
#include <time.h>

#define ENABLE_TEST 1
#undef ENABLE_TEST

#define MACOS 1

const char *progname = __FILE__;

void usage(void) {
	printf("\n\n使用说明:\n"
		"\n"
		"  %s [-e|-d] -i [file] -o [file] [-f js] -s xxtea -k key \n"
		"  %s [-e|-d] -r -i [dir] -o [dir] [-f js] -s xxtea -k key \n"
		"\n"
		"选项:\n"
		"\n"
		"  -e 加密文件 \n"
		"  -d 解密文件 \n"
		"  -f 使用-r选项时只处理指定的文件格式 \n"
		"  -i 输入文件路径，使用-r选项时为目录 \n"
		"  -o 输出文件路径，使用-r选项时为目录 \n"
		"  -s xxtea签名字符串 \n"
		"  -k xxtea密匙 \n"
		"  -r 指定输入输出参数为目录并递归所有子目录 \n"
#ifdef ENABLE_TEST
		"  -t test \n"
#endif
		"  -h 显示使用说明 \n"
		"",
		progname, progname);
	exit(1);
}

#ifdef ENABLE_TEST
int test_file(const char* input, const char*output, const char* sign, const char* key) {
	return 0;
}
#endif

int main(int argc, char **argv) {
	opts_holder opts;
	parse_opts(&opts, argc, argv);

	func fn;
#ifdef ENABLE_TEST
	if (opts.test) {
		list_files(opts.input, opts.output, test_file, &opts);
		return 0;
	}
#endif

	if (opts.encode) {
		fn = encode_file;
	}
	else if (opts.decode) {
		fn = decode_file;
	}
	else {
		usage();
		return 0;
	}

	if (opts.recursion) {
		list_files(opts.input, opts.output, fn, &opts);
	}
	else
	{
		fn(opts.input, opts.output, opts.xxteasign, opts.xxteakey);
	}
	return 0;
}
