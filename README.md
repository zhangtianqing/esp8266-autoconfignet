# esp8266-autoconfignet
基于esp8266的wemos d1 r2 mini 版的无线网络配置方案

作者 天清 - QQ2455160536 - 2020年10月15日  --  版本号 0.2

介绍:本源码旨在快速实现esp8266配网服务，PC端部署旨在为自动配网使用。


ESP8266部署:
------------------------------
将你的代码写在todo方法（等价于loop）内烧录即可



PC端部署:
------------------------------
安装python3.6，安装所需组件"pip install configparser pywifi netifaces requests"



配网模式选择：
------------------------------
如果是少量可直接手动配网：

手动配网过程：供电->打开手机搜索名为AutoConfig-XXXXXX的WIFI，密码为AutoConfig,然后网页访问网关地址，填好数据后提交即可；

如果数量较多即可使用PC端进行自动配网

自动配网过程：供电->笔记本打开WIFI，然后运行程序即可自行配网，WIFI的名字和密码在目录下的config.ini文件中修改

储存使用信息：
------------------------------
编译后使用了294008 字节储存，详细如下：

IROM   : 263576          - code in flash         (default or ICACHE_FLASH_ATTR) 

IRAM   : 27292   / 32768 - code in IRAM          (ICACHE_RAM_ATTR, ISRs...) 

DATA   : 1252  )         - initialized variables (global, static) in RAM/HEAP 

RODATA : 1888  ) / 81920 - constants             (global, static) in RAM/HEAP 

BSS    : 25552 )         - zeroed variables      (global, static) in RAM/HEAP 
