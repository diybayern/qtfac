#include "../../inc/MemTest.h"
#include "../../inc/fac_log.h"
#include "../../inc/fac_utils.h"
#include <math.h>

string mem_screen_log = "";

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
		mem_screen_log += "mem cap is right\n";
		return true;
    } else {
    	mem_screen_log += "ERROR: mem cap should be " + to_string(mem_cap) + "G but current is " + real_mem_cap + "K\n\n";
        return false;
    }
}

bool MemTest::mem_stability_test()
{
    Control *control = Control::get_control();
	string stable_result;
	stable_result = execute_command("sh " + MEM_TEST_SCRIPT);
	LOG_INFO("stable_result is:%s",stable_result.c_str());
	if (stable_result == "SUCCESS") {
		control->update_mes_log("MEM","PASS");
		return true;
	} else {
		control->update_mes_log("MEM","FAIL");
		return false;
	}
}

void MemTest::set_mem_test_result(string func,string result,string ui_log)
{
    Control *control = Control::get_control();
    control->set_test_result(func,result,ui_log);
	if (result == "PASS") {
		control->set_mem_test_finish();
	}
}

void* MemTest::test_all(void *arg)
{
	BaseInfo* baseInfo = (BaseInfo *)arg;
	bool is_pass;
	mem_screen_log += "==================== mem test =====================\n";
	is_pass    = compare_men_cap(get_int_value(baseInfo->mem_cap));
	is_pass   &= mem_stability_test();
	mem_screen_log += execute_command("cat " + MEM_UI_LOG) + "\n\nmem test result:\t\t\t";
	if (is_pass) {
		mem_screen_log += "SUCCESS\n\n";
        set_mem_test_result("内存测试","PASS",mem_screen_log);
	} else {
		mem_screen_log += "FAIL\n\n";
        set_mem_test_result("内存测试","FAIL",mem_screen_log);
	}
	mem_screen_log = "";
	return NULL;
}


void MemTest::start_test(BaseInfo* baseInfo)
{
    pthread_t tid;
    pthread_create(&tid,NULL,test_all,baseInfo);
}


