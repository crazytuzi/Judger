import os
import re


def run(CFG):
	# 默认的配置文件
	cfg_file = 'judger.cfg'
	res_file = 'judger.res'
	in_file = 'judger.in'
	out_file = 'judger.out'
	ans_file = 'judger.ans'
	# 写入cfg_file
	cfg_stream = open(cfg_file, "w")
	cfg_stream.write(str(CFG['language']) + '\n')
	cfg_stream.write(str(CFG['source_name']) + '\n')
	cfg_stream.write(str(CFG['in_file']) + '\n')
	cfg_stream.write(str(CFG['out_file']) + '\n')
	cfg_stream.write(str(CFG['ans_file']) + '\n')
	cfg_stream.write(str(CFG['time_limit']) + '\n')
	cfg_stream.write(str(CFG['memory_limit']) + '\n')
	# special_judge 暂时不考虑
	if 'special_judge' in CFG:
		cfg_stream.write(str(CFG['special_judge']) + '\n')
		cfg_stream.write(str(CFG['spj_language']) + '\n')
	cfg_stream.close()
	# 运行
	if os.path.isfile("runner"):
		os.system("./runner")
	else:
		print("Please put runner here!")
		return

	RES = {}
	# 取结果
	res_stream = open(res_file, "r")
	str_status = res_stream.readline()
	RES['status'] = int(re.sub("\D", "", str_status))
	str_score = res_stream.readline()
	RES['score'] = int(re.sub("\D", "", str_score))
	str_time = res_stream.readline()
	RES['use_time'] = int(re.sub("\D", "", str_time))
	str_memory = res_stream.readline()
	RES['use_memory'] = int(re.sub("\D", "", str_memory))
	res_stream.close()
	
	in_stream = open(in_file, "r")
	RES['in'] = in_stream.read()
	in_stream.close()
	
	out_stream = open(out_file, "r")
	RES['out'] = out_stream.read()
	out_stream.close()
	
	ans_stream = open(ans_file, "r")
	RES['ans'] = ans_stream.read()
	ans_stream.close()

	os.remove(cfg_file)
	os.remove(res_file)
	os.remove(in_file)
	os.remove(out_file)
	os.remove(ans_file)
	
	return RES
