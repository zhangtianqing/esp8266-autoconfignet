# esp8266-autoconfignet
基于esp8266的wemos d1 r2 mini 版的无线网络配置方案

作者 天清 - QQ2455160536 - 2022年12月13日  --  版本号 1.0

介绍:本源码旨在快速实现esp8266配网服务，PC端部署旨在为自动配网使用。


ESP8266开发:
------------------------------
1.编码:将setup的方法写到beforeSetup,将loop的代码写在todo方法（等价于loop）内即可
2.ide中需要将EraseFlash改为Sketch+WiFi Settings



配网模式说明：
------------------------------
基于Esp的SmartConfig编码，直接使用app即可配网：https://objects.githubusercontent.com/github-production-release-asset-2e65be/34372655/a9d1553f-9643-497f-9833-09b2d0d24c0c?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20221208%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20221208T044444Z&X-Amz-Expires=300&X-Amz-Signature=a3d98237a79412a6110a814501c4c4fb4a12e5a8d5f70fc78c3c782465781f86&X-Amz-SignedHeaders=host&actor_id=0&key_id=0&repo_id=34372655&response-content-disposition=attachment%3B%20filename%3Desptouch-v2.3.2.apk&response-content-type=application%2Fvnd.android.package-archive


编译输出信息：
------------------------------
使用了344857 字节储存，最多1044464字节
全局变量使用了38268字节，剩余43652字节可供本地变量使用，最多内存数量为81920字节（80K）
详细如下：

ICACHE : 32768           - flash instruction cache 

IROM   : 313792          - code in flash         (default or ICACHE_FLASH_ATTR) 

IRAM   : 27901   / 32768 - code in IRAM          (IRAM_ATTR, ISRs...) 

DATA   : 1568  )         - initialized variables (global, static) in RAM/HEAP 

RODATA : 1596  ) / 81920 - constants             (global, static) in RAM/HEAP 

BSS    : 35104 )         - zeroed variables      (global, static) in RAM/HEAP 
