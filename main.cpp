#include <QApplication>
#include "inc/Control.h"
#include "inc/fac_utils.h"
#include "inc/fac_log.h"

void* semi_auto_test_control(void* arg)
{
    Control* control = Control::get_control();
    while(1){
		int testStep = control->get_test_step();
		FuncFinishStatus* funcFinishStatus = control->get_func_finish_status();
        usleep(500000);
        if(testStep != STEP_IDLE){
            if (funcFinishStatus->mem_finish && funcFinishStatus->fan_finish) {
				LOG_INFO("upload log");
			}
            
            switch(testStep){
                case STEP_MEM:
                {
                    if(funcFinishStatus->mem_finish){
                        testStep = STEP_FAN;
                        LOG_INFO("interface_finish OK.\n");
                        control->start_fan_test();
                    }
                }break;
                default:
                    break;
            }
        }
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setWeight(QFont::Normal);

    QDesktopWidget* dtw = QApplication::desktop();

    if ((dtw->height() <= 1080 && dtw->height() > 1050)
           && (dtw->width() <= 1920 && dtw->width() > 1680)) {
       font.setPointSize(14);
    } else if ((dtw->height() <= 1050 && dtw->height() > 1024)
           && (dtw->width() <= 1680 && dtw->width() > 1440)) {
       font.setPointSize(13);
    } else if ((dtw->height() <= 1024 && dtw->height() >= 900)
              && (dtw->width() <= 1440 && dtw->width() >= 1280)) {
       font.setPointSize(12);
    } else if ((dtw->height() < 900 && dtw->height() >= 720)
              && (dtw->width() <= 1280 && dtw->width() > 1024)) {
       font.setPointSize(8);
    } else {
       font.setPointSize(14);
    }
    a.setFont(font);

    Control::get_control()->show_main_test_ui();

	pthread_t tid;
    pthread_create(&tid,NULL,semi_auto_test_control,NULL);

    return a.exec();
}
