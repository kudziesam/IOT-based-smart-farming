#include <ESP8266WiFi.h>;
#include <WiFiClient.h>;
#include <ThingSpeak.h>;

const char* ssid = "BlitzTech_Electronics"; //Your Network SSID 
const char* password = "0p3ns0urc3C0d3#12"; //Your Network Password
//#include "ESP8266WiFi.h"
#include "DHT.h"
#define DHTPIN D5           //where dht22 will be conected
#define DHTTYPE DHT22       //type of DHT sensor to be used

///////variables for storing data to be used
float b;
int buf[10],temp;
unsigned long int avgValue;
float phValue;
float pH;

float h;
float t; 
int moisture;
int level;
int water;

DHT dht(DHTPIN, DHTTYPE);

//pin allocation for sensors and actuators
int pH_sensor = A0;
int float_switch = D3;
int pump = D7;
int buzzer = D2;

String server = "sipambi.000webhostapp.com"; 
WiFiClient client;
unsigned long myChannelNumber = 1084338; //Your Channel Number (Without Brackets)
const char * myWriteAPIKey = "1ZCGPNXIG2KTXT47"; //Your Write API Key


void setup() 
{
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  ThingSpeak.begin(client);
 
  dht.begin();
  pinMode(moisture, INPUT);
  pinMode(D1, OUTPUT);
  pinMode(float_switch, INPUT);
  pinMode(pump, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(float_switch, LOW);
}

void loop() 
{
  DHT_vals();
  ADC();
  //send_data();
}

void DHT_vals()
{
  delay(500);
  h = dht.readHumidity();
  t = dht.readTemperature();
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  delay(100);
  ThingSpeak.writeField(myChannelNumber, 1,t, myWriteAPIKey); 
  delay(100); 
  ThingSpeak.writeField(myChannelNumber, 2,h, myWriteAPIKey);
  delay(100);
}

void digital_sensors()
{
  level=digitalRead(float_switch);
  if(water<=50)
  {
    digitalWrite(pump, LOW);
    Serial.println("pump high");
  }
  else if(water>=50)
  {
    digitalWrite(pump, HIGH);
    Serial.println("pump low");
  }
  if(level==0)
  {
    digitalWrite(buzzer, HIGH);
    Serial.println("Level low");
  }
  else if(level==1)
  {
    digitalWrite(buzzer, LOW);
    Serial.println("Level high");
  }
}

void pH_measurement()
{
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(pH_sensor);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
  avgValue+=buf[i];
  phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue;                      //convert the millivolt into pH value
  
  Serial.print("    pH:");  
  Serial.print(phValue);
  Serial.println(" ");
 // digitalWrite(13, HIGH);       
  delay(800);
 // digitalWrite(13, LOW);
  ThingSpeak.writeField(myChannelNumber, 4,phValue, myWriteAPIKey);
  delay(100);
}


void ADC()
{
  digitalWrite(D1,HIGH);
  pH_measurement();
  digitalWrite(D1,LOW);
  moisture=analogRead(pH_sensor);
  water=map(moisture, 465, 1024, 100, 0);
  Serial.println(moisture);
  Serial.println(water);
  delay(100); 
  ThingSpeak.writeField(myChannelNumber, 3,water, myWriteAPIKey);
  digital_sensors();
  delay(100); 
  ThingSpeak.writeField(myChannelNumber, 5,level, myWriteAPIKey);
  delay(1000);
}

void send_data()
{
    
  ThingSpeak.writeField(myChannelNumber, 4,pH, myWriteAPIKey);
  
}

