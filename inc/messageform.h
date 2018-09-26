#ifndef MESSAGEFORM_H
#define MESSAGEFORM_H

#include <QDialog>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QPointer>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QDebug>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include "../inc/ui.h"

class MessageForm : public QDialog
{
    Q_OBJECT
public:
    enum{
        Message = 0,
        Warnning,
        Error,
        SNMAC,
        Success,
        NOICON
    };
    MessageForm(QWidget *parent = 0,const int mode=0,const int timeout = 0);
    ~MessageForm();
    int startExec();

public slots:
    void setTitle(const QString &title)   {lb_title->setText(title);lb_title->update();}
    void setText(const QString &str)      {lb_text->setText(str);lb_text->update();}
    void setLabel(const QString &str)     {lb_snmac->setText(str);lb_snmac->update();}
    void setTestItem(const QString &item) {_m_test_item = item;}

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void timerEvent(QTimerEvent *evt);

signals:
    void sig_handled_test_result(QString test_item, QString result);

private slots:
    void proButtonOK();
    void proButtonFail();
    void proButtonCancel();
    void proButtonQuit();
    void proButtonConfirm();

public:
    QFrame          *frame;
    QLabel          *lb_title;
    QLabel          *lb_text;
    QPushButton     *bt_ok;
    QPushButton     *bt_fail;
    QPushButton     *bt_cancle;
    QPushButton     *bt_confirm;
    QLabel          *lb_icon;
    QLineEdit       *le_snmac;
    QHBoxLayout     *ly_snmac;
    QLabel          *lb_snmac;
    QPushButton     *bt_snmac;
    QGraphicsDropShadowEffect *effect;

private:
   int             mode;

   QString         m_sMsg;
   QString         _m_test_item;

   int             timerId;
   int             errTimerId;
   int             timeout;
   QGroupBox       *groupBox;
   QLineEdit       *le_input;
   int              _main_w;
   int              _main_h;
   int              _dialog_w;
   int              _dialog_h;
   int              _lb_text_w;
};

extern QString g_sn_mac_message;
extern QPointer<MessageForm> g_form;
extern bool MessageBox(QWidget *parent,const int mode,const QString &test_item, const QString &title,const QString &text,const int timeout);

#endif // MESSAGEFORM_H
