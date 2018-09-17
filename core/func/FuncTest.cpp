#include "../../inc/FuncTest.h"
#include "../../inc/fac_log.h"


CpuTest::CpuTest(Control* control)
       :_control(control)
{
    
}

void CpuTest::set_cpu_test_result(string func,string result,string ui_log)
{
    Control *control = Control::get_control();
    control->set_test_result(func,result,ui_log);
}

bool CpuTest::is_cpu_test_pass(BaseInfo* baseInfo)
{
	string hw_cpu_type = _control->get_hw_info()->cpu_type;
	string base_cpu_type = baseInfo->cpu_type;
	string::size_type idx;
	idx = hw_cpu_type.find(base_cpu_type);
    if (idx != string::npos) {
        return true;
    } else {
        return false;
    }
}

void CpuTest::start_test(BaseInfo* baseInfo)
{
    if (is_cpu_test_pass(baseInfo)) {
		set_cpu_test_result("CPU测试","PASS",_control->get_hw_info()->cpu_type);
	} else {
		set_cpu_test_result("CPU测试","FAIL",_control->get_hw_info()->cpu_type);
	}	
}


FanTest::FanTest(Control* control)
       :_control(control)
{
    
}

void FanTest::set_fan_test_result(string func,string result,string ui_log)
{
    Control *control = Control::get_control();
    control->set_test_result(func,result,ui_log);
}

string FanTest::fan_speed_test(string speed)
{
    string fan_result = execute_command("bash " + FACTORY_PATH + "fan_test.sh " + speed);
    return fan_result;
}

void* FanTest::test_all(void *arg)
{
	BaseInfo* baseInfo = (BaseInfo *)arg;
	string result = fan_speed_test(baseInfo->fan_speed);

	if (result == "SUCCESS") {
        set_fan_test_result("FAN测试","PASS",result);
	} else {
        set_fan_test_result("FAN测试","FAIL",result);
	}
    Control::get_control()->set_fan_test_finish();
	return NULL;
}

void FanTest::start_test(BaseInfo* baseInfo)
{
    pthread_t tid;
    pthread_create(&tid,NULL,test_all,baseInfo);
}

StressTest::StressTest(Control* control)
       :_control(control)
{
    
}


void* StressTest::test_all(void *arg)
{
	BaseInfo* baseInfo = (BaseInfo *)arg;
	Control *control = Control::get_control();
	bool is_lock_exist = check_file_exit(STRESS_LOCK_FILE.c_str());
	if (is_lock_exist) {
		if (control->get_stress_test_stage() == "whole") {
			LOG_INFO("whole");
	    } else if (control->get_stress_test_stage() == "PCBA") {
	    	LOG_INFO("PCBA");
	    } else if (control->get_stress_test_stage() == "next") {
	    	LOG_INFO("next");
	    } else {
	    	LOG_INFO("error");
	    }
	}
	remove_local_file(STRESS_LOCK_FILE.c_str());
	
	
	return NULL;
}

void StressTest::start_test(BaseInfo* baseInfo)
{
    pthread_t tid;
    pthread_create(&tid,NULL,test_all,baseInfo);
}


