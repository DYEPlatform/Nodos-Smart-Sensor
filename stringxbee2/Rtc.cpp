#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Wire.h"
#include "Rtc.h"


///////////////////////////////////////
//public functions



/**
 * 1) Sets the date and time on the ds1307
 * 2) Starts the clock
 * 3) Sets hour mode to 24 hour clock
 * Assumes you're passing in valid numbers
 * @param char second
 *    Seconds to assign to RTC
 * @param char minute
 *    Minutes to assign to RTC
 * @param char hour
 *    Hours to assign to RTC
 * @param char dayOfWeek
 *    Day of week to assign to RTC
 * @param char dayOfMonth
 *    Day of Month to assign to RTC
 * @param char month
 *    Month to assign to RTC
 * @param char year
 *    Year to assign to RTC (11 is 2011)
 * @return void
 */
void Rtc::SetDate(char second,        // 0-59
char minute,        // 0-59
char hour,          // 1-23
char dayOfWeek,     // 1-7
char dayOfMonth,    // 1-28/29/30/31
char month,         // 1-12
char year)          // 0-99
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(DecToBcd(0));
  Wire.write(DecToBcd(second));    // 0 to bit 7 starts the clock
  Wire.write(DecToBcd(minute));
  Wire.write(DecToBcd(hour));      // If you want 12 hour am/pm you need to set
  // bit 6 (also need to change readDateDs1307)
  Wire.write(DecToBcd(dayOfWeek));
  Wire.write(DecToBcd(dayOfMonth));
  Wire.write(DecToBcd(month));
  Wire.write(DecToBcd(year));
  Wire.endTransmission();
}


/**
 * Class constructor that sets the date, Assumes you're passing in valid numbers.
 * @param char s
 *    Seconds to assign to RTC
 * @param char m
 *    Minutes to assign to RTC
 * @param char h
 *    Hours to assign to RTC
 * @param char dow
 *    Day of week to assign to RTC
 * @param char dom
 *    Day of Month to assign to RTC
 * @param char mo
 *    Month to assign to RTC
 * @param char y
 *    Year to assign to RTC (11 is 2011)
 * @param char mask
 *    After setting time, this mask is written to RTC to avoid future writings
 * @return void
 */
Rtc::Rtc(char s,char m,char h,char dow,char dom,char mo,char y,char mask){
  Wire.begin();
  if(ReadDs1307(0x08) != mask){
    second     = s;
    minute     = m;
    hour       = h;
    dayOfWeek  = dow;
    dayOfMonth = dom;
    month      = mo;
    year       = y;
    SetDate(second, minute, hour, dayOfWeek, dayOfMonth, month, year);//send data to RTC chip
  }
  WriteDs1307(0x08,mask); 
}
/**
 * Class constructor
 * don't set the date on the chip
 * @param none
 */
Rtc::Rtc(){
  Wire.begin();
}



/**
 * Return the date of last reading (using GetDate()) in a string
 * 
 * @param none
 * @return a string containing the current date in format dd/mm/yyyy
 */
char* Rtc::Date()
{
  char a[80];
  char * buf=a;
  if(dayOfMonth < 10)
    *(buf++) = '0';
  itoa(dayOfMonth,buf,10);
  buf+=strlen(buf);
  *(buf++) = '/';
  if(month < 10)
    *(buf++) = '0';
  itoa(month,buf,10);
  buf+=strlen(buf);
  strcpy(buf,"/20");
  buf+=strlen(buf);
  itoa(year,buf,10);
  return a;
}
/**
 * Return the hour of last reading (using GetDate()) in a string
 * 
 * @param none
 * @return a string containing the current time in format hh:mm:ss
 */
char* Rtc::Time()
{
  char a[80];
  char * buf=a;
  if(hour < 10)
    *(buf++) = '0';
  itoa(hour,buf,10);
  buf+=strlen(buf);
  *(buf++) = ':';
  if(minute < 10)
    *(buf++) = '0';
  itoa(minute,buf,10);
  buf+=strlen(buf);
  *(buf++) = ':';
  if(second < 10)
    *(buf++) = '0';
  itoa(second,buf,10);  
  return a; 
}

/**
 * Gets the date and time from the ds1307 and save them into instance variables
 * 
 * @param none
 * 
 */
void Rtc::GetDate()
{
  // Reset the register pointer
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(DecToBcd(0));
  Wire.endTransmission();

  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);

  // A few of these need masks because certain bits are control bits
  second     = BcdToDec(Wire.read() & 0x7f);
  minute     = BcdToDec(Wire.read());
  hour       = BcdToDec(Wire.read() & 0x3f);  // Need to change this if 12 hour am/pm
  dayOfWeek  = BcdToDec(Wire.read());
  dayOfMonth = BcdToDec(Wire.read());
  month      = BcdToDec(Wire.read());
  year       = BcdToDec(Wire.read());
}

/**
 * 
 * 
 * @param char address
 * @param char data
 * 
 */

void Rtc::WriteDs1307(char address,char data)
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();  
}

/**
 * 
 * 
 * @param char address
 * @param char data
 * 
 */
char Rtc::ReadDs1307(char address)
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 1);
  return Wire.read();
}


/**
 * float to ascii
 * 
 * @param char *a
 * @param char f
 * @int precision
 * 
 */
char * Rtc::ftoa(char *a, double f, int precision)
{
  long p[] = {
    0,10,100,1000,10000,100000,1000000,10000000,100000000  };

  char *ret = a;
  long heiltal = (long)f;
  itoa(heiltal, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long desimal = abs((long)((f - heiltal) * p[precision]));
  itoa(desimal, a, 10);
  return ret;
}





///////////////////////////////////////
//private functions
// Convert normal decimal numbers to binary coded decimal
char Rtc::DecToBcd(char val)
{
  return ( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
char Rtc::BcdToDec(char val)
{
  return ( (val/16*10) + (val%16) );
}



unsigned long Rtc::UnixTime()
{
	unsigned long days;
        unsigned int yr=year+2000;
	const unsigned long monthcount[] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
	/* Compute days */
	days = (yr - 1970) * 365 + monthcount[month] + dayOfMonth - 1;
	/* Compute for leap year */
        if(month <= 2) 
            yr--;
	for ( ; yr >= 1970; yr--){
		if (isleap((yr)))
			days++;
//                Serial.println("=)");
        }
	/* Plus the time */
	return second + 60 * (minute + 60 * (days * 24 + hour - GMT_OFFSET));


  	/* Number of days per month *
	unsigned long days;
        unsigned char yr=year;
	const unsigned long monthcount[] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

	/* Compute days *
	days = (2000+year - 1970) * 365 + monthcount[month] + dayOfMonth - 1;

	/* Compute for leap year *
	for (month <= 2 ? yr-- : 0; yr >= 1970; yr--)
		if (isleap(yr))
			days++;

	/* Plus the time *
	return second + 60 * (minute + 60 * (days * 24 + hour - GMT_OFFSET));

    Serial.println("----------------");
  	/* Number of days per month *
	unsigned long days;
        unsigned int yr=reloj.year+2000;
	const unsigned long monthcount[] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
	/* Compute days *
	days = (2000+reloj.year - 1970) * 365 + monthcount[reloj.month] + reloj.dayOfMonth - 1;
        Serial.println(days,DEC);
        Serial.println(yr,DEC);
	/* Compute for leap year *
        if(reloj.month <= 2) 
            yr--;
	for ( ; yr >= 1970; yr--){
		if (isleap((yr)))
			days++;
//                Serial.println("=)");
        }
        Serial.println(yr,DEC);
        Serial.println(days,DEC);
	/* Plus the time *
	Serial.println(reloj.second + 60 * (reloj.minute + 60 * (days * 24 + reloj.hour - GMT_OFFSET)),DEC);
        Serial.println("----------------");

*/

}



