#include "inc/UiHandle.h"

UiHandle::UiHandle()
{
    connect(this, SIGNAL(to_draw_main_test_window()), MainTestWindow::get_main_test_window(), SLOT(draw_main_test_window()));
    connect(this, SIGNAL(need_to_show_main_test()), MainTestWindow::get_main_test_window(), SLOT(show_main_test_window()));
    connect(this, SIGNAL(print_result(QString,QString)), MainTestWindow::get_main_test_window(), SLOT(get_result_string(QString,QString)));
    connect(this, SIGNAL(to_show_stress_test_window()), MainTestWindow::get_main_test_window(), SLOT(show_stress_test_window()));
    connect(this, SIGNAL(to_show_display_test_window()), MainTestWindow::get_main_test_window(), SLOT(show_display_test_window()));
    connect(this, SIGNAL(need_to_update_screen_log(QString)), MainTestWindow::get_main_test_window(), SLOT(update_screen_log(QString)));
    connect(this, SIGNAL(to_update_stress_label_value(QString,QString)), MainTestWindow::get_main_test_window(), SLOT(update_stress_label_value(QString,QString)));
	connect(this, SIGNAL(to_confirm_test_result_dialog(QString)), MainTestWindow::get_main_test_window(), SLOT(confirm_test_result_dialog(QString)));  
    connect(this, SIGNAL(to_start_audio_progress_dialog()), MainTestWindow::get_main_test_window(), SLOT(start_audio_progress_dialog()));
    connect(this, SIGNAL(to_confirm_test_result_warning(QString)), MainTestWindow::get_main_test_window(), SLOT(confim_test_result_warning(QString)));
}

UiHandle::~UiHandle()
{
    if (_ui_handle != NULL)
    {
        delete _ui_handle;
        _ui_handle = NULL;
    }
}

UiHandle* UiHandle::_ui_handle = NULL;
UiHandle* UiHandle::get_uihandle()
{
    if (!_ui_handle) {
        _ui_handle = new UiHandle();
    }
    return _ui_handle;
}

void UiHandle::add_interface_test_button(string item)
{
    MainTestWindow::get_main_test_window()->add_interface_test_button(QString::fromStdString(item));
}

void UiHandle::add_main_label(string item, string result)
{
    MainTestWindow::get_main_test_window()->add_main_label(QString::fromStdString(item), QString::fromStdString(result));
}

void UiHandle::add_main_test_button(string item)
{
    MainTestWindow::get_main_test_window()->add_main_test_button(QString::fromStdString(item));
}

void UiHandle::add_stress_test_label(string item)
{
    MainTestWindow::get_main_test_window()->add_stress_test_label(QString::fromStdString(item));
}

void UiHandle::confirm_test_result_dialog(string title)
{
	emit to_confirm_test_result_dialog(QString::fromStdString(title));
}

void UiHandle::confirm_test_result_warning(string title)
{
    emit to_confirm_test_result_warning(QString::fromStdString(title));
}

void UiHandle::set_test_result(string item, string result)
{
    emit print_result(QString::fromStdString(item), QString::fromStdString(result));
}

void UiHandle::start_audio_progress_dialog()
{
    emit to_start_audio_progress_dialog();
}

void UiHandle::sync_main_test_ui()
{
    emit to_draw_main_test_window();
}

void UiHandle::to_show_main_test_ui()
{
    emit need_to_show_main_test();
}

void UiHandle::show_display_ui()
{
    emit to_show_display_test_window();
}

void UiHandle::show_stress_test_ui()
{
    emit to_show_stress_test_window();
}

void UiHandle::add_complete_or_single_test_label(string config)
{
    MainTestWindow::get_main_test_window()->add_complete_or_single_test_label(QString::fromStdString(config));
}

void UiHandle::update_screen_log(string textInfo)
{
    emit need_to_update_screen_log(QString::fromStdString(textInfo));
}

void UiHandle::update_stress_label_value(string item, string result)
{
    emit to_update_stress_label_value(QString::fromStdString(item), QString::fromStdString(result));
}

void UiHandle::show_sn_mac_message_box()
{
    MainTestWindow::get_main_test_window()->show_sn_mac_message_box();
}

QObject* UiHandle::get_qobject(string name)
{
    QObject *obj = new QObject;
    for (int i = 0 ; i < MainTestWindow::get_main_test_window()->itemlist.count(); i++)
    {
        string objname = (MainTestWindow::get_main_test_window()->itemlist.at(i).name).toStdString();
        if (objname.compare(name) == 0)
        {
            obj = MainTestWindow::get_main_test_window()->itemlist.at(i).button;
            QPushButton *p_b = qobject_cast<QPushButton*>(obj);
            return p_b;
        }
    }
    return obj;
}
