#ifndef _SYS_INFO_H_
#define _SYS_INFO_H_

// #ifdef __cplusplus
// extern "C" {
// #endif

/* 系统位数: 32 or 64 */
int SysInfo_SysBits();

/* 程序位数: 32 or 64 */
int SysInfo_ProcessBits();

/* cpu数量 */
int SysInfo_CpuNumber();

/* cpu架构: i386、x86_64、armv7、armv8/aarch64、mips64 ... */
int SysInfo_CpuArch(char* buff, int buffSize);

/* cpu厂商: GenuineIntel、ARM ... */
int SysInfo_CpuVendor(char* buff, int buffSize);

/* cpu全称: (常用于Intel) */
int SysInfo_CpuFullName(char* buff, int buffSize);

/* 内核名称: Windows、Linux ... */
int SysInfo_KernelName(char* buff, int buffSize);

/* 内核版本 */
int SysInfo_KernelVersion(char* buff, int buffSize);

/* 系统名称: Windows、Ubuntu 、Debian ... */
int SysInfo_SysName(char* buff, int buffSize);

/* 系统版本 */
int SysInfo_SysVersion(char* buff, int buffSize);

// #ifdef __cplusplus
// }
// #endif

#endif // _SYS_INFO_H_
