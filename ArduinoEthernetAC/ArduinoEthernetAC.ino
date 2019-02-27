#include <Ethernet.h>
#include <SPI.h>
#include <EEPROM.h>

int go=0;
byte mac[6] = { 0xBA, 0xBE, 0x00, 0x00, 0x00, 0x00 };
char macstr[18];
IPAddress ip;
IPAddress ipzero=IPAddress(0,0,0,0);
String Resultado="";
EthernetClient client;
String PostData="";

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  // Random MAC address stored in EEPROM
  if (EEPROM.read(1) == '#') {
    for (int i = 2; i < 6; i++) {
      mac[i] = EEPROM.read(i);
    }
  } else {
    randomSeed(analogRead(0));
    for (int i = 2; i < 6; i++) {
      mac[i] = random(0, 255);
      EEPROM.write(i, mac[i]);
    }
    EEPROM.write(1, '#');
  }
  snprintf(macstr, 18, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Start up networking
  Serial.print("DHCP (");
  Serial.print(macstr);
  Serial.println(")...");
  Ethernet.begin(mac);
  Serial.println("success");
  ip=Ethernet.localIP();
  Serial.println(ip);
  if(ip==ipzero) {
    Serial.println("no ip");
    Ethernet.begin(mac, IPAddress(192,168,2,2), IPAddress(8,8,8,8), IPAddress(192,168,2,1), IPAddress(255,255,255,0)); 
    Serial.println(Ethernet.localIP());
  }
//  Ethernet.begin(mac, ip, dns, gateway, subnet); 
  Serial.println("go");
}

void loop() {
  if(client.available()) {
    char c=client.read();
    Resultado=Resultado+c;
    if(!client.available()) {
      Serial.println("disconnecting.");
      client.stop();
      Serial.print(Resultado);
      }
      go=0;
  } else {
    if(go==0)
    {
      go=1;
      Serial.println("connecting...");
      if (client.connect("192.168.2.1", 8888)) {
        Serial.println("connected");
        PostData="&hex=1&tiporegisto=Ambos&terminal=1&ip="+ip;
        client.println("POST /validacartao.php HTTP/1.1");
        client.println("Host: 192.168.2.2");
        client.println("User-Agent: Arduino/1.0");
        client.println("Connection: close");
        client.println("Content-Type: application/x-www-form-urlencoded;");
        client.print("Content-Length: ");
        client.println(PostData.length());
        client.println();
        client.println(PostData);
        Resultado="";
      }
      else {
        Serial.println("connection failed");
      }
    }
  }

  
}
