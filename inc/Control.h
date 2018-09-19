#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include "fac_utils.h"
#include "MemTest.h"
#include "FuncTest.h"
#include "SoundTest.h"
#include "NetTest.h"
#include "EdidTest.h"
#include "WifiTest.h"
#include "HddTest.h"



#include "FuncBase.h"
#include "UiHandle.h"

#define   FUNC_TYPE_NUM     (13)

enum FuncType
{
    MEM = 0,
    USB,
    NET,
    EDID,
    CPU,
    HDD,
    FAN,
    WIFI,
    SOUND,
    BRIGHT,
    CAMERA,
    STRESS,
    UPLOAD_LOG,
    NEXT_PROCESS
};

enum TestStep {
    STEP_IDLE = 0,
    STEP_INTERFACE,
    STEP_SOUND,
    STEP_SCREEN,
    STEP_BRIGHTNESS,
    STEP_CAMERA,
    STEP_STRESS,
    STEP_MEM,
    STEP_FAN
};

struct FuncFinishStatus {
    bool mem_finish;
    bool usb_finish;
    bool net_finish;
    bool edid_finish;
    bool hdd_finish;
    bool fan_finish;
    bool wifi_finish;
    bool sound_finish;
    bool screen_finish;
    bool bright_finish;
    bool camera_finish;
};

class Control : public QObject
{
    Q_OBJECT
public:
    //explicit Control(QObject *parent = 0);
    Control();
    void set_test_result(string func,string result,string ui_log);
    static Control* get_control();
    void show_main_test_ui();
    void update_screen_log(string uiLog);
    void upload_mes_log();
    void init_mes_log();
    

    int get_test_step();
    FuncFinishStatus* get_func_finish_status()
    {
        return _funcFinishStatus;
    }

    BaseInfo* get_base_info()
    {
        return _baseInfo;
    }

    HwInfo* get_hw_info()
    {
        return _hwInfo;
    }

    void set_mem_test_finish()
    {
        _funcFinishStatus->mem_finish = true;
    }

    void set_fan_test_finish()
    {
        _funcFinishStatus->fan_finish = true;
    }

    string get_stress_test_stage() {
        return _stress_test_stage;
    }

    bool get_auto_upload_log_status() {
        return _autoUploadLog;
    }

private:
    void init_base_info();
    void init_hw_info();
    void init_func_test();
    void init_fac_config();
    void auto_start_stress_test();

    void ui_init();

private:
    static Control* _control;
    UiHandle* _uiHandle;
    BaseInfo* _baseInfo;
    HwInfo* _hwInfo;
    FuncFinishStatus* _funcFinishStatus;
    int _testStep;
    FuncBase* _funcBase[FUNC_TYPE_NUM];
    FacArg* _facArg;
    string _stress_test_stage;
    bool _autoUploadLog;
    string _mes_log_file;

signals:

public slots:
    void start_mem_test();
    void start_usb_test();
    void start_net_test();
    void start_edid_test();
    void start_cpu_test();
    void start_hdd_test();
    void start_fan_test();
    void start_wifi_test();
    void start_sound_test();
    void start_display_test();
    void start_bright_test();
    void start_camera_test();
    void start_stress_test();
    void start_upload_log();
    void start_next_process();
    
};

#endif // CONTROL_H
