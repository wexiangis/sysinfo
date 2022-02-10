# 1、编译和测试

## 普通编译
```
gcc -o template main.cpp sysinfo.cpp
./template
```

## cmake编译
```
cmake .
make
./template
```

# 2、信息获取原理

## unix/linux/android(ndk)
---

* 系统位数:
```
elf文件解析(/bin/bash或者/proc/self/exe), buff[4]等于1时为32bit, 等于2时为64bit
```

* 程序位数:
```
查看代码指针大小, sizeof(void*) * 8, 32bit或者64bit
```

* cpu核心数:
```C
#include <unistd.h>
...
sysconf(_SC_NPROCESSORS_CONF);
...
```

* cpu架构:
```
elf文件解析(/bin/bash或者/proc/self/exe), 其中:

*((uint16_t*)&buff[18]);

这两字节代表了系统架构类型(定义见elf.h),
再结合系统位数和大端/小端(mips架构用到),
综合得出架构类型.
```

* cpu厂商:
```
解析 /proc/cpuinfo 中的关键字 "vendor" 所在行 (部分系统无该词条)
```

* cpu全称:
```
解析 /proc/cpuinfo 中的关键字 "model name" 所在行 (部分系统无该词条)
```

* 内核名称:
```C
#include <sys/utsname.h>
...
struct utsname info = {0};
char* p = info.sysname;
...
```

* 内核版本:
```C
#include <sys/utsname.h>
...
struct utsname info = {0};
char* p = info.release;
...
```

* 系统名称:
```C
#include <sys/utsname.h>
...
struct utsname info = {0};
char* p = info.nodename;
...
```

* 系统版本:
```C
#include <sys/utsname.h>
...
struct utsname info = {0};
char* p = info.version;
...
```

## Windows
---

* 系统位数:
```
暂缺
```

* 程序位数:
```
查看代码指针大小, sizeof(void*) * 8, 32或者64
```

* cpu核心数:
```C
SYSTEM_INFO sysInfo;
GetSystemInfo(&sysInfo);
return sysInfo.dwNumberOfProcessors;
```

* cpu架构:
```
暂缺
```

* cpu厂商:
```
来自汇编指令 cpuid
```

* cpu全称:
```
来自汇编指令 cpuid
```

* 内核名称:
```
暂缺
```

* 内核版本:
```
暂缺
```

* 系统名称:
```
暂缺
```

* 系统版本:
```
暂缺
```
