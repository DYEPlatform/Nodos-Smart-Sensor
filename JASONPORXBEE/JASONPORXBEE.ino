/* El siguiente programa es una demostración de las mediciones
* que el Energy Shield es capaz de realizar. Se abarcan medidas de Voltaje RMS 
* Corriente RMS , Energía Activa Acumulada 
* durante un período de tiempo particular y Período de la señal de voltaje (Canal 2),
* cabe destacar que la energía acumulada durante un segundo 
* es igual a la potencia activa.

* Para ejecutar las mediciones se ocupó la proporcionalidad existente entre el valor 
* entregado por el Energy Shield y el valor real de la magnitud calculada. 

* Las constantes de proporcionalidad fueron calculadas de forma experimental 
* (una demostración de aquellas pruebas se encuentran en el sketch DetConst),
* para el caso del Período la constante viene definida desde el datasheet.

* Las constante son las siguientes:

* Voltaje: kv=0,000000171849155 [volt/LSB]
* Corriente: ki=0,00000039122624397277 [amp/LSB]
* Energía Activa Acumulada: ke=0.00041576242446899414 [J/LSB]
* Período: kt=2.2*pow(10,-6) [seg/LSB]
*/

#include "ADE7753.h"  
#include "Rtc.h"
#include <SPI.h>   
#include <Wire.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>


#define tXbee 3                     // SoftSerial Txpin
#define rXbee 2                     // SoftSerial Rxpin 
SoftwareSerial esXbee(rXbee,tXbee);

#define txGlcd 8                     // SoftSerial Txpin
#define rxGlcd 0                     // SoftSerial Rxpin 
SoftwareSerial esGLCD(rxGlcd,txGlcd);

#define ratioTraf (220/7)          //18.3

void setup(){
  
  // Xbee communications  
  pinMode(rXbee,INPUT);
  pinMode(tXbee,OUTPUT);
  
  // GLCD comm
//  pinMode(rxGlcd,INPUT);
  pinMode(txGlcd,OUTPUT);

  Serial.begin(9600);  // UART init
//  Serial.print("Setup");
  delay(100);
  
  esXbee.begin(57600);
  delay(100);
  
  esGLCD.begin(115200);
  serLCDSetup();
  backlightOn();
  delay(1000);
  clearLCD();
  
  

}

void loop() { 

ADE7753 meter = ADE7753();  
Rtc reloj = Rtc();

//esXbee.println("Setup");

meter.analogSetup(GAIN_1,GAIN_1, 0,0,0,0); 
meter.resetStatus();

long v1,i1,e1,e2,e3,ae1,ae2,ae3,r1,r2,r3;
float totEnergy = 0;

float kv,ki,ke,ka,kt,f1,basetime;
float voltage, current, energy, aparent, reactive, PF;
String typeLoad = "";
int t1;
int loopCounter = 1;


//Constantes de proporcionalidad.
kv = (ratioTraf)*VOLTDIV*(0.5/0x2851EC);    //(0.5/0x2851EC) From Datasheet                    
ki = CURRDIV*(0.5/0x17D338)/2.1;  //(0.5/0x17D338) From Datasheet
ke = (10/16.0)*VOLTDIV*CURRDIV/4096.0; // 1/(2^12)
basetime = (1.0*NUMCYC)/100.0; // tiempo por el cual se acumula energia
kt=CLKIN/8;  //period register, resolution x.y[us]/LSB -per bit-

//ke = (1.0*NUMCYC)/4096.0; // El 1.0 esta relacionado con el numero de ciclos //1/2^12
//ke original es 100/4096, considerando las atenuaciones de entrada de voltaje y corriente

//Period Resolution
/*
la formula para el calculo de la frecuencia
f =(CLKIN/4)*(1/32)*16*(1/COUNTER)

f = CLKIN/(8*COUNTER)
T = 1/f = 8*COUNTER/CLKIN

kt = CLKIN/8
frecuency = kt/COUNTER
period = COUNTER/kt

If COUNTER = 10.000, the frecuency is 50[Hz]
*/

  while(1){
  
    //--Reloj de tiempo real--
    reloj.GetDate();
    //--Medición de Voltaje--
    v1=meter.vrms();
    //--Medición de Corriente--
    i1=meter.irms();
    //--Medición de Período--
    t1=meter.getPeriod();
    f1 = kt/t1; //se calcula la frecuencia
    
    /*********************************************************/
    //Medición de Energía Activa Acumulada
    meter.setMode(0x0080); //Se inicia el modo de acumulación de energía.
    meter.setLineCyc(1*NUMCYC); //Se fija el número de medios ciclos ocupados en la medición. 10 medio ciclos equivalen a 0,1 segundo trabajando en una red de 50 Hz (Chile).
    e1=meter.getLAENERGY(); //Extrae la energía activa acumulada, sincronizando la medición con los cruces por cero de la señal de voltaje. 
    
    meter.setMode(0x0080);
    meter.setLineCyc(2*NUMCYC); // 0,2 segundos de medición.
    e2=meter.getLAENERGY();
    
    meter.setMode(0x0080);
    meter.setLineCyc(3*NUMCYC); // 0,3 segundos de medición.
    e3=meter.getLAENERGY();
      
    /*********************************************************/
    //Medición de Energía Aparente Acumulada
    meter.setMode(0x0080); //Se inicia el modo de acumulación de energía.
    meter.setLineCyc(1*NUMCYC); //Se fija el número de medios ciclos ocupados en la medición. 10 medio ciclos equivalen a 0,1 segundo trabajando en una red de 50 Hz (Chile).
    ae1=meter.getLVAENERGY(); //Extrae la energía aparente acumulada, sincronizando la medición con los cruces por cero de la señal de voltaje. 
    
    meter.setMode(0x0080);
    meter.setLineCyc(2*NUMCYC); // 0,2 segundos de medición.
    ae2=meter.getLVAENERGY();
    
    meter.setMode(0x0080);
    meter.setLineCyc(3*NUMCYC); // 0,3 segundos de medición.
    ae3=meter.getLVAENERGY();
  
    /*********************************************************/
    //Medición de Energía Reactiva Acumulada
    meter.setMode(0x0080); //Se inicia el modo de acumulación de energía.
    meter.setLineCyc(10); //Se fija el número de medios ciclos ocupados en la medición. 10 medio ciclos equivalen a 1 segundo trabajando en una red de 50 Hz (Chile).
    r1=meter.getReactiveEnergy(); //Extrae la energía activa acumulada, sincronizando la medición con los cruces por cero de la señal de voltaje. 
    
    meter.setMode(0x0080);
    meter.setLineCyc(20); // 0,2 segundos de medición.
    r2=meter.getReactiveEnergy();
    
    meter.setMode(0x0080);
    meter.setLineCyc(30); // 0,3 segundos de medición.
    r3=meter.getReactiveEnergy();
  
    /*********************************************************/
  
    /* Si tenemos los valores de energia activa y aparente
    podemos realizar el calculo, y nos evitamos hacer nuevamente
    las mediciones ahorrando tiempo. 
    */
  //  PF = getFPOWER();
    PF = calcFPOWER(e2,e3,ae2,ae3,ke);
    /* El calculo del Factor de Potencia es solo referencial, y su uso actual se limita
     * a saber si es distinto de cero y si la carga es de tipo Inductiva o 
     *
    */
    
//**************XBEE CON JSON***************************************************************************/
            
StaticJsonBuffer<600> jsonBuffer;
JsonObject& Sensor = jsonBuffer.createObject();

Sensor["ID"]=03;
Sensor["Fecha"]=reloj.Date();
Sensor["Tiempo"]=reloj.Time();


JsonObject& Atributo = Sensor.createNestedObject("Atributo");

//Voltaje
Atributo["id"]=3316;
Atributo["Valor"]=kv*v1;

Sensor.prettyPrintTo(Serial);
Serial.println(Sensor.measurePrettyLength());
Sensor.printTo(esXbee);

//Corriente
Atributo["id"]=3317;
Atributo["Valor"]=ki*i1;

Sensor.prettyPrintTo(Serial);
Serial.println(Sensor.measurePrettyLength());
Sensor.printTo(esXbee);

//Potencia Activa

energy = ke*(e3-e2)/basetime;
if(fabs(energy ) > 1e6){energy = 0.0;}
Atributo["id"]=33281;
Atributo["Valor"]=fabs(energy);

Sensor.prettyPrintTo(Serial);
Serial.println(Sensor.measurePrettyLength());
Sensor.printTo(esXbee);

//Potencia Aparente

aparent = ke*(ae3-ae2)/basetime;
if(fabs(aparent ) > 1e6){aparent = 0.0;}
Atributo["id"]=33282;
Atributo["Valor"]=fabs(aparent);

Sensor.prettyPrintTo(Serial);
Serial.println(Sensor.measurePrettyLength());
Sensor.printTo(esXbee);

//Potencia Reactiva

reactive = ke*(r3-r2)/basetime;
if(fabs(reactive ) > 1e6){reactive = 0.0;}
Atributo["id"]=33283;
Atributo["Valor"]=fabs(reactive);

Sensor.prettyPrintTo(Serial);
Serial.println(Sensor.measurePrettyLength());
Sensor.printTo(esXbee);

//Frecuencia

Atributo["id"]=3318;
Atributo["Valor"]=f1;

Sensor.prettyPrintTo(Serial);
Serial.println(Sensor.measurePrettyLength());
Sensor.printTo(esXbee);

//Factor de potencia

Atributo["id"]=3329;
Atributo["Valor"]=PF;

Sensor.prettyPrintTo(Serial);
Serial.println(Sensor.measurePrettyLength());
Sensor.printTo(esXbee);

//Tipo de Carga
if(reactive > 0)
      typeLoad = "cap";
    else if (reactive < 0)
      typeLoad = "ind";
    else
      typeLoad = "";
Atributo["id"]=33221;
Atributo["Valor"]=typeLoad;

Sensor.prettyPrintTo(Serial);
Serial.println(Sensor.measurePrettyLength());
Sensor.printTo(esXbee);

//Energia Total
totEnergy += fabs(energy)*basetime;
Atributo["id"]=3331;
Atributo["Valor"]=totEnergy/(3600);

Sensor.prettyPrintTo(Serial);
Serial.println(Sensor.measurePrettyLength());
Sensor.printTo(esXbee);
jsonBuffer.clear();

  delay(200);

  }
}

float calcFPOWER(long e2, long e3, long a2, long a3, float ke){

  float PF;

  if( fabs(ke*(e3-e2))  <= 0.0000002){
    PF = 1.0;
  }
  else if ( fabs(ke*(a3-a2)) <= 0.0000002)
  {
    PF = 0.0;
    Serial.println("WARNING - Power Factor");
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
    Serial.println("WARNING - Power Factor");
   
  }
    
  return PF;
}

