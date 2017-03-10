// Latest at 03/03/17

// This is a Processing sketch, see https://processing.org/ to download the IDE

// The sketch is a client that requests TFT screenshots from an Arduino board.
// The arduino must call a screenshot server function to respond with pixels.

// It has been created to work with the TFT_eSPI library here:
// https://github.com/Bodmer/TFT_eSPI

// The library provides a member function that reads the RGB values of screen pixels
// and an example TFT_Screen_Capture

// Captured images are stored in the sketch folder, use "Sketch" menu option
// "Show Sketch Folder" or press Ctrl+K

// Created by: Bodmer 27/1/17

// MIT licence applies, all text above must be included in derivative works

import processing.serial.*;

Serial serial;           // Create an instance called serial

// ###########################################################################################
// #                  These are the values to change for a particular setup                  #
//                                                                                           #
int serial_port = 1;     // Use enumerated value from list provided when sketch is run       #
int serial_baud_rate = 921600;  // Maximum tested is 921600                                  #
//                                                                                           #
int tft_width  = 240;    // TFT width in portrait orientation                                #
int tft_height = 320;    // TFT height                                                       #
//int tft_width  = 320;    // TFT width in landscape orientation                             #
//int tft_height = 240;    // TFT height                                                     #
//                                                                                           #
// Change the image file type saved here, comment out all but one                            #
//String image_type = ".jpg"; //                                                             #
String image_type = ".png";   //                                                             #
//String image_type = ".bmp"; //                                                             #
//String image_type = ".tif"; //                                                             #
//                                                                                           #
boolean save_border = true;   // Save the image with a border                                #
int border = 5;               // Border pixel width                                          #
boolean fade = true ;         // Fade out image after saving                                 #
//                                                                                           #
int max_images = 10; // Maximum of numbered saved images before over-writing files           #
//                                                                                           #
// #                   End of the values to change for a particular setup                    #
// ###########################################################################################

int serialCount = 0;    // Count of colour bytes arriving

int bgcolor = 255;			// Background color

PImage img, tft_img;

color light_blue = color(50, 128, 255);

int[] rgb = new int[6]; // Buffer for the RGB colour bytes

int indexRed   = 0;     // Colour byte index in the array
int indexGreen = 1;
int indexBlue  = 2;

long end = 10;

int n = 0;

boolean got_image = false;

int x_offset = (500 - tft_width) /2; // Image offsets in the window
int y_offset = 20; //
int xpos, ypos;                // Pixel position

int beginTime     = 0;
int pixelWaitTime = 1000;  // Maximum 1000ms wait for image pixels to arrive
int lastPixelTime = 0;     // Time that "image send" command was sent

int state = 0;  // State machine current state

int   progress_bar = 0;
int   pixel_count  = 0;
float percentage   = 0;

int drawLoopCount = 0;

void setup() {

  size(500, 540);  // Stage size, could handle 480 pixel scrren
  noStroke();      // No border on the next thing drawn

  img = createImage(500, 540, ARGB);
  for (int i = 0; i < img.pixels.length; i++) {
    float a = map(i, 0, img.pixels.length, 255, 0);
    img.pixels[i] = color(0, 153, 204, a);
  }

  tft_img = createImage(tft_width, tft_height, ARGB);
  for (int i = 0; i < tft_img.pixels.length; i++) {
    tft_img.pixels[i] = color(0, 0, 0, 255);
  }

  frameRate(5000); // High frame rate so draw() loops fast

  xpos = 0;
  ypos = 0;

  // Print a list of the available serial ports
  println("-----------------------");
  println("Available Serial Ports:");
  println("-----------------------");
  printArray(Serial.list());
  println("-----------------------");

  print("Port currently used: [");
  print(serial_port);
  println("]");

  String portName = Serial.list()[serial_port];

  delay(1000);
  
  serial = new Serial(this, portName, serial_baud_rate);

  state = 99;
}

void draw() {
  drawLoopCount++;
  switch(state) {

  case 0: // Init varaibles, send start request
    tint(0, 0, 0, 255);
    println("");
    //println("Clearing pipe...");
    beginTime = millis() + 200;
    while ( millis() < beginTime ) 
    {
      serial.read();
    }
    println("Ready to receive image");

    xpos = 0;
    ypos = 0;
    serialCount = 0;
    progress_bar = 0;
    pixel_count = 0;
    percentage   = 0;
    drawLoopCount = 0;
    lastPixelTime = millis() + 1000;
    state = 1;
    break;

  case 1: // Console message, give server some time
    println("Requesting image");
    serial.write("S");
    delay(10); 
    state = 2;
    break;

  case 2: // Get size and set start time for render time report
    // To do: Read image size info, currently hard coded
    beginTime = millis();
    noTint();
    state = 3;
    break;

  case 3: // Request pixels and render returned RGB values

    if ( serial.available() > 0 ) {

      // Add the latest byte from the serial port to array:
      while (serial.available()>0)
      {
        rgb[serialCount++] = serial.read();

        // If we have 3 colour bytes:
        if (serialCount >= 3 ) {
          serialCount = 0;
          pixel_count++;
          stroke(rgb[indexRed], rgb[indexGreen], rgb[indexBlue],1000);
          // We seem to get some pixel transparency so draw twice
          point(xpos + x_offset, ypos + y_offset);
          point(xpos + x_offset, ypos + y_offset);
          lastPixelTime = millis();
          xpos++;
          if (xpos >= tft_width) {
            xpos = 0; 
            print(".");
            progress_bar++;
            if (progress_bar >31)
            {
              progress_bar = 0;
              percentage = 0.5 + 100 * pixel_count/(0.001 + tft_width * tft_height);
              if (percentage > 100) percentage = 100;
              println(" [ " + (int)percentage + "% ]");
            }
            ypos++;
            if (ypos>=tft_height) { 
              ypos = 0;
              println("Image fetch time = " + (millis()-beginTime)/1000.0 + " s");
              state = 5;
            }
          }
        }
      }
    } else
    {
      if (millis() > (lastPixelTime + pixelWaitTime))
      {
        println("");
        System.err.println("No response, trying again...");
        state = 4;
      }
    }
    // Request 32pixels
    serial.write("RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR");
    break;

  case 4: // Time-out, flush serial buffer
    println();
    //println("Clearing serial pipe after a time-out");
    int clearTime = millis() + 50;
    while ( millis() < clearTime ) 
    {
      serial.read();
    }
    state = 6;
    break;

  case 5: // Save the image tot he sketch folder
    println();
    String filename = "tft_screen_" + n  + image_type;
    println("Saving image as \"" + filename);
    if (save_border)
    {
      PImage partialSave = get(x_offset - border, y_offset - border, tft_width + 2*border, tft_height + 2*border);
      partialSave.save(filename);
    } else {
      PImage partialSave = get(x_offset, y_offset, tft_width, tft_height);
      partialSave.save(filename);
    }

    n = n + 1;
    if (n>=max_images) n = 0;
    drawLoopCount = 0; // Reset value ready for counting in step 6
    state = 6;
    break;

  case 6: // Fade the old image if enabled
    int opacity = drawLoopCount;  // So we get increasing fade
    if (drawLoopCount > 50)       // End fade after 50 cycles
    {
      opacity = 255;
      state = 0;
    }
    delay(10);
    if (fade)
    {
      tint(255, opacity);
      image(tft_img, x_offset, y_offset);
    }

    break;

  case 99: // Draw image viewer window
    textAlign(CENTER);
    textSize(20);
    background(bgcolor);
    image(img, 0, 0);

    fill(0);
    text("Bodmer's TFT image viewer", width/2, height-10);

    stroke(0, 0, 0);

    rect(x_offset - border, y_offset - border, tft_width - 1 + 2*border, tft_height - 1 + 2*border);

    fill(255);
    rect(x_offset, y_offset, tft_width-1, tft_height-1);

    state = 0;
    break;

  default:
    println("");
    System.err.println("Error state reached - check sketch!");
    break;
  }
}