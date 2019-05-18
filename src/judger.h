#ifndef JUDGER_H
#define JUDGER_H


#include <string.h>
#include <sys/stat.h>
#include "sandbox.h"
#include "comparator.h"
#include "status.h"


typedef struct Result {
	int score;
	int status;
	char* in;
	char* out;
	char* ans;
	int use_time;
	int use_memory;
} Result;


typedef struct Config {
	char* language;
	char* source_name;
	char* in_file;
	char* out_file;
	char* ans_file;
	int time_limit;
	int memory_limit;
	char* special_judge;
	char* spj_language;
} Config;

int get_result(Config *, Result*);

Result run(Config*);

void delete_files(Result*);

void delete_all(Result*);


#endif
