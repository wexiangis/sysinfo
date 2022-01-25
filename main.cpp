
#include <stdio.h>
#include <string.h>
#include "sysinfo.h"

int main(void)
{
    char buff[128] = {0};

    printf("SysInfo_SysBits: %d \r\n", SysInfo_SysBits());
    printf("SysInfo_ProcessBits: %d \r\n", SysInfo_ProcessBits());
    printf("SysInfo_CpuNumber: %d \r\n", SysInfo_CpuNumber());

    memset(buff, 0, sizeof(buff));
    SysInfo_CpuArch(buff, sizeof(buff));
    printf("SysInfo_CpuArch: %s \r\n", buff);

    memset(buff, 0, sizeof(buff));
    SysInfo_CpuVendor(buff, sizeof(buff));
    printf("SysInfo_CpuVendor: %s \r\n", buff);

    memset(buff, 0, sizeof(buff));
    SysInfo_CpuFullName(buff, sizeof(buff));
    printf("SysInfo_CpuFullName: %s \r\n", buff);

    memset(buff, 0, sizeof(buff));
    SysInfo_KernelName(buff, sizeof(buff));
    printf("SysInfo_KernelName: %s \r\n", buff);

    memset(buff, 0, sizeof(buff));
    SysInfo_KernelVersion(buff, sizeof(buff));
    printf("SysInfo_KernelVersion: %s \r\n", buff);

    memset(buff, 0, sizeof(buff));
    SysInfo_SysName(buff, sizeof(buff));
    printf("SysInfo_SysName: %s \r\n", buff);

    memset(buff, 0, sizeof(buff));
    SysInfo_SysVersion(buff, sizeof(buff));
    printf("SysInfo_SysVersion: %s \r\n", buff);

    printf("\r\n ---------- Enter to exit --------- \r\n");
    getchar();
    return 0;
}
