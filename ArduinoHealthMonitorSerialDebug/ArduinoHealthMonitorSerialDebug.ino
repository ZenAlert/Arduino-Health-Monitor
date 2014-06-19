/*
 Zen Alert Client with Serial Debug 
 
 This sketch connects to Zen Alert Service
 using a Arduino Ethernet shield or Arduino Ethernet.
 
 This example use DNS, by assigning the Ethernet client with a MAC address,
 IP address, and DNS address.

 In order to work you have to get an thing_id ID of your Arduino on www.zenalert.com 
 
 This code is in the public domain.
 
 */

#include <SPI.h>
#include <Ethernet.h>


// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// initialize the library instance:
EthernetClient client;

char server[] = "api.zenalert.com";

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in seconds
unsigned long nowTime= 0;
unsigned long connectionWait= 0;
int connectionTimeOut = 20;

const unsigned long postingInterval = 5L*60L*1000L;  // delay between http calls, in milliseconds - 5 minutes

IPAddress empty(0,0,0,0);

union IPAddressConverter {
  uint32_t ipInteger;
  uint8_t ipArray[4];
};

char buf[16];

char strTMP;  
boolean readIt=false;  


void setup() {
  Serial.begin(9600);
  delay (2000);
  Serial.println("Starting");
  waitConnection();
  viewIP();

}


void loop() {
  
  if (client.connect(server, 80)) {
      //Replace thing_id and security_key with your data (you need a free account Zen Alert - www.zenalert.com)
      client.println("GET /signal?code=alive&thing_id=AAAAAAAA-AAAA-AAAA-AAAA-AAAAAAAAAAAA&api_key=AAAAAAAA-AAAA-AAAA-AAAA-AAAAAAAAAAAA&description=Arduino-Test HTTP/1.0");      
      client.println();
      Serial.println("Call executed");

      lastConnectionTime = millis() / 1000;
      nowTime = lastConnectionTime;
      connectionWait = nowTime - lastConnectionTime;

      while(client.connected() && !client.available() && (connectionWait < connectionTimeOut)) {  //waits for data   
        String waitSTR = String(connectionWait);             
        nowTime = millis() / 1000;
        connectionWait = nowTime - lastConnectionTime;
      }        
 
     Serial.println(""); 
     Serial.println("Reading");
      
      readIt=false;
      while ((client.connected() || client.available()) && (connectionWait < connectionTimeOut)) { //connected or data available
        strTMP= client.read();   // gets byte from ethernet buffer
        if (strTMP == '{') readIt=true;
        if (readIt) {
          Serial.print(strTMP);
        }        
      }
      

      Serial.println();
      Serial.println("Stopping");
      Serial.println("==================");
      Serial.println();
      client.stop(); 
        
        
      if (connectionWait < connectionTimeOut) {     
        delay(postingInterval); 
      } 
      else {
        waitConnection();
        viewIP();       
      }
   }
   else {
      Serial.println("Missing Connection");
      client.stop();
      waitConnection();
      viewIP();
   }
}



void waitConnection() {
  
  boolean check=true;
  
  Serial.println("PLEASE CONNECT ETHERNET CABLE"); 
  
  // give the ethernet module time to boot up:
  delay(3000);
  
  while (check) {
    // start the Ethernet connection using a DNS server:
    Ethernet.begin(mac);
    if (Ethernet.localIP()!=empty) { 
       Serial.println("Ip OK"); 
       check=false;
    } else { 
        Serial.println("Ip Missing");      
    }
  }
}


void viewIP() {
  
  IPAddressConverter ipAddress;
  ipAddress.ipInteger = Ethernet.localIP();
  sprintf(buf, "%d.%d.%d.%d", ipAddress.ipArray[0], ipAddress.ipArray[1], ipAddress.ipArray[2], ipAddress.ipArray[3]);
  
  Serial.println("My IP address is");
  
  // print the Ethernet board/shield's IP address:  
  Serial.println(buf);  
}


