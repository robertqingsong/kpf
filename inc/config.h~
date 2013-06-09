/********************************************************
 * file name: config.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: config file for framework kpf
 * *****************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define KERNEL_DEV_SUPPORT  (0)

#if (KERNEL_DEV_SUPPORT)

#include <linux/init.h>
#include <linux/module.h>

#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif


#if defined(__cplusplus)
extern "C"
{
#endif

#define DEFAULT_STACK_SIZE  (1024 * 512)


/*************************************************
 * OS Type Config.
 * ***********************************************/
#define __OS_WIN32__  (0)//is windows platform.
#define __OS_LINUX__  (1)//is linux platform.
#define __OS_MAC__  (0)//is mac platform.
#define __OS_IOS__  (0)//is ios platform.
#define __OS_ANDROID__  (0)//is android platform.

//If enable debug support.
#define DEBUG_SUPPORT  (1)
	
#if defined(__cplusplus)
}
#endif

#endif/* __CONFIG_H__ */
