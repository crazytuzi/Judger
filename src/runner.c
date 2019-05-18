#include "runner.h"

// 读取配置
int read_config(Config *CFG) {
	char tmp[300];
	int cp_nums = 0;
	FILE *stream;
	
	stream = fopen("judger.cfg", "r");
	if (stream == NULL) {
		REPORTER("Open config fail");
		return -1;
	}
	
	fscanf(stream, "%s\n", tmp);
	CFG -> language = (char *)malloc((strlen(tmp) + 1) * sizeof(char));
	strcpy(CFG -> language, tmp);
	
	fscanf(stream, "%s", tmp);
	CFG -> source_name = (char *)malloc((strlen(tmp) + 1) * sizeof(char));
	strcpy(CFG -> source_name, tmp);	
	
	fscanf(stream, "%s", tmp);
	CFG -> in_file = (char *)malloc((strlen(tmp) + 1) * sizeof(char));
	strcpy(CFG -> in_file, tmp);	
	
	fscanf(stream, "%s", tmp);
	CFG -> out_file = (char *)malloc((strlen(tmp) + 1) * sizeof(char));
	strcpy(CFG -> out_file, tmp);	
	
	fscanf(stream, "%s", tmp);
	CFG -> ans_file = (char *)malloc((strlen(tmp) + 1) * sizeof(char));
	strcpy(CFG -> ans_file, tmp);	
	
	fscanf(stream, "%d%d%d", &CFG -> time_limit, &CFG -> memory_limit, &cp_nums);
	
	// spj 暂时不用
	if (fscanf(stream, "%s", tmp) != EOF) {
		CFG -> special_judge = (char *)malloc((strlen(tmp) + 1) * sizeof(char));
		strcpy(CFG -> special_judge, tmp);
		
		fscanf(stream, "%s", tmp);
		CFG -> spj_language = (char *)malloc((strlen(tmp) + 1) * sizeof(char));
		strcpy(CFG -> spj_language, tmp);
	} else {
		CFG -> spj_language = NULL;
		CFG -> special_judge = NULL;
	}
	
	fclose(stream);
	
	return 0;
}

// 释放
void free_cfg(Config *CFG) {
	if (CFG -> language != NULL) free(CFG -> language);
	if (CFG -> source_name != NULL) free(CFG -> source_name);
	if (CFG -> in_file != NULL) free(CFG -> in_file);
	if (CFG -> out_file != NULL) free(CFG -> out_file);
	if (CFG -> ans_file != NULL) free(CFG -> ans_file);
}
// 将结果写入文件
int write_result(Result *RES) {
	FILE *stream;
	stream = fopen("judger.res", "w");
	if (stream == NULL) {
		REPORTER("Open result fail");
		return -1;
	}
	fprintf(stream, "%d\n%d\n%d\n%d", RES -> status, RES -> score, RES -> use_time, RES -> use_memory);
	
	stream = fopen("judger.in", "w");
	if (stream == NULL) {
		REPORTER("Open in file fail");
		return -1;
	}
	fprintf(stream, "%s\n", RES -> in);
	
	stream = fopen("judger.out", "w");
	if (stream == NULL) {
		REPORTER("Open out file fail");
		return -1;
	}
	fprintf(stream, "%s\n", RES -> out);
	
	stream = fopen("judger.ans", "w");
	if (stream == NULL) {
		REPORTER("Open ans file fail");
		return -1;
	}
	fprintf(stream, "%s\n", RES -> ans);
	
	fclose(stream);
	
	return 0;
}


int main(void) {
	Config CFG;
	Result RES;	
	// 首先读取配置
	if (read_config(&CFG) != 0) {
		REPORTER("Read config fail");
		free_cfg(&CFG);
		return 0;
	}
	printf("pid=%d\n",getpid());
	// 运行
	RES = run(&CFG);
	//把结果写入文件
	if (write_result(&RES) != 0) {
		REPORTER("Write config fail");
		free_cfg(&CFG);
		return 0;
	}
	// 释放
	free_cfg(&CFG);
	
	return 0;
}
