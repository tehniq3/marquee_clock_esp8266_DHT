//V.3 - data convertita in numero e calcolo ora legale
//V.4 - data localizzata
//V.6 - temperature e umidita'
//V.7 - animated clock corretto + secondi +1,5
//V.8 - taratura termometro e igrometro
//v.9 - corretta ora legale
// receiced from Fabrizio Croce - fabrizio_croce@hotmail.com
// small changes by Nicu FLORICA (niq_ro) - nicu.florica@gmail.com
//v.10 - automatic brightness amd some small changes in sisplay style (add degree sign, etc)

#include "Arduino.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h> //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

WiFiClient client;

#define NUM_MAX 6  // or 4
#define ROTATE 0 // 0, 90 or 270 degree

// for NodeMCU 1.0
#define DIN_PIN 13  // D7
#define CS_PIN  15  // D8
#define CLK_PIN 14  // D5

#include "max7219.h"
#include "fonts.h"

#include "DHT.h" // sensor DHT, I use library salved here: https://github.com/tehniq3/used_library

// DHT sensor
#define DHTPIN 12     // what pin we're connected to // GPIO 12 = D6
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE, 11); // for ESP8266

//DHT
float temperature = 0;
int temp = 0;
byte minus = 0;
int umidity = 0;
int poz, poz2;
int mult = 0;

float temp_offset = 0; // taratura termometro
float rh_offset = 0; // taratura igrometro

char TempLabel[] = "    temperatura:  "; // Fixxed text
char UmiLabel[] = "    umiditate:  "; // Fixxed text

//modifica test ip

const char ssid[] = "bbk2";  //  your network SSID (name)
const char password[] = "internet2";       // your network password

// Localized Months and DoWeek

String M_arr[13] = {" ", "Ianuarie", "Februarie", "Martie", "Aprilie", "Mai", "Iunie", "Iulie", "August", "Septembrie", "Octombrie", "Noiembrie", "Decembrie"};
String Dow_arr[8] = {" ", "Luni", "Marti", "Miercuri", "Joi", "Vineri", "Sambata", "Duminica"};

// speed text
int viteza = 45;  // 45 - slow, 30 - fast

// autobrightness 
int lumina = 0;
int lumina0 = 1;

void setup() 
{
  Serial.begin(115200);

  initMAX7219(); //
  sendCmdAll(CMD_SHUTDOWN,1); //
  sendCmdAll(CMD_INTENSITY,0); // brightness from 0 to 7
   
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  printStringWithShift("Conectare" , viteza); //
  delay (1000);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  
  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());

  printStringWithShift((String("  IP:")+WiFi.localIP().toString()).c_str(), viteza);//
  delay(1000);//

  // DHT
dht.begin();
temperature = dht.readTemperature();
umidity = dht.readHumidity();

Serial.print(temperature);
Serial.println(" gr.Celsius");

Serial.print(umidity);
Serial.println(" %RH");

  if (temperature < 0)
   {
    temp = -10*temperature;
    minus = 1;
   }
   else
   {
    minus = 0;
    temp = 10*temperature;
   }
    
}


// =============================DEFINE VARS==============================
#define MAX_DIGITS 20
byte dig[MAX_DIGITS]={0};
byte digold[MAX_DIGITS]={0};
byte digtrans[MAX_DIGITS]={0};
int updCnt = 0;
int dots = 0;
long dotTime = 0;
long clkTime = 0;
int dx=0;
int dy=0;
byte del=0;
int h,m,s;
float utcOffset = +2; // ==> your timezone
long localEpoc = 0;
long localMillisAtUpdate = 0;
int day, month, year, dayOfWeek;
int monthnum = 0;
int downum = 0;
int summerTime = 0;
String date; 
String dayloc; 
String monthloc; 



// =======================================================================

void loop()
{
  contrast(); // //autobrightness 
  
//print data
 
  if(millis()-clkTime > 30000 && !del && dots) { // clock for 30s, then scrolls for about 30s
   // Reading temperature or humidity 
    int umidity = dht.readHumidity();
    float temperature = dht.readTemperature(); 

contrast(); // //autobrightness     
   printStringWithShift("            " , viteza);       //Space before
contrast(); // //autobrightness    
   printStringWithShift(date.c_str() , viteza);         //data1
/*
// Reading temperature or humidity 
    int umidity = dht.readHumidity();
    float temperature = dht.readTemperature(); 
 */   
    umidity = umidity + rh_offset;
    temperature = temperature + temp_offset;
    
    Serial.println("Temp e Umid corretti");
    Serial.println(temperature);
    Serial.println(umidity);
    
    int t2 = 10*temperature; 

contrast(); // //autobrightness 
// print temperature

// http://www.arduino-hacks.com/converting-integer-to-character-vice-versa/
char c[3], d[2]; 
String str1, str2;
int t2z = t2/10;
int t2u = t2 - t2z*10;
str1=String(t2z);
str1.toCharArray(c,3);
str2=String(t2u);
str2.toCharArray(d,2);
printStringWithShift(TempLabel, viteza);  // Send scrolling Text
printStringWithShift("+", viteza);
printStringWithShift(c, viteza);
printStringWithShift(",", viteza);
printStringWithShift(d, viteza);
printStringWithShift(" ~C", viteza);  // $ is replaced by degree sign

contrast(); // //autobrightness 
// print umidity

// http://www.arduino-hacks.com/converting-integer-to-character-vice-versa/
char b[3];
String str;
str=String(umidity);
str.toCharArray(b,3);
printStringWithShift(UmiLabel, viteza);  // Send scrolling Text
printStringWithShift(b , viteza);
printStringWithShift(" %", viteza);

contrast(); // //autobrightness 
 
   printStringWithShift("                 " , viteza);  //Space after
   delay(200);                          
contrast(); // //autobrightness       
    updCnt--;
    clkTime = millis();
  }
  if(millis()-dotTime > 500) {
    dotTime = millis();
    dots = !dots;
  }

  if(updCnt<=0) { // every 10 scrolls, ~450s=7.5m
    updCnt = 2;
    Serial.println("Getting data ...");
    // clr();
    printStringWithShift("!" , viteza);
    getTime();
    Serial.println("Data loaded");
    clkTime = millis();
  }
  
  updateTime(); // get time

// print clock
  showAnimClock();
  //showSimpleClock();
  
} // end main loop

// =======================================================================

void showSimpleClock()
{
  dx=dy=0;
  clr();
  showDigit(h/10,  0, dig6x8);
  showDigit(h%10,  8, dig6x8);
  showDigit(m/10, 17, dig6x8);
  showDigit(m%10, 25, dig6x8);
  showDigit(s/10, 34, dig6x8);
  showDigit(s%10, 42, dig6x8);
  setCol(15,dots ? B00100100 : 0);
  setCol(32,dots ? B00100100 : 0);
  refreshAll();
}

// =======================================================================

void showAnimClock()
{
  byte digPos[6]={0,8,17,25,34,42};
  int digHt = 12;
  int num = 6; 
  int i;
  if(del==0) {
    del = digHt;
    for(i=0; i<num; i++) digold[i] = dig[i];
    dig[0] = h/10 ? h/10 : 10;
    dig[1] = h%10;
    dig[2] = m/10;
    dig[3] = m%10;
    dig[4] = s/10;
    dig[5] = s%10;
    for(i=0; i<num; i++)  digtrans[i] = (dig[i]==digold[i]) ? 0 : digHt;
  } else
    del--;
  
  clr();
  for(i=0; i<num; i++) {
    if(digtrans[i]==0) {
      dy=0;
      showDigit(dig[i], digPos[i], dig6x8);
    } else {
      dy = digHt-digtrans[i];
      showDigit(digold[i], digPos[i], dig6x8);
      dy = -digtrans[i];
      showDigit(dig[i], digPos[i], dig6x8);
      digtrans[i]--;
    }
  }
  dy=0;
  setCol(15,dots ? B00100100 : 0);
  setCol(32,dots ? B00100100 : 0);
  refreshAll();
  delay(10); //default 30
}

// =======================================================================

void showDigit(char ch, int col, const uint8_t *data)
{
  if(dy<-8 | dy>8) return;
  int len = pgm_read_byte(data);
  int w = pgm_read_byte(data + 1 + ch * len);
  col += dx;
  for (int i = 0; i < w; i++)
    if(col+i>=0 && col+i<8*NUM_MAX) {
      byte v = pgm_read_byte(data + 1 + ch * len + 1 + i);
      if(!dy) scr[col + i] = v; else scr[col + i] |= dy>0 ? v>>dy : v<<-dy;
    }
}

// =======================================================================

void setCol(int col, byte v)
{
  if(dy<-8 | dy>8) return;
  col += dx;
  if(col>=0 && col<8*NUM_MAX)
    if(!dy) scr[col] = v; else scr[col] |= dy>0 ? v>>dy : v<<-dy;
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
  /*
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
  */
  return c;
}


// =======================================================================

void printCharWithShift(unsigned char c, int shiftDelay) {
  c = convertPolish(c);
  if (c < ' ' || c > '~'+25) return;
  c -= 32;
  int w = showChar(c, font);
  for (int i=0; i<w+1; i++) {
    delay(shiftDelay);
    scrollLeft();
    refreshAll();
  }
}

// =======================================================================

void printStringWithShift(const char* s, int shiftDelay){
  while (*s) {
    printCharWithShift(*s, shiftDelay);
    s++;
  }
}
// =======================================================================

void getTime()
{
  WiFiClient client;
  if (!client.connect("www.google.com", 80)) {
    Serial.println("connection to google failed");
    return;
  }

  client.print(String("GET / HTTP/1.1\r\n") +
               String("Host: www.google.com\r\n") +
               String("Connection: close\r\n\r\n"));
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) {
    delay(500);
    //Serial.println(".");
    repeatCounter++;
  }

  String line;
  client.setNoDelay(false);
  while(client.connected() && client.available()) {
    line = client.readStringUntil('\n');
    line.toUpperCase();
    // Serial.println(line); //test stringa
    if (line.startsWith("DATE: ")) {
      //date = "     "+line.substring(6, 22);
      date =line.substring(6, 22);
      date.toUpperCase();

      Serial.println(date); //check
          
      int day = atoi (date.substring(5,7).c_str()); //day number
      //Serial.print("Giorno: " );
      //Serial.println(day);
      
      int year = 2000 + atoi (date.substring(13,16).c_str()); //year number
      //Serial.print("Anno: " );
      //Serial.println(year);

      monthnum = month2index(date.substring(8,11)); // convert month in mumber
      //Serial.print("Mese: " );
      //Serial.println(monthnum); //check
      month = monthnum; //check

      downum = dow2index(date.substring(0,3)); // convert dow in mumber
      
      //Serial.println(downum); //check
      dayOfWeek = downum; //check
      
      Serial.print("Giorno: " );
      Serial.println(day); //check
      Serial.println(month); //check
      Serial.print("Anno: " );
      Serial.println(year); //check
      Serial.print("Giorno Sett: " );
      Serial.println(dayOfWeek); //check

      monthloc = M_arr[month]; // localize month
      dayloc = Dow_arr[dayOfWeek]; // localize DayOfWeek

      Serial.println(dayloc); //check
      Serial.println(monthloc); //check

      date = dayloc + ",   " + day + "-" + monthloc + "-" + year;
      Serial.print("Data: " );
      Serial.println(date); //check

      
 //   decodeDate(date); //data3
      h = line.substring(23, 25).toInt();
      m = line.substring(26, 28).toInt();
      s = line.substring(29, 31).toInt()+1.5; //correzione ora
  
      // summerTime = checkSummerTime();

// calcolo ora legale

      if(month>3 && month<10) summerTime=1;
      if(month==3 && day>=31-(((5*year/4)+4)%7) ) summerTime=1;
      if(month==10 && day<31-(((5*year/4)+1)%7) ) summerTime=0;
       
      Serial.print("Summertime: ");
      Serial.println(summerTime);
      
        if(h+utcOffset+summerTime>23) {
          if(++day>31) { day=1; month++; };  // needs better patch
          if(++dayOfWeek>7) dayOfWeek=1; 
        }
      localMillisAtUpdate = millis();
      localEpoc = (h * 60 * 60 + m * 60 + s);
    }
  }
  client.stop();
}

// =======================================================================

//convert month string in month number 

int month2index (String month) 
{
        String months[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
        for (int i = 0; i < 12; i++) {
                if (months[i] == month)
                        return i + 1;  
                        
        }
        return 0;
}

// =======================================================================

//convert dow string in dow number 

int dow2index (String dayOfWeek) 
{
        String dows[7] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
        for (int i = 0; i < 12; i++) {
                if (dows[i] == dayOfWeek)
                        return i + 1;  
                        
        }
        return 0;
}

// =======================================================================

//check if it is summer time

// int checkSummerTime()
 
//{

  //if(month>3 && month<10) return 1;
  //if(month==3 && day>=31-(((5*year/4)+4)%7) ) return 1;
  //if(month==10 && day<31-(((5*year/4)+1)%7) ) return 0;
  //return 0;

 // return 1;
 
//}



// =======================================================================



void updateTime()
{
  long curEpoch = localEpoc + ((millis() - localMillisAtUpdate) / 1000);
 // long epoch = round(curEpoch + 3600 * (utcOffset+summerTime) + 86400L) % 86400L;
   long epoch = round(curEpoch + 3600 * (utcOffset+summerTime) + 86400L);
  h = ((epoch  % 86400L) / 3600) % 24;
  m = (epoch % 3600) / 60;
  s = epoch % 60;
}

// =======================================================================

void contrast()
{
 lumina = analogRead(A0);          //read light level
     lumina = map(lumina, 0, 1023, 0, 7); //mapping value 
      if (lumina0 != lumina)
      {
      sendCmdAll(CMD_INTENSITY, lumina);
      Serial.print("brightness = ");
      Serial.print(lumina);
      Serial.println (" / 7");
      lumina0 = lumina;      
      Serial.println ("change brightness...");
      }
}
