#ifndef UI_H
#define UI_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QDesktopWidget>
#include <QDebug>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QProgressBar>
#include <QProgressDialog>

#include "VideoTestThread.h"
#include "ImageTestThread.h"
#include "messageform.h"
#include "UiHandle.h"
/*
extern "C" {
    #include <glib.h>
    #include <gst/video/videooverlay.h>
    #include <gst/gst.h>
    #include <string.h>
}


typedef struct _CustomData {
        GstElement *playbin;
        QWidget    *window;
} CustomData;*/

class InterfaceTestItem
{
    public:
        QString itemname;
};

class MainLabelItem
{
    public:
        QString itemname;
        QString result;
};

class MainTestItem
{
    public:
        QString itemname;
};

class StressTestItem
{
    public:
        QString itemname;
};

class ItemCheck
{
    public:
        QString name;
        QCheckBox *checkbox;
        QObject *button;
        QObject *label;
};

class MyEventLoop : public QObject
{
    Q_OBJECT
    public:
        MyEventLoop(QObject *parent = 0);
        virtual ~MyEventLoop();

        int exec(QEventLoop::ProcessEventsFlags = QEventLoop::AllEvents,bool hassignal=false);
        void exit(int returnCode = 0);
        bool isRunning() const;
    signals:
        void enterEventLoop();
        void exitEventloop();
    public slots:
        void quit();
    private:
        bool    runflag;

};

class CustomProgressDialog: public QDialog
{
    Q_OBJECT

    public:
        explicit CustomProgressDialog(QWidget *parent = 0);
        ~CustomProgressDialog();
        void startExec();
        QProgressBar    *progressbar;
        QLabel          *lb_title;
    private:
        QFrame          *frame;

};

class MainTestWindow : public QDialog
{
    Q_OBJECT

    public:
        enum {
            MAINFUNC = 0,
            INTERFACE,
        };
        enum {
            RECORD = 0,
            PLAY,
            PLAY_END,
        };

        explicit MainTestWindow(QWidget *parent = 0);
        ~MainTestWindow();
        void setupUI();
        void add_interface_test_button(QString item);
        void add_main_label(QString item, QString result);
        void add_main_test_button(QString item);
        void add_stress_test_label(QString item);
        void add_complete_or_single_test_label(QString config);
        void show_sn_mac_message_box();
        QList<ItemCheck> itemlist;
        void _get_sn_num();
        void _get_mac_addr();
        static MainTestWindow* get_main_test_window();
        int get_current_res_h;
        int get_current_res_w;
        QList<StressTestItem> stress_test_item_list;
        QTimer  updatetimer;

    private:
        static MainTestWindow* _main_test_window;
        //resolution
        QDesktopWidget *_desktopWidget;

        //main label layout
        QGridLayout *_grid_main_label_layout;
        QHBoxLayout *_hbox_main_label_layout;

        //main test layout
        QGridLayout *_grid_main_test_layout;
        QVBoxLayout *_vbox_main_test_layout;
        QHBoxLayout *_hbox_main_test_layout;

        //testcount and autoupload layout
        QLineEdit *_lineedit_test_count;
        QCheckBox *_checkbox_auto_upload_log;
        QLabel *_lab_test_count;
        QHBoxLayout *_hbox_test_count_layout;
        QHBoxLayout *_hbox_checkbox_auto_upload_log;
        QVBoxLayout *_vbox_test_count_auto_upload_layout;

        //bottom left layout
        QHBoxLayout *_hbox_bottom_left_layout;

        //screen log layout
        QTextEdit *_editInfo;
        QString _editloglist;
        QVBoxLayout *_vbox_screenlog_layout;

        //function layout
        QVBoxLayout *_vbox_function_layout;

        //main test window layout
        QGridLayout *_grid_main_test_window_layout;
        //others
        QFrame *_spilter_line;

        //lab of complete test or single board test
        QLabel *_lab_complete_or_single_test;

        void _create_main_label_layout();
        void _create_main_test_layout();
        void _create_screen_log_layout();
        void _create_test_count_and_upload_layout();
        void _create_spilter_line_layout();

        //buffer
        QList<InterfaceTestItem> _interface_test_list;
        QList<MainLabelItem> _main_label_item_list;
        QList<MainTestItem> _main_test_item_list;

        void _insert_item_record(ItemCheck &record);
        ItemCheck iteminfo;

        //label color
        QPalette _get_label_color(QString result);

        //sn mac check
        bool _is_complete_test = true;
        QString _get_current_configs() {return _is_complete_test ?"SN" : "MAC";}
        QString _local_sn_num;
        QString _local_mac_addr;

        CustomProgressDialog *_custom_progress_dialog;
        int _status;

        MyEventLoop eventloop;

    signals:
        void to_quit_test_window(QString item);

    public slots:
        void draw_main_test_window();
        void show_main_test_window();
        void resume_message_box();
        void get_result_string(QString func, QString result);
        void show_stress_test_window();
        void show_display_test_window();
        void slot_finish_show_stress_window();
        void slot_finish_show_display_window();
        void update_screen_log(QString info);
        void update_stress_label_value(QString item, QString result);
        void confirm_test_result_dialog(QString title);
        void confim_test_result_warning(QString title);
        void start_audio_progress_dialog();

    private slots:
        void on_state_changed(int state);
        void _record_play_audio();
};



class video_attr {
public:
    int video_start_x;
    int video_start_y;
    int video_w;
    int video_h;
};

class image_attr {
public:
    int image_start_x;
    int image_start_y;
    int image_w;
    int image_h;
};

class info_attr {
public:
    int info_start_x;
    int info_start_y;
    int info_w;
    int info_h;
};

class image_layout_attr {
public:
    QLabel * lb_image;
};

class Stress_Test_Info
{
    public:
        QString name;
        QLabel *label;
};


class StressTestWindow : public QWidget
{
    Q_OBJECT
public:
    explicit StressTestWindow(QWidget *parent = 0);
    ~StressTestWindow();
    void start_exec();
    static StressTestWindow* g_get_stress_test_window();
    static StressTestWindow* get_stress_test_window();
    void finish_stress_window();
    void _set_picture(QPixmap& pix);
    QList<Stress_Test_Info> stress_test_info_list;
    void update_stress_label_value(QString item, QString result);
    //void mediaPlay();

protected:
    void mousePressEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void slot_get_one_frame(QImage img);
    void slot_get_one_pixmap(QPixmap pix);

private:
    static StressTestWindow* _stress_test_window;
    QFrame *_frame;
    QLabel *_lb_video;
    QLabel *_lb_image_frame;
    QFrame *_lb_info;
    QList<image_layout_attr> _image_label_list;
    video_attr _v_a;
    image_attr _im_a;
    info_attr  _if_a;
    QImage mImage;
    QPixmap _m_pixmap;
    QGroupBox *_group_box;
    QGridLayout *_grid_box;
    int st_w;
    int st_h;
	//CustomData del_data;
    MyEventLoop eventloop;

signals:
    void sig_finish_show_stress_window();
    void sig_finish_video_test_thread();
    void sig_finish_image_test_thread();

};

class DisplayTestWindow : public QWidget
{
    Q_OBJECT
public:
    enum {
        RGB = 1,
        BLACK,
        WHITE
    };
    static DisplayTestWindow* g_get_display_test_window();
    explicit DisplayTestWindow(QWidget *parent = 0);
    ~DisplayTestWindow();
    static DisplayTestWindow* get_display_test_window();
    void start_exec();
    void finish_display_window();

protected:
    void mousePressEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent* event);

private:
    static DisplayTestWindow* _display_test_window;
    int _state;
    int _st_w;
    int _st_h;

signals:
    void sig_finish_show_display_window();
};


extern void start_display_test_ui();
extern bool start_stress_ui();
extern MainTestWindow *gstr_maintestwindow;
#endif //UI_H
