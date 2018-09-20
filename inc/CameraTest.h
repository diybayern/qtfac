#ifndef _CAMERA_TEST_H
#define _CAMERA_TEST_H

#include "fac_utils.h"
#include "Control.h"
#include "FuncBase.h"

class Control;

class CameraTest : public FuncBase
{
public:
    CameraTest(Control* control);
    static bool camera_test_all();
    static void *test_all(void *arg);
    void start_test(BaseInfo* baseInfo);

private:
    Control* _control;
    static void start_camera_xawtv();
//    static void close_xawtv_welcome_window();
    static unsigned long get_window_id(const char *winid_file);
    static bool check_if_xawtv_started();
    
};

#endif

