//Inicializar el pin del led y la variable para almacenar el mensaje enviado por Serial

const int ledPin = 13;   //const es para mantener el valor fijo de la variable (no permite cambiarla en el resto del código)
int mensaje;             // declaración variable mensaje

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);   //Declara pin 13 como salida
}


void loop() {
  if (Serial.available()>0){    //Si es mayor a 0 se comprueba que recibe un byte via serial
    mensaje=Serial.read();      //almacena el byte recibido en variable mensaje

    if (mensaje=='h'){
      digitalWrite(ledPin,HIGH);
    }
  else if (mensaje=='1'){
     digitalWrite(ledPin,LOW);
    }
  else{}
  }
}
