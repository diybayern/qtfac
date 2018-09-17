#include "../../inc/MemTest.h"
#include "../../inc/fac_log.h"
#include "../../inc/fac_utils.h"
#include <math.h>


MemTest::MemTest(Control* control)
       :_control(control)
{
    _test_result="";
}

bool MemTest::compare_men_cap(int mem_cap)
{
	float mem_cap_min = mem_cap * 1024 * 0.9;
	float mem_cap_max = mem_cap * 1024;
    string real_mem_cap = execute_command("free -m | awk '/Mem/ {print $2}'");
    if (get_int_value(real_mem_cap) > mem_cap_min  && get_int_value(real_mem_cap) < mem_cap_max){
        return true;
    } else {
        return false;
    }
}

bool MemTest::mem_stability_test()
{
	string stable_result;
	stable_result = execute_command("sh " + MEM_TEST_SCRIPT);
	LOG_INFO("stable_result is:%s",stable_result.c_str());
	if (stable_result == "SUCCESS") {
		return true;
	} else {
		return false;
	}
}

void MemTest::set_mem_test_result(string func,string result,string ui_log)
{
    Control *control = Control::get_control();
    control->set_test_result(func,result,ui_log);
}

void* MemTest::test_all(void *arg)
{
	BaseInfo* baseInfo = (BaseInfo *)arg;
	bool is_pass;
	string log;
	is_pass    = compare_men_cap(get_int_value(baseInfo->mem_cap));
	is_pass   &= mem_stability_test();
	log        = execute_command("cat " + MEM_UI_LOG);
	if (is_pass) {
        set_mem_test_result("内存测试","PASS",log);
	} else {
        set_mem_test_result("内存测试","FAIL",log);
	}
	Control::get_control()->set_mem_test_finish();
	return NULL;
}


void MemTest::start_test(BaseInfo* baseInfo)
{
    pthread_t tid;
    pthread_create(&tid,NULL,test_all,baseInfo);
}


