include <OneWire.h> // For the temp sensor
#include <DallasTemperature.h> // For the temp sensor
#include <LiquidCrystal.h>  // For the LCD Display

#define ONE_WIRE_BUS 10 // For the temperature sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Creates an LC object. Parameters: (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

float temps[24];          // array where the last 24 temperatures are kept
float temperature;        // the most recent temperature
float mean = 0;           // the mean of the most recent temperatures
int samples = 0;          // the current number of samples
int maxsamples = 24;      // the maximum number of samples

// temp limits
float maxtemp = 32;       // red led temp
float mintemp = 26;       // blue led temp
float relaytemp = 29;     // yellow led temp

// leds
int red_led = 9;          // red led pin
int green_led = 8;        // blue led pin
int yellow_led = 7;       // yellow led pin

// distance sensor
const int trigPin = 13;   // trig pin
const int echoPin = 6;    // echo pin
long duration;            // duration of the pulse from echo pin
int distance;             // distance measurement
int minDist = 15;         // minimum distance that the temps display is triggered
bool flag = 0;            // this flag helps to moderate the update of the display 

unsigned long currentMillis = 0;   // stopwatch variables
unsigned long previousMillis = 0;
unsigned long interval = 5000;    

void setup() {
  
  Serial.begin(9600); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  sensors.begin(); // start measuring temperature module
  pinMode(red_led, OUTPUT);   // set output ports for leds
  pinMode(green_led, OUTPUT);
  pinMode(yellow_led, OUTPUT);
  digitalWrite(red_led, LOW); // and switch them off
  digitalWrite(green_led, LOW);
  digitalWrite(yellow_led, LOW);
  // Initializes the interface to the LCD screen,
  //and specifies the dimensions (width and height) of the display
  lcd.begin(16, 2);
  
}

void loop() {

  distance = measureDist();
  //Serial.println(distance);
  
  if ((distance <= minDist) && !flag){    // print mean and last temp measurement
    lcd.clear();                          // if user gets too close
    lcd.print("Mean: ");
    lcd.print(mean);                      
    lcd.print(" oC");
    lcd.setCursor(0,1);
    lcd.print("Last: ");
    lcd.print(temperature);
    lcd.print(" oC");
    flag = true;
  }
  else if((distance > minDist) && flag){
    flag = false;
    lcd.clear();
  }

    currentMillis = millis();
  
  // this is triggered every ~ 5 s
  if ((currentMillis - previousMillis) >= interval) {
    previousMillis = currentMillis;
    // take a temp measurement
    tempCheck();
    flag = false;
  }


}

void tempCheck() {

  // take a temp measurement
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0); // take measurement
  // display it on stdout
  Serial.print("Temperature is: ");
  Serial.println(sensors.getTempCByIndex(0));

  // save it
  temps[samples] = temperature;
  samples++;

  // trigger any leds, if necessary
  alarmLedsCheck();
  relayCheck();

  // clear the mean display after 2 measurements = ~ 10 sec
  if (samples == 2) {
    lcd.clear();  // upon the second sample clear the display, as approx 10 sec have passed
  }
  else if (samples == maxsamples) {

    // produce the mean of the last 24 samples
    mean = 0; 
    for (int i = 0; i < maxsamples; i++) {
      mean += temps[i];
    }
    mean /= (float)maxsamples;

    // and display it on the LCD
    lcd.clear();
    lcd.print("Mean temperature: ");
    lcd.setCursor(0, 1);
    lcd.print(mean);
    lcd.print("oC");
    // display it also in stdout
    Serial.print("Mean temperature: ");
    Serial.print(mean);
    Serial.println(" oC");
    
    // reset sample counter
    samples = 0;
  }
}

// This func triggers any warning leds
// if its too hot or too cold
void alarmLedsCheck() {

  if (temperature <= mintemp) {
    digitalWrite(green_led, HIGH);
    digitalWrite(red_led, LOW);
    lcd.clear();
    lcd.print("Temperature LOW");
  }
  else if (temperature >= maxtemp) {
    digitalWrite(red_led, HIGH);
    digitalWrite(green_led, LOW);
    lcd.clear();
    lcd.print("Temperature HIGH");
  }
  else {
    digitalWrite(red_led, LOW);
    digitalWrite(green_led, LOW);
  }
}

// relay activator
void relayCheck() {

  if (temperature >= relaytemp)
    digitalWrite(yellow_led, HIGH);
  else
    digitalWrite(yellow_led, LOW);
}

// take a measurement of the distance
int measureDist() {
  
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  return duration * 0.034 / 2;

}