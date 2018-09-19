#ifndef _HDD_TEST_H
#define _HDD_TEST_H

#include "fac_utils.h"
#include "Control.h"
#include "FuncBase.h"

class Control;

class HddTest : public FuncBase
{
public:
    HddTest(Control* control);
    static string hdd_test_all(string hdd_cap);
    static bool check_if_hdd_pass();
    static void set_hdd_test_result(string func,string result,string ui_log);
    static void *test_all(void *arg);
    void start_test(BaseInfo* baseInfo);

private:
    Control* _control;
};

#endif

