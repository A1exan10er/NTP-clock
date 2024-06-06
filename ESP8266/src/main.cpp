#include <ESP8266WiFi.h>

#include <Arduino.h>
#include <U8g2lib.h>

// NTP clock
#include <NTPClient.h> // Network Time Protocol
#include <WiFiUdp.h>   // WiFi UDP                          Why?
#include <TimeLib.h>   // Time Library

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define SDA_PIN 14 // GPIO12 / 6
#define SCL_PIN 12 // GPIO14 / 5

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL_PIN, SDA_PIN, U8X8_PIN_NONE);

// Ausgeben der Daten zum WiFi Netzwerk auf dem OLED Display
void printWiFiNetworkData(int idx, String name)
{
    String msg = String(idx, DEC) + " " + name;
    u8g2.drawStr(0, idx * 15, msg.c_str());
}

// **************************************************************************************************
// Network credentials
const char *ssid = "U.S.S. Selection";
const char *password = "a4b3c2d1";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

// Weekdays, Months
String weekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

// **************************************************************************************************

void setup()
{
    Serial.begin(115200);
    // WiFi.mode(WIFI_STA);
    // WiFi.disconnect();

    // **************************************************************************************************
    // Connect to WiFi
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("");
        Serial.print("WiFi connected. ");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("WiFi not connected");
    }

    // Initialize a NTPClient to get time
    timeClient.begin();
    timeClient.setTimeOffset(7200); // GMT +2
    // **************************************************************************************************

    delay(100);
    u8g2.begin();
}

void loop()
{
    // **************************************************************************************************
    // Update time
    timeClient.update();
    // Get time
    String formattedTime = timeClient.getFormattedTime(); // HH:MM:SS
    Serial.print("Formatted time: ");
    Serial.println(formattedTime);
    // Get day of the week
    String weekDay = weekDays[timeClient.getDay()];
    Serial.print("Week day: ");
    Serial.println(weekDay);
    // Format time and day of the week
    String formattedTimeAndWeekDay = formattedTime + " " + weekDay;
    // Get month
    unsigned long epochTime = timeClient.getEpochTime();
    // struct tm *ptm = gmtime((time_t *)&epochTime);
    // int monthDay = ptm->tm_mday; // Day of the month
    // int currentMonth = ptm->tm_mon+1; // Month (0-11)
    // String currentMonthName = months[currentMonth-1]; // Month name
    int currentDay = day(epochTime);
    int currentMonth = month(epochTime);
    String currentMonthName = months[currentMonth - 1];
    int currentYear = year(epochTime);
    String currentDate = String(currentDay) + "-" + currentMonthName + "-" + String(currentYear);
    Serial.printf("Date: %s\n", currentDate.c_str());
    Serial.printf("Year: %d\n", currentYear);
    // **************************************************************************************************

    // Display results on OLED
    u8g2.clearBuffer(); // Clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr);
    // u8g2.drawStr(0, 10, formattedTime.c_str());
    // u8g2.drawStr(0, 20, weekDay.c_str());
    u8g2.drawStr(0, 10, formattedTimeAndWeekDay.c_str());
    u8g2.drawStr(0, 24, currentDate.c_str());
    // u8g2.drawStr(0, 50, currentMonthName.c_str());
    // u8g2.drawStr(0, 60, String(currentYear).c_str());
    u8g2.sendBuffer();
    delay(1000);
    // **************************************************************************************************


    // String ssid;
    // int32_t rssi;
    // uint8_t encryptionType;
    // uint8_t *bssid;
    // int32_t channel;
    // bool hidden;
    // int scanResult;

    // scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);

    // if (scanResult == 0)
    // {
    //     Serial.println(F("Keine WiFi-Netzwerke gefunden"));
    // }
    // else if (scanResult > 0)
    // {
    //     // Zwischenspeicher und Display leeren
    //     u8g2.clearBuffer();
    //     // Schriftgröße setzen
    //     u8g2.setFont(u8g2_font_ncenB08_tr);

    //     for (int8_t i = 0; i < scanResult; i++)
    //     {
    //         WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);
    //         printWiFiNetworkData(i, ssid.c_str());
    //         Serial.printf(PSTR("  %02d: %s\n"), i + 1, ssid.c_str());
    //         yield();
    //     }
    //     // Absenden der Daten vom Zwischenspeichern an das Display
    //     u8g2.sendBuffer();
    // }
    // else
    // {
    //     Serial.printf(PSTR("WiFi scan error %d"), scanResult);
    // }
    // // 5 Sekunden Pause zwischen den Scans
    // delay(5000);
}