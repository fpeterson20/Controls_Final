#include "Timer.h"

Timer timer(MILLIS);
#define BLOWDRYERPIN 3

#define SERIESRESISTOR1 9940    
 #define SERIESRESISTOR2 9950  
// What pin to connect the sensor to
#define THERMISTORPIN1 A0 
#define THERMISTORPIN2 A1 
 // resistance at 25 degrees C
#define THERMISTORNOMINAL 10000      
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950

#define NUMSAMPLES 5

int samples1[NUMSAMPLES];
int samples2[NUMSAMPLES];

float readToTemp(float temp){
  float steinhart;
  steinhart = temp / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert absolute temp to C
  steinhart = (steinhart * 9/5) + 32;         // convert to F
  return steinhart;
}

float readToRes(float average, int seriesRes){
  average = (1023 / average)  - 1;     // (1023/ADC - 1) 
  average = seriesRes / average;
  return average;
}

void setup(void) {
  Serial.begin(9600);
  timer.start();
  pinMode(BLOWDRYERPIN, OUTPUT);
}
 
void loop(void) {
  uint8_t i;
  float average1 = 0;
  float average2 = 0;
  Serial.print("Time: ");
  Serial.println(timer.read());

  if(timer.read()<10000){
    digitalWrite(BLOWDRYERPIN, HIGH);
  }else if(timer.read()<60000){
    digitalWrite(BLOWDRYERPIN, LOW);
  }else{
    digitalWrite(BLOWDRYERPIN, LOW);
    timer.stop();
    timer.start();
  }

  for(i=0; i < NUMSAMPLES; i++){
    samples1[i] = analogRead(THERMISTORPIN1);
    samples2[i] = analogRead(THERMISTORPIN2);
    delay(10);
  }
  
  // average all the samples out
  average1 = 0;
  average2 = 0;
  for (i=0; i< NUMSAMPLES; i++) {
    average1 += samples1[i];
    average2 += samples2[i];
  }
  average1 /= NUMSAMPLES;
  average2 /= NUMSAMPLES;
  /*
  Serial.print("Average analog readings: "); 
  Serial.print(average1);
  Serial.print(" ");
  Serial.println(average2);
  */
  
  // convert the value to resistance
  average1 = readToRes(average1, SERIESRESISTOR1);
  average2 = readToRes(average2, SERIESRESISTOR2);

  /*
  Serial.print("Thermistor resistance "); 
  Serial.print(average1);
  Serial.print(" ");
  Serial.println(average2);
  */

  float temp1;
  float temp2;
  temp1 = readToTemp(average1);
  temp2 = readToTemp(average2);

  Serial.print("Temperature 1:"); 
  Serial.print(temp1);
  Serial.println(" *F");

  Serial.print("Temperature 2:"); 
  Serial.print(temp2);
  Serial.println(" *F");

  delay(500);
}

