
ͼ���Ĳ���˵����

1. ����˵����
ͼ��֧��ƽ̨�� bk7231u��bk7252
֧��ϵͳ��     freertos�� rtthread�� alios 
֧�ֽӴ��ڣ�   sccb�ӿڡ�spi-slave(������)
֧�ִ��䷽ʽ�� tcp��udp��video_buffer(��ȡ����ͼ)
֧��ģʽ��     station�� softap

2.ʹ��˵����

����ͷĬ��ʹ�� GC0328C�� ����������� camera_intf.h ���޸ģ����������ʼ���ļ���
ʹ��I2C1����������ͷ��uart2Ϊ��ӡ��

1). �������º�Ϊ 1
λ�� sys_config_bkxxxx.h��
#define CFG_USE_APP_DEMO_VIDEO_TRANSFER            1
#define CFG_USE_CAMERA_INTF                        1

λ�� video_demo_config.h��
#define APP_DEMO_VIDEO_TRANSFER                    1


2). RFУ׼���
֧�� �Զ�����У׼ �� �ֶ�����У׼(Ĭ��)

�л��ķ���������

1> ����ʱѡ��bk72xx_cal.c �е� g_cali_mode�� 
CALI_MODE_MANUAL �� �ֶ�ģʽ������Itest��������У׼���� 
CALI_MODE_AUTO   �� �Զ�ģʽ�������趨��Ŀ��ֵ��ÿ���ϵ綼��У׼����

2> ����ʱѡ��
ATEģʽ�£�ͨ�����У��·�ָ�����ã�
rfcali_cfg_mode: x,  ���� 1:manual, 0:auto


3).����sation �� softap ��ʽ��ͼ
��ATEģʽ�£�ͨ�����У��·�ָ��
video_transfer -a XXXXX  xxxxx   // ����softap ��ͼ��XXXXX Ϊssid�� xxxxx ����(��ѡ������ʱΪ������)
video_transfer -s XXXXX  xxxxx   // ����station��ͼ��XXXXX Ϊ·������ssid�� xxxxx ����(��ѡ������ʱΪ������)

����Ĭ��֧�� UDP�� TCP ��ʽ���������λ�� video_transfer_config.h ��


4).��ͼ��
λ��freertos��Ŀ¼��
\tools\video_tool\PC_tool    PC������
\tools\video_tool\android    ��׿�ֻ���ֻ֧��softapģʽ��

PC�����ߣ�֧��softap ��stationģʽ��ͼ��
softapģʽ�£� PC������������ֻ�����������������������Ƚ��õ�������ģ���������ȵ㣬��play�Ϳ��Գ�ͼ
stationģʽ�£�PC�����������߻��������������������Ƚ��õ�����ģ��λ��ͬһ�����Σ���playǰ����Ҫ��remote ip(�ұ�)���ֶ���дģ���ip��ַ��

5).��ȡһ��ͼƬ����
��ATEģʽ�£�ͨ�����У��·�ָ��
video_buffer open  �򿪳ɹ����ٷ�

video_buffer read xxxx  xxxxΪsize,byteΪ��λ��  ��ȡ����ͼ�����ӡ�������ݡ�

video_buffer close �رոù���