#include "../../inc/ui.h"
#include "../../inc/messageform.h"

MainTestWindow::MainTestWindow(QWidget *parent)
        : QDialog(parent)
{
    _desktopWidget = QApplication::desktop();

}

MainTestWindow::~MainTestWindow()
{
    if (_main_test_window)
    {
        delete _main_test_window;
        _main_test_window = NULL;
    }
}

MainTestWindow* MainTestWindow::_main_test_window = NULL;
MainTestWindow* MainTestWindow::get_main_test_window()
{
    if (!_main_test_window) {
        _main_test_window = new MainTestWindow();
    }
    return _main_test_window;
}

void MainTestWindow::add_interface_test_button(QString item)
{
    InterfaceTestItem listitem;
    listitem.itemname = item;
    _interface_test_list.append(listitem);
}

void MainTestWindow::add_main_label(QString item, QString result)
{
    MainLabelItem listitem;
    listitem.itemname = item;
    listitem.result = result;
    _main_label_item_list.append(listitem);
}

void MainTestWindow::add_main_test_button(QString item)
{
    MainTestItem listitem;
    listitem.itemname = item;
    _main_test_item_list.append(listitem);
}

void MainTestWindow::add_stress_test_label(QString item)
{
    StressTestItem listitem;
    listitem.itemname = item;
    stress_test_item_list.append(listitem);
}

void MainTestWindow::add_complete_or_single_test_label(QString config)
{
    QPalette pa;
    QFont ft;
    ft.setPointSize(18);
    pa.setColor(QPalette::WindowText,Qt::red);

    _lab_complete_or_single_test = new QLabel(config);
    _lab_complete_or_single_test->setFont(ft);
    _lab_complete_or_single_test->setPalette(pa);
}

void MainTestWindow::confirm_test_result_dialog(QString title)
{

    //MessageBox(NULL, MessageForm::SNMAC, title, "SN序列号", 0);
}

void MainTestWindow::show_sn_mac_message_box()
{
    this->_get_sn_num();
    this->_get_mac_addr();

    if (_is_complete_test)
    {
        MessageBox(NULL, MessageForm::SNMAC, _get_current_configs() + "测试", _get_current_configs(), 0);
    }
    else
    {
        MessageBox(NULL, MessageForm::SNMAC, _get_current_configs() + "测试", _get_current_configs(), 0);
    }


}

void MainTestWindow::get_result_string(QString func, QString result)
{
    if (itemlist.isEmpty())
    {
        return ;
    }

    QObject *obj;
    for (int i = 0 ; i < itemlist.count(); i++)
    {
        QString objname = itemlist.at(i).name;
        if (objname == func)
        {
            obj = itemlist.at(i).label;
            QLabel *b = qobject_cast<QLabel*>(obj);
            b->setPalette(_get_label_color(result));
            b->setText(result);
            break;
        }
    }
}

QPalette MainTestWindow::_get_label_color(QString result)
{
    QPalette pa;
    if (result.compare("PASS") == 0 || result.compare("pass") == 0)
    {
        pa.setColor(QPalette::WindowText,Qt::green);
    }
    else
    {
        pa.setColor(QPalette::WindowText,Qt::red);
    }
    return pa;
}

void MainTestWindow::update_screen_log(QString textInfo)
{
    if (_editInfo == NULL)
    {
        return ;
    }

    _editloglist.append(textInfo);
    _editInfo->setText(_editloglist);
}

void MainTestWindow::setupUI()
{

    _bottom_left_QHlayout = new QHBoxLayout;
    _bottom_left_QHlayout->addLayout(_main_test_QHLayout);
    _bottom_left_QHlayout->addLayout(_test_count_auto_upload_layout);

    _function_layout = new QVBoxLayout;
    _function_layout->addLayout(_main_label_QHLayout);
    _function_layout->addWidget(_spilter_line);
    _function_layout->addLayout(_bottom_left_QHlayout);

    _main_test_window_layout = new QGridLayout(this);
    _main_test_window_layout->setMargin(40);
    _main_test_window_layout->addLayout(_function_layout, 0, 0);
    _main_test_window_layout->addLayout(_screenlog_layout, 0, 1);
    _main_test_window_layout->setColumnStretch(0, 3);
    _main_test_window_layout->setColumnStretch(1, 4);
}

void MainTestWindow::_prepare_main_label_layout()
{
    int i;

    if (_main_label_item_list.isEmpty())
    {
        return ;
    }

    _main_label_layout = new QGridLayout;
    _main_label_QVLayout = new QVBoxLayout;
    _main_label_QHLayout = new QHBoxLayout;
    for (i = 0 ; i < _main_label_item_list.count(); i++)
    {
        QString label = _main_label_item_list.at(i).itemname;
        QString result = _main_label_item_list.at(i).result;
        _main_label_layout->addWidget(new QLabel(label), i, 0);
        _main_label_layout->addWidget(new QLabel(result), i, 1);
    }

    if (_lab_complete_or_single_test != NULL)
    {
        _main_label_layout->addWidget(_lab_complete_or_single_test, i , 7);
    }

    _main_label_QVLayout->addLayout(_main_label_layout);
    _main_label_QVLayout->addStretch();
    _main_label_QHLayout->addLayout(_main_label_QVLayout);
    _main_label_QHLayout->addStretch();
}

void MainTestWindow::_prepare_main_test_layout()
{
    if (_main_test_item_list.isEmpty())
    {
        return ;
    }

    _main_test_layout = new QGridLayout;
    _main_test_QVLayout = new QVBoxLayout;
    _main_test_QHLayout = new QHBoxLayout;

    if (_interface_test_list.isEmpty())
    {
        for (int i = 0 ; i < _main_test_item_list.count(); i++)
        {
            QString name = _main_test_item_list.at(i).itemname;
            QPushButton *button = new QPushButton(name);
            QLabel *label = new QLabel;
            _main_test_layout->addWidget(button, i, 0);
            _main_test_layout->addWidget(label, i, 1);

            iteminfo.name = name;
            iteminfo.button = button;
            iteminfo.label = label;
            iteminfo.mode = MAINFUNC;
            _insert_item_record(iteminfo);
        }

    } else {
         QPushButton *button;
         QLabel *label;

         for (int i = 0 ; i < _main_test_item_list.count(); i++)
         {
             if (i == 0)
             {
                button = new QPushButton(_main_test_item_list.at(0).itemname);
                label = new QLabel;
                _main_test_layout->addWidget(button, 0, 0);
                _main_test_layout->addWidget(label, 0, 1);
                iteminfo.name = _main_test_item_list.at(0).itemname;
                iteminfo.button = button;
                iteminfo.label = label;
                iteminfo.mode = MAINFUNC;
                _insert_item_record(iteminfo);

                for (int j = 0 ; j < _interface_test_list.count(); j++)
                {
                    QCheckBox*r_button = new QCheckBox(_interface_test_list.at(j).itemname);
                    r_button->setObjectName(_interface_test_list.at(j).itemname);
                    label = new QLabel;
                    _main_test_layout->addWidget(r_button, j+1, 1);
                    _main_test_layout->addWidget(label, j+1, 2);
                    iteminfo.name = _interface_test_list.at(j).itemname;
                    iteminfo.button = r_button;
                    iteminfo.label = label;
                    iteminfo.mode = INTERFACE;
                    _insert_item_record(iteminfo);
                }

             } else {
                 button = new QPushButton(_main_test_item_list.at(i).itemname);
                 label = new QLabel;
                 _main_test_layout->addWidget(button, i + _interface_test_list.count(), 0);
                 _main_test_layout->addWidget(label, i + _interface_test_list.count(), 1);
                 iteminfo.name = _main_test_item_list.at(i).itemname;
                 iteminfo.button = button;
                 iteminfo.label = label;
                 iteminfo.mode = MAINFUNC;
                 _insert_item_record(iteminfo);
             }
         }
    }

    _main_test_QVLayout->addLayout(_main_test_layout);
    _main_test_QVLayout->addStretch();
    _main_test_QHLayout->addLayout(_main_test_QVLayout);
    _main_test_QHLayout->addStretch();
}

void MainTestWindow::_prepare_screen_log_layout()
{
    _screenlog_layout = new QVBoxLayout;
    _editInfo = new QTextEdit;
    _editInfo->setReadOnly(true);
    _editInfo->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    _screenlog_layout->addWidget(_editInfo);
}

void MainTestWindow::_prepare_test_count_and_upload_layout()
{
    _lab_test_count = new QLabel(tr("测试次数:"));
    _test_count_edit = new QLineEdit;
    _test_count_layout = new QHBoxLayout;
    _test_count_auto_upload_layout = new QVBoxLayout;
    _test_count_layout->addWidget(_lab_test_count);
    _test_count_layout->addWidget(_test_count_edit);
    _checkbox_auto_upload_log = new QCheckBox(tr("自动上传"));
    _test_count_auto_upload_layout->addLayout(_test_count_layout);
    _test_count_auto_upload_layout->addStretch();
    _test_count_auto_upload_layout->addWidget(_checkbox_auto_upload_log);
}

void MainTestWindow::_prepare_spilter_line_layout()
{
    _spilter_line = new QFrame;
    _spilter_line->setFixedSize(_desktopWidget->width()*3/7, 3);
    _spilter_line->setFrameShape(QFrame::HLine);
    _spilter_line->setFrameShadow(QFrame::Sunken);
}

void MainTestWindow::_insert_item_record(ItemCheck &record)
{
    itemlist.append(record);
}

void MainTestWindow::draw_main_test_window()
{
    _prepare_screen_log_layout();
    _prepare_test_count_and_upload_layout();
    _prepare_main_test_layout();
    _prepare_main_label_layout();
    _prepare_spilter_line_layout();
}

void MainTestWindow::show_main_test_window()
{
    setupUI();
    this->setWindowTitle("工程测试程序");
    get_current_res_h = _desktopWidget->height();
    get_current_res_w = _desktopWidget->width();
    this->resize(_desktopWidget->width(), _desktopWidget->height());
    this->show();
}

void MainTestWindow::_get_sn_num()
{
    if (_main_label_item_list.isEmpty())
    {
        return ;
    }

    for (int i = 0 ; i < _main_label_item_list.count(); i++)
    {
        QString str = _main_label_item_list.at(i).itemname;
        if (str.contains("SN"))
        {
            _local_sn_num = _main_label_item_list.at(i).result;
            break;
        }
    }
    return ;
}

void MainTestWindow::_get_mac_addr()
{
    if (_main_label_item_list.isEmpty())
    {
        return ;
    }

    for (int i = 0 ; i < _main_label_item_list.count(); i++)
    {
        QString str = _main_label_item_list.at(i).itemname;
        if (str.contains("MAC"))
        {
            _local_mac_addr = _main_label_item_list.at(i).result;
            break;
        }
    }
    return ;
}

void MainTestWindow::resume_message_box()
{
    qDebug()<<"message = " << g_sn_mac_message;
    qDebug()<<"localsn = " << _local_sn_num;
    qDebug()<<"localmac = " << _local_mac_addr;
    if (_is_complete_test) {

        if (g_sn_mac_message.compare(_local_sn_num) == 0) {
            MessageBox(NULL, MessageForm::Message, "扫描成功", "SN比对成功", 1000);
        } else {
            MessageBox(NULL, MessageForm::Error, "警告", "SN比对失败,请重试！", 0);
        }

    } else {
        if (g_sn_mac_message.compare(_local_mac_addr) == 0) {
            MessageBox(NULL, MessageForm::Message, "扫描成功", "MAC比对成功", 1000);
        } else {
            MessageBox(NULL, MessageForm::Error, "警告", "MAC比对失败,请重试！", 0);
        }
    }
}

void MainTestWindow::show_stress_test_window()
{
    start_stress_ui();
}

void MainTestWindow::show_display_test_window()
{
    start_display_test_ui();
}

void MainTestWindow::slot_finish_show_stress_window()
{
    StressTestWindow::get_stress_test_window()->finish_stress_window();
}

void MainTestWindow::slot_finish_show_display_window()
{
    DisplayTestWindow::get_display_test_window()->finish_display_window();
}

void MainTestWindow::update_stress_label_value(QString item, QString result)
{
    StressTestWindow::get_stress_test_window()->update_stress_label_value(item, result);
}
