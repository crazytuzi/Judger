#ifndef RUNNER_H
#define RUNNER_H


#include "judger.c"


int read_config(Config*);

int write_result(Result*);

void free_cfg(Config*);


#endif
