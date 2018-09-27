#include "../../inc/HddTest.h"
#include "../../inc/fac_log.h"

string hdd_screen_log = "";

HddTest::HddTest(Control* control)
       :_control(control)
{
    
}

void HddTest::set_hdd_test_result(string func,string result,string ui_log)
{
    Control *control = Control::get_control();
    control->set_test_result(func,result,ui_log);	
	if (result == "PASS") {
		control->set_hdd_test_finish();
	}
}

string HddTest::hdd_test_all(string hdd_cap)
{
    execute_command("bash " + HDD_TEST_SCRIPT + " " + hdd_cap);
	if(check_if_hdd_pass() == true)
		return "SUCCESS";
	return "FAIL";
}

bool HddTest::check_if_hdd_pass()
{
    char hdd_status[CMD_BUF_SIZE];
	
	memset(hdd_status, 0, CMD_BUF_SIZE);
	int size = 0;
	if(!get_file_size("/tmp/hdd.status",&size)) {
		LOG_ERROR("/tmp/hdd.status is null\n");
		hdd_screen_log += "/tmp/hdd.status is null\n\n";
		return false;
	}
	if(!read_local_data("/tmp/hdd.status", hdd_status, size)) {
		hdd_screen_log += "read /tmp/hdd.status fail\n\n";
		return false;
	}
    if (!strcmp(delNL(hdd_status), "SUCCESS")) {
        LOG_INFO("HDD Test result: \t%s\n", "PASS");
        return true;
    } else {
        LOG_ERROR("HDD test failed: \t%s\n", hdd_status);
		hdd_screen_log += "HDD test failed:\t\t" + (string)hdd_status + "\n\n";
        return false;
    }
}

void* HddTest::test_all(void *arg)
{
	hdd_screen_log += "===================== hdd test =====================\n";
	BaseInfo* baseInfo = (BaseInfo *)arg;
	string result = hdd_test_all(baseInfo->hdd_cap);
	hdd_screen_log += "hdd test result:\t\t\t" + result + "\n\n";
	if (result == "SUCCESS") {
        set_hdd_test_result("HDD测试","PASS",hdd_screen_log);
	} else {
        set_hdd_test_result("HDD测试","FAIL",hdd_screen_log);
	}
	hdd_screen_log = "";
	return NULL;
}

void HddTest::start_test(BaseInfo* baseInfo)
{
    pthread_t tid;
    pthread_create(&tid,NULL,test_all,baseInfo);
}


