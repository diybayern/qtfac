#include "../../inc/Control.h"
#include "../../inc/fac_log.h"


Control::Control():QObject()
{
    _funcBase[MEM]          = new MemTest(this);
    _funcBase[USB]          = new UsbTest(this);
    _funcBase[CPU]          = new CpuTest(this);
    _funcBase[EDID]         = new EdidTest(this);
    _funcBase[NET]          = new NetTest(this);
    _funcBase[HDD]          = new HddTest(this);
    _funcBase[FAN]          = new FanTest(this);
    _funcBase[WIFI]         = new WifiTest(this);
    _funcBase[SOUND]        = new SoundTest(this);
    _funcBase[BRIGHT]       = new BrightTest(this);
    _funcBase[CAMERA]       = new CameraTest(this);
    _funcBase[STRESS]       = new StressTest(this);
    _funcBase[UPLOAD_LOG]   = new StressTest(this);
    _funcBase[NEXT_PROCESS] = new NextProcess(this);
    
    _uiHandle               = UiHandle::get_uihandle();

    
    _baseInfo               = new BaseInfo;
    _hwInfo                 = new HwInfo;
    _facArg                 = new FacArg;

    _funcFinishStatus                   = new FuncFinishStatus;
    _funcFinishStatus->interface_finish = false;
    _funcFinishStatus->mem_finish       = false;
    _funcFinishStatus->usb_finish       = false;
    _funcFinishStatus->cpu_finish       = false;
    _funcFinishStatus->net_finish       = false;
    _funcFinishStatus->edid_finish      = false;
    _funcFinishStatus->hdd_finish       = false;
    _funcFinishStatus->fan_finish       = false;
    _funcFinishStatus->wifi_finish      = false;
    _funcFinishStatus->sound_finish     = false;
    _funcFinishStatus->display_finish   = false;
    _funcFinishStatus->bright_finish    = false;
    _funcFinishStatus->camera_finish    = false;
    _funcFinishStatus->stress_finish    = false;

    _interfaceSelectStatus              = new InterfaceSelectStatus;
    _interfaceSelectStatus->mem_select  = true;
    _interfaceSelectStatus->usb_select  = true;
    _interfaceSelectStatus->cpu_select  = true;
    _interfaceSelectStatus->net_select  = true;
    _interfaceSelectStatus->edid_select = true;
    _interfaceSelectStatus->hdd_select  = true;
    _interfaceSelectStatus->fan_select  = true;
    _interfaceSelectStatus->wifi_select = true;

    _testStep = STEP_IDLE;
    _stress_test_stage = "";
    _autoUploadLog = true;
    _mes_log_file = "";
    _interface_test_times = 1;
    
    init_base_info();
    init_hw_info();
    ui_init();
    init_func_test();
    init_fac_config();
    init_mes_log();

    auto_start_stress_test();
}

Control* Control::_control = NULL;
Control* Control::get_control()
{
    if (!_control) {
        _control = new Control();
    }
    return _control;
}

void Control::init_base_info()
{
    string dmi = "CPU:i5-6200U;MEM:8;USB:1/2;SSD:256;EMMC:0;HDD:500;WIFI:1;FAN:3800;VGA:1;HDMI:1;LCD:1920*1080;BRT:6;CAM:1";
    get_baseinfo(_baseInfo,dmi);
}

void Control::init_hw_info()
{
    get_hwinfo(_hwInfo);
}

void Control::ui_init()
{
    _uiHandle->add_main_label("产品型号:", "Rain410");
    _uiHandle->add_main_label("硬件版本:", "V1.00");
    _uiHandle->add_main_label("SN序列号:", "12345678");
    _uiHandle->add_main_label("MAC地址:", "00:11:22:33:44:55:66");
    _uiHandle->add_main_label("CPU型号:", "i3-6100U");
    
    _uiHandle->add_main_test_button("接口测试");
    
    _uiHandle->add_interface_test_button("内存测试");
    _uiHandle->add_interface_test_button("USB测试");
    _uiHandle->add_interface_test_button("网口测试");
    _uiHandle->add_interface_test_button("EDID测试");
    _uiHandle->add_interface_test_button("CPU测试");
    
    if (_baseInfo->hdd_cap != "0" || _baseInfo->hdd_cap != "") {
        _uiHandle->add_interface_test_button("HDD测试");
    } else {
        _funcFinishStatus->hdd_finish = true;
    }
    
    if (_baseInfo->fan_speed != "0" || _baseInfo->fan_speed != "") {
        _uiHandle->add_interface_test_button("FAN测试");
    } else {
        _funcFinishStatus->fan_finish = true;
    }

    if (_baseInfo->wifi_exist != "0" || _baseInfo->wifi_exist != "") {
        _uiHandle->add_interface_test_button("WIFI测试");
    } else {
        _funcFinishStatus->wifi_finish = true;
    }

    _uiHandle->add_main_test_button("音频测试");
    _uiHandle->add_main_test_button("显示测试");
    
    if (_baseInfo->bright_level != "0" || _baseInfo->bright_level != ""){
        _uiHandle->add_main_test_button("亮度测试");
    }
    
    if (_baseInfo->camera_exist != "0" || _baseInfo->camera_exist != "") {
        _uiHandle->add_main_test_button("摄像头测试");
    }
    
    _uiHandle->add_main_test_button("拷机测试");
    _uiHandle->add_main_test_button("上传日志");
    _uiHandle->add_main_test_button("下道工序");
    
    _uiHandle->add_complete_or_single_test_label("整机测试");
    
    _uiHandle->sync_main_test_ui();
    
    _uiHandle->add_stress_test_label("运行时间");
    _uiHandle->add_stress_test_label("CPU温度");
    _uiHandle->add_stress_test_label("编码状态");
    _uiHandle->add_stress_test_label("解码状态");
    _uiHandle->add_stress_test_label("产品型号");
    _uiHandle->add_stress_test_label("硬件版本");
    _uiHandle->add_stress_test_label("SN序列号");
    _uiHandle->add_stress_test_label("MAC地址");
    _uiHandle->add_stress_test_label("CPU频率");
    _uiHandle->add_stress_test_label("Mem");
    _uiHandle->add_stress_test_label("Cpu");
    
    connect(_uiHandle->get_qobject("接口测试"), SIGNAL(clicked()), this, SLOT(start_interface_test()));
    connect(_uiHandle->get_qobject("内存测试"), SIGNAL(clicked()), this, SLOT(start_mem_test()));
    connect(_uiHandle->get_qobject("USB测试"), SIGNAL(clicked()), this, SLOT(start_usb_test()));
    connect(_uiHandle->get_qobject("网口测试"), SIGNAL(clicked()), this, SLOT(start_net_test()));
    connect(_uiHandle->get_qobject("EDID测试"), SIGNAL(clicked()), this, SLOT(start_edid_test()));
    connect(_uiHandle->get_qobject("CPU测试"), SIGNAL(clicked()), this, SLOT(start_cpu_test()));
    if (_baseInfo->hdd_cap != "0" || _baseInfo->hdd_cap != "") {
        connect(_uiHandle->get_qobject("HDD测试"), SIGNAL(clicked()), this, SLOT(start_hdd_test()));
    }
    
    if (_baseInfo->fan_speed != "0" || _baseInfo->fan_speed != "") {
        connect(_uiHandle->get_qobject("FAN测试"), SIGNAL(clicked()), this, SLOT(start_fan_test()));
    }

    if (_baseInfo->wifi_exist != "0" || _baseInfo->wifi_exist != "") {
        connect(_uiHandle->get_qobject("WIFI测试"), SIGNAL(clicked()), this, SLOT(start_wifi_test()));
    }

    connect(_uiHandle->get_qobject("音频测试"), SIGNAL(clicked()), this, SLOT(start_sound_test()));
    connect(_uiHandle->get_qobject("显示测试"), SIGNAL(clicked()), this, SLOT(start_display_test()));

    if (_baseInfo->bright_level != "0" || _baseInfo->bright_level != ""){
        connect(_uiHandle->get_qobject("亮度测试"), SIGNAL(clicked()), this, SLOT(start_bright_test()));
    }
    if (_baseInfo->camera_exist != "0" || _baseInfo->camera_exist != "") {
        connect(_uiHandle->get_qobject("摄像头测试"), SIGNAL(clicked()), this, SLOT(start_camera_test()));
    }
    connect(_uiHandle->get_qobject("拷机测试"), SIGNAL(clicked()), this, SLOT(start_stress_test()));
    connect(_uiHandle->get_qobject("上传日志"), SIGNAL(clicked()), this, SLOT(start_upload_log()));
    connect(_uiHandle->get_qobject("下道工序"), SIGNAL(clicked()), this, SLOT(start_next_process()));
    connect(_uiHandle, SIGNAL(to_show_test_confirm_dialog(string)), this, SLOT(show_test_confirm_dialog(string)));
    connect(_uiHandle, SIGNAL(sig_ui_handled_test_result(string, string)), this, SLOT(set_test_result_pass_or_fail(string, string)));
    connect(_uiHandle, SIGNAL(sig_ui_check_state_changed(string, bool)), this, SLOT(set_interface_select_status(string, bool)));
}

void Control::show_test_confirm_dialog(string item)
{
    if (item.compare("拷机测试") == 0) {
        stress_test_window_quit_status = false;
    }
    _uiHandle->confirm_test_result_dialog(item);
}

void Control::init_func_test()
{
    cout << "init func" << endl;
}

void Control::init_fac_config()
{
    int ret = get_fac_config_from_conf(FAC_CONFIG_FILE, _facArg);
    if (ret == NO_FTP_PATH) {
        LOG_INFO("NO_FTP_PATH");
    }
    if (ret == NO_JOB_NUMBER) {
        LOG_INFO("NO_JOB_NUMBER");
    }
    //_uiHandle->add_main_label("NO FTP INFO","NO INFO");
}

void Control::start_interface_test()
{
    LOG_INFO("start interface test");
    _testStep = STEP_INTERFACE;
    if (_interfaceSelectStatus->mem_select) {
        _funcBase[MEM]->start_test(_baseInfo);
    }
    if (_interfaceSelectStatus->usb_select) {
        _funcBase[USB]->start_test(_baseInfo);
    }
    if (_interfaceSelectStatus->mem_select) {
        _funcBase[MEM]->start_test(_baseInfo);
    }
    if (_interfaceSelectStatus->net_select) {
        _funcBase[NET]->start_test(_baseInfo);
    }
    if (_interfaceSelectStatus->edid_select) {
        _funcBase[EDID]->start_test(_baseInfo);
    }
    if (_interfaceSelectStatus->cpu_select) {
        _funcBase[CPU]->start_test(_baseInfo);
    }

    if (_baseInfo->hdd_cap != "0" || _baseInfo->hdd_cap != "") {
        if (_interfaceSelectStatus->hdd_select) {
            _funcBase[HDD]->start_test(_baseInfo);
        }
    }
    if (_baseInfo->fan_speed != "0" || _baseInfo->fan_speed!= "") {
        if (_interfaceSelectStatus->fan_select) {
            _funcBase[FAN]->start_test(_baseInfo);
        }
    }
    if (_baseInfo->wifi_exist!= "0" || _baseInfo->wifi_exist!= "") {
        if (_interfaceSelectStatus->wifi_select) {
            _funcBase[WIFI]->start_test(_baseInfo);
        }
    }
}


void Control::start_mem_test()
{
    _testStep = STEP_INTERFACE;
    _funcBase[MEM]->start_test(_baseInfo);
    LOG_INFO("start mem test");
}

void Control::start_cpu_test()
{
    _testStep = STEP_INTERFACE;
    _funcBase[CPU]->start_test(_baseInfo);
    LOG_INFO("start cpu test");
}


void Control::start_usb_test()
{
    _testStep = STEP_INTERFACE;
    _funcBase[USB]->start_test(_baseInfo);
    LOG_INFO("start usb test");
}

void Control::start_net_test()
{
    _testStep = STEP_INTERFACE;
    _funcBase[NET]->start_test(_baseInfo);
    LOG_INFO("start net test");
}

void Control::start_edid_test()
{
    _testStep = STEP_INTERFACE;
    _funcBase[EDID]->start_test(_baseInfo);
    LOG_INFO("start edid test");
}

void Control::start_hdd_test()
{    
    _testStep = STEP_INTERFACE;
    _funcBase[HDD]->start_test(_baseInfo);   
    LOG_INFO("start hdd test");
}


void Control::start_fan_test()
{
    _testStep = STEP_INTERFACE;
    _funcBase[FAN]->start_test(_baseInfo);
    LOG_INFO("start fan test");
}

void Control::start_wifi_test()
{   
    _testStep = STEP_INTERFACE;
    _funcBase[WIFI]->start_test(_baseInfo);    
    LOG_INFO("start wifi test");
}


void Control::start_sound_test()
{
    _testStep = STEP_SOUND;
    _uiHandle->start_audio_progress_dialog();    
    _funcBase[SOUND]->start_test(_baseInfo);
    LOG_INFO("start sound test");
}

void Control::start_display_test()
{
    _testStep = STEP_DISPLAY;
    _uiHandle->show_display_ui();
    cout << "2" << endl;
}

void Control::start_bright_test()
{
    _testStep = STEP_BRIGHTNESS;
    _funcBase[BRIGHT]->start_test(_baseInfo);
    LOG_INFO("start sound test");
}

void Control::start_camera_test()
{
    _testStep = STEP_CAMERA;
    _funcBase[CAMERA]->start_test(_baseInfo);
    LOG_INFO("start sound test");
}

void Control::start_stress_test()
{
    _funcBase[STRESS]->start_test(_baseInfo);
    LOG_INFO("start_stress_test");
}

void Control::start_next_process()
{
    _funcBase[NEXT_PROCESS]->start_test(_baseInfo);
    LOG_INFO("start next process");
}

void Control::start_upload_log()
{
    upload_mes_log();
}


void Control::set_test_result(string func,string result,string ui_log)

{
    _uiHandle->set_test_result(func, result);
    _uiHandle->update_screen_log(ui_log);
}

void Control::confirm_test_result(string func)
{
    LOG_INFO("confirm_test_result");
    _uiHandle->confirm_test_result_dialog(func);
}

void Control::show_main_test_ui()
{
    _uiHandle->to_show_main_test_ui();
}

int Control::get_test_step()
{
    return _testStep;
}

void Control::init_mes_log()
{
    int i=0;
    int j=0;
    char date[64] = { 0, };
    char new_mac_name[128];
    struct tm *timenow = NULL;
    time_t timep;
 
    while(_hwInfo->mac[i] != '\0'){
        if(_hwInfo->mac[i] != ':'){
            new_mac_name[j] = _hwInfo->mac[i];
            j++;
        }
        i++;
    }

    new_mac_name[j] = '\0';
    char* mac_capital = (char*)malloc(128);
    char* sn_capital = (char*)malloc(128);
    
    mac_capital = lower_to_capital(new_mac_name, mac_capital);
    sn_capital = lower_to_capital(_hwInfo->sn.c_str(), sn_capital);
    
    if (access(MES_FILE, F_OK) == 0) {
        remove(MES_FILE);
    }

    sprintf(_facArg->ftp_dest_path,"%s%s.txt",_facArg->ftp_dest_path,mac_capital);
    LOG_INFO("_facArg->ftp_dest_path:%s",_facArg->ftp_dest_path);
    
    time(&timep);
    timenow = localtime(&timep);
    strftime(date, 64, "%Y%m%d-%H:%M:%S", timenow);



    LOG_MES("---------------------Product infomation-----------------------\n");
    LOG_MES("Model: \t%s\n", _hwInfo->product_name.c_str());
    LOG_MES("SN: \t%s\n", sn_capital);
    LOG_MES("MAC: \t%s\n", mac_capital);
    LOG_MES("DATE: \t%s\n", date);
    LOG_MES("OPERATION: \t%s\n", _facArg->ftp_job_number);
    LOG_MES("---------------------Simple test result-----------------------\n");
    LOG_MES("MEMORY:    NULL\n");
    LOG_MES("USB:       NULL\n");
    LOG_MES("NET:       NULL\n");
    LOG_MES("EDID:      NULL\n");
    if (_baseInfo->hdd_cap != "0" || _baseInfo->hdd_cap != "") {
        LOG_MES("HDD:       NULL\n");
    }
    if (_baseInfo->fan_speed != "0" || _baseInfo->fan_speed != "") {
        LOG_MES("FAN:       NULL\n");
    }
    if (_baseInfo->wifi_exist != "0" || _baseInfo->wifi_exist != "") {
        LOG_MES("WIFI:      NULL\n");
    }
    LOG_MES("AUDIO:     NULL\n");
    LOG_MES("DISPLAY:   NULL\n");
    if (_baseInfo->bright_level != "0" || _baseInfo->bright_level != "") {
        LOG_MES("BRIGHTNESS:NULL\n");
    }
    if (_baseInfo->camera_exist != "0" || _baseInfo->camera_exist != "") {
        LOG_MES("CAMERA:    NULL\n");
    }
    LOG_MES("STRESS:    NULL\n");
    LOG_MES("---------------------Detail test result-----------------------\n");

    free(mac_capital);
    free(sn_capital);
}

void Control::update_mes_log(char* tag,char* value)
{
    FILE* fp;
    char line[200];
    char* sp = NULL;
    int file_size;
    int first=1;
 
    bzero(line,sizeof(line));
 
    if ((fp = fopen(MES_FILE,"r"))==NULL) {
        LOG_ERROR("open %s failed",MES_FILE);
    }
 
    fseek(fp,0,SEEK_END);
    file_size = ftell(fp);
    fseek(fp,0,SEEK_SET);
    char* buf = (char*)malloc(file_size+128);
    bzero(buf,file_size);
 
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (first &&((sp = strstr(line, tag)) != NULL)) {
            char value_temp[128];
            bzero(value_temp,128);
            sprintf(value_temp,"%s\n",value);
            memcpy(sp+11,value_temp,strlen(value_temp)+1);//修改标签内容，加TAG_OFFSET是为了对齐值
            first = 0;
        }
        strcat(buf, line);
    }
 
    fclose(fp);
    fp = fopen(MES_FILE, "w");
    int len = strlen(buf);
    fwrite(buf, 1, len, fp);
    fclose(fp);

    if (buf != NULL) {
        free(buf);
    }
}


void Control::upload_mes_log() {
    if (combine_fac_log_to_mes(MES_FILE)) {
        char* response = ftp_send_file(MES_FILE,_facArg);
        response = response_to_chinese(response);
        LOG_INFO("upload %s",response);
		if (!strcmp(response,"上传成功")) {
			_uiHandle->confirm_test_result_warning("upload success");
		} else {
			_uiHandle->confirm_test_result_warning("upload fail");
		}
    } else {
        LOG_INFO("combine mes fail");
		
    }
    _testStep = STEP_IDLE;
}

void Control::update_screen_log(string uiLog)
{
    _uiHandle->update_screen_log(uiLog);
}

int Control::get_screen_height()
{
    return _uiHandle->get_screen_height();
}

int Control::get_screen_width()
{
    return _uiHandle->get_screen_width();
}

void Control::auto_start_stress_test()
{
    if (check_file_exit(STRESS_LOCK_FILE.c_str())) {
        LOG_INFO("auto start stress test");
        _stress_test_stage = execute_command("cat " + STRESS_LOCK_FILE);
        LOG_INFO("stress stage:%s",_stress_test_stage.c_str());
        _funcBase[STRESS]->start_test(_baseInfo);
    }
}

void Control::set_interface_select_status(string func, bool state) {
    if (func == "内存测试") {
        _interfaceSelectStatus->mem_select  = state;
        if (_interfaceSelectStatus->mem_select) {
            _funcFinishStatus->mem_finish = false;
        } else {
            _funcFinishStatus->mem_finish = true;
        }
    }
    if (func ==  "USB测试") {
        _interfaceSelectStatus->usb_select  = state;
        if (_interfaceSelectStatus->usb_select) {
            _funcFinishStatus->usb_finish = false;
        } else {
            _funcFinishStatus->usb_finish = true;
        }
    }
    if (func == "网口测试") {
        _interfaceSelectStatus->net_select  = state;
        if (_interfaceSelectStatus->net_select) {
            _funcFinishStatus->net_finish = false;
        } else {
            _funcFinishStatus->net_finish = true;
        }
    }
    if (func == "EDID测试") {
        _interfaceSelectStatus->edid_select = state;
        if (_interfaceSelectStatus->edid_select) {
            _funcFinishStatus->edid_finish = false;
        } else {
            _funcFinishStatus->edid_finish = true;
        }
    }
    if (func == "CPU测试") {
        _interfaceSelectStatus->cpu_select  = state;
        if (_interfaceSelectStatus->cpu_select) {
            _funcFinishStatus->cpu_finish = false;
        } else {
            _funcFinishStatus->cpu_finish = true;
        }
    }
    if (func == "HDD测试") {
        _interfaceSelectStatus->hdd_select  = state;
        if (_interfaceSelectStatus->mem_select) {
            _funcFinishStatus->hdd_finish = false;
        } else {
            _funcFinishStatus->hdd_finish = true;
        }
    }
    if (func == "FAN测试") {
        _interfaceSelectStatus->fan_select  = state;
        if (_interfaceSelectStatus->fan_select) {
            _funcFinishStatus->fan_finish = false;
        } else {
            _funcFinishStatus->fan_finish = true;
        }
    }
    if (func == "WIFI测试") {
        _interfaceSelectStatus->wifi_select = state;
        if (_interfaceSelectStatus->wifi_select) {
            _funcFinishStatus->wifi_finish = false;
        } else {
            _funcFinishStatus->wifi_finish = true;
        }
    }
}

void Control::set_test_result_pass_or_fail(string func, string result)
{
    if (result == "PASS") {
        if (func == "音频测试") {
            _funcFinishStatus->sound_finish= true;
        }
        if (func == "显示测试") {
            _funcFinishStatus->display_finish= true;
        }
        if (func == "亮度测试") {
            _funcFinishStatus->bright_finish= true;
        }
        if (func == "摄像头测试") {
            _funcFinishStatus->camera_finish= true;
        }
        if (func == "拷机测试") {
            _funcFinishStatus->stress_finish= true;
        }
    } else {

        if (func == "音频测试") {
            _funcFinishStatus->sound_finish= false;
        }
        if (func == "显示测试") {
            _funcFinishStatus->display_finish= false;
        }
        if (func == "亮度测试") {
            _funcFinishStatus->bright_finish= false;
        }
        if (func == "摄像头测试") {
            _funcFinishStatus->camera_finish= false;
        }
        if (func == "拷机测试") {
            _funcFinishStatus->stress_finish= false;
        }
    }
    _uiHandle->set_test_result(func, result);
}

bool Control::is_stress_test_window_quit_safely()
{
    return stress_test_window_quit_status;
}


