
/* ==================================================================================
File:       ADE7753.h
                    
Author:	Eduard Martin
        MCI Electronics
        www.olimex.cl
        
Description:  Header File, Data types, objects definition of WIZ610 Class.

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



#ifndef ADE7753_H
#define ADE7753_H

/***
 * Defines
 *
 */
 
//Registers
#define WAVEFORM 0x01
#define AENERGY 0x02
#define RAENERGY 0x03
#define LAENERGY 0x04
#define VAENERGY 0x05
#define RVAENERGY 0x06
#define LVAENERGY 0x07
#define LVARENERGY 0x08
#define MODE 0x09
#define IRQEN 0x0A
#define STATUS 0x0B
#define RSTSTATUS 0x0C
#define CH1OS 0x0D
#define CH2OS 0x0E
#define GAIN 0x0F
#define PHCAL 0x10
#define APOS 0x11
#define WGAIN 0x12
#define WDIV 0x13
#define CFNUM 0x14
#define CFDEN 0x15
#define IRMS 0x16
#define VRMS 0x17
#define IRMSOS 0x18
#define VRMSOS 0x19
#define VAGAIN 0x1A
#define VADIV 0x1B
#define LINECYC 0x1C
#define ZXTOUT 0x1D
#define SAGCYC 0x1E
#define SAGLVL 0x1F
#define IPKLVL 0x20
#define VPKLVL 0x21
#define IPEAK 0x22
#define RSTIPEAK 0x23
#define VPEAK 0x24
#define RSTVPEAK 0x25
#define TEMP 0x26
#define PERIOD 0x27
#define TMODE 0x3D
#define CHKSUM 0x3E
#define DIEREV 0X3F


//bits

//of MODE register
#define DISHPF     0 
#define DISLPF2    1 
#define DISCF      2
#define DISSAG     3
#define ASUSPEND   4 
#define TEMPSEL    5
#define SWRST      6
#define CYCMODE    7 
#define DISCH1     8
#define DISCH2     9
#define SWAP      10
#define DTRT1     11
#define DTRT0     12
#define WAVSEL1   13
#define WAVSEL0   14
#define POAM      15

//constants
#define GAIN_1    0x0
#define GAIN_2    0x1
#define GAIN_4    0x2
#define GAIN_8    0x3
#define GAIN_16   0x4
#define INTEGRATOR_ON 1
#define INTEGRATOR_OFF 0




// Class Atributes
#define CS 10                 // Chip Select ADE7753   
#define WRITE 0x80
#define CLKIN 4000000         //ADE7753 frec, max 4MHz
#define METER_ID 42         //meter ID (used in xbee)
#define VOLTDIV 48               //By desing, rel 1/48 
#define CURRDIV 50               //By desing, rel 1/50
#define NUMCYC  20

class ADE7753 {
  
private:
  
float kv,ki,ke;

 //public methods
   public:
      ADE7753();
      
      void setInterruptsMask(int i);
      void setMode(int m);
      void senseTemp(void);
      void setInterrupts(int i);
      void analogSetup(char gain_ch1, char gain_ch2,char os_ch1,char os_ch2,char scale_ch1,char integrator_ch1);
      void frecuencySetup(int cfnum, int cfden);
      void energySetup(int wgain, char wdiv, int apos, int vagain, char vadiv, char phcal);
      void rmsSetup(int vrmsos, int irmsos);
      void energyGain(int wgain, int vagain);
            int  getInterruptStatus(void);
      int  getresetInterruptStatus(void);
      
//      void setCurrentOffset(int d);
//      void setVoltageOffset(int d);
      void setPowerOffset(int d);
      void setPhaseCallibration(char d);
      void setEnergyGain(char d);
      void setActivePowerGain(int d);
      void setActiveEnergyDivider(char d);
//      void setFrecuencyDividerNumerator(int d);
//      void setFrecuencyDividerDenominator(int d);
      void setApparentPowerGain(int d);
      void setApparentEnergyDivider(char d);
      void setZeroCrossingTimeout(int d);
      void setSagVoltageLevel(char d);
      void setSagCycles(char d);
      void setIPeakLevel(char d);
      void setVPeakLevel(char d);
 //     void calibrateGain(int Imax, int I, int MeterConstant, long power);
      void calibrateEnergyAccurateSource(int Imax, int I, int V, int MeterConstant, long power, int linecyc);
      void calibrateEnergy(int CFnominal, int CFexpected);
      void calibrateRMSOS(int v1,int v2,int i1,int i2);
      void setLineCyc(int d);
      
      void changeKV(float d);
      void changeKI(float d);
      void changeKE(float d);
      
      void setKV(float d);
      void setKI(float d);
      void setKE(float d);
      
      char chkSum(void);
      char getTemp(void);
      char getCH1Offset(void);
      char getCH2Offset(void);
      char getPhaseCallibration(void);
      char getEnergyGain(void);
      char getActiveEnergyDivider(void);
      char getApparentEnergyDivider(void);
      char getSagCycles(void);
      char getSagVoltageLevel(void);
      char getIPeakLevel(void);
      char getVPeakLevel(void);
      char waitKey(char * msg);
      
      
 //     int calibrate(int energy, int v, int i, int f, char phi, int meter_constant);
//      int calibrateEnergyToFrecuency(int cfnum, int cfden, char wdiv);
      int getWattGain(int load);
      int getVoltageOffset(void);
      int getCurrentOffset(void);     
      int getMode(void);
      int getInterrupts(void);
      int getStatus(void);
      int resetStatus(void);
      int getActivePowerOffset(void);
      int getPeriod(void);
      int getPowerOffset(void);
      int getActivePowerGain(void);
      int getFrecuencyDividerNumerator(void);
      int getFrecuencyDividerDenominator(void);
      int getZeroCrossingTimeout(void);
      int getApparentPowerGain(void);
      int getWattGain();
      int getLineCyc();
      
      long getWaveform(void);
      long getActivePower(void);     
      long getVRMS(void);  
      long getIRMS(void);  
      long getLAENERGY(void);
      long getLVAENERGY(void);
      
      long getReactiveEnergy(void);
      
      float getFPOWER(void);
      long getActiveEnergy(void);
      long getActiveEnergyReset(void);
      long getApparentEnergy(void);
      long getApparentEnergyReset(void);
      long getApparentPower(void);
      long getReactivePower(void);
      long getIpeak(void);
      long getIpeakReset(void);
      long getVpeak(void);
      long getVpeakReset(void);
      long vrms();
      long irms();
      long energy();
      long waitInterrupt2(unsigned int interrupt);
      
      float getKV();
      float getKI();
      float getKE();
      
      
//      long calV();
      
   //private methods
   private:
      unsigned char read8(char reg);
      unsigned int read16(char reg);
      unsigned long read24(char reg);
      void write16(char reg, int data);
      void write8(char reg, char data);
      void enableChip(void);  
      void disableChip(void);
      long waitInterrupt(unsigned int interrupt);
//      void analogSetup2(char gain_ch1, char gain_ch2,char os_ch1,char os_ch2,char scale_ch1,char integrator_ch1);
//      long waitInterrupt3(unsigned int interrupt);

};


#endif
