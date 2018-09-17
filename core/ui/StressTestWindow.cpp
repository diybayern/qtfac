#include "../../inc/ui.h"

StressTestWindow* StressTestWindow::_stress_test_window = NULL;
StressTestWindow* StressTestWindow::get_stress_test_window()
{
    if (!_stress_test_window) {
        _stress_test_window = new StressTestWindow();
    }
    return _stress_test_window;
}

StressTestWindow::StressTestWindow(QWidget *parent)
    : QWidget(parent)
{
    if (this->objectName().isEmpty()) {
        this->setObjectName(QString::fromUtf8("StressTestWindow"));
    }
    st_w = MainTestWindow::get_main_test_window()->get_current_res_w;
    st_h = MainTestWindow::get_main_test_window()->get_current_res_h;
    this->setGeometry(0, 0, st_w, st_h);

    _frame = new QFrame(this);
    _frame->setObjectName(QString::fromUtf8("StressTestFrame"));
    _frame->setGeometry(QRect(0, 0, st_w, st_h));

    // video layout
    _lb_video = new QLabel(_frame);
    _v_a.video_start_x = 0;
    _v_a.video_start_y = st_h/3;
    _v_a.video_w = st_w/2;
    _v_a.video_h = st_h - st_h/3;
    _lb_video->setObjectName(QString::fromUtf8("lb_video"));
    _lb_video->setGeometry(QRect(_v_a.video_start_x, _v_a.video_start_y, _v_a.video_w, _v_a.video_h));
    _lb_video->installEventFilter(this);

    // image layout
    _lb_image_frame = new QLabel(_frame);
    _im_a.image_start_x = 0;
    _im_a.image_start_y = 0;
    _im_a.image_w = st_w;
    _im_a.image_h = st_h/3;
    _lb_image_frame->setObjectName(QString::fromUtf8("lb_image"));
    _lb_image_frame->setGeometry(QRect(_im_a.image_start_x, _im_a.image_start_y, _im_a.image_w, _im_a.image_h));

    for (int i = 0; i < 4; i++) {
        QLabel* label = new QLabel(_lb_image_frame);
        label->setObjectName(QString::fromUtf8("_lb_image"));
        label->setGeometry(QRect(st_w/4*i, 0, st_w/4, st_h/3));
        image_layout_attr tmp;
        tmp.lb_image = label;
        _image_label_list.append(tmp);
    }

    // info layout
    _lb_info = new QLabel(_frame);
    _if_a.info_start_x = st_w/2;
    _if_a.info_start_y = st_h/3;
    _if_a.info_w = st_w/2;
    _if_a.info_h = st_h - st_h/3;
    _lb_info->setObjectName(QString::fromUtf8("lb_info"));
    _lb_info->setGeometry(QRect(_if_a.info_start_x, _if_a.info_start_y, _if_a.info_w, _if_a.info_h));
    _lb_info->installEventFilter(this);

    //connect and start thread
    connect(ImageTestThread::get_image_test_thread(), SIGNAL(sig_send_one_pixmap(QPixmap)), this, SLOT(slot_get_one_pixmap(QPixmap)));
    connect(VideoTestThread::get_video_test_thread(), SIGNAL(sig_send_one_frame(QImage)), this, SLOT(slot_get_one_frame(QImage)));

    ImageTestThread::get_image_test_thread()->start_run();
    VideoTestThread::get_video_test_thread()->start_play();
    connect(this, SIGNAL(sig_finish_video_test_thread()), VideoTestThread::get_video_test_thread(), SLOT(slot_finish_video_test_thread()));
    connect(this, SIGNAL(sig_finish_image_test_thread()), ImageTestThread::get_image_test_thread(), SLOT(slot_finish_image_test_thread()));
    connect(this, SIGNAL(sig_finish_show_stress_window()), MainTestWindow::get_main_test_window(), SLOT(slot_finish_show_stress_window()));
}

StressTestWindow::~StressTestWindow()
{

}

void StressTestWindow::finish_stress_window()
{
    if (NULL != _stress_test_window) {
        this->deleteLater();
        _stress_test_window = NULL;
    }
}

bool StressTestWindow::eventFilter(QObject *obj, QEvent *event)
{

    if (obj == _lb_video && event->type() == QEvent::Paint) {

        QPainter painter(_lb_video);
        painter.setBrush(Qt::black);
        painter.drawRect(0,0,_lb_video->width(),_lb_video->height()); //先画成黑色

        if (mImage.size().width() <= 0)
            return false;

        //将图像按比例缩放成和窗口一样大小
        QImage img = mImage.scaled(_lb_video->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int x = _lb_video->width() - img.width();
        int y = _lb_video->height() - img.height();

        x /= 2;
        y /= 2;

        painter.drawImage(QPoint(x,y),img);

    }

    return QObject::eventFilter(obj,event);
}

void StressTestWindow::start_exec()
{
    show();
}

void StressTestWindow::slot_get_one_frame(QImage img)
{
    mImage = img.copy();
    update();
}

void StressTestWindow::slot_get_one_pixmap(QPixmap pix)
{
    qDebug()<<"slot_get_one_pixmap ......";

    QPixmap fixtmp = pix.scaled(_lb_image_frame->width()/4, _lb_image_frame->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    _set_picture(fixtmp);
}

void StressTestWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        if (NULL != _stress_test_window) {
            emit sig_finish_show_stress_window();
            emit sig_finish_video_test_thread();
            emit sig_finish_image_test_thread();
        }
        //MessageBox(NULL, MessageForm::Message, "GPU测试结果确认", "请确认GPU测试结果是PASS还是FAIL", 0);
    }
}

void StressTestWindow::_set_picture(QPixmap& pix)
{
    if (_image_label_list.isEmpty()) {
        return ;
    }

    for (int i = 0; i < _image_label_list.count(); i++) {
        QLabel* lb = _image_label_list.at(i).lb_image;
        lb->setPixmap(pix);
        lb->update();
    }
}

void StressTestWindow::keyPressEvent(QKeyEvent *event)
{


}

bool start_stress_ui()
{
    StressTestWindow::get_stress_test_window()->start_exec();

    return true;
}



