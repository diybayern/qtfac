#include "inc/messageform.h"

QPointer<MessageForm> g_form = NULL;
QString g_sn_mac_message;

MessageForm::MessageForm(QWidget *parent, const int mode, const int timeout) : QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_DeleteOnClose, false);
    //setAttribute(Qt::WA_TranslucentBackground,true);
    effect = new QGraphicsDropShadowEffect;
    effect->setBlurRadius(8);
    effect->setColor(Qt::gray);
    effect->setOffset(-5, 5);

    _main_w = MainTestWindow::get_main_test_window()->get_current_res_w;
    _main_h = MainTestWindow::get_main_test_window()->get_current_res_h;

    if (this->objectName().isEmpty()) {
        this->setObjectName(QString::fromUtf8("MessageFormClass"));
    }
    QPalette pa;
    pa.setColor(QPalette::Background,QColor(0xD6, 0xD6, 0xD6));
    this->setPalette(pa);

    if (mode == Message) {
        _dialog_w = 900;
        _dialog_h = 220;
        _lb_text_w = 500;

    } else if (mode == Warnning || mode == Success) {
        _dialog_w = 400;
        _dialog_h = 220;
        _lb_text_w = 200;
    }

    resize(_dialog_w, _dialog_h);
    frame = new QFrame(this);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setGeometry(QRect(0, 0, _dialog_w, _dialog_h));
    frame->setGraphicsEffect(effect);

    lb_title = new QLabel(frame);
    lb_title->setObjectName(QString::fromUtf8("lb_title"));
    lb_title->setGeometry(QRect(0, 0, _dialog_w, 20));
    QFont lb_font;
    lb_font.setPointSize(8);

    QFont font;
    font.setPointSize(18);
    font.setWeight(QFont::Bold);
    lb_title->setFont(lb_font);
    lb_title->setStyleSheet("background-color: rgb(0, 255, 255);");
    lb_title->setAlignment(Qt::AlignCenter);
    groupBox = new QGroupBox(frame);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setStyleSheet("QGroupBox{border:none}");

    this->mode = mode;

    if (mode != NOICON) {
        groupBox->setGeometry(QRect(0, 50, _dialog_w, 80));
        groupBox->setFont(font);

        lb_icon = new QLabel(groupBox);
        lb_icon->setObjectName(QString::fromUtf8("lb_icon"));
        lb_icon->setGeometry(QRect(20, 0, 80, 80));

        if (mode == Message)
        {
            lb_icon->setPixmap(QPixmap("./img/message.png"));
        }
        else if (mode == Warnning || mode == Success)
        {
            lb_icon->setPixmap(QPixmap("./img/warning.png"));
        }
        else if (mode == Error)
        {
            lb_icon->setPixmap(QPixmap("./img/error.png"));
        }
    }

    if (mode != SNMAC) {
        lb_text = new QLabel(groupBox);
        lb_text->setObjectName(QString::fromUtf8("lb_text"));
        lb_text->setGeometry(QRect(100, 0, _lb_text_w, 80));
        lb_text->setFont(font);
        lb_text->setAlignment(Qt::AlignLeading|Qt::AlignCenter);
        lb_text->setWordWrap(true);
    } else {
        ly_snmac = new QHBoxLayout(groupBox);
        ly_snmac->setObjectName(QString::fromUtf8("ly_snmac"));
        lb_snmac = new QLabel;
        lb_snmac->setObjectName(QString::fromUtf8("lb_snmac"));
        lb_snmac->setFont(font);
        le_snmac = new QLineEdit;
        le_snmac->setObjectName(QString::fromUtf8("le_snmac"));
        ly_snmac->addWidget(lb_snmac);
        ly_snmac->addWidget(le_snmac);
        le_snmac->setFocusPolicy(Qt::StrongFocus);
        le_snmac->installEventFilter(this);
    }

    if (mode == Message || mode == Error || mode == Warnning || mode == NOICON || mode == SNMAC || mode == Success)
    {
        if (mode == Warnning || mode == Success) {
            lb_title->setStyleSheet("background-color: rgb(255, 255, 0);");
        } else if (mode == Error) {
            lb_title->setStyleSheet("background-color: rgb(255, 0, 0);");        
        } else if (mode == Message || mode == SNMAC) {
            lb_title->setStyleSheet("background-color: rgb(174, 238, 238);");
        }

        if (mode == Message) {
            bt_ok = new QPushButton(frame);
            bt_ok->setObjectName(QString::fromUtf8("bt_ok"));
            if (mode == NOICON) {
                bt_ok->setGeometry(QRect(600, 150, 100, 40));
            } else {
                bt_ok->setGeometry(QRect(600, 150, 100, 40));
            }

            bt_ok->setFont(font);
            bt_ok->setText(tr("PASS"));

            bt_fail = new QPushButton(frame);
            bt_fail->setObjectName(QString::fromUtf8("bt_fail"));
            if (mode == NOICON) {
                bt_fail->setGeometry(QRect(750, 150, 100, 40));
            } else {
                bt_fail->setGeometry(QRect(750, 150, 100, 40));
            }

            bt_fail->setFont(font);
            bt_fail->setText(tr("FAIL"));
            connect(bt_ok, SIGNAL(clicked()), this, SLOT(proButtonOK()));
            connect(bt_fail, SIGNAL(clicked()), this, SLOT(proButtonFail()));
            connect(this, SIGNAL(sig_handled_test_result(QString, QString)), UiHandle::get_uihandle(), SLOT(slot_handled_test_result(QString, QString)));
        } else if (mode == SNMAC) {
            bt_snmac = new QPushButton(frame);
            bt_snmac->setObjectName(QString::fromUtf8("bt_snmac"));
            bt_snmac->setGeometry(QRect(470, 230, 100, 40));
            bt_snmac->setFont(font);
            bt_snmac->setText(tr("CANCEL"));
            connect(bt_snmac, SIGNAL(clicked()), this, SLOT(proButtonCancel()));
        } else if (mode == Warnning) {
            bt_cancle = new QPushButton(frame);
            bt_cancle->setObjectName(QString::fromUtf8("bt_cancel"));
            if (mode == NOICON) {
                bt_cancle->setGeometry(QRect(250, 150, 100, 40));
            } else {
                bt_cancle->setGeometry(QRect(250, 150, 100, 40));
            }

            bt_cancle->setFont(font);
            bt_cancle->setText(tr("退出"));
            connect(bt_cancle, SIGNAL(clicked()), this, SLOT(proButtonQuit()));
        } else if (mode == Success) {
            bt_confirm = new QPushButton(frame);
            bt_confirm->setObjectName(QString::fromUtf8("bt_confirm"));
            bt_confirm->setGeometry(QRect(250, 150, 100, 40));
            bt_confirm->setFont(font);
            if (MainTestWindow::get_main_test_window()->is_complete_test) {
                bt_confirm->setText(tr("关机"));
            } else {
                bt_confirm->setText(tr("下道工序"));
            }
            connect(bt_confirm, SIGNAL(clicked()), this, SLOT(proButtonConfirm()));
        }
    }
    this->timeout = timeout;
    timerId = 0;
}

MessageForm::~MessageForm()
{

}

bool MessageForm::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == le_snmac && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
        {
            QString str = le_snmac->text();
            g_sn_mac_message = str;
            qDebug()<<"message form m_sn_mac_msg = " << g_sn_mac_message;
            QTimer::singleShot(1000, MainTestWindow::get_main_test_window(), SLOT(resume_message_box()));
        }
    }

    return QObject::eventFilter(obj, event);
}

void MessageForm::timerEvent(QTimerEvent *evt)
{
    if (evt->timerId() == timerId)
    {
        killTimer(timerId);
        accept();
    }
}

void MessageForm::proButtonOK()
{
    emit sig_handled_test_result(_m_test_item, "PASS");
    accept();
}

void MessageForm::proButtonFail()
{
    emit sig_handled_test_result(_m_test_item, "FAIL");
    reject();
}

void MessageForm::proButtonQuit()
{
    reject();
}

void MessageForm::proButtonCancel()
{
    reject();
}

void MessageForm::proButtonConfirm()
{
    accept();
}

int MessageForm::startExec()
{
    if (timeout)
    {
        timerId = startTimer(timeout);
    }

    show();
    return exec();
}

bool MessageBox(QWidget *parent,const int mode,const QString &test_item, const QString &title,const QString &text,const int timeout)
{
    int timeoutTemp = timeout;
    if (g_form != NULL)
    {
        delete g_form;
        g_form = NULL;
    }

    QPointer<MessageForm> form = new MessageForm(NULL, mode, timeoutTemp);
    g_form = form;

    form->setTitle(title);
    if (mode != MessageForm::SNMAC) {
        form->setText(text);
    } else {
        form->setLabel(text);
    }
    form->setTestItem(test_item);
    int ret = form->startExec();

    delete form;
    form = NULL;
    return !!ret;
}


