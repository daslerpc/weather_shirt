
#include <gLCD.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>

// Modes of operation.  SENSOR_READING is the normal mode of operaion.  The rest are meant for demoing.
#define SENSOR_READING 0
#define SUN 1
#define HOT 2
#define MUGGY 3
#define CLOUDY 4
#define RAIN 5
#define END_OF_STATES 4

// Mode select button
#define BUTTON 3
#define DUMMY_VOLTS 2

// Humidity sensor
#define DHTPIN 4     // what pin humidity is connected to
#define DHTTYPE DHT11   // DHT 11 

// LCD Screen pins
#define RST 8
#define CS 9
#define Clk 13
#define Dat 11

// Heating Pads
#define HEATING_PAD_1 5
#define HEATING_PAD_2 6

// Pressure Sensor
/* 
SDA to I2C Data (Analog 4)
SCL to I2C Clock (Analog 5)
*/

// Current mode
int mode = 0;

// State Variables
boolean humid = false;
boolean hot = false;

// Thresholds
const int humidity_thresh = 44;

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

int sweatX = 110;
int sweatY = 90;


void setup() {
  pinMode( BUTTON, INPUT );
  pinMode( HEATING_PAD_1, OUTPUT );
  pinMode( HEATING_PAD_2, OUTPUT );
  
  // For supplying a test voltage to the button
  pinMode( DUMMY_VOLTS, OUTPUT );
  digitalWrite( DUMMY_VOLTS, HIGH );
  
  graphic.Init(0,1,0,1,1); //Revesed X direction.

  clearImage();  

  dht.begin();

  Serial.begin(9600);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {
    }
  }
  
  digitalWrite( HEATING_PAD_1, HIGH );
  digitalWrite( HEATING_PAD_2, HIGH );

  for( int x=0; x<END_OF_STATES; x++ ) {
    advanceMode();
    delay(3000);
  }
    
  changeMode(0);
}

void loop() {
  /*
  if( digitalRead(BUTTON) ) {
    advanceMode();
    delay(250);
  }
*/

  if( mode == SENSOR_READING ) {
    readSensors();
    processSensors();
  }
}

void advanceMode() {
  int prev_mode = mode;
  
  mode++;
  if( mode == END_OF_STATES )
    mode = 0;

  Serial.print("Changing mode from mode ");
  Serial.print(prev_mode);
  Serial.print(" to ");
  Serial.print(mode);
  Serial.println(".");
  handleModeChange();
}

void changeMode( int new_mode ){
  mode = new_mode;
  handleModeChange();
}

void handleModeChange() {
  //clearImage();
  digitalWrite( HEATING_PAD_1, HIGH );
  digitalWrite( HEATING_PAD_2, HIGH );

  switch( mode ) {
  case SENSOR_READING:
    drawSun();
    break;
  case SUN:
    drawSun();
    break;
  case HOT:
    drawSunglasses();
    break;
  case MUGGY:
    drawSweat(sweatX, sweatY);
    break;
  case RAIN:
    drawRain();
    break;  
  case CLOUDY:
    drawSun();
    drawCloud();
    break;
  }
}

void processSensors() {
  if( !humid && hum > humidity_thresh) {
    humid = true;
    drawSweat(sweatX, sweatY);
  }
  
  if( humid && hum <= humidity_thresh ) {
    humid = false;
    drawSun();
  }
  
  if ( !hot && temp > 75 ) {
    hot = true;
    drawSunglasses();
  }
  
   if ( hot && temp <= 75 ) {
    hot = false;
    drawSun();
  }
}

void drawRain() {
  digitalWrite( HEATING_PAD_1, LOW );
}

void drawCloud() {
  digitalWrite( HEATING_PAD_2, LOW );
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
  Serial.println("Sun");
  graphic.SetForeColour(15,15,0);
  graphic.Circle(65,65,64,5); 
}

void drawSunglasses(){
  graphic.SetForeColour(0,0,0);
  
  // Lenses
  graphic.Circle(75,42,18,5); 
  graphic.Circle(75,88,18,5);

  // Cross piece
  graphic.Box(77,42,82,88,5);

  // Ear pieces
  graphic.Box(78,0,81,30,5);
  graphic.Box(78,100,81,130,5);

  // Square off lenses
  graphic.SetForeColour(15,15,0);
  graphic.Box(83,20,100,60,5);
  graphic.Box(83,70,100,110,5);

  // Reflections
  graphic.SetForeColour(15,15,15);
  graphic.Box(78,35,79,39,5);
  graphic.Box(78,75,79,79,5);
}

void drawSweat(int x, int y) {
  graphic.SetForeColour(0, 0, 15);
  graphic.Circle(y+3,x,6,5); 

  for(int i=0; i<5; i++){
    graphic.Line(y+i,x-6,y+7+i,x,1);
    graphic.Line(y+i,x+6,y+7+i,x,1);
  }

  graphic.SetForeColour(15, 15, 15);
  graphic.Line(y+5, x-3, y+6, x-2, 1);
}













