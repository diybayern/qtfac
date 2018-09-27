#ifndef _FUNC_TEST_H
#define _FUNC_TEST_H

#include "fac_utils.h"
#include "Control.h"
#include "FuncBase.h"

#include <dirent.h>
#include <libudev.h>
#include <stdint.h>
#include <stdbool.h>

class Control;

class CpuTest : public FuncBase
{
public:
    CpuTest(Control* control);
    bool is_cpu_test_pass(BaseInfo* baseInfo);
    void start_test(BaseInfo* baseInfo);

private:
    Control* _control;
};


class FanTest : public FuncBase
{
public:
    FanTest(Control* control);
    static string fan_speed_test(string speed);
    static void *test_all(void *arg);
    void start_test(BaseInfo* baseInfo);

private:
    Control* _control;
};

class UsbTest : public FuncBase
{
public:
    UsbTest(Control* control);

    void read_fac_config();
    static bool usb_num_test(string total_num, string num_3);
    
    static bool get_dev_mount_point(struct udev_device* dev, char* dst);
    static struct udev_device* get_child(struct udev* udev, struct udev_device* parent, const char* subsystem);
    static void get_usb_mass_storage(USB_INFO_T* info);
    static bool usb_test_mount(char* block, char* dir);
    static bool usb_test_write(char* dir, char* file_name);
    static bool usb_test_read(char* dir, char* file_name);
    static bool usb_test_umount(char* dir);
    static bool usb_test_write_read(USB_INFO_T* info);
    static bool usb_test_all(int num);
    
    static void *test_all(void *arg);
    void start_test(BaseInfo* baseInfo);

    bool usb_test_read_status();
    static bool usb_test_read_cfg(USB_INFO_T* info);
    static bool usb_test_read_cfg(char* dir);

private:
    Control* _control;

};


class StressTest : public FuncBase
{
public:
    StressTest(Control* control);
    static void *test_all(void *arg);
    void start_test(BaseInfo* baseInfo);

private:
    Control* _control;
};


class NextProcess : public FuncBase
{
public:
    NextProcess(Control* control);
    static void* test_all(void *arg);
    void start_test(BaseInfo* baseInfo);
    static void next_process_handle();
    static bool create_stress_test_lock();

private:
    Control* _control;
    
};

class InterfaceTest : public FuncBase
{
public:
    InterfaceTest(Control* control);
    static void* test_all(void *arg);
    void start_test(BaseInfo* baseInfo);

private:
    Control* _control;
    
};



#endif

