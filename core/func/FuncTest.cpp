#include "../../inc/FuncTest.h"
#include "../../inc/fac_log.h"

string cpu_screen_log = "";
string fan_screen_log = "";

pthread_mutex_t g_next_process_lock;

CpuTest::CpuTest(Control* control)
       :_control(control)
{
    
}

void CpuTest::set_cpu_test_result(string func,string result,string ui_log)
{
    Control *control = Control::get_control();
    control->set_test_result(func,result,ui_log);
	if (result == "PASS") {
		control->set_cpu_test_finish();
	}
}

bool CpuTest::is_cpu_test_pass(BaseInfo* baseInfo)
{
	string hw_cpu_type = _control->get_hw_info()->cpu_type;
	string base_cpu_type = baseInfo->cpu_type;	
	string::size_type idx;
	idx = hw_cpu_type.find(base_cpu_type);
    if (idx != string::npos) {
		cpu_screen_log += "cpu type is right\n";
        return true;
    } else {    
		cpu_screen_log += "cpu type should be\t\t" + base_cpu_type + "\nbut current is\t\t" + hw_cpu_type + "\n\n";
        return false;
    }
}

void CpuTest::start_test(BaseInfo* baseInfo)
{
	cpu_screen_log += "==================== cpu test ====================\n";
    if (is_cpu_test_pass(baseInfo)) {
		cpu_screen_log += "cpu test result:\t\t\tSUCCESS\n\n";
		set_cpu_test_result("CPU测试","PASS",cpu_screen_log);
	} else {	
		cpu_screen_log += "cpu test result:\t\t\tFAIL\n\n";
		set_cpu_test_result("CPU测试","FAIL",cpu_screen_log);
	}	
	cpu_screen_log = "";
}


FanTest::FanTest(Control* control)
       :_control(control)
{
    
}

void FanTest::set_fan_test_result(string func,string result,string ui_log)
{
    Control *control = Control::get_control();
    control->set_test_result(func,result,ui_log);
	if (result == "PASS") {
		control->set_fan_test_finish();
	}
}

string FanTest::fan_speed_test(string speed)
{
    string fan_result = execute_command("bash " + FACTORY_PATH + "fan_test.sh " + speed);
    return fan_result;
}

void* FanTest::test_all(void *arg)
{
	fan_screen_log += "==================== fan test ====================\n";
	BaseInfo* baseInfo = (BaseInfo *)arg;
	string result = fan_speed_test(baseInfo->fan_speed);
	if (result == "SUCCESS") {		
		fan_screen_log += "fan test result:\t\t\t" + result + "\n\n";
        set_fan_test_result("FAN测试","PASS",fan_screen_log);
	} else {
		fan_screen_log += "fan speed should be\t" + baseInfo->fan_speed + "but current is\t" + result + "\n\n";		
		fan_screen_log += "fan test result:\t\t\tFAIL\n\n";
        set_fan_test_result("FAN测试","FAIL",fan_screen_log);
	}
	fan_screen_log = "";
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
    TimeInfo init_time = {0,0,0,0};
    TimeInfo tmp_dst = {0,0,0,0};
    char datebuf[CMD_BUF_SIZE] = {0};

	if (check_file_exit(STRESS_LOCK_FILE.c_str())) {
		string stress_stage = control->get_stress_test_stage();		
		remove_local_file(STRESS_LOCK_FILE.c_str());
		if (stress_stage == "whole" || stress_stage == "PCBA") {
			LOG_INFO("last stress test exit error\n");			
	    } else if (stress_stage == "next") {
			LOG_INFO("next process -> stress test\n");
	    } else {
			LOG_ERROR("stress test lock file wrong\n");
	    }
		remove_local_file(STRESS_LOCK_FILE.c_str());
	}
	
	if (check_file_exit(WHOLE_TEST_FILE)) {
		write_local_data(STRESS_LOCK_FILE.c_str(),"w+",WHOLE_LOCK,sizeof(WHOLE_LOCK));
	} else {
		write_local_data(STRESS_LOCK_FILE.c_str(),"w+",PCBA_LOCK,sizeof(PCBA_LOCK));
	}

    get_current_open_time(&init_time);

    while(true)
    {
        if (!Control::get_control()->is_stress_test_window_quit_safely()) {
            break;
        }

        diff_running_time(&tmp_dst, &init_time);
        snprintf(datebuf, CMD_BUF_SIZE, "%d天%d时%d分%d秒", tmp_dst.day, tmp_dst.hour, tmp_dst.minute, tmp_dst.second);
        UiHandle::get_uihandle()->update_stress_label_value("运行时间", datebuf);
        get_current_open_time(&tmp_dst);



        sleep(1);
    }
	
	return NULL;
}

void StressTest::start_test(BaseInfo* baseInfo)
{
    pthread_t tid;
    pthread_create(&tid,NULL,test_all,baseInfo);
}


NextProcess::NextProcess(Control* control)
       :_control(control)
{
    pthread_mutex_init(&g_next_process_lock, NULL);
}

bool NextProcess::create_stress_test_lock() 
{
    LOG_INFO("start creating stress lock\n");
    write_local_data(STRESS_LOCK_FILE.c_str(), "w+", (char*)NEXT_LOCK, sizeof(NEXT_LOCK));
    
/*    if (system("sync") < 0) {
        LOG_ERROR("system sync error\n");
        return false;
    }*/
	
    if (check_file_exit(STRESS_LOCK_FILE.c_str())) {
        LOG_INFO("create stress test lock success\n");
        return true;
    } else {
        LOG_ERROR("create stress test lock failed\n");
        return false;
    }
}

void NextProcess::next_process_handle() 
{
    int next_process_f = -1;

    pthread_detach(pthread_self());    
    if (pthread_mutex_trylock(&g_next_process_lock)) {
        LOG_ERROR("g_next_process_lock has been locked\n");
        return;
    }

    //g_idle_add(gtk_win_next_process, "正在处理，请等待...");
    next_process_f = system("bash /etc/diskstatus_mgr.bash --product-detach");
    usleep(1000000);

    LOG_INFO("bache check result value is %d\n",WEXITSTATUS(next_process_f));
    pthread_mutex_unlock(&g_next_process_lock);
	
    if (WEXITSTATUS(next_process_f) == 0) {
        if (!create_stress_test_lock()) {
            LOG_ERROR("create stress test lock fail!\n");			
           // g_idle_add(gtk_win_next_process,"EMMC异常，无法关机！\n");
        } else if (system("shutdown -h now") < 0) {
            LOG_ERROR("shutdown cmd run error\n");			
           // g_idle_add(gtk_win_next_process,"终端异常，无法关机！\n");
        }
    } else if (WEXITSTATUS(next_process_f) > 0) {
        LOG_ERROR("The disk is abnormal and cannot enter the next process.\n");		
		//g_idle_add(gtk_win_next_process,"终端异常，无法关机！\n");
    }
 
    return;
}

void* NextProcess::test_all(void *arg)
{
	next_process_handle();
	return NULL;
}

void NextProcess::start_test(BaseInfo* baseInfo)
{
    pthread_t tid;
    pthread_create(&tid,NULL,test_all,baseInfo);
}


