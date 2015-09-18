#include <Time.h>  

// Thanks to Michael Margolis, for his Time library and examples (see http://www.pjrc.com/teensy/td_libs_Time.html) and
// also to NickTG at Smoke and wires for the libraries and examples for the Smoke and Wires TFT shield.

#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message

//setup swtft
#include <Adafruit_GFX.h>    // Core graphics library
#include "SWTFT.h" // Hardware-specific library

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

SWTFT tft;

// T1262347200  //noon Jan 1 2010

// Unix command to sync: TZ_adjust=0;  echo T$(($(date +%s)+60*60*$TZ_adjust)) > /dev/ttyUSB0
// Alternative unix command: TZ_adjust=0;d=$(date +%s);t=$(echo "60*60*$TZ_adjust/1" | bc);echo T$(echo $d+$t | bc ) > /dev/ttyUSB0


void setup()  {
  Serial.begin(9600);
  tft.reset();

  uint16_t identifier = tft.readID();


  Serial.print(F("LCD driver chip: "));
  Serial.println(identifier, HEX);


  tft.begin(identifier);
  tft.setRotation(2);
  tft.fillScreen(RED);
  tft.fillScreen(GREEN);
  tft.fillScreen(BLACK);
}

void loop(){    
  if(Serial.available() )
  {
    processSyncMessage();
  }
  if(timeStatus() == timeNotSet){
    tft.setCursor(0,2);
    tft.setTextSize(1);
    tft.println("waiting for sync message");
  }
  else    
    digitalClockDisplay();  
  delay(1000);
}

void digitalClockDisplay(){
  // digital clock display of the time
  float starDate;
  float starTime;
  float julianDate;
  int st;
  tft.fillRoundRect(0, 0, 240, 180, 30, BLUE);

  tft.setCursor(1, 3);
  tft.setTextSize(4);
  tft.setTextColor(GREEN);
  tft.print(" ");
  tft.print(hour());
  tft.print(":");
  printDigits(minute());
  tft.print(":");
  printDigits(second());
  tft.println();
  printDigits(day());
  tft.print(" ");
  printDigits(month());
  tft.print(" ");
  tft.print(year());
  tft.println();
  tft.setTextSize(3);
  julianDate = now();
  starDate = int(julianDate / 86400);
  starTime = ((hour()*3600.00)+(minute()*60.00)+second())/86400.00;
  tft.setTextColor(RED);
  // tft.println("Unix Time");
  // tft.println(julianDate, 0);
  tft.setTextColor(YELLOW);
  tft.println("  StarDate");
  tft.print(" ");
  tft.print(starDate, 0);
  tft.print(":");
  tft.println(starTime * 100000, 0);

  int bcd = hour() / 10;
  displayNybble(0, 200, 8, RED, bcd);
  bcd = hour() % 10;
  displayNybble(0, 218, 8, RED, bcd);
  bcd = minute() / 10;
  displayNybble(0, 240, 8, YELLOW, bcd);
  bcd = minute() % 10;
  displayNybble(0, 258, 8, YELLOW, bcd);
  bcd = second() / 10;
  displayNybble(0, 280, 8, GREEN, bcd);
  bcd = second() % 10;
  displayNybble(0, 298, 8, GREEN, bcd);
  Serial.println("----");

  for(int j=0; j < 5; j++) {
    starTime = (starTime * 10);
    st = starTime;
    st = st % 10;
    displayNybble(120, 200 + (j * 18), 8, BLUE, st);
  }
  Serial.println("----");

}

void printDigits(int digits){
  // utility function for digital clock display: prints leading 0

  if(digits < 10)
    tft.print('0');
  tft.print(digits);
}

void processSyncMessage() {
  // if time sync available from serial port, update time and return true
  while(Serial.available() >=  TIME_MSG_LEN ){  // time message consists of header & 10 ASCII digits
    char c = Serial.read() ;
    Serial.print(c);  
    if( c == TIME_HEADER ) {      
      time_t pctime = 0;
      for(int i=0; i < TIME_MSG_LEN -1; i++){  
        c = Serial.read();          
        if( c >= '0' && c <= '9'){  
          pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
        }
      }  
      setTime(pctime);   // Sync Arduino clock to the time received on the serial port
      tft.fillScreen(RED);
      tft.fillScreen(GREEN);
      tft.fillScreen(BLACK);
    }  
  }
}

void displayNybble(int x, int y, int ledSize, int colour, int value) {
  tft.fillRect(x , y - ledSize, x + ledSize * 11, y + ledSize, BLACK);

  value = value % 16;
  Serial.print(value);
  if(value >= 8) 
    tft.fillCircle(x + ledSize * 1.5, y, ledSize, colour);
  else
    tft.drawCircle(x + ledSize * 1.5, y, ledSize, colour);

  value = value % 8;
  Serial.print(value);
  if(value >= 4) 
    tft.fillCircle(x + ledSize * 4, y, ledSize, colour);
  else
    tft.drawCircle(x + ledSize * 4, y, ledSize, colour);

  value = value % 4;
  Serial.print(value);
  if(value >= 2) 
    tft.fillCircle(x + ledSize * 6.5, y, ledSize, colour);
  else
    tft.drawCircle(x + ledSize * 6.5, y, ledSize, colour);

  value = value % 2;
  Serial.println(value);
  if(value >= 1) 
    tft.fillCircle(x + ledSize * 9, y, ledSize, colour);
  else
    tft.drawCircle(x + ledSize * 9, y, ledSize, colour);

}




