#include "judger.h"


// in case of execve("exe");
// 生成一个随机文件名
void generate_name(const char *in_name, char *out_name) {
	srand((unsigned)time(NULL) + 970830);
	int len = sizeof(in_name) / sizeof(char);
	int i;
	for (i = 0; i < len; i++) out_name[i] = rand() % 26 + 65;
}

// 编译
// 			CFG -> language, CFG -> source_name, file_name, CFG -> compile_option
int compile(const char *lan, char *file_name, char *out_name, char **compile_opt) {
	char compiler[32] = {0};
	char out_com[32] = {0};
	// 语言
	if (strcmp(lan, "C++") == 0) {
		strcpy(compiler, "g++");
	} else if (strcmp(lan, "C") == 0) {
		strcpy(compiler, "gcc");
	} else if (strcmp(lan, "Python2") == 0) {
		strcpy(compiler, "python");
	} else if (strcmp(lan, "Python3") == 0) {
		strcpy(compiler, "python3");
	} else {
		REPORTER("No Langue");
		return -1;
	}
	
	generate_name(file_name, out_name);
	sprintf(out_com, "-o%s", out_name);
	
	char **argv = (char **)malloc(sizeof(char*) * 10);
	argv[0] = compiler;
	argv[1] = file_name;
	argv[2] = out_com;
	if (compile_opt != NULL) {
		int len = 3;
		while(compile_opt[len - 3] != NULL) argv[len] = compile_opt[len - 3], len += 1;
		argv[len] = NULL;
	} else argv[3] = NULL;
	/*
	typedef struct LimitList {
	int time_lim;
	int memory_lim;
	int output_lim;
	} LimitList;
	typedef struct RunConfig {
	int is_compilation;
	int use_sandbox;
	int is_limited;
	char* run_program;
	char* in_file;
	char* out_file;
	char** argv;
	LimitList lims;
	} RunConfig;
	*/
	RunConfig RRCF = {1, 0, 1, compiler, "", "compile.out", argv, {10000, 128, 512 * 1024 * 1024}};
	/*
	typedef struct RunResult {
	int use_time;
	int use_memory;
	int run_signal;
	int return_value;
	int judger_error;
	} RunResult;
	*/
	RunResult RRES = {0, 0, 0, 0, 1};
	
	if (runner(&RRCF, &RRES) != 0) {
		REPORTER("Compile fail.");
		if (argv != NULL) free(argv);
		return -1;
	}

	if (argv != NULL) free(argv);
	return 0;
}


int get_result(Config *CFG, Result *RES) {
	if (CFG -> special_judge != NULL) {
		char file_name[64] = {0};
		char* argv[] = {NULL};

		if (compile(CFG -> spj_language, CFG -> special_judge, file_name, argv) != 0) {
			RES -> status=SYSTEM_ERROR;
			return -1;
		}
		if (access(file_name, 0) != 0) {
			RES -> status=COMPILE_SPECIAL_JUDGE_ERROR;
			return 0;
		}
		
		char* spjargv[] = {RES -> in, RES -> out, RES -> ans, NULL};
		char spj_tmp_out[] = "spj_tmp.out";
		char* spj_res;
		RunResult RRES = {0, 0, 0, 0, 1};
		RunConfig RCFG = {0, 1, 1, file_name, CFG -> in_file, spj_tmp_out, spjargv, 
					{10, 512, 16 * 1024 * 1024}};

		if (runner(&RCFG, &RRES) != 0) {
			REPORTER("Run progream fail");
			if (remove(file_name)) {
				REPORTER("Delete program fail");
				return -1;
			}
			return -1;
		}
		if (remove(file_name) != 0) {
			REPORTER("Delete program fail");
			if (remove(spj_tmp_out) != 0) {
				REPORTER("Delete special judge out fail");
				return -1;
			}
			return -1;
		}
		
		if (RRES.judger_error != 0) {
			RES -> status=JUDGER_ERROR;
			if (remove(spj_tmp_out) != 0) {
				REPORTER("Delete special judge out fail");
				return -1;
			}
			return 0;
		} else if ((RRES.return_value | RRES.run_signal) != 0) {
			REPORTER("Special Judger exit wrongly");
			RES -> status=RUN_SPECIAL_JUDGE_ERROR;
			if (remove(spj_tmp_out) != 0) {
				REPORTER("Delete special judge out fail");
				return -1;
			}
			return 0;
		}
		
		if ((spj_res = READFILE(spj_tmp_out, 1024)) == NULL) {
			REPORTER("Read special judge result file fail");
			if (remove(spj_tmp_out) != 0) {
				REPORTER("Delete special judge out fail");
				return -1;
			}
			return -1;
		}
		/*
		if (remove(spj_tmp_out) != 0) {
			REPORTER("Delete special judge out fail");
			return -1;
		}
			*/
		int score = 0;
		int i;
		for (i = 0; i < 7 && spj_res[i]; i++) {
			if('0' <= spj_res[i] && spj_res[i] <= '9')
				score = (score << 3) + (score << 1) + spj_res[i] - '0';
		}
		RES -> score = score > 100 ? score % 100 : score;
		if (score == 100) {
			RES -> status=ACCEPTED;
		} else if (score == 0) {
			RES -> status=WRONG_ANSWER;
		} else {
			RES -> status=PARTLY_CORRECT;
		}
		
		if (spj_res != NULL) free(spj_res);
	} else {
		int tmp_res = compare(RES -> out, RES -> ans);
		if (tmp_res == -1) {
			REPORTER("Run comparator fail");
			return -1;
		}
		
		if (! tmp_res) {
			RES -> score = 100;
			RES -> status=ACCEPTED;
		} else {
			RES -> status=WRONG_ANSWER;
		}
	}
	return 0;
} 

// 运行入口
Result run(Config *CFG) {
	// 默认为 SYSTEM_ERRO
	int status=SYSTEM_ERROR;
	/*
	typedef struct Result {
	int score;
	int status;
	char* compile_info;
	char* in;
	char* out;
	char* ans;
	int use_time;
	int use_memory;
	 Result;
	*/
	Result RES = {0, status, NULL, NULL, NULL, NULL, 0, 0};
	char file_name[64] = {0};
	char **argv = NULL;
	if (strcmp(CFG -> language,"C") == 0 || strcmp(CFG -> language,"C++") == 0)
	{
		if (compile(CFG -> language, CFG -> source_name, file_name, CFG -> compile_option) != 0) {
		status=COMPILE_ERROR;
		RES.status = status;
		return RES;
		}
		if ((RES.compile_info = READFILE("compile.out", 256)) == NULL) {
			REPORTER("Read compile result fail");
			return RES;
		}
		if (access(file_name, 0) != 0) {
			status=COMPILE_ERROR;
			RES.status = status;
			return RES;
		}
	}else{
		strcpy(file_name,CFG -> source_name);
		argv = (char **)malloc(sizeof(char*) * 10);
		if (strcmp(CFG -> language,"Python2") == 0)
		{
			strcpy(argv[0],"/usr/bin/python");
		}else if (strcmp(CFG -> language,"Python3") == 0)
		{
			strcpy(argv[0],"/usr/bin/python3");
		}
	}
	struct stat statbuf;
	if (stat(CFG -> ans_file, &statbuf)) {
		status=NO_ANSWERS;
		RES.status = status;
		REPORTER("Get answer file size fail");
		if (remove(file_name)) {
			REPORTER("Delete program fail");
			return RES;
		}
		return RES;
	}
	int size = statbuf.st_size;
	
	RunResult RRES = {0, 0, 0, 0, SIGABRT};
	RunConfig RCFG = {0, 1, 1, file_name, CFG -> in_file, CFG -> out_file, argv, 
					{CFG -> time_limit, CFG -> memory_limit, size}};
	if (strcmp(CFG -> language,"C") != 0 && strcmp(CFG -> language,"C++") != 0){
		RCFG.use_sandbox = 0;
	}

	if (runner(&RCFG, &RRES) != 0) {
		REPORTER("Run progream fail");
		if (remove(file_name) != 0) {
			REPORTER("Delete program fail");
			return RES;
		}
		return RES;
	}
	if (strcmp(CFG -> language,"C") == 0 || strcmp(CFG -> language,"C++") == 0){
		if (remove(file_name) != 0) {
		REPORTER("Delete program fail");
		return RES;
		}
	}
	RES.use_time = RRES.use_time;
	RES.use_memory = RRES.use_memory;
	if (RRES.judger_error) {
		status=JUDGER_ERROR;
	} else if (RRES.run_signal == 0) {
		if (RRES.return_value == 0) {
			status=RUN_SUCCESSFULLY;
		} else {
			status=RUNTIME_ERROR;
		}
		if (RRES.use_time > CFG -> time_limit) {
			status=TIME_LIMIT_EXCEED;
		} else if (RRES.use_memory > CFG -> memory_limit * 1024) {
			status=MEMORY_LIMIT_EXCEED;
		}
	} else if (RRES.run_signal == 31) {
		status=RESTRICTED_FUNCTION;
	} else if (RRES.run_signal == 9) {
		if (RRES.use_time > CFG -> time_limit) {
			status=TIME_LIMIT_EXCEED;
		} else {
			status=RUNTIME_ERROR;
		}
	} else if (RRES.run_signal == 11) {
		if (RRES.use_memory > CFG -> memory_limit * 1024) {
			status=MEMORY_LIMIT_EXCEED;
		} else {
			status=RUNTIME_ERROR;
		}
	} else if (RRES.run_signal == 25) {
		status=OUTPUT_LIMIT_EXCEED;
	} else {
		if (RRES.use_time > CFG -> time_limit) {
			status=TIME_LIMIT_EXCEED;
		} else if (RRES.use_memory > CFG -> memory_limit * 1024) {
			status=MEMORY_LIMIT_EXCEED;
		} else {
			status=RUNTIME_ERROR;
		}
	}
	RES.status = status;
	
	// limit singel file size below 512MB
	if ((RES.in = READFILE(CFG -> in_file, 512 * 1024 * 1024)) == NULL) {
		REPORTER("Read in file fail");
		delete_files(&RES);
		return RES;
	}
	if ((RES.out = READFILE(CFG -> out_file, 512 * 1024 * 1024)) == NULL) {
		REPORTER("Read out file fail");
		delete_files(&RES);
		return RES;
	}
	if ((RES.ans = READFILE(CFG -> ans_file, 512 * 1024 * 1024)) == NULL) {
		REPORTER("Read ans file fail");
		delete_files(&RES);
		return RES;
	}
	
	if (RES.status == RUN_SUCCESSFULLY) {
		if (get_result(CFG, &RES) != 0) {
			REPORTER("Get Result fail");
			delete_files(&RES);
			return RES;
		}
	}
	
	return RES;
}


void delete_files(Result *RES) {
	if (RES -> in != NULL) free(RES -> in);
	if (RES -> out != NULL) free(RES -> out);
	if (RES -> ans != NULL) free(RES -> ans);
	if (RES -> compile_info != NULL) free(RES -> compile_info);
}


void delete_all(Result *RES) {
	if (RES -> in != NULL) free(RES -> in);
	if (RES -> out != NULL) free(RES -> out);
	if (RES -> ans != NULL) free(RES -> ans);
	if (RES -> compile_info != NULL) free(RES -> compile_info);
}

