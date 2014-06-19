/*
 Zen Alert Client 
 
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

char buf[16];

char strTMP;  
boolean readIt=false;  


void setup() {
  waitConnection();
}


void loop() {
  
  if (client.connect(server, 80)) {
      //Replace object_id and security_key with your data (you need a free account Zen Alert - www.zenalert.com)
      client.println("GET /signal?code=alive&thing_id=AAAAAAAA-AAAA-AAAA-AAAA-AAAAAAAAAAAA&security_key=AAAAAAAA-AAAA-AAAA-AAAA-AAAAAAAAAAAA&description=Arduino-Test HTTP/1.0");      
      client.println();

      lastConnectionTime = millis() / 1000;
      nowTime = lastConnectionTime;
      connectionWait = nowTime - lastConnectionTime;

      while(client.connected() && !client.available() && (connectionWait < connectionTimeOut)) {  //waits for data   
        String waitSTR = String(connectionWait);             
        nowTime = millis() / 1000;
        connectionWait = nowTime - lastConnectionTime;
      }        
      
      readIt=false;
      while ((client.connected() || client.available()) && (connectionWait < connectionTimeOut)) { //connected or data available
        strTMP= client.read();   // gets byte from ethernet buffer   
      }
      
      client.stop(); 
        
        
      if (connectionWait < connectionTimeOut) {     
        delay(postingInterval); 
      } 
      else {
        waitConnection();
      }
   }
   else {
      client.stop();
      waitConnection();
   }
}



void waitConnection() {
  
  boolean check=true;
  
  // give the ethernet module time to boot up:
  delay(3000);
  
  while (check) {
    // start the Ethernet connection using a DNS server:
    Ethernet.begin(mac);
    if (Ethernet.localIP()!=empty) { 
       check=false;
    } 
  }
}



