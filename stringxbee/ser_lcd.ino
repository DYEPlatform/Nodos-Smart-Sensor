/*
Basada parcialmente en la Libreria serLCD de Sparkfun
Adaptada para trabajar con LCD grafico 128x64 LED STN Backlight
(Solo usa el PIN TX)
*/
// since the LCD does not send data back to the Arduino, we should only define the txPin

const int LCDdelay=10;  // conservative, 2 actually works

// wbp: goto with row [0:63] & column [0:127]
void lcdPosition(int x, int y) {
  esGLCD.write(0x7C);   //command flag
  esGLCD.write(24);
  esGLCD.write(x);    //position (improve)
  delay(LCDdelay);
  esGLCD.write(0x7C);   //command flag
  esGLCD.write(25);
  esGLCD.write(63-y);    //position (improve)
  delay(LCDdelay);
}
void clearLCD(){
  esGLCD.write(0x7C);   //command flag
  //esGLCD.write(0x00);   //clear command.
  esGLCD.write(byte(0x00));  
//  esGLCD.write('0');   //clear command.
  delay(LCDdelay);
}
void backlightOn() {  //turns on the backlight
  esGLCD.write(0x7C);   //command flag 
  esGLCD.write(2);      //for backlight stuff
  esGLCD.write(100);    //light level.
  delay(LCDdelay);
}

void backlightlum(int lum) {  //turns  the backlight to lum [0:100]
  esGLCD.write(0x7C);   //command flag 
  esGLCD.write(2);      //for backlight stuff
  esGLCD.write(lum);    //light level.
  delay(LCDdelay);
}

void backlightOff(){  //turns off the backlight
  esGLCD.write(0x7C);   //command flag 
  esGLCD.write(2);      //for backlight stuff
  esGLCD.write(byte(0x00));     //light level for off.
  delay(LCDdelay);
}
void serCommand(){   //a general function to call the command flag for issuing all other commands   
  esGLCD.write(0x7C);
}

void serbackspace(){
  esGLCD.write(0x7C);    //command flag 
  esGLCD.write(8);     //light level for off.
  delay(LCDdelay);
}

void serSetPixel(int x, int y, int s_r){ //x [0:127]; y [0:63]; s_r [0:1]
  esGLCD.write(0x7C);   //command flag 
  esGLCD.write(16);     //for pixel stuff
  
  esGLCD.write(x);     //for x coord
  esGLCD.write(y);     //for y coor
  esGLCD.write(s_r);     //for pixel on/off
  
  delay(LCDdelay);
}

void serCircle(int s_r, int x, int y, int r){ //s_r [0:1]; x [0:127]; y [0:63]; r [0:127]
  esGLCD.write(0x7C);   //command flag 
  esGLCD.write(3);     //for pixel stuff
  
  esGLCD.write(x);     //for x coord
  esGLCD.write(y);     //for y coor
  esGLCD.write(r);     //for radius
  esGLCD.write(s_r);   //for draw or erase
   
  delay(LCDdelay);
}

 //s_r [0:1]; x [0:127]; y [0:63]; r [0:127]
void serEraseBlock(int x1, int y1, int x2, int y2){
  esGLCD.write(0x7C);   //command flag 
  esGLCD.write(5);     //for erase stuff
  
  esGLCD.write(x1);   //for draw or erase
  esGLCD.write(y1);     //for x coord
  esGLCD.write(x2);     //for y coord
  esGLCD.write(y2);     //for radius
  
  delay(LCDdelay);
}

// x1 [0:127]; y1 [0:63]; x2 [x1:127]; y2 [y1:63];
void serBox(int x1, int y1, int x2, int y2){ 
  esGLCD.write(0x7C);   //command flag 
  esGLCD.write(15);     //for box  stuff
  
  esGLCD.write(x1);     //for x1 coord
  esGLCD.write(y1);     //for y1 coord
  esGLCD.write(x2);     //for x2 coord
  esGLCD.write(y2);     //for y2 coord
  
  delay(LCDdelay);
}

// Line from x1,y1 to x2,y2
//s_r [0:1]; x1 [0:127]; y1 [0:63]; x2 [x1:127]; y2 [y1:63];
void serLine(int s_r, int x1, int y1, int x2, int y2){ 
  esGLCD.write(0x7C);   //command flag 
  esGLCD.write(12);     //for box  stuff
  
  esGLCD.write(x1);     //for x1 coord
  esGLCD.write(y1);     //for y1 coord
  esGLCD.write(x2);     //for x2 coord
  esGLCD.write(y2);     //for y2 coord
  esGLCD.write(s_r);     //for x1 coord
  
  delay(LCDdelay);
}

void serReserveMode(int s_r, int x1, int y1, int x2, int y2){ 
  esGLCD.write(0x7C);   //command flag 
  esGLCD.write(0x14);     //for box  stuff
  delay(LCDdelay);
}

void resetLCD() 
{	/* Resets the LCD, clears the screen and resets x,y offsets to 0,0
	 Example: lcd.resetLCD();
	 */
	esGLCD.write(0x7C);
	esGLCD.write(0x06);
}

void toggleFont()
{	/* Toggles between the standard and AUX font. Only affects text written after the command.
	 Example: lcd.toggleFont();
	 */
	esGLCD.write(0x7C);
	esGLCD.write(0x08);
}

void drawFilledBox(int x1, int y1, int x2, int y2, byte fill)
{	/* Same as drawBox, but accepts a fill byte. 0xFF for black, 0x00 for white. 
	 Other values can be used to create patterns (like 0x55). 
	 Example: drawFilledBox(10,10,20,20,0xFF);
	 */
	esGLCD.write(0x7C);
	esGLCD.write(0x12);
	esGLCD.write(x1);
	esGLCD.write(y1);
	esGLCD.write(x2);
	esGLCD.write(y2);
	esGLCD.write(fill);
	
}



void drawData(int x, int y, int mode, int w, int h, byte *data)
{	/* Allows you to draw graphics to the screen like sprites, but the data doesn't have to be uploaded first, and there is no size
	 restrictions.(other than the drawable area of the display). 
	 Consult with firmware README for more info
	 Example:
	 byte data[]={0x00,0x12.....};
	 lcd.drawData(0,0,4,128,64,data); 
	 */
	esGLCD.write(0x7C);
	esGLCD.write(0x16);
	esGLCD.write(x);
	esGLCD.write(y);
	esGLCD.write(mode);
	esGLCD.write(w);
	esGLCD.write(h);
	int length = w*h/8;
	for(x = 0; x < length; x++) {
		esGLCD.write(data[x]);
	}
}
void gotoLine(int line)
{	/* Uses the gotoPosition function to select "line" 1-8 on the display. 
	 Text can be written between these lines using gotoPosition. This function makes it simpler.
	 Example: lcd.gotoLine(2);
	 */
	int y;
	if(line > 8) {
		line = 8;
	}
	else if(line < 1) {
		line = 1;
	}
	else {
		y = -8 + line * 8;
		lcdPosition(1,y);
	}
}
void serLCDSetup()
{
  clearLCD();
  lcdPosition(0,0);
  esGLCD.write("Arduino Energy Shield");
}

