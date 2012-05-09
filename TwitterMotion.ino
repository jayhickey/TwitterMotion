/* -*- c++ -*-
 *
 * Twitter.pde
 *
 * Author: Markku Rossi <mtr@iki.fi>
 *
 * Copyright (c) 2011-2012 Markku Rossi
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <sha1.h>
#include <Time.h>
#include <EEPROM.h>
#include <Twitter.h>
#include <pitches.h>

/* OneWire bus pin. */
//#define ONE_WIRE_BUS 4
//
//OneWire one_wire(ONE_WIRE_BUS);

#define ECHOPIN 2
#define TRIGPIN 3

/* Local network configuration. */
//uint8_t mac[6] =     {0xc4, 0x2c, 0x03, 0x0a, 0x3b, 0xb5};
//IPAddress ip(192, 168, 1, 43);
//IPAddress dns(192, 168, 1, 43);
//IPAddress gateway(192, 168, 1, 1);
//IPAddress subnet(255, 255, 255, 0);

byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0x7F, 0xEA };
IPAddress ip(192,168,0,165);


/* The IP address to connect to: Twitter or local HTTP proxy. */
IPAddress twitter_ip(199, 59, 148, 82);
uint16_t twitter_port = 80;

unsigned long last_tweet = 0;
int distance;
int melody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};
int noteDurations[] = {
  4, 8, 8, 4,4,4,4,4 };


#define TWEET_DELTA (60L * 60L)

/* Work buffer for twitter client.  This shold be fine for normal
   operations, the biggest items that are stored into the working
   buffer are URL encoded consumer and token secrets and HTTP response
   header lines. */
char buffer[512];

const static char consumer_key[] PROGMEM = "9bVSrh5eUVd0lCvMB97w";
const static char consumer_secret[] PROGMEM = "0PKeqZF94m9Bt8nrCyf7kik2ruOKzcPh2kLFDUlkoZ0";

Twitter twitter(buffer, sizeof(buffer));

void
setup()
{
  Serial.begin(9600);
  Serial.println("Arduino Twitter demo");
  
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN, OUTPUT);


#if 1
  if (Ethernet.begin(mac))
    {
      Serial.print("DHCP: ");
      Ethernet.localIP().printTo(Serial);
      Serial.println("");
    }
  else
    Serial.println("DHCP configuration failed");
#else
  Ethernet.begin(mac, ip);
#endif

  twitter.set_twitter_endpoint(PSTR("api.twitter.com"),
                               PSTR("/1/statuses/update.json"),
                               twitter_ip, twitter_port, false);
  twitter.set_client_id(consumer_key, consumer_secret);

//#if 1
  /* Read OAuth account identification from EEPROM. */
  twitter.set_account_id(256, 384);
//#else
  /* Set OAuth account identification from program memory. */
  twitter.set_account_id(PSTR("498939959-RMUzUEZuPcQbkX9UqAvjbIrodEwVesFps53vGV65"),
                         PSTR("kjoAlaD1zcCJtbl6nFMmMsEVaCT2cfveQFi0MJEs0"));
  Serial.println("Setting account identification");
//#endif

  delay(500);
}

void loop()
{
  if (twitter.is_ready())
    {
      unsigned long now = twitter.get_time();

      if (last_tweet == 0)
        {
          /* First round after twitter initialization. */
          Serial.print("Time is ");
          Serial.println(now);

          /* Wait few seconds before making our first tweet.  This
             gives our sensors some time to get running (I hope). */
          last_tweet = now - TWEET_DELTA + 15L;
        }
        
 
      int prevDistance = distance;
      
      
      digitalWrite(TRIGPIN, LOW);
      delayMicroseconds(2);
      digitalWrite(TRIGPIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIGPIN, LOW);
      distance = (pulseIn(ECHOPIN, HIGH) / 58);

      Serial.println(distance);
      
      int distanceChange = abs(prevDistance - distance);
                  
      if (distanceChange > 100)
        {
          
          char msg[64];


          sprintf(msg, "Someone walked by! Distance change: %d", distanceChange);
//
//          last_tweet = now;

          if (twitter.post_status(msg))
          {
            for (int thisNote = 0; thisNote < 8; thisNote++)
            {    
            int noteDuration = 1000/noteDurations[thisNote];
            tone(8, melody[thisNote],noteDuration);

            int pauseBetweenNotes = noteDuration * 1.30;
            delay(pauseBetweenNotes);
            // stop the tone playing:
            noTone(8);
            }          
            Serial.println("Status updated.");
            prevDistance = 0;
            distance = 0;
            distanceChange = 0;
          }
          else
            Serial.println("Update failed");
            
            delay(10000);
        }
    }

  delay(200);
}

<<<<<<< HEAD
// I'm removing your commit!

=======
>>>>>>> Revert "Tim's First Commit!"
