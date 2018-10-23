#! /usr/bin/python
# -*- coding: utf-8 -*-

import os
import Judger


CFG = {
	'language':'C++',
	'source_name':'MLE/lll.cpp',
	'in_file':'lll.in',
	'out_file':'lll.out',
	'ans_file':'lll.ans',
	'time_limit':1000,
	'memory_limit':64,
	'compile option':['-O2', '-lm', '-DONLINE_JUDGE']
}

a = ["AC", "WA", "RE", "TLE", "MLE", "OLE", "MLE(stack)", "CE", "DSC"]

for item in a:
	CFG['source_name'] = item + '/lll.cpp'
	print(item + " Config:")
	print(CFG)
	RES = Judger.run(CFG)
	print(item + " Result:")
	print(RES)
	print("")


