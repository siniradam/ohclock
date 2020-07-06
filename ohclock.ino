//Based on code of Pawel A. Hernik //https://www.youtube.com/watch?v=bePgZIXHSkM&feature=youtu.be

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

WiFiClient client;

#define NUM_MAX 4
#define ROTATE 90
#define DIN_PIN 15  // D8
#define CS_PIN  13  // D7
#define CLK_PIN 12  // D6

#include "max7219.h"
#include "fonts.h"

const char* ssid     = "OhShift";      // SSID of local network
const char* password = "";    // Password on network
const char* YTchannel = "";   // YT user id
// =======================================================================
unsigned long t_unix_date1;

HTTPClient http;

void setup() 
{
  Serial.begin(115200);
  initMAX7219();
  sendCmdAll(CMD_SHUTDOWN,1);
  sendCmdAll(CMD_INTENSITY,0);
  Serial.print("Connecting WiFi ");
  WiFi.begin(ssid, password);
  printStringWithShift(" WiFi ... ",15);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("."); delay(500);
  }
  Serial.println("");
  Serial.print("Connected: "); Serial.println(WiFi.localIP());
  }
// =======================================================================

void loop()
{
  long timeVal = getTime();  

  printStringWithShift(" Hello ",20);
  printCharWithShift(145,15);// :)
  delay(3000);

  showTime(timeVal);
  delay(5000);
  
  printStringWithShift(" Word Of The Day      ",20);
  getWord();
  delay(3000);
  
  showTime(timeVal);
  delay(10000);

  
  
  /*
  Serial.println("Getting data ...");
  printStringWithShift("  ... YT ...",15);
  int subs, views, cnt = 0;
  String yt1,yt2;
  while(1) {
    if(!cnt--) {
      cnt = 50;  // data is refreshed every 50 loops
      if(getYTSubs(YTchannel,&subs,&views)==0) {
        yt1 = "     SUBSCRIBERS:      "+String(subs)+" ";
        yt2 = "      VIEWS:   "+String(views);
      } else {
        yt1 = "   YouTube";
        yt2 = "   Error!";
      }
    }
    printStringWithShift(yt1.c_str(),20);
    delay(3000);
    printStringWithShift(yt2.c_str(),20);
    delay(3000);
  }*/


}
// =======================================================================

long getTime(){  
  http.useHTTP10(true);
  http.begin("http://worldtimeapi.org/api/timezone/Europe/Istanbul.json");
  http.GET();
  
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, http.getStream());
  //Serial.println(timenow);
  //Serial.println(doc["unixtime"].as<long>());
  
  http.end();
  return doc["unixtime"].as<long>() + 10800;
}

void showTime(long t_unix_date1){
  
  String timenow = "  ";
  timenow.concat(hour(t_unix_date1));
  timenow.concat(":");
  timenow.concat(minute(t_unix_date1));
  timenow.concat("   ");
  char timeFull[10];
  timenow.toCharArray(timeFull, 50);
  printStringWithShift(timeFull,15);

  //year(t_unix_date1), month(t_unix_date1), day(t_unix_date1), 
  //hour(t_unix_date1), minute(t_unix_date1), second(t_unix_date1)  
}

const char* getWord(){
  //printStringWithShift("♡", 15);
  printCharWithShift(94,15);//Star
  printCharWithShift(148,15);
  printCharWithShift(148,15);
  printCharWithShift(148,15);
  
  http.useHTTP10(true);
  http.begin("http://api.wordnik.com/v4/words.json/wordOfTheDay?hasDictionaryDef=true&limit=1&api_key=a2a73e7b926c924fad7001ca3111acd55af2ffabf50eb4ae5");
  http.GET();

  DynamicJsonDocument doc(2048);
  deserializeJson(doc, http.getStream());
  //Serial.print(http.getString());
  const char* Word = doc["word"].as<char*>();
  printStringWithShift(Word, 50);
  delay(1000);
  printStringWithShift(" >>>>> ", 15);

  const char* definition = doc["definitions"][0]["text"].as<char*>();
  printStringWithShift(definition, 50);
  
  http.end();
  return Word;
}

// =======================================================================

int showChar(char ch, const uint8_t *data)
{
  int len = pgm_read_byte(data);
  int i,w = pgm_read_byte(data + 1 + ch * len);
  for (i = 0; i < w; i++)
    scr[NUM_MAX*8 + i] = pgm_read_byte(data + 1 + ch * len + 1 + i);
  scr[NUM_MAX*8 + i] = 0;
  return w;
}

// =======================================================================
int dualChar = 0;

unsigned char convertPolish(unsigned char _c)
{
  unsigned char c = _c;
  if(c==196 || c==197 || c==195) {
    dualChar = c;
    return 0;
  }
  if(dualChar) {
    switch(_c) {
      case 133: c = 1+'~'; break; // 'ą'
      case 135: c = 2+'~'; break; // 'ć'
      case 153: c = 3+'~'; break; // 'ę'
      case 130: c = 4+'~'; break; // 'ł'
      case 132: c = dualChar==197 ? 5+'~' : 10+'~'; break; // 'ń' and 'Ą'
      case 179: c = 6+'~'; break; // 'ó'
      case 155: c = 7+'~'; break; // 'ś'
      case 186: c = 8+'~'; break; // 'ź'
      case 188: c = 9+'~'; break; // 'ż'
      //case 132: c = 10+'~'; break; // 'Ą'
      case 134: c = 11+'~'; break; // 'Ć'
      case 152: c = 12+'~'; break; // 'Ę'
      case 129: c = 13+'~'; break; // 'Ł'
      case 131: c = 14+'~'; break; // 'Ń'
      case 147: c = 15+'~'; break; // 'Ó'
      case 154: c = 16+'~'; break; // 'Ś'
      case 185: c = 17+'~'; break; // 'Ź'
      case 187: c = 18+'~'; break; // 'Ż'
      default:  break;
    }
    dualChar = 0;
    return c;
  }    
  switch(_c) {
    case 185: c = 1+'~'; break;
    case 230: c = 2+'~'; break;
    case 234: c = 3+'~'; break;
    case 179: c = 4+'~'; break;
    case 241: c = 5+'~'; break;
    case 243: c = 6+'~'; break;
    case 156: c = 7+'~'; break;
    case 159: c = 8+'~'; break;
    case 191: c = 9+'~'; break;
    case 165: c = 10+'~'; break;
    case 198: c = 11+'~'; break;
    case 202: c = 12+'~'; break;
    case 163: c = 13+'~'; break;
    case 209: c = 14+'~'; break;
    case 211: c = 15+'~'; break;
    case 140: c = 16+'~'; break;
    case 143: c = 17+'~'; break;
    case 175: c = 18+'~'; break;
    default:  break;
  }
  return c;
}

// =======================================================================

void printCharWithShift(unsigned char c, int shiftDelay) {
  c = convertPolish(c);
  if (c < ' ' || c > MAX_CHAR) return;
  c -= 32;
  int w = showChar(c, font);
  Serial.print(c);
  Serial.print("=");
  Serial.println(w);
  for (int i=0; i<w+1; i++) {
    delay(shiftDelay);
    scrollLeft();
    refreshAll();
  }
}

// =======================================================================

void printStringWithShift(const char* s, int shiftDelay){
  while (*s) {
    printCharWithShift(*s++, shiftDelay);
  }
}

// =======================================================================
unsigned int convToInt(const char *txt)
{
  unsigned int val = 0;
  for(int i=0; i<strlen(txt); i++)
    if(isdigit(txt[i])) val=val*10+(txt[i]&0xf);
  return val;
}
// =======================================================================



const char* ytHost = "www.youtube.com";
int getYTSubs(const char *channelId, int *pSubs, int *pViews)
{
  if(!pSubs || !pViews) return -2;
  WiFiClientSecure client;
  Serial.print("connecting to "); Serial.println(ytHost);
  if (!client.connect(ytHost, 443)) {
    Serial.println("connection failed");
    return -1;
  }
  client.print(String("GET /channel/") + String(channelId) +"/about HTTP/1.1\r\n" + "Host: " + ytHost + "\r\nConnection: close\r\n\r\n");
  int repeatCounter = 10;
  while (!client.available() && repeatCounter--) {
    Serial.println("y."); delay(500);
  }
  int idxS, idxE, statsFound = 0;
  *pSubs = *pViews = 0;
  while (client.connected() && client.available()) {
    String line = client.readStringUntil('\n');
    if(statsFound == 0) {
      statsFound = (line.indexOf("about-stats")>0);
    } else {
      idxS = line.indexOf("<b>");
      idxE = line.indexOf("</b>");
      String val = line.substring(idxS + 3, idxE);
      if(!*pSubs)
        *pSubs = convToInt(val.c_str());
      else {
        *pViews = convToInt(val.c_str());
        break;
      }
    }
  }
  client.stop();
  return 0;
}
