#ifndef _KLOG_H
#define _KLOG_H

//#define DEBUG

#define klog(message, ...) kprintf("[%s] "message"\n", __FILE__, ##__VA_ARGS__)
#define kerr(message, ...) kprintf("\033[031m[%s:%d in %s] ERROR: "message"\033[0m\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#ifdef DEBUG
	#define kdebug(message, ...) kprintf("[%s] DEBUG: "message"\n", __FILE__, ##__VA_ARGS__)
#else
	#define kdebug(message, ...)
#endif

#endif
