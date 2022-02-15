#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* ---------- Windows ---------- */
#if defined(WIN32) || defined(WINCE) || defined(_MSC_VER)

#include <windows.h>
#if _MSC_VER >= 1400 // VC2005
#include <intrin.h>
#endif // #if _MSC_VER >=1400

void _GetCpuIdex(uint32_t result[4], uint32_t addr, uint32_t ecxVal)
{
#if defined(_WIN64) || _MSC_VER >= 1600
    __cpuidex((int *)result, addr, ecxVal);
#else
    _asm {
        mov edi, result;
        mov eax, addr;
        mov ecx, ecxVal;
        cpuid;
        mov [edi], eax;
        mov [edi+4], ebx;
        mov [edi+8], ecx;
        mov [edi+12], edx;
    }
#endif
}

static void _GetCpuId(uint32_t result[4], uint32_t addr)
{
#if _MSC_VER >= 1400
    __cpuid((int *)result, addr);
#else
    _GetCpuIdex(result, addr, 0);
#endif
}

int SysInfo_SysBits()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
        return (int)sizeof(void *) * 8;
    else if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
             sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ||
             sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64)
        return 64;
    else
        return 32;
}

int SysInfo_ProcessBits()
{
    return (int)sizeof(void *) * 8;
}

int SysInfo_CpuNumber()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return (int)sysInfo.dwNumberOfProcessors;
}

int SysInfo_CpuArch(char *buff, int buffSize)
{
    return 0;
}

int SysInfo_CpuVendor(char *buff, int buffSize)
{
    uint32_t dwBuf[4] = {0};
    // Function 0: Vendor-ID and Largest Standard Function
    _GetCpuId(dwBuf, 0);
    // save. 保存到pvendor
    if (buffSize >= 4)
        *(uint32_t *)&buff[0] = dwBuf[1];
    if (buffSize >= 8)
        *(uint32_t *)&buff[4] = dwBuf[3];
    if (buffSize >= 12)
        *(uint32_t *)&buff[8] = dwBuf[2];
    if (buffSize > 12)
        buff[12] = '\0';
    return 0;
}

int SysInfo_CpuFullName(char *buff, int buffSize)
{
    uint32_t dwBuf[4] = {0};
    // Function 0x80000000: Largest Extended Function Number
    _GetCpuId(dwBuf, 0x80000000U);
    if (dwBuf[0] < 0x80000004U)
        return -1;
    // Function 80000002h,80000003h,80000004h: Processor Brand String
    if (buffSize >= 16)
        _GetCpuId((uint32_t *)&buff[0], 0x80000002U);
    if (buffSize >= 32)
        _GetCpuId((uint32_t *)&buff[16], 0x80000003U);
    if (buffSize >= 48)
        _GetCpuId((uint32_t *)&buff[32], 0x80000004U);
    if (buffSize > 48)
        buff[48] = '\0';
    return 0;
}

int SysInfo_KernelName(char *buff, int buffSize)
{
    return 0;
}

int SysInfo_KernelVersion(char *buff, int buffSize)
{
    return 0;
}

int SysInfo_SysName(char *buff, int buffSize)
{
    return 0;
}

int SysInfo_SysVersion(char *buff, int buffSize)
{
    return 0;
}

/* ---------- Linux ---------- */
#else // Unix/Linux

#include <stdbool.h>
#include <unistd.h> // sysconf()
#include <elf.h>    // elf format
#include <sys/utsname.h> // uname()

static int _GetElfBuff(uint8_t* buff, int buffSize)
{
    const char* list[] = {
        "/bin/bash",
        "/bin/ls",
        "/bin/ps"
    };
    FILE* fp = NULL;
    int i;
    
    for (i = 0; i < sizeof(list) / sizeof(list[0]); i++)
    {
        if (access(list[i], F_OK) == 0)
        {
            fp = fopen(list[i], "rb");
            if (fp)
                break;
        }
    }
    if (!fp)
        return -1;

    fread(buff, 1, buffSize, fp);
    fclose(fp);

    if (buffSize > 3 &&
        (buff[0] != 0x7F || buff[1] != 0x45 ||
        buff[2] != 0x4C || buff[3] != 0x46))
    {
        return -1;
    }
    return 0;
}

static int _GetCpuInfoBuff(char* buff, int buffSize, const char* hit)
{
    char* p;
    char line[1024] = {0};

    FILE* fp = fopen("/proc/cpuinfo", "rb");
    if (!fp)
        return -1;

    while (fgets(line, sizeof(line), fp))
    {
        p = strstr(line, hit);
        if (p)
        {
            p += strlen(hit);
            while (*p && *p++ != ':');
            while (*p && *p <= ' ') p++;
            if (*p)
            {
                while (*p >= ' ' && buffSize > 0)
                    *buff++ = *p++;
                break;
            }
        }
        memset(line, 0, sizeof(line));
    }

    fclose(fp);
    return 0;
}

int SysInfo_SysBits()
{
    uint8_t elfBuff[8] = {0};
    if (_GetElfBuff(elfBuff, sizeof(elfBuff)) != 0)
        return -1;
    return elfBuff[4] == 2 ? 64 : 32;
}

int SysInfo_ProcessBits()
{
    return (int)sizeof(void *) * 8;
}

int SysInfo_CpuNumber()
{
    return (int)sysconf(_SC_NPROCESSORS_CONF);
}

int SysInfo_CpuArch(char *buff, int buffSize)
{
    uint8_t elfBuff[32] = {0};
    bool isLittleEndian;
    int bits;
    uint16_t arch;

    if (_GetElfBuff(elfBuff, sizeof(elfBuff)) != 0)
        return -1;
    
    bits = elfBuff[4] == 2 ? 64 : 32;
    isLittleEndian = elfBuff[5] == 1 ? true : false;
    arch = *((uint16_t*)&elfBuff[18]);

    switch (arch)
    {
        case 3: // EM_386
            snprintf(buff, buffSize, "80386");
            break;
        case 4: // EM_68K
            snprintf(buff, buffSize, "8068k");
            break;
        case 5: // EM_88K
            snprintf(buff, buffSize, "8088k");
            break;
        case 6: // EM_IAMCU
            snprintf(buff, buffSize, "ia32");
            break;
        case 7: // EM_860
            snprintf(buff, buffSize, "80860");
            break;
        case 8: // EM_MIPS
            snprintf(buff, buffSize, "mips%s%s", bits == 64 ? "64" : "", isLittleEndian ? "el" : "");
            break;
        case 19: // EM_960
            snprintf(buff, buffSize, "80960");
            break;
        case 20: // EM_PPC
            snprintf(buff, buffSize, "ppc");
            break;
        case 21: // EM_PPC64
            snprintf(buff, buffSize, "ppc64");
            break;
        case 40: // EM_ARM
            snprintf(buff, buffSize, "arm");
            break;
        case 50: // EM_IA_64
            snprintf(buff, buffSize, "ia64");
            break;
        case 62: // EM_X86_64
            snprintf(buff, buffSize, "x86_64");
            break;
        case 183: // EM_AARCH64
            snprintf(buff, buffSize, "aarch64");
            break;
        case 224: // EM_AMDGPU
            snprintf(buff, buffSize, "amdgpu");
            break;
        case 243: // EM_RISCV
            snprintf(buff, buffSize, "riscv");
            break;
        case 258: // Loongson(3A5000)
            snprintf(buff, buffSize, "loongarch64");
            break;
        default:
            snprintf(buff, buffSize, "unknown(%d)", arch);
            break;
    }
    return 0;
}

int SysInfo_CpuVendor(char *buff, int buffSize)
{
    return _GetCpuInfoBuff(buff, buffSize, (const char*)"endor");
}

int SysInfo_CpuFullName(char *buff, int buffSize)
{
    return _GetCpuInfoBuff(buff, buffSize, (const char*)"odel name");
}

int SysInfo_KernelName(char *buff, int buffSize)
{
    struct utsname info = {0};
    if (uname(&info) != 0)
        return -1;
    snprintf(buff, buffSize, "%s", info.sysname);
    return 0;
}

int SysInfo_KernelVersion(char *buff, int buffSize)
{
    struct utsname info = {0};
    if (uname(&info) != 0)
        return -1;
    snprintf(buff, buffSize, "%s", info.release);
    return 0;
}

int SysInfo_SysName(char *buff, int buffSize)
{
    struct utsname info = {0};
    if (uname(&info) != 0)
        return -1;
    snprintf(buff, buffSize, "%s", info.nodename);
    return 0;
}

int SysInfo_SysVersion(char *buff, int buffSize)
{
    struct utsname info = {0};
    if (uname(&info) != 0)
        return -1;
    snprintf(buff, buffSize, "%s", info.version);
    return 0;
}

#endif
