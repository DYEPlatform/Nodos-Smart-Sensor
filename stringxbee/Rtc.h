#ifndef Rtc_h
#define Rtc_h

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "Wire.h"

#define DS1307_I2C_ADDRESS 0x68
#define isleap(x)    ((((x)%4) == 0 && ((x)%100) != 0) || ((x)%400) == 0)
#define GMT_OFFSET   -4
class Rtc{

   public:
   
      //constructors
      Rtc();
      Rtc(char s, char m, char h, char dow, char dom, char mo, char y, char mask);
      
      
      //functions
      void SetDate(char second,        // 0-59
                         char minute,        // 0-59
                         char hour,          // 1-23
                         char dayOfWeek,     // 1-7
                         char dayOfMonth,    // 1-28/29/30/31
                         char month,         // 1-12
                         char year);
      void GetDate();
      void WriteDs1307(char address,char data);
      
      char * Date();
      char* Time();
      char ReadDs1307(char address);
      char * ftoa(char *a, double f, int precision);
  
  
      unsigned long UnixTime();    
      //variables
      char second, minute, hour, dayOfWeek, dayOfMonth, month, year;

private:
      char DecToBcd(char val);
      char BcdToDec(char val);
      

};
#endif
