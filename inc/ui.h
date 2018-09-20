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

#include "VideoTestThread.h"
#include "ImageTestThread.h"
#include "messageform.h"
#include "UiHandle.h"

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
        QObject *button;
        QObject *label;
        int     mode;
};

class MainTestWindow : public QDialog
{
    Q_OBJECT

    public:
        enum {
            MAINFUNC = 0,
            INTERFACE,
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

    private:
        static MainTestWindow* _main_test_window;
        //resolution
        QDesktopWidget *_desktopWidget;

        //main label layout
        QGridLayout *_main_label_layout;
        QVBoxLayout *_main_label_QVLayout;
        QHBoxLayout *_main_label_QHLayout;

        //main test layout
        QGridLayout *_main_test_layout;
        QVBoxLayout *_main_test_QVLayout;
        QHBoxLayout *_main_test_QHLayout;

        //testcount and autoupload layout
        QLineEdit *_test_count_edit;
        QCheckBox *_checkbox_auto_upload_log;
        QLabel *_lab_test_count;
        QHBoxLayout *_test_count_layout;
        QVBoxLayout *_test_count_auto_upload_layout;

        //bottom left layout
        QHBoxLayout *_bottom_left_QHlayout;

        //screen log layout
        QTextEdit *_editInfo;
        QString _editloglist;
        QVBoxLayout *_screenlog_layout;

        //function layout
        QVBoxLayout *_function_layout;

        //main test window layout
        QGridLayout *_main_test_window_layout;
        //others
        QFrame *_spilter_line;

        //lab of complete test or single board test
        QLabel *_lab_complete_or_single_test;

        void _prepare_main_label_layout();
        void _prepare_main_test_layout();
        void _prepare_screen_log_layout();
        void _prepare_test_count_and_upload_layout();
        void _prepare_spilter_line_layout();

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
    static StressTestWindow* get_stress_test_window();
    void finish_stress_window();
    void _set_picture(QPixmap& pix);
    QList<Stress_Test_Info> stress_test_info_list;
    void update_stress_label_value(QString item, QString result);

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
