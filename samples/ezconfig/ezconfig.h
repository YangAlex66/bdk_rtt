#ifndef BK_EZCONFIG_H
#define BK_EZCONFIG_H


typedef struct
{
    char *passwd;                       /* wifi密码，以'\0'结尾 */
    char *ssid;                         /* wifi ssid，以'\0'结尾 */
    char ip[4];
    unsigned char passwd_len;           /* wifi密码长度 */
    unsigned char ssid_len;             /* wifi ssid长度 */
    unsigned char ip_len;               /* ip地址，根据EZcconfig协议，当wifi连接成功后，需要通过udp向手机ip的10000端口广播这个随机值，这样EZconfig发送端（手机端）就能知道设备已配网成功 */
    unsigned char reserved;             /* 保留值 */
} EZconfig_result_t;

/*
 * bk_ezconfig_recv()正常情况下的返回值
 */
typedef enum
{
    /* 解码正常，无需特殊处理，继续调用bk_ezconfig_recv()直到解码成功 */
    EZCONFIG_STATUS_CONTINUE = 0,

    /* wifi信道已经锁定，上层应该立即停止切换信道 */
    EZCONFIG_STATUS_CHANNEL_LOCKED = 1,

    /* 解码成功，可以调用airkiss_get_result()取得结果 */
    EZCONFIG_STATUS_COMPLETE = 2

} ezconfig_status_t;


void bk_ezconfig_init(void);
int  bk_ezconfig_recv(uint8_t * frame);
void bk_ezconfig_stop(void);
void bk_ezconfig_reset_recive(void);
int  ezconfig_get_result(void);
#endif // BK_EZCONFIG_H
