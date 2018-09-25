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
#include "BrightTest.h"
#include "CameraTest.h"

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
    STEP_DISPLAY,
    STEP_BRIGHTNESS,
    STEP_CAMERA,
    STEP_STRESS,
    STEP_MEM,
    STEP_FAN
};

struct FuncFinishStatus {
    bool interface_finish;
    bool mem_finish;
    bool usb_finish;
    bool cpu_finish;
    bool net_finish;
    bool edid_finish;
    bool hdd_finish;
    bool fan_finish;
    bool wifi_finish;
    bool sound_finish;
    bool display_finish;
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
    void confirm_test_result(string func);
    static Control* get_control();
    void show_main_test_ui();
    void update_screen_log(string uiLog);
    void upload_mes_log();
    void init_mes_log();
    void update_mes_log(char* tag,char* value);
    

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

    void set_interfacetest_finish()
    {
        _funcFinishStatus->interface_finish = true;
    }

    void set_mem_test_finish()
    {
        _funcFinishStatus->mem_finish = true;
    }

    void set_usb_test_finish()
    {
        _funcFinishStatus->usb_finish = true;
    }

    void set_cpu_test_finish()
    {
        _funcFinishStatus->cpu_finish = true;
    }

    void set_edid_test_finish()
    {
        _funcFinishStatus->edid_finish = true;
    }

    void set_net_test_finish()
    {
        _funcFinishStatus->net_finish = true;
    }

    void set_hdd_test_finish()
    {
        _funcFinishStatus->hdd_finish = true;
    }

    void set_fan_test_finish()
    {
        _funcFinishStatus->fan_finish = true;
    }

    void set_wifi_test_finish()
    {
        _funcFinishStatus->wifi_finish = true;
    }

    string get_stress_test_stage() {
        return _stress_test_stage;
    }

    bool get_auto_upload_log_status() {
        return _autoUploadLog;
    }

    int get_interface_test_times() {
        return _interface_test_times;
    }

    void set_interface_test_times(string time) {
        _interface_test_times = get_int_value(time);
    }

    bool get_auto_upload_mes_status() {
        return _auto_upload_mes;
    }

    void set_auto_upload_mes_status(bool status) {
        _auto_upload_mes = status;
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
    int _interface_test_times;
    bool _auto_upload_mes;

signals:

public slots:
    void start_interface_test();
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
