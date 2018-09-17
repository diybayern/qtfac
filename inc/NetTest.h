
#ifndef _NET_TEST_H_
#define _NET_TEST_H_

#include "Control.h"
#include "FuncBase.h"

#define MAC_ADDR_LEN     (6)
#define ETH_NAME_LEN     (16)
#define ETH_STATUS_UP    (0)
#define ETH_STATUS_DOWN  (1)

struct NetInfo {
	char mac[MAC_ADDR_LEN];
	char eth_name[ETH_NAME_LEN];
	int eth_index;
	int eth_status;
	int eth_link;
    int eth_speed;
    char eth_duplex;
	int seq;
	int recv_num;
};

struct MacPacket {
    char dst_mac[MAC_ADDR_LEN];
    char src_mac[MAC_ADDR_LEN];
    short type;
    int magic;
    int index;
    char data[100];
};


class Control;

class NetTest : public FuncBase
{
public:
    NetTest(Control* control);
    static bool init();
    static bool net_test_all();
    static void* test_all(void* arg);
    static void set_net_test_result(string func,string result,string ui_log);
    void start_test(BaseInfo* baseInfo);

private:
    Control* _control;

    static bool net_get_eth_name(char* eth_name, int size);
    static bool net_get_eth_index(char* eth_name, int* index);
    static bool net_get_mac_addr0(char* eth_name, char* hw_buf);
    static bool net_sprintf_mac_addr(char* src, char* dst);
    static void* net_recv_loopback_msg(void *arg);
    static bool net_get_eth_info(NetInfo *info);
    static int net_eth_no(char *eth_name);
    static bool net_get_eth_status(int fd, char *eth_name, int *status);
    static int net_test_ioctl(int fd, char *eth_name, void *cmd);
//    static char *net_get_duplex_desc(char duplex);
    static bool net_send_broadcast_msg(NetInfo* info, int num);
    static bool net_send_msg(char* src_mac, char* dst_mac, int index, int seq);
};


#endif



