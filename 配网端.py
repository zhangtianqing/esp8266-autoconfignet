#-*- coding:utf-8 -*-
#配网端 py3.6 
#作者 天清 - QQ2455160536 - 2020年9月22日  --  版本号 0.1
#介绍:本源码旨在快速实现esp8266配网服务（参考硬件 Wemos d1 mini），并配套有esp源码
#部署:安装python3.6，安装所需组件"pip install configparser pywifi netifaces requests"
#手动配网过程：供电->打开手机搜索名为AutoConfig-XXXXXX的WIFI，密码为AutoConfig,然后网页访问网关地址，填好数据后提交即可；
#自动配网过程：WIFI的名字和密码在目录下的config.ini文件中修改，分配配置ssid(目标WIFI名称)、psw(目标WIFI密码)
import configparser
import os
import pywifi
import socket
import netifaces
import requests
config = configparser.ConfigParser() # 类实例化
target_ssid=""
target_psw=""



def sendPostFormData(ssid):
    print("=="+ssid+"配网")
    routingGateway = netifaces.gateways()['default'][netifaces.AF_INET][0]
    datas = {'ssid':target_ssid,'password':target_psw}
    r = requests.post('http://'+routingGateway,data=datas)
    
    if r.status_code==200:
        print(ssid+"配网成功")
        return
    if r.status_code!=200:
        print(ssid+"配网失败")

def configWifi(iface,ssid):
    profile=pywifi.Profile()
    profile.ssid=ssid   #wifi名称
    profile.auth=const.AUTH_ALG_OPEN    #auth - AP的认证算法
    profile.akm.append(const.AKM_TYPE_WPA2PSK) #选择wifi加密方式
    profile.cipher=const.CIPHER_TYPE_CCMP  #cipher - AP的密码类型
    profile.key=config.get('autoconfig','psd')  #wifi密钥   key （optinoal） - AP的关键。如果无密码，则应该设置此项CIPHER_TYPE_NONE
    iface.remove_all_network_profiles()  #删除其他配置文件
    tmp_profile=iface.add_network_profile(profile)   #加载配置文件
    iface.connect(tmp_profile)   #按配置文件进行连接
    time.sleep(2)

    if iface.status() == const.IFACE_CONNECTED:   #判断连接状态
        return True
    else:
        print("--"+ssid+"配网失败")

def start():
    configFilePath=r'./config.ini'
    configFilePath=os.getcwd()+configFilePath

    config.read(configFilePath)

    target_ssid=config.get('wifi','ssid')
    target_psw=config.get('wifi','psw')

    iface=pywifi.PyWifi().interfaces()[0]
    time.sleep(2)
    n=0
    while True:
        result = iface.scan_results()
        print("扫描结果:"+result+"\n")
        for i in range(len(result)):
            print(result[i].ssid, result[i].bssid)
            if result[i].ssid.startswith("AutoConfig-") :
                #配网操作
                print("开始对"+result[i].ssid+"进行配网"+"\n")
                configWifi(iface,result[i].ssid)
        if n>10:
            break
        n+=1
    print("配网完成！")

if __name__ == "__main__":
    start()
