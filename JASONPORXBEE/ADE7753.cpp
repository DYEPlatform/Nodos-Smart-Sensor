
/* ==================================================================================
File:       ADE7753.cpp
                    
Author:	Eduard Martin
        MCI Electronics
        www.olimex.cl
        
Description:  Class constructor and function implementation for XXX


Target Device: Arduino Duemilanove, Uno, Mega
==================================================================================
Copyright 2010 MCI electronics

Licensed under the Apache License, 
Version 2.0 (the "License"); you may not use this file except in compliance 
with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for
the specific language governing permissions and limitations under the License.

// ==================================================================================
//
// Original by E.M.U.
//
//  Ver | dd mmm yyyy | Author  | Description
// =====|=============|========|=====================================================
// 1.00 | 19 Nov 2010 | E.M    | First release 
// ==================================================================================*/
// 1.10 | 13 Ene 2011 | S.D    | 
// ==================================================================================*/
// 1.20 | 29 Feb 2012 | R.L    | 
// ==================================================================================*/
//Some functions were extracted from Ardugrid7753 proyect (http://code.google.com/p/ardugrid7753/)

#include "ADE7753.h"
#include <string.h>
#include <avr/pgmspace.h>
#include <SPI.h>


/**
 * Class constructor, sets chip select pin (CS define) and SPI communication with arduino.
 * @param none
 * @return void
 */

 ADE7753::ADE7753() {
  
  pinMode(CS,OUTPUT);
  digitalWrite(CS, HIGH);//disabled by default
  // SPI Init
  SPI.setDataMode(SPI_MODE2);
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setBitOrder(MSBFIRST);
  SPI.begin();
  
  
 // Serial.begin(9600);//enable serial port
  
  delay(10);
}


/*****************************
 *
 * private functions
 *
 *****************************/

/**
 * Enable chip, setting low ChipSelect pin (CS)
 * @param none
 *
 */
void ADE7753::enableChip(void){
  digitalWrite(CS,LOW);
}


/**
 * Disable chip, setting high ChipSelect pin (CS)
 * @param none
 *
 */
void ADE7753::disableChip(void){
  digitalWrite(CS,HIGH);  
}


/**
 * Read 8 bits from the device at specified register
 * @param char containing register direction
 * @return char with contents of register
 *
 */
unsigned char ADE7753::read8(char reg){
    enableChip();
    delayMicroseconds(5);
    SPI.transfer(reg);
    delayMicroseconds(5);
    return (unsigned long)SPI.transfer(0x00);
    disableChip();
}


/**
 * Read 16 bits from the device at specified register
 * @param char containing register direction
 * @return int with contents of register
 *
 */
unsigned int ADE7753::read16(char reg){
    enableChip();
    unsigned char b1,b0;
    delayMicroseconds(5);
    SPI.transfer(reg);
    delayMicroseconds(5);
    b1=SPI.transfer(0x00);
    delayMicroseconds(5);
    b0=SPI.transfer(0x00);
    return (unsigned int)b1<<8 | (unsigned int)b0;
    disableChip();
}


/**
 * Read 24 bits from the device at specified register
 * @param: char containing register direction
 * @return: char with contents of register
 *
 */
unsigned long ADE7753::read24(char reg){
    enableChip();
    unsigned char b2,b1,b0;
    delayMicroseconds(10);
    SPI.transfer(reg);
    delayMicroseconds(25);
    b2=SPI.transfer(0x00);
    delayMicroseconds(5);
    b1=SPI.transfer(0x00);
    delayMicroseconds(5);
    b0=SPI.transfer(0x00);
    return (unsigned long)b2<<16 | (unsigned long)b1<<8 | (unsigned long)b0;
    disableChip();
}



/**
 * write8: Write 8 bits to the device at specified register
 * @param reg char containing register direction
 * @param data char, 8 bits of data to send
 *
 */
void ADE7753::write8(char reg, char data){
    enableChip();
    //we have to send a 1 on the 8th bit in order to perform a write
    reg |= WRITE;
    delayMicroseconds(10);
    SPI.transfer((unsigned char)reg);          //register selection
    delayMicroseconds(5);
    SPI.transfer((unsigned char)data);
    delayMicroseconds(5);
    disableChip();
}


/**
 * write16: Write 16 bits to the device at specified register
 * @param reg: char containing register direction
 * @param data: int, 16 bits of data to send
 *
 */
void ADE7753::write16(char reg, int data){
    enableChip();
    char aux=reg;
    unsigned char data0=0,data1=0;
    reg |= WRITE;
    //split data
    data0 = (unsigned char)data;
    data1 = (unsigned char)(data>>8);
    
    //register selection, we have to send a 1 on the 8th bit to perform a write
    delayMicroseconds(10);
    SPI.transfer((unsigned char)reg);    
    delayMicroseconds(5);    
    //data send, MSB first
    SPI.transfer((unsigned char)data1);
    delayMicroseconds(5);
    SPI.transfer((unsigned char)data0);  
    delayMicroseconds(5);
    disableChip();
}

/*****************************
 *
 *     public functions
 *
 *****************************/


/**
 * In general:
 * @params:  void
 * @return: register content (measure) of the proper type depending on register width
 */

/**
 * This read-only register contains the sampled waveform data from either Channel 1,
 * Channel 2, or the active power signal. The data source and the length of the waveform 
 * registers are selected by data Bits 14 and 13 in the mode register.
 * @param none
 * @return long with the data (24 bits 2-complement signed).
 */
long ADE7753::getWaveform(void){
    return read24(WAVEFORM);
}
/**
 * Active power is accumulated (integrated) over time in this 24-bit, read-only register
 * @param none
 * @return long with the data (24 bits 2-complement signed).
 */
long ADE7753::getActiveEnergy(void){
    return read24(AENERGY);
}
/**
 * Same as the active energy register except that the register is reset to 0 following a read operation.
 * @param none
 * @return long with the data (24 bits 2-complement signed).
 */
long ADE7753::getActiveEnergyReset(void){
    return read24(RAENERGY);
}


/**
 * Line Accumulation Active Energy Register. The instantaneous active power is
 * accumulated in this read-only register over the LINECYC number of half line cycles.
 * @param none
 * @return long with the data (24 bits 2-complement signed).
 */
long ADE7753::getActivePower(void){
    return read24(LAENERGY);
}


/**
 * Apparent Energy Register. Apparent power is accumulated over time in this read-only register.
 * @param none
 * @return long with the data (24 bits unsigned).
 */
long ADE7753::getApparentEnergy(void){
    return read24(VAENERGY);
}


/**
 * Same as the VAENERGY register except that the register is reset to 0 following a read operation.
 * @param none
 * @return long with the data (24 bits unsigned).
 */
long ADE7753::getApparentEnergyReset(void){
    return read24(RVAENERGY);
}

/**
 * The instantaneous real power is accumulated in this read-only register over the LINECYC number of half line cycles.
 * @param none
 * @return long with the data (24 bits unsigned).
 */
long ADE7753::getApparentPower(void){
    return read24(LVAENERGY);
}

/**
 * The instantaneous reactive power is accumulated in this read-only register over the LINECYC number of half line cycles.
 * @param none
 * @return long with the data (24 bits 2-complement signed).
 */
long ADE7753::getReactivePower(void){
    return read24(LVARENERGY);
}

/**
 * This is a 16-bit register through which most of the ADE7753 functionality is accessed.
 * Signal sample rates, filter enabling, and calibration modes are selected by writing to this register.
 * The contents can be read at any time.
 * 
 * 
 * The next table describes the functionality of each bit in the register:
 * 
 * Bit     Location	Bit Mnemonic	Default Value Description			
 * 0	   DISHPF       0	        HPF (high-pass filter) in Channel 1 is disabled when this bit is set.		
 * 1	   DISLPF2      0	        LPF (low-pass filter) after the multiplier (LPF2) is disabled when this bit is set.		
 * 2	   DISCF        1	        Frequency output CF is disabled when this bit is set.		
 * 3	   DISSAG       1	        Line voltage sag detection is disabled when this bit is set.		
 * 4	   ASUSPEND     0	        By setting this bit to Logic 1, both ADE7753 A/D converters can be turned off. In normal operation, this bit should be left at Logic 0. All digital functionality can be stopped by suspending the clock signal at CLKIN pin.		
 * 5	   TEMPSEL      0	        Temperature conversion starts when this bit is set to 1. This bit is automatically reset to 0 when the temperature conversion is finished.		
 * 6	   SWRST        0	        Software Chip Reset. A data transfer should not take place to the ADE7753 for at least 18 ?s after a software reset.		
 * 7	   CYCMODE      0	        Setting this bit to Logic 1 places the chip into line cycle energy accumulation mode.		
 * 8	   DISCH1       0	        ADC 1 (Channel 1) inputs are internally shorted together.		
 * 9	   DISCH2       0	        ADC 2 (Channel 2) inputs are internally shorted together.		
 * 10	   SWAP         0	        By setting this bit to Logic 1 the analog inputs V2P and V2N are connected to ADC 1 and the analog inputs V1P and V1N are connected to ADC 2.		
 * 12, 11  DTRT1,0      0	        These bits are used to select the waveform register update rate.		
 * 				        DTRT 1	DTRT0	Update Rate
 * 				            0	0	27.9 kSPS (CLKIN/128)
 * 				            0	1	14 kSPS (CLKIN/256)
 * 				            1	0	7 kSPS (CLKIN/512)
 * 				            1	1	3.5 kSPS (CLKIN/1024)
 * 14, 13  WAVSEL1,0	0	        These bits are used to select the source of the sampled data for the waveform register.		
 * 			                  WAVSEL1, 0	Length	Source
 * 			                  0	        0	24 bits active power signal (output of LPF2)
 * 			                  0	        1	Reserved
 * 			                  1	        0	24 bits Channel 1
 * 			                  1	        1	24 bits Channel 2
 * 15	POAM	        0	        Writing Logic 1 to this bit allows only positive active power to be accumulated in the ADE7753.		
 * 
 * 
 * @param none
 * @return int with the data (16 bits unsigned).
 */
void ADE7753::setMode(int m){
    write16(MODE, m);
}
int ADE7753::getMode(){
    return read16(MODE);
}


/**The next commands make reference to the proportionality constant for the real Voltage, Current and accumulate Energy values.

* @param float d with a proportionality constant value.
* @return none, change value of the voltaje proportionality constant.
*/

void ADE7753::changeKV(float d){
  kv=d;
}

/*@param: none
* @return: a float with the value of the voltage proportionality constant .
*/

float ADE7753::getKV(){
  return kv;
}

/*This function allows set the value of the voltage proportionality constant according to a preset voltage value.
* @param: float with the preset voltaje  value.
* @return: none
*/

void ADE7753::setKV(float vtran){  
float aux=0;
for (int i=1;i<=5;++i){ //It takes five samples to obtain a more stable result.
  aux=aux+vtran/((ADE7753::vrms())*VOLTDIV); //A factor of VOLTDIV is applied due to the Energy Shield self-attenuation. See the schematic.
}
 ADE7753::changeKV(aux/5);
}

/*
* @param float d with a proportionality constant value.
* @return none, change value of the current proportionality constant.
*/

void ADE7753::changeKI(float d){
  ki=d;
}

/*@param: none
* @return: a float with the value of the current proportionality constant .
*/

float ADE7753::getKI(){
  return ki;
}

/*This function allows set the value of the current proportionality constant according to a preset voltage value.
* @param: float with the preset current  value.
* @return: none
*/

void ADE7753::setKI(float iprob){  
 ADE7753::changeKI(iprob/(ADE7753::irms()*VOLTDIV)); //A factor of VOLTDIV is applied due to the Energy Shield self-attenuation. See the schematic.
}


/*
* @param float d with a proportionality constant value.
* @return none, change value of the energy proportionality constant.
*/
void ADE7753::changeKE(float d){
  ke=d;
}

/*@param: none
* @return: a float with the value of the current proportionality constant .
*/

float ADE7753::getKE(){
  return ke;
}

/*This function allows set value of the active energy proportionality constant according to a preset active energy value.
* @param: float with the preset accumulated active energy  value.
* @return: none
*/

void ADE7753::setKE(float eprob){  
 ADE7753::setMode(0x0080); //Active energy accumulation mode begins.
 ADE7753::setLineCyc(100); //Number of semi cicles occupied in the active energy calculation. With 50 hz 100 semi cicles are 1 sec. 
 long e=ADE7753::getLAENERGY(); //Active energy accumulation after 1 second,
 ADE7753::setLineCyc(200);//Active energy accumulation after 2 second,
 long e2=ADE7753::getLAENERGY();
 ADE7753::setLineCyc(300); //Active energy accumulation after 3 second
 long e3=ADE7753::getLAENERGY();
 ADE7753::changeKE(eprob/((e3-e2)*VOLTDIV)); //Finally the result is the accumulated active energy between second two and three. A factor of VOLTDIV is applied due to the Energy Shield self-attenuation. See the schematic.
}


#define AEHF      0x0001 // bit 0 - Indicates that an interrupt occurred because the active energy register, AENERGY, is more than half full.
#define SAG       0x0002 // bit 1 - Indicates that an interrupt was caused by a SAG on the line voltage.
#define CYCEND    0x0004 // bit 2 - Indicates the end of energy accumulation over an integer number of half line cycles as defined by the content of the LINECYC registerï¿½see the Line Cycle Energy Accumulation Mode section.
#define WSMP      0x0008 // bit 3 - Indicates that new data is present in the waveform register.
#define ZX        0x0010 // bit 4 - This status bit is set to Logic 0 on the rising and falling edge of the the voltage waveform. See the Zero-Crossing Detection section.
#define TEMPREADY 0x0020 // bit 5 - Indicates that a temperature conversion result is available in the temperature register.
#define RESET     0x0040 // bit 6 - Indicates the end of a reset (for both software or hardware reset). The corresponding enable bit has no function in the interrupt enable register, i.e., this status bit is set at the end of a reset, but it cannot be enabled to cause an interrupt.
#define AEOF      0x0080 // bit 7 - Indicates that the active energy register has overflowed.
#define PKV       0x0100 // bit 8 - Indicates that waveform sample from Channel 2 has exceeded the VPKLVL value.
#define PKI       0x0200 // bit 9 - Indicates that waveform sample from Channel 1 has exceeded the IPKLVL value.
#define VAEHF     0x0400 // bit 10 - Indicates that an interrupt occurred because the active energy register, VAENERGY, is more than half full.
#define VAEOF     0x0800 // bit 11 - Indicates that the apparent energy register has overflowed.
#define ZXTO      0x1000 // bit 12 - Indicates that an interrupt was caused by a missing zero crossing on the line voltage for the specified number of line cyclesï¿½see the Zero-Crossing Timeout section.
#define PPOS      0x2000 // bit 13 - Indicates that the power has gone from negative to positive.
#define PNEG      0x4000 // bit 14 - Indicates that the power has gone from positive to negative.
#define RESERVED  0x8000 // bit 15 - Reserved.		

int ADE7753::getInterrupts(void){
    return read16(IRQEN);
}
void ADE7753::setInterrupts(int i){
    write16(IRQEN,i);
}
/**
 * This is an 16-bit read-only register. The status register contains information regarding the source of ADE7753 interrupts
 * @param none
 * @return int with the data (16 bits unsigned).
 */
int ADE7753::getStatus(void){
    return read16(STATUS);
}
/**
 * Same as the interrupt status register except that the register contents are reset to 0 (all flags cleared) after a read operation.
 * @param none
 * @return int with the data (16 bits unsigned).
 */
int ADE7753::resetStatus(void){
    return read16(RSTSTATUS);
}


/** === getIRMS ===
* Channel 2 RMS Value (Current Channel).
* The update rate of the Channel 2 rms measurement is CLKIN/4.
* To minimize noise, synchronize the reading of the rms register with the zero crossing 
* of the voltage input and take the average of a number of readings.
* @param none
* @return long with the data (24 bits unsigned).
*/
long ADE7753::getIRMS(void){
	long lastupdate = 0;
	ADE7753::getresetInterruptStatus(); // Clear all interrupts
	lastupdate = millis();
	while( !  ( ADE7753::getInterruptStatus() & ZX )  )   // wait Zero-Crossing
	{}
//        while( !  ( ADE7753::getInterruptStatus()  )  )   // wait Zero-Crossing
//	{} 
	return read24(IRMS);
}


/** === getVRMS ===
* Channel 2 RMS Value (Voltage Channel).
* The update rate of the Channel 2 rms measurement is CLKIN/4.
* To minimize noise, synchronize the reading of the rms register with the zero crossing 
* of the voltage input and take the average of a number of readings.
* @param none
* @return long with the data (24 bits unsigned).
*/

long ADE7753::getVRMS(void){
	long lastupdate = 0;
	ADE7753::getresetInterruptStatus(); // Clear all interrupts
	lastupdate = millis();
	while( !  ( ADE7753::getInterruptStatus() & ZX )  )   // wait Zero-Crossing
	{} 
//        while( !  ( ADE7753::getInterruptStatus()  )  )   // wait Zero-Crossing
//	{} 
return read24(VRMS);
}

/** === getLAENERGY ===
* Accumulated Active Energy until a fixed number of semi cicles according to the LineCyc register.
* @param none
* @return long with the data (24 bits unsigned).
*/

long ADE7753::getLAENERGY(){
 ADE7753::getresetInterruptStatus(); // Clear all interrupts
long aux;
      while( !  ( ADE7753::getInterruptStatus() & CYCEND )  )   // wait until reach LineCyc register
	{}
	aux=read24(LAENERGY);
	return aux;
}

/** === getLAENERGY ===
* Accumulated Active Energy until a fixed number of semi cicles according to the LineCyc register.
* @param none
* @return long with the data (24 bits unsigned).
*/

long ADE7753::getLVAENERGY(){
 ADE7753::getresetInterruptStatus(); // Clear all interrupts
long aux;
      while( !  ( ADE7753::getInterruptStatus() & CYCEND )  )   // wait until reach LineCyc register
	{}
	aux=read24(LVAENERGY);
	return aux;
}


/**********************************************************************************/

/** === getRAENERGY ===
* Reactive Energy until a fixed number of semi cicles according to the LineCyc register.
* @param none
* @return long with the data (24 bits unsigned).
*/

long ADE7753::getReactiveEnergy(){
 ADE7753::getresetInterruptStatus(); // Clear all interrupts
long aux;
      while( !  ( ADE7753::getInterruptStatus() & CYCEND )  )   // wait Cylces End
	{}
	aux=getReactivePower();
	return aux;
}



/******************************************************************************/


/** === getFPOWER ===
* Power Factor Calculation.
* @param none
* @return long with the data (24 bits unsigned).
*/

float ADE7753::getFPOWER(){

  long e1,e2,e3,a1,a2,a3;
  float PF;
  float ke = (1.0*NUMCYC)/4096.0; // El 2 esta relacionado con el numero de ciclos
  //4096 = 2^12 y corresponde al valor maximo que almacena el registro
  
//Medición de Energía Activa Acumulada
ADE7753::setMode(0x0080); 
ADE7753::setLineCyc(1*NUMCYC); 
e1=ADE7753::getLAENERGY(); 

ADE7753::setMode(0x0080);
ADE7753::setLineCyc(2*NUMCYC); 
e2=ADE7753::getLAENERGY();

ADE7753::setMode(0x0080);
ADE7753::setLineCyc(3*NUMCYC); 
e3=ADE7753::getLAENERGY();

ADE7753::setMode(0x0080); 
ADE7753::setLineCyc(1*NUMCYC); 
a1=ADE7753::getLVAENERGY(); 

ADE7753::setMode(0x0080);
ADE7753::setLineCyc(2*NUMCYC); 
a2=ADE7753::getLVAENERGY();

ADE7753::setMode(0x0080);
ADE7753::setLineCyc(3*NUMCYC); 
a3=ADE7753::getLVAENERGY();  
  
  if( fabs(ke*(e3-e2))  <= 0.0000002){
    PF = 1.0;
  }
  else if ( fabs(ke*(a3-a2)) <= 0.0000002)
  {
    PF = 0.0;
    Serial.print("WARNING - Power Factor");
  }
  else if( (ke*(a3-a2)) >= (ke*(e3-e2)) ) {
    PF = fabs((ke*(e3-e2))/(ke*(a3-a2)));
  }
  else{
    PF = fabs((ke*(a3-a2)) / (ke*(e3-e2)) );
  }
  
  if(PF > 1.0)
    PF = 2-PF;
    
  if( (PF > 1.0) || (PF < 0.0000002)){
   PF = 1.0;
    Serial.print("WARNING - Power Factor");
  }
    
  return PF;
}

/** === vrms ===
* Returns the mean of last 20 readings of RMS voltage. Also supress first reading to avoid 
* corrupted data.
* rms measurement update rate is CLKIN/4.
* To minimize noise, synchronize the reading of the rms register with the zero crossing 
* of the voltage input and take the average of a number of readings.
* @param none
* @return long with RMS voltage value 
*/
long ADE7753::vrms(){
	char i=0;
	long v=0;
	getVRMS();//Ignore first reading to avoid garbage
	for(i=0;i<20;++i){
		v+=getVRMS();
	}
	return v/20;
}

/** === irms ===
* Returns the mean of last 20 readings of RMS current. Also supress first reading to avoid 
* corrupted data.
* rms measurement update rate is CLKIN/4.
* To minimize noise, synchronize the reading of the rms register with the zero crossing 
* of the voltage input and take the average of a number of readings.
* @param none
* @return long with RMS current value in hundreds of [mA], ie. 6709=67[mA]
*/
long ADE7753::irms(){
	char n=0;
	long i=0;
	getIRMS();//Ignore first reading to avoid garbage
	for(n=0;n<20;++n){
		i+=getIRMS();
	}
	return i/20;
}

/**
 * Channel 2 RMS Offset Correction Register.
 * @param none
 * @return int with the data (12 bits 2-complement signed).
 */
int ADE7753::getCurrentOffset(){
    return read16(IRMSOS);
}

/**
 * Channel 2 RMS Offset Correction Register.
 * 
 * @param none
 * @return int with the data (12 bits 2-complement's signed).
 */
int ADE7753::getVoltageOffset(){
    return read16(VRMSOS);
}

/**
 * Zero-Crossing Timeout. If no zero crossings are detected
 * on Channel 2 within a time period specified by this 12-bit register,
 * the interrupt request line (IRQ) is activated
 * @param none
 * @return int with the data (12 bits unsigned).
 */
void ADE7753::setZeroCrossingTimeout(int d){
    write16(ZXTOUT,d);
}
int ADE7753::getZeroCrossingTimeout(){
    return read16(ZXTOUT);
}


/**
 * Sag Line Cycle Register. This 8-bit register specifies the number of
 * consecutive line cycles the signal on Channel 2 must be below SAGLVL
 * before the SAG output is activated.
 * @param none
 * @return char with the data (8 bits unsigned).
 */
char ADE7753::getSagCycles(){
    return read8(SAGCYC);
}
void ADE7753::setSagCycles(char d){
    write8(SAGCYC,d);
}

/**
 * Line Cycle Energy Accumulation Mode Line-Cycle Register. 
 * This 16-bit register is used during line cycle energy accumulation  mode 
 * to set the number of half line cycles for energy accumulation
 * @param none
 * @return int with the data (16 bits unsigned).
 */
int ADE7753::getLineCyc(){
    return read16(LINECYC);
}
void ADE7753::setLineCyc(int d){
    write16(LINECYC,d);
}


/**
 * Sag Voltage Level. An 8-bit write to this register determines at what peak
 * signal level on Channel 2 the SAG pin becomes active. The signal must remain
 * low for the number of cycles specified in the SAGCYC register before the SAG pin is activated
 * @param none
 * @return char with the data (8 bits unsigned).
 */
char ADE7753::getSagVoltageLevel(){
    return read8(SAGLVL);
}
void ADE7753::setSagVoltageLevel(char d){
    write8(SAGLVL,d);
}


/**
 * Channel 1 Peak Level Threshold (Current Channel). This register sets the levelof the current
 * peak detection. If the Channel 1 input exceeds this level, the PKI flag in the status register is set.
 * @param none
 * @return char with the data (8 bits unsigned).
 */
char ADE7753::getIPeakLevel(){
    return read8(IPKLVL);
}
void ADE7753::setIPeakLevel(char d){
    write8(IPKLVL,d);
}


/**
 * Channel 2 Peak Level Threshold (Voltage Channel). This register sets the level of the
 * voltage peak detection. If the Channel 2 input exceeds this level, 
 * the PKV flag in the status register is set.
 * @param none
 * @return char with the data (8bits unsigned).
 */
char ADE7753::getVPeakLevel(){
    return read8(VPKLVL);
}
void ADE7753::setVPeakLevel(char d){
    write8(VPKLVL,d);
}


/**
 * Channel 1 Peak Register. The maximum input value of the current channel since the last read of the register is stored in this register.
 * @param none
 * @return long with the data (24 bits unsigned) .
 */
long ADE7753::getIpeak(void){
    return read24(IPEAK);
}

/**
 * Same as Channel 1 Peak Register except that the register contents are reset to 0 after read.
 * @param none
 * @return long with the data (24 bits 24 bits unsigned).
 */
long ADE7753::getIpeakReset(void){
    return read24(RSTIPEAK);
}

/**
 * Channel 2 Peak Register. The maximum input value of the voltage channel since the last read of the register is stored in this register.
 * @param none
 * @return long with the data (24 bits unsigned).
 */
long ADE7753::getVpeak(void){
    return read24(VPEAK);
}

/**
 * Same as Channel 2 Peak Register except that the register contents are reset to 0 after a read.
 * @param none
 * @return long with the data (24 bits  unsigned).
 */
long ADE7753::getVpeakReset(void){
    return read24(RSTVPEAK);
}

/**
 * Period of the Channel 2 (Voltage Channel) Input Estimated by Zero-Crossing Processing. The MSB of this register is always zero.
 * @param none
 * @return int with the data (16 bits unsigned).
 */
int ADE7753::getPeriod(void){
    return read16(PERIOD);
}

/**
 * Checksum Register. This 6-bit read-only register is equal to the sum of all the ones in the previous read—see the ADE7753 Serial Read Operation section.
 * @param none
 * @return char with the data (6 bits unsigned).
 */
char ADE7753::chkSum(){
    return read8(CHKSUM);
}


/**
 * Force a temperature measure and then returns it. This is done by setting bit 5 HIGH in MODE register.
 * Temperature measuring can't be calibrated, the values used in this function are according to the datasheet
 * (register TEMP is 0x00 at -25 celsius degrees).
 * @param none
 * @return char with the temperature in celsius degrees.
 */
char ADE7753::getTemp(){
    long r=0; 
    //Temp measure
    setMode(0x20 | getMode());
    delay(20);//wait zero crossing
    //Read register
    r=read8(TEMP);
    return (1806*r-25000)/1000;
}

/*
 * Calibrations
 *
 *
 * To calibrate the device, following stages must be followed
 * - Watt/VA gain calibration
 * - Offset calibration
 * - RMS calibration
 * - Watt offset calibration
 * - Phase calibration
 * - Temp calibration??
 * - Waveform??
 * Note: Reactive energy can't be calibrated, it has to be done with a MCU
 * 
 * 
 */


/**
 * wait until a key is sent to the arduino trough serial port
 * @param message to print to user
 * @return the key pressed
 */
char ADE7753::waitKey(char * msg){
    Serial.println(msg);
    while(Serial.available() == 0){
        delay(20);
    }
    return Serial.read();   
}


long ADE7753::waitInterrupt(unsigned int interrupt){
    int n = 1;
    long out;
    long prev_status=read16(STATUS);
//      setInterrupts(interrupt);//Enable line cycle accumulation interrupt (bit 2)
    //wait two times, because the first reading next to this function call contains garbage
    while(n){
     read16(RSTSTATUS);
      write16(IRQEN,interrupt);
      // read16(RSTSTATUS);//Reset the interrupt status read register
      //wait for the interrupt to take place
      if(read16(STATUS) == prev_status){
          delay(100);//busywait
      }
      else{
      out=read16(STATUS);
      read16(STATUS);
      n=0;}
    }
    read16(RSTSTATUS);
    return out;
}

/**
 * Calibration using an accurate source must follow the next stages:
 * 
 * 1)Gain calibration
 * """""""""""""""""""
 * 1.1 - Calculate CFDEN, CFNUM, LINECYC and CFDIV values, CFNUM=CFDIV=0 generally for resolution improvement,
 *       LINECYC arbitrary.
 * 1.2 - Set mode for line cycle accumulation and enable line cycle accumulation interrupt
 * 1.3 - Reset the interrupt status read register
 * 1.4 - Wait until interrupt takes place (two times, because first reading contains garbage, and reset interrupt
 *       status register before each time)
 * 1.5 - Read LAenergy and LVAenergy
 * 1.6 - Calculate and write WGAIN and VAGAIN
 * 
 * 
 * 1)Offset calibration
 * """""""""""""""""""""
 * 1.2 - Set mode for line cycle accumulation and enable line cycle accumulation interrupt
 * 1.3 - Reset the interrupt status read register
 * 1.4 - Wait until interrupt takes place (two times, because first reading contains garbage, and reset interrupt
 *       status register before each time)
 * 1.5 - Read LAenergy 
 * 1.6 - Calculate and write APOS
 * 
 * 1)Phase calibration
 * """""""""""""""""""""
 * 1.2 - Set mode for line cycle accumulation and enable line cycle accumulation interrupt
 * 1.3 - Reset the interrupt status read register
 * 1.4 - Wait until interrupt takes place (two times, because first reading contains garbage, and reset interrupt
 *       status register before each time)
 * 1.5 - Read LAenergy 
 * 1.6 - Calculate and write APOS
 * 
 * 
 * @param int Imax is the maximum current at wich the meter will operate
 * @param int I present current (nominal)
 * @param int V present voltage (nominal)
 * @param int MeterConstant A meter constant, typically 3200
 * @param int power current active power consumed by load
 * @param int linecyc number of cicles for energy measuring
 * 
 */
void ADE7753::calibrateEnergyAccurateSource(int Imax, int I, int V, int MeterConstant, long power, int linecyc){
    //variables
    char n     = 0,
    phcal      = 0;
    
    int cfden  = 0,
    CFexpected = MeterConstant*power/3600,
    CFnominal  = (23000/4)*I/Imax,/*23kHz is the max frecuency of CF, we can calculate it instead of reading the CF pin =) */
    prevMode   = getMode(),
    prevInt    = getInterrupts(),
    wgain      = 0,
    vagain     = 0,
    wdiv       = 0,
    period     = read16(PERIOD),
    cfnum      = 0,
    apos       = 0;
    
    long LAenergyExpected = 0,
    LVAenergyExpected     = 0,
    LAenergyNom           = 0,
    LVAenergyNom          = 0,
    AEnergyErrorRate      = 0;
    
    
    //wait adjustment
    
    waitKey("Adjust the voltage and current to be nominal, and the load to have power factor 1. Press a key when ready.");

    ////////////////////
    //Gain calibration
    cfden = (CFnominal/CFexpected - 1);//Calculate CFDEN value (CFNUM=0)
    write16(CFDEN,cfden);//and write it...
    write16(VADIV,cfden);//for apparent energy we use the same value (VADIV is like CFDEN but for apparent))
    write16(CFNUM,0);//CFNUM=0 for better precision
    write16(WDIV,0);//the same as cfnum
    //set half linecycles
    write16(LINECYC,linecyc);//arbitrary... wait time for energy accumulation
    setMode(0x0080);//set mode for line cycle accumulation
    setInterrupts(0x04);//Enable line cycle accumulation interrupt (bit 2)
    Serial.print("Calibrating gain...");
    LAenergyNom = waitInterrupt(0x04);//LAenergyNom=reading
    LVAenergyNom = read24(LVAENERGY);//LVAenergyNom=reading
    setInterrupts(prevInt);
    
    Serial.println("OK");
    //active energy gain calculation
    //LAenergyExpected=CFexpected*AcTime/(((cfnum+1)/(cfden+1))*WDIV)
    LAenergyExpected = ((MeterConstant*power/3600) * ((8/4)*linecyc*period/CLKIN))   /   ( ((cfnum+1)/(cfden+1)) * wdiv);
    wgain = (LAenergyExpected/LAenergyNom - 1)*(int)1<<12;
    
    //apparent energy gain calculation
    LVAenergyExpected = ((MeterConstant*V*I/3600) * ((8/4)*linecyc*period/CLKIN))*cfden;
    vagain = (int)(LVAenergyExpected/LVAenergyNom-1)*(1<<12);

    //and write it
    write16(WGAIN,wgain);
    write16(VAGAIN,vagain);
    ////////////////////
    //Offset calibration
//    period = read16(PERIOD);
    setMode(0x0080);//set mode for line cycle accumulation
    setInterrupts(0x04);//Enable line cycle accumulation interrupt (bit 2)
    Serial.print("Calibrating offset...");
    LAenergyNom = waitInterrupt(0x04);//LAenergyNom=reading
    Serial.println("OK");
    
//    AEnergyErrorRate = (CFnominal-CFexpected)*(cfden+1)/(cfnum+1);
    AEnergyErrorRate = (LAenergyNom-LAenergyExpected)*CLKIN/(linecyc/2*8*period);
    apos = (int)(((long)1<<23/CLKIN)*AEnergyErrorRate*((long)1<<12));
    write16(APOS,apos);
    
    setInterrupts(prevInt);
    
    
    ////////////////////
    //Phase calibration
    
    //wait adjustment of power factor to 0.5 inductive
    waitKey("Adjust the load to have power factor 0.5 inductive. Press a key when ready");

    setMode(0x0080);//set mode for line cycle accumulation
    setInterrupts(0x04);//Enable line cycle accumulation interrupt (bit 2)
    Serial.print("Calibrating phase...");
    LAenergyNom = waitInterrupt(0x04);//LAenergyNom=reading at FP=0.5
    Serial.println("OK");
    //calculations made comparing lectures at fp=0.5 with lectures at fp=1
    phcal=(char)(0x0D + period/360 * (1000*(2*LAenergyNom-LAenergyExpected)/(1732*LAenergyExpected)) );
    if(phcal <= 0x1F && phcal >= 0x21){/*Valid range of PHCAL*/
        write8(PHCAL,phcal);
    }
    else{
        Serial.println("Error: Invalid PHCAL value, assumming 0");
        write8(PHCAL,0x0D);//at PHCAL=0x0D phase lag=0
    }
    
    setInterrupts(prevInt);
    setMode(prevMode);
}


/**
 * calibrateEnergy
 * 
 * @param int CFnominal frecuency of CF signal
 * @param int CFexpected frecuency of reference meter
 */
void ADE7753::calibrateEnergy(int CFnominal, int CFexpected){
    long LAenergyExpected = 0,
    LVAenergyExpected     = 0,
    LAenergyNom           = 0,
    LVAenergyNom          = 0,
    AEnergyErrorRate      = 0;
  
    int cfden  = 0,
    wgain      = 0,
    wdiv       = 0,
    cfnum      = 0,
    apos       = 0,
    period     = read16(PERIOD),
    error      = 0;
    
    char phcal,resp;
    
    
    frecuencySetup(0,1231);//32000 imp/kWh
    
    //gain calibration
    waitKey("Ajustar voltaje y corriente a nominales, carga con factor de potencia 1 y presionar una tecla.");   
    Serial.print("Calibrando ganancia...");
    
    energySetup(417,40,17,300,11,0); //32000 imp/kWh

    error = (CFnominal-CFexpected)*100/CFexpected;
    wgain = -error*10000/244;
    write16(WGAIN,wgain);
    
    Serial.print("OK  WGAIN=");
    Serial.print(read16(WGAIN),DEC);
    Serial.print(" | WDIV=");
    Serial.print(read16(WDIV),DEC);
    Serial.print(" | CFDEN=");
    Serial.println(read16(CFDEN),DEC);
    Serial.print(" | APOS=");
    Serial.println(read16(APOS),DEC);
    write16(VAGAIN,wgain);//power factor=1
    
    
    //offset calibration
    Serial.println("Ajustar el voltaje a nominal,desconectar sonda de corriente.");
    resp=waitKey("Si la energia no sigue aumentando presione <n>, si la energia aumenta presione <s>");  
    if(resp=='s'){
    Serial.print("Calibrating offset...");
    AEnergyErrorRate = (CFnominal - CFexpected)*((cfden+1)/(cfnum+1))/1000;
    apos = (int)(((long)1<<23/CLKIN)*AEnergyErrorRate*((long)1<<12));
    write16(APOS,apos);
    Serial.print("OK  APOS=");
    Serial.print(apos,DEC);
    }
    else
        Serial.println("No se requiere calibrar offset");

    //phase calibration
    waitKey("Adjust the load to have power factor 0.5 inductive. Press a key when ready");
    
    Serial.print("Calibrating phase...");
    error = (CFnominal-CFexpected)*10000/CFexpected;
    phcal = error*1000/1732*period/360+0x0D;
    //Valid range of PHCAL
    if(phcal <= 0x1F && phcal >= 0x21){
        write8(PHCAL,phcal);
        Serial.println("OK");
    }
    else{
        Serial.println("Error: Valor de PHCAL fuera de rango, asumiendo 0");
        Serial.println("Probablemente el desfase de demasiado grande.");
        write8(PHCAL,0x0D);//at PHCAL=0x0D phase lag=0
    }
}

void ADE7753::calibrateRMSOS(int v1,int v2,int i1,int i2){
  
    int vrmsos  = 0,
        irmsos  = 0;
    long vrms1  = 0,
         vrms2  = 0,
         irms1  = 0,
         irms2  = 0;
    Serial.println("The idea is that the nominal values are half the full-scale analog input range.");
    waitKey("Set the supply voltage to V nominal and then send a character");
    //measure @ v=Vnom
    vrms1=vrms();
    
    waitKey("Set the supply voltage to Vnom/10 approx. and then send a character");
    //measure @ v=Vnom/10    
    vrms2=vrms();
    waitKey("Set the current to be Ibase and then send a character");
    //measure @ i=Inom
    irms1=irms();
    waitKey("Set the current to be Imax/50 (Imax=half the full-scale) and then send a character");
    //measure @ i=Inom/10
    irms2=irms();
    
    vrmsos = (v1*vrms2-v2*vrms1)/(v2-v1);
    irmsos = (i1*i1*irms2*irms2-i2*i2*irms1*irms1)/(32768*(i2*i2-i1*i1));
    
    Serial.print("guardando valores: VRMSOS=");
    Serial.print(vrmsos,DEC);
    Serial.print(", IRMSOS=");
    Serial.print(irmsos,DEC);
    
    
    write16(VRMSOS,vrmsos);
    write16(IRMSOS,irmsos);
    Serial.print("valores escritos: VRMSOS=");
    Serial.print(read16(VRMSOS),DEC);
    Serial.print(", IRMSOS=");
    Serial.print(read16(IRMSOS),DEC);
    
}


//funciones de ajuste manual

void ADE7753::energySetup(int wgain, char wdiv, int apos, int vagain, char vadiv, char phcal){
  write16(WGAIN,wgain);
  write8(WDIV,wdiv);
  write16(APOS,apos);  
  write16(VAGAIN,vagain);
  write8(VADIV,vadiv);
  write8(PHCAL,phcal);
}

void ADE7753::energyGain(int wgain, int vagain){
  write16(WGAIN,wgain);
  write16(VAGAIN,vagain);
}


/**
 * The output frequency on the CF pin is adjusted by writing to this 12-bit
 * read/write register—see the Energy-to-Frequency Conversion section.
 * @param cfnum: integer containing number (12 bits available unsigned. ie range=[0,4095])
 * @param cfden: the same as cfnum
 */
void ADE7753::frecuencySetup(int cfnum, int cfden){
  write16(CFNUM,cfnum);
  write16(CFDEN,cfden);
}

/**
 * This 8-bit register is used to adjust the gain selection for the PGA in Channels 1 and 2
 * @param gain_ch1 char set the PGA channel 1 gain, use constants GAIN_1, GAIN_2, GAIN_4, GAIN_8 and gain_16
 * @param gain_ch2 char  set the PGA channel 2 gain, use constants GAIN_1, GAIN_2, GAIN_4, GAIN_8 and gain_16
 * @param os_ch1 char set channel 1 analog offset, range : [-32,32]
 * @param os_ch2 char  set channel 1 analog offset, range : [-32,32]
 * @param scale_ch1 char
 * @param integrator_ch1 char
 * @return char with the data (8 bits unsigned).

 for gains 

 */
 
void ADE7753::analogSetup(char gain_ch1, char gain_ch2,char os_ch1,char os_ch2,char scale_ch1,char integrator_ch1){
     char pga = (gain_ch2<<5) | (scale_ch1<<3) | (gain_ch1);
     char sign = 0;
     char ch1os = 0, ch2os = 0;
     
     
     write8(GAIN,pga);//write GAIN register, format is |3 bits PGA2 gain|2 bits full scale|3 bits PGA1 gain
     
     //ch1 offset, sign magnitude and integrator    
     if(os_ch1<0){
         sign=1;
         os_ch1=-os_ch1;
     }
     ch1os = (integrator_ch1<<7) | (sign<<5) | os_ch1;
     write8(CH1OS,ch1os);

     //ch2 offset, sign magnitude (ch2 doesn't have integrator) and the offset applied is inverted (ie offset of -1 sums 1)
     if(os_ch2<0){
         sign=0;
         os_ch1=-os_ch1;
     }
     else{
         sign=1;
     }
     ch2os = (sign<<5) | os_ch2;
     write8(CH2OS,ch2os);
} 

int ADE7753::getresetInterruptStatus(void){
	return read16(RSTSTATUS);
}

int ADE7753::getInterruptStatus(void){
	return read16(STATUS);
}

void ADE7753::rmsSetup(int vrmsos, int irmsos){
      write16(VRMSOS,vrmsos);
      write16(IRMSOS,irmsos);
}

void ADE7753::setInterruptsMask(int Mask16){
	write16(IRQEN, Mask16);
}
