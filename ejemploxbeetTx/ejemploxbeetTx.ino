void setup() {
 Serial.begin(57600); //Inicio del serial a 9600 baudios
 }

void loop() {
 //Escribe 'h' y 'l' en intervalos de un segundo vía Serial, que XBee emitirá por radio
 Serial.print('h');
 delay(1000);      //Retrasos de 1 segundo
 Serial.print('l');
 delay(1000); 
}
