#ifndef _FAC_UTILS_H
#define _FAC_UTILS_H

#include <pthread.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <memory>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

using std::string;

const string FACTORY_PATH         = "/usr/local/bin/factory/";
const string STRESS_LOCK_FILE     = FACTORY_PATH + "lock";
const string FAN_TEST_SCRIPT      = FACTORY_PATH + "fan_test.sh";
const string MEM_TEST_SCRIPT      = FACTORY_PATH + "mem_test.sh";

const string MEM_UI_LOG           = FACTORY_PATH + "mem_ui_log";
const string FAC_CONFIG_FILE      = "/tmp/fac_config.conf";


#define SUCCESS         (0)
#define FAIL            (1)
#define TIME_MAX_LEN    (50)

#define USB_MAX_NUM       (10)
#define USB_BLOCK_LEN     (16)
#define USB_VENDOR_LEN    (64)
#define USB_WRITE_LEN     (1024 * 1024)

#define USB_PATH_LEN    1024
#define USB_SPEED_LEN   32

struct BaseInfo {
    string mem_cap;
    string usb_total_num;
    string usb_3_num;
    string cpu_type;
    string ssd_cap;
    string emmc_cap;
    string hdd_cap;
    string wifi_exist;
    string fan_speed;
    string bright_level;
    string camara_exist;
    string vga_exist;
    string hdmi_exist;
    string lcd_info;
};

struct HwInfo {
    string sn;
    string mac;
    string product_name;
    string product_hw_version;
    string product_id;
    string cpu_type;
    string cpu_fre;
    string mem_cap;
};

struct TimeInfo {
    int day;
    int hour;
    int minute;
    int second;
};

struct FacArg{
    char* ftp_ip;
    char* ftp_user;
    char* ftp_passwd;
    char* ftp_dest_path;
    char* ftp_job_number;
    char* wifi_ssid;
    char* wifi_passwd;
    char* wifi_enp;
};

typedef struct tagUdevInfo {
	char block[USB_BLOCK_LEN];
	char vendor[USB_VENDOR_LEN];
    int speed; /* Mbps */
} UDEV_INFO_T;

typedef struct tagUsbInfo {

	int dev_num;
	struct udev* udev;
	UDEV_INFO_T dev[USB_MAX_NUM];

} USB_INFO_T;

string execute_command(string cmd);
int get_random();
int get_int_value(const string str);
void get_current_time(char tmp_buf[]);
void get_current_open_time(TimeInfo* date);
void diff_running_time(TimeInfo* dst, TimeInfo* src);
bool check_file_exit(const char* filename);
bool get_file_size(const char *filename, int *size);
bool write_local_data(const char* filename, const char* mod, char* buf, int size);
bool read_local_data(const char* filename, char* buf, int size);
bool remove_local_file(const char* filename);
void get_hwinfo(HwInfo* hwInfo);
void get_baseinfo(BaseInfo* baseInfo,const string baseinfo);
int get_fac_config_from_conf(const string conf_path, FacArg *fac);
char* ftp_send_file(const char* local_file_path, FacArg* fac);
char* response_to_chinese(const char* response);
bool combine_fac_log_to_mes(string sendLogPath);
bool is_digit(char *str);



#endif
