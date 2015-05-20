
#include <gLCD.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>

// Modes of operation.  SENSOR_READING is the normal mode of operaion.  The rest are meant for demoing.
#define SENSOR_READING 0
#define SUN 1
#define HOT 2
#define MUGGY 3
#define RAIN 4
#define END_OF_STATES 5

// Mode select button
#define BUTTON 4

// Humidity sensor
#define DHTPIN 2     // what pin humidity is connected to
#define DHTTYPE DHT11   // DHT 11 

// LCD Screen pins
#define RST 8
#define CS 9
#define Clk 13
#define Dat 11

// Heating Pads
#define HEATING_PAD_1 5
#define HEATING_PAD_2 6

// Current mode
int mode = 0;

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

// Initialize pressure sensor
Adafruit_BMP085 bmp;

// Initialize the LCD screen
gLCD graphic(RST,CS,Clk,Dat,1);

// Sensor values
float hum; // humidity
float temp; // temperature
float pressure; // pressure, duh

// calculated values
float hi; // heat index


void setup() {
  pinMode( BUTTON, INPUT );
  pinMode( HEATING_PAD_1, OUTPUT );
  pinMode( HEATING_PAD_2, OUTPUT );

  graphic.Init(0,1,0,1,1); //Revesed X direction.

  clearImage();  

  dht.begin();

  Serial.begin(9600);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {
    }
  }

}

void loop() {
  if( digitalRead(BUTTON) ) {
    advanceMode();
    delay(250);
  }

  if( mode == SENSOR_READING ) {
    readSensors();
    processSensors();
  }
}

void advanceMode() {
  mode++;
  if( mode == END_OF_STATES )
    mode = 0;

  handleModeChange();
}

void changeMode( int new_mode ){
  mode = new_mode;
  handleModeChange();
}

void handleModeChange() {
  clearImage();
  analogWrite( HEATING_PAD_1, 0 );
  analogWrite( HEATING_PAD_2, 0 );

  switch( mode ) {
  case SUN:
    drawSun();
    break;
  case HOT:
    drawSunglasses();
    break;
  case MUGGY:
    drawSweat(120, 65);
    break;
  case RAIN:
    drawRain();
    break;
  }
}

void processSensors() {

}

void drawRain() {
  analogWrite( HEATING_PAD_1, 255 );
  analogWrite( HEATING_PAD_2, 255 );
}

void readSensors() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  hum = dht.readHumidity();
  // Read temperature as Celsius (average from two sensors) and convert to Fahrenheit
  temp = 32 + 1.8*(dht.readTemperature() + bmp.readTemperature())/2.0;
  // Read pressue in pascals
  pressure = bmp.readPressure();

  // Check if any reads failed and exit early (to try again).
  if (isnan(hum) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  hi = dht.computeHeatIndex(temp, hum);

  Serial.print("Humidity: "); 
  Serial.print(hum);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(temp);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F\t");
  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" Pa");
}

void clearImage() {
  graphic.SetBackColour(0,0,0);
  graphic.Box(0,0,130,130,0);
}

void drawSun() {
  graphic.SetForeColour(15,15,0);
  graphic.Circle(65,65,64,5); 
}

void drawSunglasses(){
  graphic.SetForeColour(0,0,0);
  graphic.Circle(42,55,18,5); 
  graphic.Circle(88,55,18,5); 

  graphic.Box(42,48,88,53,5);

  graphic.Box(0,49,30,52,5);
  graphic.Box(100,49,130,52,5);

  graphic.SetForeColour(15,15,0);
  graphic.Box(20,30,60,47,5);
  graphic.Box(70,30,110,47,5);

  graphic.SetForeColour(15,15,15);
  graphic.Box(35,51,39,52,5);
  graphic.Box(75,51,79,52,5);
}

void drawSweat(int x, int y) {
  graphic.SetForeColour(0, 0, 15);
  graphic.Circle(x,y+3,6,5); 

  for(int i=0; i<5; i++){
    graphic.Line(x-6,y+i,x,y-7+i,1);
    graphic.Line(x+6,y+i,x,y-7+i,1);
  }

  graphic.SetForeColour(15, 15, 15);
  graphic.Line(x-3, y, x-2, y-1, 1);
}










