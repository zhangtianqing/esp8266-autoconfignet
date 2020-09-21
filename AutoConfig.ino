//作者 天清 - QQ2455160536 - 2020年9月22日  --  版本号 0.1
//介绍:本源码旨在快速实现esp8266配网服务（参考硬件 Wemos d1 mini），并配套有配网端（以后发，还没写好，目前可以使用网络配网）
//部署:将你的代码写在todo方法（等价于loop）内即可
//手动配网过程：供电->打开手机搜索名为AutoConfig-XXXXXX的WIFI，密码为AutoConfig,然后网页访问网关地址，填好数据后提交即可；
//自动配网过程：供电->笔记本打开WIFI，然后运行程序即可自行配网，WIFI的名字和密码在目录下的config.ini文件中修改
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <ArduinoUniqueID.h>

#define AP_name "AutoConfig-"
#define AP_PassWord "AutoConfig"
#define Begin(baud) Serial.begin(baud)
#define Print(message) Print(message)
#define Println(message) Println(message)




struct config_wifi
{
  char ssid[32];
  char psw[64];
};

config_wifi config;

ESP8266WebServer server(80);

const int led = LED_BUILTIN;

const String postForms = "<html>\
  <head>\
    <title>ESP8266 Web Server POST handling</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>POST form data to /postform/</h1><br>\
    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postform/\">\
      wifiName:\
      <input type=\"text\" name=\"ssid\" value=\"TP_LINK-2345\"><br>\
      wifiPassword:\
      <input type=\"text\" name=\"password\" value=\"123456789\"><br>\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
  </body>\
</html>";

String getArduinoUniqueId(){
  /*获得arduino板子硬件唯一ID*/  
  String uniqid = "";
  for (size_t i = 0; i < UniqueIDsize; i++)
  {
    if( i > 0 and i < UniqueIDsize){
      uniqid = uniqid + "-";
    }
    if (UniqueID[i] < 0x10){
      uniqid = uniqid + "0";
    }
    uniqid = uniqid + String(UniqueID[i], HEX);
  }
  return uniqid;
}

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/html", postForms);
  digitalWrite(led, 0);
}

void handlePlain() {
  if (server.method() != HTTP_POST) {
    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  } else {
    digitalWrite(led, 1);
    server.send(200, "text/plain", "POST body was:\n" + server.arg("plain"));
    digitalWrite(led, 0);
  }
}

void handleForm() {
  if (server.method() != HTTP_POST) {
    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  } else {
    digitalWrite(led, 1);
    String message = "POST form was:\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
      if(server.argName(i)=="ssid"){
        config.ssid=server.arg(i).c_str();
      }
      if(server.argName(i)=="password"){
        config.pwd=server.arg(i).c_str();
      }
    }
    Println(message);
    SaveConfigWifi();
    server.send(200, "text/plain", "success Work:"+message);
    digitalWrite(led, 0);
    delay(1000);
    ESP.restart();
  }
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}


void SaveConfigWifi(){
//  strcpy(config.ssid, WiFi.SSID().c_str());
//  strcpy(config.psw, WiFi.psk().c_str());
  EEPROM.begin(1024);
  uint8_t *p = (uint8_t*)(&config);
  for (int i = 0; i < sizeof(config); i++)
  {
    EEPROM.write(i, *(p + i));
  }
  EEPROM.commit();
}

void ReadConfigWifi(){
  EEPROM.begin(1024);
  uint8_t *p = (uint8_t*)(&config);
  for (int i = 0; i < sizeof(config); i++)
  {
    *(p + i) = EEPROM.read(i);
  }
  EEPROM.commit();
}

String charToString(char *c){
  String item="";
  for(int k=0; k<sizeof(&c); k++){
      item += String(c[k]);
  }
  return item;
}


void setupWebServer(){
//  if (MDNS.begin("esp8266")) {
//    Println("MDNS responder started");
//  }

  server.on("/", handleRoot);

  server.on("/postplain/", handlePlain);

  server.on("/postform/", handleForm);

  server.onNotFound(handleNotFound);

  server.begin();
  Println("HTTP Config Server Started");
}

bool setupWifiConnection(){
  ReadConfigWifi();
  WiFi.begin(String(config.ssid), String(config.pwd));
  Print("\nWIFI--账号：");
  Print(config.ssid);
  Print(",密码：");
  Print(config.pwd);
  Println(",Wifi 连接中");

  int count=0;
  do{// Wait for connection
    delay(500);
    Print(".");
    if(count>20){//timeout
      Println("Wifi Connection faild");
      return false;
    }
    count++;
  }while(WiFi.status() != WL_CONNECTED);
  
  Println("");
  Print("Connected to ");
  Println(ssid);
  Print("IP address: ");
  Println(WiFi.localIP());
  return true;
}
void setupAPModel(){
      Print("设置AP......");
    //配置IP信息
    WiFi.mode(WIFI_AP);
    //启动AP模式
    Println("启动AP...");
    boolean result = WiFi.softAP(AP_name+getArduinoUniqueId(),AP_PassWord);
    if(result)
    {
        Print("开始\nIP短地址：");
        //输出短地址
        Print(WiFi.softAPIP()+"-");
        //输出MAC地址
        Println(String("MAC地址：")+WiFi.softAPmacAddress().c_str()); 
    }
    else
    {
        Println("启动失败！");
    }
    //设置完毕
    Println("设置结束");
}

void todo(){
  Println("todo");
  delay(1000);
}
bool isWork=false;
void setup(void) {
  //default
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  Println(""); 

  //connectionToWifi
  if(!setupWifiConnection()==true){//startAPConfig
    isWork=false;
    Println("连接WIFI失败，开始进入配网模式");
    setupAPModel();
    setupWebServer();
  }else{
    Println("连接WIFI成功，开始进入工作模式");
    isWork=true;
  }
  
}

void loop(void) {
  if(isWork!=true){
    server.handleClient();
    Println(String("已连接的个数：")+WiFi.softAPgetStationNum());
    return;
  }
  todo();
}
