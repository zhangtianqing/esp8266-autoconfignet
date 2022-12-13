//作者 天清 - QQ2455160536 - 2020年9月22日  --  版本号 0.2
//基于Wemos D1 R2 Mini（ESP8266）的基础外围代码  

//编码:将setup的方法写到beforeSetup,将loop的代码写在todo方法（等价于loop）内即可

//功能1:SmartConfig(App:https://objects.githubusercontent.com/github-production-release-asset-2e65be/34372655/a9d1553f-9643-497f-9833-09b2d0d24c0c?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20221208%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20221208T044444Z&X-Amz-Expires=300&X-Amz-Signature=a3d98237a79412a6110a814501c4c4fb4a12e5a8d5f70fc78c3c782465781f86&X-Amz-SignedHeaders=host&actor_id=0&key_id=0&repo_id=34372655&response-content-disposition=attachment%3B%20filename%3Desptouch-v2.3.2.apk&response-content-type=application%2Fvnd.android.package-archive)
//功能2:无线OTA
//功能3:自动上线机制

//第一次使用时，强制使用SmartConfig配网
//之后使用会自动调用储存了的WiFi配置自动联网
//如果需要强制刷新配置信息，需要将 forceReSmartConfigPin 引脚接地
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>

#include <ArduinoOTA.h>
#include <ArduinoUniqueID.h>

//自动上线设置--------------------------------------------------------
WiFiUDP Udp;

unsigned int localPort = 8888;//本地通讯端口
IPAddress remoteEndPoint;//服务端地址
unsigned int remotePort = 9981;//服务器端ip
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1];//最大接收消息缓冲区
char ReplyBuffer[] = "recv";//已收到上线广播回复信息
char token[] = "1as3d4a5d1a";//上线密钥
bool serverBind = false;//服务上线标识


//无线网络初始化账号密码----------------------------------------------
struct WifiConfig { //Wifi配置结构体
  char stassid[32];  //定义配网得到的WIFI名长度(最大32字节)
  char stapsw[64];   //定义配网得到的WIFI密码长度(最大64字节)
};
WifiConfig config;//Wifi配置结构体对象
int forceReSmartConfigPin = D0;//接地会强制进入配网模式 的引脚
String ssid;
String psw;

//业务逻辑开始>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//等效Setup方法
void beforeSetup(){
  Serial.println("setup");
}
//等效Loop方法
void todo(){
  Serial.println("loop");
}
//业务逻辑结束<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void toggleLED();
void initNetwork();

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  UniqueID8dump(Serial);
  digitalWrite(LED_BUILTIN, LOW);
  initNetwork();
  initOTA();
  initOnline();
  digitalWrite(LED_BUILTIN, HIGH);
  beforeSetup();
}

void loop() {
  checkOnline();
  checkOTA();
  todo();
}


void toggleLED() {
  static int pinStatus = LOW;
  if (pinStatus == HIGH)
    pinStatus = LOW;
  else
    pinStatus = HIGH;
  digitalWrite(LED_BUILTIN, pinStatus);
}
void initNetwork() {
  pinMode(forceReSmartConfigPin, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);
  loadConfig();
  if (ssid != 0 && psw != 0) {
    WiFi.begin(ssid, psw);  //如果有密码则自动连接
    while (WiFi.status() != WL_CONNECTED) {
      if (digitalRead(0) == LOW) {
        smartConfig();  //如果配网按钮被按下则停止当前连接开始配网
        break;          //跳出所有循环进入主程序
      } else {
        digitalWrite(LED_BUILTIN, LOW);  //加个LED慢闪，确认联网是否成功！成功就不闪了。
        delay(1000);
        digitalWrite(LED_BUILTIN, HIGH);  
        delay(1000);
      }
    }
  } else {
    smartConfig();
  }
  //ipL
  Serial.println(WiFi.localIP());
}
void smartConfig() {
  Serial.println();
  Serial.print("AutoConnect:");
  Serial.println(WiFi.getAutoConnect());
  Serial.println("ConnectWifiTimeout,use smartconfig");
  WiFi.begin();

  WiFi.beginSmartConfig();
  while (true) {
    delay(500);
    Serial.print(".");
    if (WiFi.smartConfigDone()) {
      strcpy(config.stassid, WiFi.SSID().c_str());  //名称复制
      strcpy(config.stapsw, WiFi.psk().c_str());    //密码复制
      saveConfig();                                 //调用保存函数
      WiFi.setAutoConnect(true);                    // 设置自动连接
      Serial.println("\nSmartConfig succ");
      break;
    }
    toggleLED();
  }


  // 等待获取网络ip配置成功
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
  }
  Serial.println();
  //设置Debug信息输出到串口
  WiFi.printDiag(Serial);
  // //wifi名称
  // Serial.println(WiFi.SSID());
  // //WIFI密码
  // Serial.println(WiFi.psk());
}

void SerialNum() {
  // byte ids[8];
  UniqueID8dump(Serial);
}

void initOnline() {
  Udp.begin(localPort);
}

void checkOnline() {
  // if(serverBind) return;
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.printf("Received packet of size %d from %s:%d\n    (to %s:%d, free heap = %d B)\n",
                  packetSize,
                  Udp.remoteIP().toString().c_str(), Udp.remotePort(),
                  Udp.destinationIP().toString().c_str(), Udp.localPort(),
                  ESP.getFreeHeap());

    // read the packet into packetBufffer
    int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    packetBuffer[n] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    bool onl = strcmp(token, packetBuffer) == 0;
    if (onl) {
      Serial.println("verity succ,online");
      remoteEndPoint = Udp.remoteIP();
      Udp.beginPacket(Udp.remoteIP(), remotePort);
      Udp.write(_UniqueID.id, 8);
      Udp.endPacket();
    }
    serverBind = onl;
    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
}
void checkOTA() {
  ArduinoOTA.handle();
}
void initOTA() {

  ArduinoOTA.setPassword(token);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}
void saveConfig()  //保存函数
{
  EEPROM.begin(1024);  //向系统申请1024kb ROM
  //开始写入
  uint8_t *p = (uint8_t *)(&config);
  for (int i = 0; i < sizeof(config); i++) {
    EEPROM.write(i, *(p + i));  //在闪存内模拟写入
  }
  EEPROM.commit();  //执行写入ROM
}

void loadConfig()  //读取函数
{
  EEPROM.begin(1024);
  uint8_t *p = (uint8_t *)(&config);
  for (int i = 0; i < sizeof(config); i++) {
    *(p + i) = EEPROM.read(i);
  }
  EEPROM.commit();
  ssid = config.stassid;
  psw = config.stapsw;
}
