// Open Lanuza Thingspeak Ver 1.3
//
//MEDIDOR DE TEMPERATURA Y HUMEDAD
//Publica datos en Thingspeak
//www.openlanuza.com


#include "ThingSpeak.h" // Include ThingSpeak Library
#include "Barometer.h"
#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>


float temperature;
float pressure;
float atm;
float altitude;
Barometer myBarometer;

// MAC address for your Ethernet shield
byte mac[] = { 0x**, 0x**, 0x**, 0x**, 0x**, 0x** };
IPAddress ip(192,168,1,9);

unsigned long myChannelNumber = 909908; // Thingspeak número de canal
const char * myWriteAPIKey = "MHRJSJ5L6JS1415H"; // ThingSpeak write API Key

EthernetClient client;

void setup() {

ThingSpeak.begin(client); // Inicia ThingSpeak
myBarometer.init(); // Inicia el sensor

Serial.begin(9600); // Velocidad puerto serie


 while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(1000);
  }
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {

// Hacemos pausa de dos segundos antes de cada nueva medición
//al sensor le cuesta 250ms leer estos datos
delay(2000);

    temperature = myBarometer.bmp085GetTemperature(myBarometer.bmp085ReadUT()); //Get the temperature, bmp085ReadUT MUST be called first
    pressure = myBarometer.bmp085GetPressure(myBarometer.bmp085ReadUP());//Get the temperature
    altitude = myBarometer.calcAltitude(pressure); //Uncompensated calculation - in Meters
    atm = pressure / 101325;


// Comprobamos si las lecturas pueden dar algún fallo mediante la función isnan()
// Esta función devuelve un 1 en caso de que el valor no sea numérico
// Los caracteres || son como un OR. Si se cumple una de las dos condiciones mostramos error
if (isnan(temperature) || isnan(pressure)) {
Serial.println("¡Fallo en la lectura del sensor!");
return;
}

Serial.print("Temperatura: ");
Serial.print(temperature);
Serial.println(" *C\t ");
Serial.print("Presión: ");
Serial.print(pressure);
Serial.print(" Pa\t");


// Carga los valores a enviar
ThingSpeak.setField(1, temperature);
ThingSpeak.setField(2, pressure);

// Escribe todos los campos a la vez.
ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

Serial.println("¡Datos enviados a ThingSpeak!");

// Añadimos un retraso para limtitar el número de escrituras en Thinhspeak

int duracionDelay = 30; //En segundos
for (int i = 0; i < duracionDelay; i ++) { //Esto es debido a que el máximo que el Arduino puede procesar con precisión es 5000ms o 5 segundos
delay (1000);
}

}
