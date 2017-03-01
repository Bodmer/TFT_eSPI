// TFT screenshot server

// This is a sketch support tab containing function calls to read a screen image
// off a TFT and send it to a processing client sketch over the serial port.

// See the processing_sketch tab, it contains a copy of the processing sketch.

// Use a high baud rate, for an ESP8266:
/*
   Serial.begin(921600);
*/
// 240 x 320 images take about 3.5s to transfer at 921600 baud(minimum is ~2.5s)

// This sketch has been created to work with the TFT_eSPI library here:
// https://github.com/Bodmer/TFT_eSPI

// Created by: Bodmer 27/1/17

// The MIT permissive free software license applies, include all text above in
// derivatives.

#define BAUD_RATE 250000      // Maximum Arduino IDE Serial Monitor rate
#define DUMP_BAUD_RATE 921600 // Rate used for screen dumps by ESP8266

#define PIXEL_TIMEOUT 100    // 100ms Time-out between pixel requests
#define START_TIMEOUT 10000  // 10s Maximum time to wait at start transfer

// Start a screen dump server (serial or network)
boolean screenServer(void)
{
  Serial.end();                 // Stop the serial port (clears buffers too)
  Serial.begin(DUMP_BAUD_RATE); // Force baud rate to be high
  yield();
  
  boolean result = serialScreenServer(); // Screenshot serial port server
  //boolean result = wifiDump();           // Screenshot WiFi UDP port server (WIP)
  
  Serial.end();                 // Stop the serial port (clears buffers too)
  Serial.begin(BAUD_RATE);      // Return baud rate to normal
  yield();

  //Serial.println();
  //if (result) Serial.println(F("Screen dump passed :-)"));
  //else        Serial.println(F("Screen dump failed :-("));
  
  return result;
}

// Screenshot serial port server (Processing sketch acts as client)
boolean serialScreenServer(void)
{
  // Serial commands from client:
  //    'S' to start the transfer process (To do: reply with width + height)
  //    'R' or any character except 'X' to request pixel
  //    'X' to abort and return immediately to caller
  // Returned boolean values:
  //     true = image despatched OK
  //    false = time-out or abort command received

  // Precautionary receive buffer garbage flush for 50ms
  uint32_t clearTime = millis() + 50;
  while ( millis() < clearTime ) {
    Serial.read();
    yield();
  }

  boolean wait = true;
  uint32_t lastCmdTime = millis();     // Initialise start of command time-out

  // Wait for the starting flag with a start time-out
  while (wait)
  {
    yield();
    // Check serial buffer
    if (Serial.available() > 0) {
      // Read the command byte
      uint8_t cmd = Serial.read();
      // If it is 'S' (start command) then clear the serial buffer for 100ms and stop waiting
      if ( cmd == 'S' ) {
        // Precautionary receive buffer garbage flush for 50ms
        clearTime = millis() + 50;
        while ( millis() < clearTime ) {
          Serial.read();
          yield();
        }
        wait = false;           // No need to wait anymore
        lastCmdTime = millis(); // Set last received command time

        // Send screen size, not supported by processing sketch yet
        //Serial.write('W');
        //Serial.write(tft.width()  >> 8);
        //Serial.write(tft.width()  & 0xFF);
        //Serial.write('H');
        //Serial.write(tft.height() >> 8);
        //Serial.write(tft.height() & 0xFF);
        //Serial.write('Y');
      }
    }
    else
    {
      // Check for time-out
      if ( millis() > lastCmdTime + START_TIMEOUT) return false;
    }
  }

  uint8_t color[3]; // RGB color buffer for 1 pixel
  
  // Send all the pixels on the whole screen (typically 5 seconds at 921600 baud)
  for ( uint32_t y = 0; y < tft.height(); y++)
  {
    // Increment x by 2 as we send 2 pixels for every byte received
    for ( uint32_t x = 0; x < tft.width(); x += 1)
    {
      yield();

      // Wait here for serial data to arrive or a time-out elapses
      while ( Serial.available() == 0 )
      {
        yield;
        if ( millis() > lastCmdTime + PIXEL_TIMEOUT) return false;
      }

      // Serial data must be available to get here, read 1 byte and
      // respond with N pixels, i.e. N x 3 RGB bytes
      if ( Serial.read() == 'X' ) {
        // X command byte means abort, so clear the buffer and return
        clearTime = millis() + 50;
        while ( millis() < clearTime ) Serial.read();
        return false;
      }
      // Save arrival time of the read command (for later time-out check)
      lastCmdTime = millis();

      // Fetch data for N pixels starting at x,y
      tft.readRectRGB(x, y, 1, 1, color);
      // Send values to client
      Serial.write(color[0]); // Pixel 1 red
      Serial.write(color[1]); // Pixel 1 green
      Serial.write(color[2]); // Pixel 1 blue
      //Serial.write(color[3]); // Pixel 2 red
      //Serial.write(color[4]); // Pixel 2 green
      //Serial.write(color[5]); // Pixel 2 blue
    }
  }

  // Receive buffer excess command flush for 50ms
  clearTime = millis() + 50;
  while ( millis() < clearTime ) Serial.read();

  return true;
}

