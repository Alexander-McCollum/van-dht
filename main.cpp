#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include "Adafruit_Sensor.h"
#include "DHT.h"
#include "Functions.h" // declarations for custom functions

#define DHTPIN 7      // data pin
#define DHTTYPE DHT22 // DHT 22  (AM2302)

File datafile;
RTC_DS3231 rtc;
DHT dht(DHTPIN, DHTTYPE); // create dht object

const char *filename = "newvandata.txt";
const int sampleDelay = 1000; // delay for each DHT measurement [ms]
const float interval = 900;   // collection interval of averaged data samples [s]

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  beginSD(10);
  beginRTC();
  dht.begin();
  delay(1000);

  newFileHeader(filename, "t\tH[%]\tT[C]\tT_RTC[C]");
}

void loop()
{
  float H = 0;          // humidity value from dht
  float T = 0;          // temperature value from dht
  float T_rtc = 0;      // temperature value from rtc
  String dataline = ""; // line to write to file

  for (int i = 0; i < ((int)interval); i++)
  {
    H += dht.readHumidity();
    T += dht.readTemperature();
    T_rtc += rtc.getTemperature();

    delay(sampleDelay);
  }
  H = H / (interval);
  T = T / (interval);
  T_rtc = T_rtc / (interval);

  dataline = getTimestamp() + '\t' + String(H) + '\t' + String(T) + '\t' + String(rtc.getTemperature());
  writeFile(filename, dataline);
}

//=============================================Functions=================================================//

void beginSD(const int CS)
{
  // Activates SD card
  if (CS != 10)
  {
    pinMode(CS, OUTPUT); // initialize CS as output if not using harware ss pin (arduino nano)
  }

  if (SD.begin(CS))
  {
    Serial.println("SD card started successfully.");
  }
  else
  {
    Serial.println("Failed to start SD card");
    return;
  }
}

void newFileHeader(const char *name, const char *header)
{
  // Checks if a file is empty. If so, add a header.
  datafile = SD.open(name, FILE_WRITE);
  if (datafile.size() == 0)
  {
    Serial.println("Header added to file.\n");
    datafile.println(header);
  }
  datafile.close();
  Serial.println(header);
}

void writeFile(const char *name, String line)
{
  // Writes a line of text to a file
  datafile = SD.open(name, FILE_WRITE);
  if (datafile)
  {
    Serial.println(line);
    datafile.println(line);
  }
  datafile.close();
}

void beginRTC()
{
  // Start RTC and calibrate if needed
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (rtc.lostPower())
  {
    Serial.println("Setting RTC to current time.");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

String getTimestamp()
{
  // Returns a timestamp formatted for easy use in Excel
  DateTime now = rtc.now();
  String timestamp = "";
  timestamp += String(now.day()) + '/';
  timestamp += String(now.month()) + '/';
  timestamp += String(now.year()) + ' ';
  timestamp += String(now.hour()) + ':';
  timestamp += String(now.minute()) + ':';
  timestamp += String(now.second());
  return timestamp;
}