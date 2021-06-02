#ifndef __NOTIFIER_H_
#define __NOTIFIER_H_


typedef void (*notify_func)(void *, int type, int value);

struct notifier {
    struct notifier *next;
    notify_func	    func;
    void	    	*arg;
};

struct notifier_req {
    notify_func	    func;
    void	    	*arg;
};

enum {
	SYS_EVENT = 0,
	WLAN_EVENT,
	POWER_EVENT,
};

extern struct notifier *wlan_evt_notifer;

int add_notifier(struct notifier **notif,  notify_func func, void *arg);
void remove_notifier(struct notifier **notif,  notify_func func, void *arg);
void notify(struct notifier *notif, int type, int val);

int register_wlan_notifier(notify_func func, void *arg);
void remove_wlan_notifier(notify_func func, void *arg);

#endif //__NOTIFIER_H_
