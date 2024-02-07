#include <SoftwareSerial.h>
#include <PN532_SWHSU.h>
#include <PN532.h>
#include <TFT.h>  // Arduino LCD library
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient client;
boolean kesken=false;
int    HTTP_PORT   = 80;
String HTTP_METHOD = "GET"; // or GET

//Yhdistetään palvelimelle, tässä kannattaa käyttää paikallista palvelinta jotta voidaan saada vastaus nopeaa
char   HOST_NAME[] = "postdoor.jaakkagames.net";
String PATH_NAME   = "/index.php?data=";
#define cs   3
#define dc   9
#define rst  8
int animation=0;
TFT TFTscreen = TFT(cs, dc, rst);
SoftwareSerial SWSerial( 5, 6 ); // RX, TX 10, 11

PN532_SWHSU pn532swhsu( SWSerial );
String tunnus ="";
PN532 nfc( pn532swhsu );
boolean light=false;
int lcd_show=0; 
boolean lcd_reset=true;

void setup() 
{
  Ethernet.init(10); 
  Serial.begin(9600);
  nfc.begin();
  nfc.SAMConfig();
  TFTscreen.begin();
  TFTscreen.setRotation(0);
  TFTscreen.background(0, 0, 0);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.setTextSize(2);
  TFTscreen.text("Esita", 34, 30);
  TFTscreen.text("kulkulupa", 14, 50);
  
  if (Ethernet.begin(mac) == 0) 
  {
    Serial.println("Failed to obtaining an IP address using DHCP");
    TFTscreen.background(0, 0, 0);
    TFTscreen.stroke(255, 255, 255);
    TFTscreen.setTextSize(2);
    TFTscreen.text("DHCP", 34, 30);
    TFTscreen.text("yhteysvirhe", 14, 50);
    while(true);
  }
    else if (Ethernet.linkStatus() == LinkOFF) 
    {
      Serial.println("Ethernet cable is not connected.");
      
      TFTscreen.background(0, 0, 0);
      TFTscreen.stroke(255, 255, 255);
      TFTscreen.setTextSize(2);
      TFTscreen.text("Nettijohto", 34, 30);
      TFTscreen.text("on irti", 14, 50);
      while(true);
    }

 
}

void eisallittu()
{
  TFTscreen.background(0, 0, 0);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.setTextSize(2);
  TFTscreen.text("Kulku", 34, 30);
  TFTscreen.text("kielletty", 14, 50);
  lcd_show=2; lcd_reset=false;

  TFTscreen.stroke(0, 0, 255);
  TFTscreen.line(TFTscreen.width()/4+15-2, 100, TFTscreen.width()/2+15-2, 135);
  TFTscreen.line(TFTscreen.width()/4+15-1, 100, TFTscreen.width()/2+15-1, 135);
  TFTscreen.line(TFTscreen.width()/4+15, 100, TFTscreen.width()/2+15, 135); 
  TFTscreen.line(TFTscreen.width()/4+15+1, 100, TFTscreen.width()/2+15+1, 135);
  TFTscreen.line(TFTscreen.width()/4+15+2, 100, TFTscreen.width()/2+15+2, 135);

  TFTscreen.line(TFTscreen.width()/4+15-2, 135, TFTscreen.width()/2+15-2, 100);
  TFTscreen.line(TFTscreen.width()/4+15-1, 135, TFTscreen.width()/2+15-1, 100);
  TFTscreen.line(TFTscreen.width()/4+15, 135, TFTscreen.width()/2+15, 100); 
  TFTscreen.line(TFTscreen.width()/4+15+1, 135, TFTscreen.width()/2+15+1, 100);
  TFTscreen.line(TFTscreen.width()/4+15+2, 135, TFTscreen.width()/2+15+2, 100);
}

void yhteysvirhe()
{
  TFTscreen.background(0, 0, 0);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.setTextSize(2);
  TFTscreen.text("Yhteys", 34, 30);
  TFTscreen.text("virhe!", 14, 50);

  TFTscreen.stroke(0, 0, 255);
  TFTscreen.line(TFTscreen.width()/4+15-2, 100, TFTscreen.width()/2+15-2, 135);
  TFTscreen.line(TFTscreen.width()/4+15-1, 100, TFTscreen.width()/2+15-1, 135);
  TFTscreen.line(TFTscreen.width()/4+15, 100, TFTscreen.width()/2+15, 135); 
  TFTscreen.line(TFTscreen.width()/4+15+1, 100, TFTscreen.width()/2+15+1, 135);
  TFTscreen.line(TFTscreen.width()/4+15+2, 100, TFTscreen.width()/2+15+2, 135);

  TFTscreen.line(TFTscreen.width()/4+15-2, 135, TFTscreen.width()/2+15-2, 100);
  TFTscreen.line(TFTscreen.width()/4+15-1, 135, TFTscreen.width()/2+15-1, 100);
  TFTscreen.line(TFTscreen.width()/4+15, 135, TFTscreen.width()/2+15, 100); 
  TFTscreen.line(TFTscreen.width()/4+15+1, 135, TFTscreen.width()/2+15+1, 100);
  TFTscreen.line(TFTscreen.width()/4+15+2, 135, TFTscreen.width()/2+15+2, 100);
}

void onnistu()
{
  TFTscreen.background(0, 0, 0);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.setTextSize(2);
  TFTscreen.text("Kulku", 34, 30);
  TFTscreen.text("sallittu", 14, 50);
  TFTscreen.stroke(0, 200, 0);
  TFTscreen.line(TFTscreen.width()/4+15-2-10, 110, TFTscreen.width()/2+7-2-10, 135); 
  TFTscreen.line(TFTscreen.width()/4+15-1-10, 110, TFTscreen.width()/2+7-1-10, 135); 
  TFTscreen.line(TFTscreen.width()/4+15-10, 110, TFTscreen.width()/2+7-10, 135); 
  TFTscreen.line(TFTscreen.width()/4+15+1-10, 110, TFTscreen.width()/2+7+1-10, 135); 

  TFTscreen.line(TFTscreen.width()/2+7-1-10, 135, TFTscreen.width()-TFTscreen.width()/4, 90); 
  TFTscreen.line(TFTscreen.width()/2+7-1-10+1, 135, TFTscreen.width()-TFTscreen.width()/4+1, 90); 
  TFTscreen.line(TFTscreen.width()/2+7-1-10+2, 135, TFTscreen.width()-TFTscreen.width()/4+2, 90); 
  lcd_show=2; lcd_reset=false;
}

void yhdistetaan()
{
  TFTscreen.background(0, 0, 0);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.setTextSize(2);
  TFTscreen.text("Haetaan", 25, 30);
  TFTscreen.text("oikeuksia", 14, 50);
}

void laheta(String key)
{ 
    kesken=true;
    yhdistetaan();
    // connect to web server on port 80:
    if(client.connect(HOST_NAME, HTTP_PORT)) 
    {
      // if connected:
      Serial.println("Connected to server");
      // make a HTTP request:
      // send HTTP header  
      client.println(HTTP_METHOD+" " + PATH_NAME +key+ " HTTP/1.1");
      client.println("Host: " + String(HOST_NAME));
      client.println("Connection: close");
      client.println(); // end HTTP header
      String testdata="";
      while(client.connected()) 
      {
        if(client.available())
        {
          // read an incoming byte from the server and print it to serial monitor:
          char c = client.read();
          Serial.print(c);
          testdata=testdata+String(c);
        }
      }
    if(testdata.indexOf("eihyvaksytty")>0)
    {
      eisallittu();
    }
       else //pitää olla else koska sama sana molemmissa
    {
        if(testdata.indexOf("hyvaksytty")>0){onnistu();}
    }
        
    // the server's disconnected, stop the client:
    client.stop();
    Serial.println(); 
    Serial.println("disconnected");
  } 
    else 
  {
    // if not connected:
    Serial.println("connection failed");
    yhteysvirhe();
  }
  kesken=false;
}

void loop() 
{   
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                       // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  if (success) 
  {
    tunnus="";
    Serial.println("Found A Card!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    //light=true;
    for (uint8_t i=0; i < uidLength; i++)
    {
      Serial.print(" 0x");Serial.print(uid[i], HEX);
      tunnus=tunnus+""+String(uid[i]);
    }
    Serial.println("");
    Serial.println(tunnus);
    if(kesken==false)
    {
        laheta(tunnus);
    }
    Serial.println("");
    // 1 second halt
    delay(1000);
    //digitalWrite(6,LOW);
  }
    else
  {
  if(light==true)
  {
    light=false;
  }
    // PN532 probably timed out waiting for a card

   //Serial.println("Timed out! Waiting for a card...");
  }

  if(lcd_reset==false)
  { 
    if(lcd_show>0)
    {
      lcd_show-=1;
    }
      else
    {
      lcd_reset=true;
      TFTscreen.background(0, 0, 0);
      TFTscreen.stroke(255, 255, 255);
      TFTscreen.setTextSize(2);
      TFTscreen.text("Esita", 34, 30);
      TFTscreen.text("kulkulupa", 14, 50);  
    }
  }

  if(lcd_reset==true)
  {
    if(animation==0){ TFTscreen.stroke(255, 0, 0);}else{ TFTscreen.stroke(50, 50, 50);}
    TFTscreen.line(TFTscreen.width()/4, 130, TFTscreen.width()/2, 150); 
    TFTscreen.line(TFTscreen.width()-TFTscreen.width()/4, 130, TFTscreen.width()/2, 150); 
    TFTscreen.line(TFTscreen.width()/4, 130-1, TFTscreen.width()/2, 150-1); 
    TFTscreen.line(TFTscreen.width()-TFTscreen.width()/4, 130-1, TFTscreen.width()/2, 150-1); 
    TFTscreen.line(TFTscreen.width()/4, 130-2, TFTscreen.width()/2, 150-2); 
    TFTscreen.line(TFTscreen.width()-TFTscreen.width()/4, 130-2, TFTscreen.width()/2, 150-2); 
    if(animation==1){ TFTscreen.stroke(255, 0, 0);}else{ TFTscreen.stroke(50, 50, 50);}
    TFTscreen.line(TFTscreen.width()/4, 130-10, TFTscreen.width()/2, 150-10); 
    TFTscreen.line(TFTscreen.width()-TFTscreen.width()/4, 130-10, TFTscreen.width()/2, 150-10); 
    TFTscreen.line(TFTscreen.width()/4, 130-1-10, TFTscreen.width()/2, 150-1-10); 
    TFTscreen.line(TFTscreen.width()-TFTscreen.width()/4, 130-1-10, TFTscreen.width()/2, 150-1-10); 
    TFTscreen.line(TFTscreen.width()/4, 130-2-10, TFTscreen.width()/2, 150-2-10); 
    TFTscreen.line(TFTscreen.width()-TFTscreen.width()/4, 130-2-10, TFTscreen.width()/2, 150-2-10);
    if(animation==0){animation=1;}else{animation=0;} 
   }
}
