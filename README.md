# esp8266-autoconfignet
基于esp8266的wemos d1 mini 版的无线网络配置方案

//作者 天清 - QQ2455160536 - 2020年9月22日  --  版本号 0.2
//介绍:本源码旨在快速实现esp8266配网服务（参考硬件 Wemos d1 mini）
//esp8266部署:将你的代码写在todo方法（等价于loop）内即可
//手动配网过程：供电->打开手机搜索名为AutoConfig-XXXXXX的WIFI，密码为AutoConfig,然后网页访问网关地址，填好数据后提交即可；
//自动配网过程：供电->笔记本打开WIFI，然后运行程序即可自行配网，WIFI的名字和密码在目录下的config.ini文件中修改
