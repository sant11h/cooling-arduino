/**
 * Sistema de control de enfriamiento automático
 * Teoría de Control - 2020 
 * Santiago Gerhart
 */

//Pines Digitales
const int fanPin = 13;                  //Pin del Cooler
const int buttonPin = 12;               //Pin del Switch
const int ledPin = 11;                  //Pin del LED 

//Pines Analogicos
const int thermistorPin = A0;           //Pin del Termistor
const int potPin = A1;                  //Pin del Potenciómetro

//Variables del Switch
int state = LOW;                         //Estado actual del switch             
int reading;                             //Lectura actual del switch
int previous = HIGH;                     //Lectura anterior del switch
long time = 0;                           //La ultima vez que la salida fue modificada
long debounce = 200;                     //Tiempo de debounce

//Termistor
const long thermistorResistor = 9770;    //Valor de la resistencia en serie al termistor (Ohms)
float A = 1.129148e-3;                   //Coeficiente A de Steinhart-Hart 
float B = 2.34125e-4;                    //Coeficiente B de Steinhart-Hart
float C = 8.76741e-8;                    //Coeficiente C de Steinhart-Hart 

float getTemperature(float sensorRead){

  //Lee la entrada analogica el termistor y calcula la resistencia
  float R = (thermistorResistor *((1023 / sensorRead) - 1.0));  

  //Aplica la ecuación de Steinhart-Hart para calcular la temperatura
  float logR = log(R);
  float T_sh = 1.0 / (A + B * logR + C * pow(logR, 3));
  float celsius = T_sh - 273.15;
  return celsius;
}

void setup() {
  //Salidas
  pinMode(fanPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  //Entradas
  pinMode(buttonPin, INPUT);

  //Abre el puerto serie
  Serial.begin(9600);
}

void loop() {
  reading = digitalRead(buttonPin);

  //Logica del Switch ON-OFF
  if (reading == HIGH && previous == LOW && millis() - time > debounce){
    if (state == HIGH)
      state = LOW;
    else state = HIGH;

    time = millis();
  }

  //Si state es HIGH, calcula la temperatura y obtiene la temeperatura deseada del potenciómetro
  //luego imprime estos valores por el puerto serie.
  if (state){
    digitalWrite(ledPin, HIGH);
    
    float desiredTemp = map(analogRead(potPin), 0, 1023, 10, 30);
    float temp = getTemperature(analogRead(thermistorPin));

    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.print("°C  -  Temperatura deseada: ");
    Serial.print(desiredTemp);
    Serial.print("°C\n");

    if (temp > desiredTemp){
      digitalWrite(fanPin, HIGH);
    }
    else{
      digitalWrite(fanPin, LOW);
    }
    
    delay(1000);
  }
  else{
    digitalWrite(ledPin, LOW);
    digitalWrite(fanPin, LOW);  
  }

  previous = reading;
}
