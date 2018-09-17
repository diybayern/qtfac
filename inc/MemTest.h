#ifndef _MEM_TEST_H
#define _MEM_TEST_H

#include "Control.h"
#include "FuncBase.h"

class Control;

class MemTest : public FuncBase
{
public:
    MemTest(Control* control);
    static bool compare_men_cap(int mem_cap);
    static bool mem_stability_test();
    static void *test_all(void *arg);
    static void set_mem_test_result(string func,string result,string ui_log);
    void start_test(BaseInfo* baseInfo);

private:
    int _real_mem_cap;
    Control* _control;
    bool _if_test_pass;
    string _test_result;
};

#endif