#include "../../inc/CameraTest.h"
#include "../../inc/fac_log.h"
#include "../../inc/fac_utils.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#define XAWTV_MAX_FAIL_COUNT   (5)

CameraTest::CameraTest(Control* control)
       :_control(control)
{

}

unsigned long CameraTest::get_window_id(const char *winid_file)
{
    char winidbuf[CMD_BUF_SIZE];
    unsigned long winid;
	int size = 0;

	memset(winidbuf, 0, CMD_BUF_SIZE);
	if(!get_file_size(winid_file, &size))
		return 0;
	LOG_INFO("%s file size %d\n", winid_file, size);
	
	if(!read_local_data(winid_file, winidbuf, size))
		return 0;

    winid = strtoul(winidbuf, NULL, 16);
    LOG_INFO("%s: xawtv window ID: [0x%x]\n", winid_file, winid);

    /* check if the window exists */
    memset(winidbuf, 0, CMD_BUF_SIZE);
    snprintf(winidbuf, CMD_BUF_SIZE, "xwininfo -id 0x%lx 2>&1", winid);
	string str = execute_command(winidbuf);
	if (strstr(str.c_str(), "X Error")) {
		LOG_ERROR("%s: xawtv window does not exist!\n", winid_file);
		winid = 0;
	}
	
    return winid;
}

#if 0
void CameraTest::close_xawtv_welcome_window()
{

    char wincmdbuf[CMD_BUF_SIZE];
    unsigned long winid;

    winid = get_window_id("/tmp/xawtv_welcome.winid");
    if (winid == 0) {
        LOG_ERROR("Failed to close xawtv welcome window!\n");
        return;
    }

    memset(wincmdbuf, 0, CMD_BUF_SIZE);
    snprintf(wincmdbuf, CMD_BUF_SIZE, "xdotool key --window 0x%lx Return", winid);
	
    if (execute_command(wincmdbuf) == "error"){
    	LOG_ERROR("run %s error\n", wincmdbuf);
    }
	else { 
    	LOG_ERROR("xawtv welcome window has been closed.\n");
	}
}
#endif

void CameraTest::start_camera_xawtv()
{
	string result = execute_command("sh " + CAMERA_START_SCRIPT);
    if (result == "error"){
    	LOG_ERROR("system run error!\n");
    }
    usleep(50000);

#if 0    /* close welcome window */
    close_xawtv_welcome_window();
    usleep(5000);
#endif
}

bool CameraTest::check_if_xawtv_started()
{
    unsigned long winid;

    winid = get_window_id("/tmp/xawtv.winid");
    if (winid == 0) {
        LOG_ERROR("Failed to start xawtv window!\n");
        return false;
    }

    LOG_INFO("xawtv window started OK.\n");
    return true;
}

bool CameraTest::camera_test_all()
{
    int failed_count = 0;
    bool xawtv_ok = false;

    /* check if camera device exists */
	string result = execute_command("sh " + CAMERA_CHECK_SCRIPT);
	if (result == "error"){
    	LOG_ERROR("system run error!\n");
    }
    usleep(50000);
    if (result != "VIDEOOK") {
        return false;
    }

    do {
        start_camera_xawtv();
        if (check_if_xawtv_started()) {
            /* xawtv started, show dialog */
            xawtv_ok = true;
            break;
        } else {
            usleep(50000);
            failed_count++;
            LOG_ERROR("xawtv started failed count: %d\n", failed_count);
        }
    } while (failed_count < XAWTV_MAX_FAIL_COUNT);

    if (!xawtv_ok && failed_count >= XAWTV_MAX_FAIL_COUNT) {
        /* xawtv started failed, just report FAIL result */
        LOG_ERROR("ERROR: Failed to start xawtv, GPU fault may be detected!\n");
    }
    return false;
}

void* CameraTest::test_all(void *arg)
{
	camera_test_all();

	//Control::get_control()->set_bright_test_finish();
	return NULL;
}

void CameraTest::start_test(BaseInfo* baseInfo)
{
    pthread_t tid;
    pthread_create(&tid,NULL,test_all,baseInfo);
}


