#ifndef UIHANDLE_H
#define UIHANDLE_H
#include "ui.h"
#include <iostream>
using namespace std;

class UiHandle: public QObject
{
    Q_OBJECT

    public:
        UiHandle();
        ~UiHandle();
        static UiHandle* get_uihandle();

    public:
        void add_interface_test_button(string item);
        void add_main_label(string item, string result);
        void add_main_test_button(string item);
        void add_stress_test_label(string item);
        void confirm_test_result_dialog(string title);
        void set_test_result(string item, string result);
        void show_display_ui();
        void show_stress_test_ui();
        void update_screen_log(string textInfo);
        void update_stress_label_value(string item, string result);
        void sync_main_test_ui();
        void to_show_main_test_ui();
        void add_complete_or_single_test_label(string config);
        void show_sn_mac_message_box();
        QObject *get_qobject(string name);
       // QCheckBox *get_qobject_interface(string name);
    private:
        static UiHandle* _ui_handle;

    signals:
        void to_draw_main_test_window();
        void need_to_show_main_test();
        void print_result(QString func, QString result);
        void to_update_stress_label_value(QString item, QString result);
        void to_show_stress_test_window();
        void to_show_display_test_window();
        void need_to_update_screen_log(QString info);
	void to_confirm_test_result_dialog(QString title);
};


#endif//UIHANDLE_H
