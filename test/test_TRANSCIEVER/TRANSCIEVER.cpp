// RFM69HCW Example Sketch
// Send serial input characters from one RFM69 node to another
// Based on RFM69 library sample code by Felix Rusu
// http://LowPowerLab.com/contact
// Modified for RFM69HCW by Mike Grusin, 4/16

// This sketch will show you the basics of using an
// RFM69HCW radio module. SparkFun's part numbers are:
// 915MHz: https://www.sparkfun.com/products/12775
// 434MHz: https://www.sparkfun.com/products/12823

// See the hook-up guide for wiring instructions:
// https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide

// Uses the RFM69 library by Felix Rusu, LowPowerLab.com
// Original library: https://www.github.com/lowpowerlab/rfm69
// SparkFun repository: https://github.com/sparkfun/RFM69HCW_Breakout

// Include the RFM69 and SPI libraries:

#include "Wire.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_I2CDevice.h>
#include <Arduino.h>
#include <SPI.h>
#include <RHHardwareSPI.h>
#include <RHHardwareSPI1.h>

#include <RFM69.h>

// Addresses for this node. CHANGE THESE FOR EACH NODE!

#define NETWORKID 0 // Must be the same for all nodes
#define MYNODEID 2  // My node ID
#define TONODEID 1  // Destination node ID

// RFM69 frequency, uncomment the frequency of your module:

#define FREQUENCY RF69_433MHZ
// #define FREQUENCY     RF69_915MHZ

// AES encryption (or not):

#define ENCRYPT true                  // Set to "true" to use encryption
#define ENCRYPTKEY "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes

// Use ACKnowledge when sending messages (or not):

#define USEACK false // Request ACKs or not

// Packet sent/received indicator LED (optional):

#define LED 9 // LED positive pin
#define GND 8 // LED ground pin

#define BUFFERLEN 10
#define RFM69_RST_PIN 25

// Create a library object for our RFM69HCW module:

RFM69 radio(24, 28, true, &SPI1);

void setup()
{
  // Open a serial port so we can send keystrokes to the module:

  Serial.begin(9600);
  Serial.print("Node ");
  Serial.print(MYNODEID, DEC);
  Serial.println(" ready");

  // Set up the indicator LED (optional):

  pinMode(RFM69_RST_PIN, OUTPUT);
  digitalWrite(RFM69_RST_PIN, HIGH);
  delay(100);
  digitalWrite(RFM69_RST_PIN, LOW);
  delay(100);

  // Initialize the RFM69HCW:
  // radio.setCS(10);  //uncomment this if using Pro Micro
  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  radio.setHighPower(); // Always use this for RFM69HCW
  radio.spyMode();
  radio.setPowerLevel(23);
  // Turn on encryption if desired:

  if (ENCRYPT)
    radio.encrypt(ENCRYPTKEY);
}

void loop()
{
  // Set up a "buffer" for characters that we'll send:

  static char sendbuffer[BUFFERLEN];
  static int sendlength = 0;

  // SENDING

  // In this section, we'll gather serial characters and
  // send them to the other node if we (1) get a carriage return,
  // or (2) the buffer is full (61 characters).

  // If there is any serial input, add it to the buffer:

  // test //
  // delay(100);
  // sendlength = 5;
  // sendbuffer[0] = 'h';
  // sendbuffer[1] = 'e';
  // sendbuffer[2] = 'l';
  // sendbuffer[3] = 'l';
  // sendbuffer[4] = 'o';
  // Serial.print("sending to node ");
  // Serial.print(TONODEID, DEC);
  // Serial.print(", message [");
  // for (byte i = 0; i < sendlength; i++)
  //   Serial.print(sendbuffer[i]);
  // Serial.println("]");

  // // There are two ways to send packets. If you want
  // // acknowledgements, use sendWithRetry():

  // if (USEACK)
  // {
  //   if (radio.sendWithRetry(TONODEID, sendbuffer, sendlength))
  //     Serial.println("ACK received!");
  //   else
  //     Serial.println("no ACK received");
  // }

  // // If you don't need acknowledgements, just use send():

  // else // don't use ACK
  // {
  //   radio.send(TONODEID, sendbuffer, sendlength);
  // }

  // sendlength = 0; // reset the packet
  // // test //

  if (Serial.available() > 0)
  {
    char input = Serial.read();

    if (input != '\r') // not a carriage return
    {
      sendbuffer[sendlength] = input;
      sendlength++;
    }

    // If the input is a carriage return, or the buffer is full:

    if ((input == '\r') || (sendlength == BUFFERLEN - 1)) // CR or buffer full
    {
      // Send the packet!

      Serial.print("sending to node ");
      Serial.print(TONODEID, DEC);
      Serial.print(", message [");
      for (byte i = 0; i < sendlength; i++)
        Serial.print(sendbuffer[i]);
      Serial.println("]");

      // There are two ways to send packets. If you want
      // acknowledgements, use sendWithRetry():

      if (USEACK)
      {
        if (radio.sendWithRetry(TONODEID, sendbuffer, sendlength))
          Serial.println("ACK received!");
        else
          Serial.println("no ACK received");
      }

      // If you don't need acknowledgements, just use send():

      else // don't use ACK
      {
        radio.send(TONODEID, sendbuffer, sendlength);
      }

      sendlength = 0; // reset the packet
      // Blink(LED,10);
    }
  }

  // RECEIVING

  // In this section, we'll check with the RFM69HCW to see
  // if it has received any packets:

  if (radio.receiveDone()) // Got one!
  {
    // Print out the information:

    Serial.print("received from node ");
    Serial.print(radio.SENDERID, DEC);
    Serial.print(", message [");

    // The actual message is contained in the DATA array,
    // and is DATALEN bytes in size:

    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);

    // RSSI is the "Receive Signal Strength Indicator",
    // smaller numbers mean higher power.

    Serial.print("], RSSI ");
    Serial.println(radio.RSSI);

    // Send an ACK if requested.
    // (You don't need this code if you're not using ACKs.)

    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.println("ACK sent");
    }
    // Blink(LED,10);
  }
}

void Blink(byte PIN, int DELAY_MS)
// Blink an LED for a given number of ms
{
  digitalWrite(PIN, HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN, LOW);
}