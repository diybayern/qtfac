#include "../../inc/ui.h"

DisplayTestWindow* DisplayTestWindow::_display_test_window = NULL;
DisplayTestWindow* DisplayTestWindow::get_display_test_window()
{
    if (!_display_test_window) {
        _display_test_window = new DisplayTestWindow();
    }
    return _display_test_window;
}

DisplayTestWindow* DisplayTestWindow::g_get_display_test_window()
{
    return _display_test_window;
}

DisplayTestWindow::DisplayTestWindow(QWidget *parent)
    : QWidget(parent)
{
    if (this->objectName().isEmpty()) {
        this->setObjectName(QString::fromUtf8("DisplayTestWindow"));
        this->setFocus();
    }
    _st_w = MainTestWindow::get_main_test_window()->get_current_res_w;
    _st_h = MainTestWindow::get_main_test_window()->get_current_res_h;
    this->setGeometry(0, 0, _st_w, _st_h);
    this->_state = 0;

    connect(this, SIGNAL(sig_finish_show_display_window()), MainTestWindow::get_main_test_window(), SLOT(slot_finish_show_display_window()));
}

DisplayTestWindow::~DisplayTestWindow()
{

}

void DisplayTestWindow::finish_display_window()
{
    if (NULL != _display_test_window) {
        hide();
        this->deleteLater();
        _display_test_window = NULL;
    }
}

void DisplayTestWindow::start_exec()
{
    show();
}

void DisplayTestWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        switch(_state) {
           case 0:{
                _state = RGB;
                update();
                break;
           }
           case 1: {
                _state = BLACK;
                update();
                break;
           }
           case 2: {
                if (NULL != _display_test_window) {
                    emit sig_finish_show_display_window();
                }
                break;
           }
           default:
           break;
        }

    } else if (event->button() == Qt::RightButton) {
        if (NULL != _display_test_window) {
            emit sig_finish_show_display_window();
        }
    }

}

void DisplayTestWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        switch(_state) {
            case 0:{
                 _state = RGB;
                 update();
                 break;
            }
            case 1: {
                 _state = BLACK;
                 update();
                 break;
            }
            case 2: {
                 if (NULL != _display_test_window) {
                     emit sig_finish_show_display_window();
                 }
                 break;
            }
            default:
            break;
        }

    } else if (event->key() == Qt::Key_Escape) {
        if (NULL != _display_test_window) {
            emit sig_finish_show_display_window();
        }
    }

}

void DisplayTestWindow::paintEvent(QPaintEvent* event)
{
    if (event->type() == QEvent::Paint) {
        QPainter painter(this);
        switch(_state) {
          case 0:{
              painter.setBrush(QColor(255, 0, 0)); //red
              painter.drawRect(0, 0, _st_w/3, _st_h);
              painter.setBrush(QColor(0, 255, 0));//green
              painter.drawRect(_st_w/3, 0, _st_w/3, _st_h);
              painter.setBrush(QColor(0, 0, 255));//blue
              painter.drawRect(_st_w/3*2, 0, _st_w/3, _st_h);
              break;
          }
          case 1:{
              painter.setBrush(QColor(0, 0, 0)); //black
              painter.drawRect(0, 0, _st_w, _st_h);
              break;
          }
          case 2:{
              painter.setBrush(QColor(255, 255, 255)); //white
              painter.drawRect(0, 0, _st_w, _st_h);
              break;
          }
        }
    }
}

void start_display_test_ui()
{
    DisplayTestWindow::get_display_test_window()->start_exec();
}
