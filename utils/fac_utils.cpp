#include "../inc/fac_utils.h"
#include "../inc/fac_log.h"

#include <ftplib.h>
#include <map>
using namespace std;

netbuf* ftp_handle;

#define DEFAULT_FTP_IP       "172.21.5.79"
#define DEFAULT_FTP_USER     "djy"
#define DEFAULT_FTP_PASSWD   "djy"
#define DEFAULT_WIFI_SSID    "sfc-test"
#define DEFAULT_WIFI_PASSWD  "12345678"
#define DEFAULT_WIFI_ENP     "WPA"

#define NO_FTP_PATH          (1)
#define NO_JOB_NUMBER        (2)

/*
**execute command and return output result
*/
string execute_command(string cmd) {
    string cmd_result = "";
    char result[1024];
    int rc = 0;
    FILE *fp;
    fp = popen(cmd.c_str(),"r");
    if(fp == NULL)
    {
        perror("popen execute fail.");
        return "error";
    }
    while(fgets(result,sizeof(result),fp) != NULL)
    {
        string tempResult = result;
        cmd_result = cmd_result + tempResult;
    }
    rc = pclose(fp);
    if(rc == -1)
    {
        perror("close fp fail.");
        return "error";
    }else{
        LOG_INFO("command:%s,subprocess end status:%d,command end status:%d",cmd.c_str(),rc,WEXITSTATUS(rc));

        if(WEXITSTATUS(rc) != 0)
        {
            return "error";
        }

        if(0 < cmd_result.length())
        {
            string tmp = cmd_result.substr(cmd_result.length() - 1, cmd_result.length());
            if(tmp == "\n" || tmp == "\r"){
                return cmd_result.substr(0, cmd_result.length() - 1) + "\0";
            }else{
                return cmd_result;
            }
        }else
        {
            return cmd_result;
        }
   }
}

void get_current_time(char tmp_buf[]) {
    struct timeval  tv;
    struct timezone tz;
    struct tm nowtime;
    gettimeofday(&tv, &tz);
    localtime_r(&tv.tv_sec, &nowtime);
    strftime(tmp_buf, TIME_MAX_LEN, "%Y-%m-%d %H:%M:%S", &nowtime);
    tmp_buf += strlen(tmp_buf);
    snprintf(tmp_buf, TIME_MAX_LEN, ".%03ld", tv.tv_usec/1000);
}

void get_current_open_time(TimeInfo* date) {
	struct timespec time_space;
	
	clock_gettime(CLOCK_MONOTONIC, &time_space);
	
	date->day = time_space.tv_sec / (24 *60 * 60);
	date->hour = (time_space.tv_sec % (24 *60 * 60))/(60 * 60);
	date->minute = (time_space.tv_sec % (60 * 60))/60;
	date->second = time_space.tv_sec % 60;
}

void diff_running_time(TimeInfo* dst, TimeInfo* src) {

	if (dst->second < src->second) {
		dst->second += 60;
		dst->minute -= 1;
	}
	dst->second -= src->second;
	dst->second %= 60;

	if (dst->minute < src->minute) {
		dst->minute += 60;
		dst->hour -= 1;
	}
	dst->minute -= src->minute;
	dst->minute %= 60;

	if (dst->hour < src->hour) {
		dst->hour += 24;
		dst->day -= 1;
	}
	dst->hour -= src->hour;
	dst->hour %= 24;

	dst->day -= src->day;
}

bool check_file_exit(const char* filename) { 
    if (filename == NULL){
        return false;
    }

    if (access(filename, F_OK) == 0){
        return true;
    }
    
    return false;
}

bool get_file_size(const char *filename, int *size) {
    FILE* infile = NULL;

	if ((infile = fopen(filename, "rb")) == NULL) {
		return false;
	}
    
	fseek(infile, 0L, SEEK_END);
	*size = ftell(infile);

	fclose(infile);

	return true;
}

bool write_local_data(const char* filename, const char* mod, char* buf, int size) {
	int count = 0;
	FILE * outfile = NULL;

	if ((outfile = fopen(filename, mod)) == NULL) {
		LOG_ERROR("Can't open %s\n",filename);
        return false;
	}

	count = fwrite(buf, size, 1, outfile);
	if (count != 1) {
        LOG_ERROR("Write data failed: file=%s, count=%d, size=%d\n", filename, count, size);
		fclose(outfile);
		return false;
	}

	fflush(outfile);
	fclose(outfile);

	return true;
}

bool read_local_data(const char* filename, char* buf, int size) {
	int ret = 0;
	FILE * infile = NULL;

	if ((infile = fopen(filename, "rb")) == NULL) {
		LOG_ERROR("Can't open %s\n", filename);
		return false;
	}

	ret = fread(buf, size, 1, infile);
	if (ret != 1) {
		LOG_ERROR("Read file failed: file=%s, size=%d\n", filename, size);
		fclose(infile);
		return false;
	}

	fclose(infile);
	return true;
}

bool remove_local_file(const char* filename) {
	int ret;
    if (filename == NULL){
        return true;
    }
    ret = remove(filename);
 	/*   if (system("sync") < )
		LOG_ERROR("system sync error\n");
	*/
	if(ret == 0)
		return true;
    return false;
}
    

void get_hwinfo(HwInfo* hwInfo) {

	hwInfo->sn = execute_command("dmidecode -s system-serial-number");
	hwInfo->mac = execute_command("ifconfig | grep HWaddr | awk '/eth0/ {print $5}'");
	hwInfo->product_name = execute_command("dmidecode -s system-product-name");
    hwInfo->product_id = execute_command("dmidecode -s baseboard-product-name");
	hwInfo->product_hw_version = execute_command("dmidecode -s system-version");
	hwInfo->cpu_type = execute_command("dmidecode -s processor-version");
	hwInfo->cpu_fre = execute_command("cat /proc/cpuinfo | grep 'model name' |uniq | awk '/model name/ {print $NF}'");
	hwInfo->mem_cap = execute_command("free -m | awk '/Mem/ {print $2}'");
}



int get_int_value(const string str)
{
    if (str.size() == 0) {
        return -1;
    } else {
        return atoi(str.c_str());
    }
}

void get_baseinfo(BaseInfo* baseInfo, const string baseinfo) {
	map<string, string> tmap;
	string str;
	char buf[128] = {0};
	int cnt = 0;
	int idx;
	for (unsigned int i = 0; i < baseinfo.size(); i++) {
        if (baseinfo[i] != ';') {
            buf[cnt++]  = baseinfo[i];
        } else {
            buf[cnt] = '\0';
            str = string(buf);

            idx = str.find_first_of(':');
            tmap[str.substr(0, idx)] = str.substr(idx+1, str.length()-idx-1); 
            cnt = 0;
        }
    }
	
	if (cnt != 0) { 
		buf[cnt] = '\0';
		str = string(buf); 
		idx = str.find_first_of(':');
		tmap[str.substr(0, idx)] = str.substr(idx+1, str.length()-idx-1); 	 
	}

	string usb         = tmap["USB"];
	idx = usb.find_first_of('/');
    string usb_3 = usb.substr(0, idx);
	string usb_t = usb.substr(idx+1, usb.length()-idx-1); 

	baseInfo->mem_cap       = tmap["MEM"];
	baseInfo->usb_total_num = usb_t;
	baseInfo->usb_3_num     = usb_3;
	baseInfo->cpu_type      = tmap["CPU"];
	baseInfo->ssd_cap       = tmap["SSD"];
	baseInfo->emmc_cap      = tmap["EMMC"];
	baseInfo->hdd_cap       = tmap["HDD"];
	baseInfo->wifi_exist    = tmap["WIFI"];
	baseInfo->fan_speed     = tmap["FAN"];
	baseInfo->bright_level  = tmap["BRT"];
	baseInfo->camara_exist  = tmap["CAM"];
	baseInfo->vga_exist     = tmap["VGA"];
	baseInfo->hdmi_exist    = tmap["HDMI"]; 
	baseInfo->lcd_info      = tmap["LCD"];
}

bool is_digit(char *str) {
	size_t len = 0;

	len = strlen(str);
	if (0 == len) {
		return false;
	}

	while (len > 0) {
		if (*str < '0' || *str > '9') {
			return false;
		}
		str++;
		len--;
	}

	return true;
}

bool read_conf_line(const string conf_path, const char* tag,char* value)
{
    FILE* conf_fp;
    if((conf_fp = fopen(conf_path.c_str(), "r")) == NULL){
        LOG_ERROR("ftp_config.conf open failed\n");
        return false;
    }
    else{
        char match[128];
        char line[256];
        sprintf(match, "%s=%%s", tag);
        
        while(fgets(line, sizeof(line), conf_fp) != NULL){
            if(line[0] != '#') {//ignore the comment
                if(strstr(line, tag)!=NULL){
                    sscanf(line, match, value);
                    return true;
                } 
            }
        }
    }
    return false;
}

int get_fac_config_from_conf(const string conf_path, FacArg *fac)
{
    char* IP = (char*)malloc(128);
	memset(IP, 0, 128);
    if(read_conf_line(conf_path, "ftp_ip",IP) == false){
		memcpy(IP, DEFAULT_FTP_IP, strlen(DEFAULT_FTP_IP));
		LOG_INFO("use default ftp_ip\n");
    }

    char* ftp_user = (char*)malloc(128); 
	memset(ftp_user, 0, 128);
    if(read_conf_line(conf_path, "ftp_username", ftp_user) == false){
		memcpy(ftp_user, DEFAULT_FTP_USER, strlen(DEFAULT_FTP_USER));
		LOG_INFO("use default ftp_username\n");
    }

    char* ftp_passwd = (char*)malloc(128);
    memset(ftp_passwd, 0, 128);
	if(read_conf_line(conf_path, "ftp_passwd",ftp_passwd) == false){
        memcpy(ftp_passwd, DEFAULT_FTP_PASSWD, strlen(DEFAULT_FTP_PASSWD));
		LOG_INFO("use default ftp_passwd\n");
    }

    char* dest_path = (char*)malloc(128);
	memset(dest_path, 0, 128);
	if(read_conf_line(conf_path, "ftp_dest_path",dest_path) == false){
        LOG_ERROR("read dest_path failed\n");
		return NO_FTP_PATH;
    }

    char* job_number = (char*)malloc(128);
    memset(job_number, 0, 128);
	if(read_conf_line(conf_path, "job_number", job_number) == false){
        LOG_ERROR("read job_number faild\n");
		return NO_JOB_NUMBER;
    }

    fac->ftp_ip = IP;
    fac->ftp_user = ftp_user;
    fac->ftp_passwd = ftp_passwd;
    fac->ftp_dest_path = dest_path;
    fac->ftp_job_number = job_number;


	char* ssid = (char*)malloc(128);
	memset(ssid, 0, 128);
	if(read_conf_line(conf_path, "wifi_ssid",ssid) == false){
		memcpy(ssid, DEFAULT_WIFI_SSID, strlen(DEFAULT_WIFI_SSID));
		LOG_INFO("use default wifi_ssid\n");
	}
		  
	char* wifi_passwd = (char*)malloc(128);
	memset(wifi_passwd, 0, 128);
	if(read_conf_line(conf_path, "wifi_passwd",wifi_passwd) == false){
		memcpy(wifi_passwd, DEFAULT_WIFI_PASSWD, strlen(DEFAULT_WIFI_PASSWD));
		LOG_INFO("use default wifi_passwd\n");
	}
			
	char* wifi_enp = (char*)malloc(128);			
	memset(wifi_enp, 0, 128);
	if(read_conf_line(conf_path, "wifi_enp", wifi_enp) == false){
		memcpy(wifi_enp, DEFAULT_WIFI_ENP, strlen(DEFAULT_WIFI_ENP));
		LOG_INFO("use default wifi_enp\n");
	}

	fac->wifi_ssid = ssid;
	fac->wifi_passwd = wifi_passwd;;
	fac->wifi_enp = wifi_enp;

	return 0;
}

char* response_to_chinese(const char* response)
{
    char* ch_res;
    ch_res = (char*)malloc(128);
    if (strstr(response, "226") != NULL) {
        strcpy(ch_res, "上传成功");
        return ch_res;
    } else if (strstr(response, "530") != NULL) {
        strcpy(ch_res, "错误！登录失败！");
        return ch_res;
    } else if (strstr(response, "553") != NULL) {
        strcpy(ch_res, "错误！无法创建文件！");
        return ch_res;
    } else if(strstr(response, "426") != NULL) {
        strcpy(ch_res, "错误！连接关闭，传送中止！");
        return ch_res;
    } else if(strstr(response, "connect faild") != NULL) {
        strcpy(ch_res, "错误！连接失败!");
        return ch_res;
    } else if(strstr(response, "550") != NULL) {
        strcpy(ch_res, "错误！路径错误！");
        return ch_res;
    } else {
        strcpy(ch_res, "未知错误！");
        return ch_res;
    }
}

bool combine_fac_log_to_mes(string sendLogPath) {
    FILE* fp_mes;
    FILE* fp_fac;
    int c;
    fp_mes = fopen(sendLogPath.c_str(),"ab");
    fp_fac = fopen("/var/log/factory_test.log","rb");
    if (fp_mes == NULL || fp_fac == NULL) {
        if (fp_mes) {
            fclose(fp_mes);
        }
        if (fp_fac) {
            fclose(fp_fac);
        }
        return false;
    }
    while ((c = fgetc(fp_fac)) != EOF) {
        fputc(c,fp_mes);
    }
    fclose(fp_mes);
    fclose(fp_fac);
    return true;
}



char* ftp_send_file(const char* local_file_path, FacArg* fac)
{
    LOG_INFO("send log start.\n");
    FtpInit();
    char* ftp_rsp;
    char* ret_rsp = 0;
    ret_rsp = (char*)malloc(256);
    LOG_INFO("ftp_ip:%s, user:%s, passwd:%s, path:%s\n", fac->ftp_ip, fac->ftp_user, fac->ftp_passwd, fac->ftp_dest_path);
	
    if(FtpConnect(fac->ftp_ip, &ftp_handle) != 1){
        strcpy(ret_rsp, "connect faild");
        return ret_rsp;
    }
    if(FtpLogin(fac->ftp_user, fac->ftp_passwd, ftp_handle) != 1){
        ftp_rsp = FtpLastResponse(ftp_handle);
        ret_rsp = (char*)memcpy(ret_rsp, ftp_rsp, strlen(ftp_rsp)+1);
        FtpQuit(ftp_handle);
        return ret_rsp;
    }
    if(FtpPut(local_file_path, fac->ftp_dest_path, FTPLIB_ASCII, ftp_handle) != 1){
        ftp_rsp = FtpLastResponse(ftp_handle);
        ret_rsp = (char*)memcpy(ret_rsp, ftp_rsp, strlen(ftp_rsp)+1);
        FtpQuit(ftp_handle);
        return ret_rsp;
    }
    ftp_rsp = FtpLastResponse(ftp_handle);	
    ret_rsp = (char*)memcpy(ret_rsp, ftp_rsp, strlen(ftp_rsp)+1);
    FtpQuit(ftp_handle);
    return ret_rsp;
}