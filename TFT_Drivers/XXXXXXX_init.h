// These are various test setups and are not used by the library!

// This is the command sequence that initialises the ????? driver
//
// This setup information uses simple 8 bit SPI writecommand() and writedata() functions


{
#if defined (TINYLCD)
//ILI9486 wave share 3.5 A B is ILI9340?
    writecommand(0x01);
    writedata(0x00);
    delay(50);

    writecommand(0x28);
    writedata(0x00);

	//writecommand(0xB0);		// Power Control 1
	//writedata(0x00);

	//writecommand(0x11);		// Sleep OUT
	//delay(50);
/*
	writecommand(0xC5);		// VCOM Control
	writedata(0x00);
	writedata(0x00);
	writedata(0x00);
	writedata(0x00);
	//writedata(0x48); was 00 48
*/
/*
	writecommand(0xF2);		// ?????
	writedata(0x1C);
	writedata(0xA3);
	writedata(0x32);
	writedata(0x02);
	writedata(0xb2);
	writedata(0x12);
	writedata(0xFF);
	writedata(0x12);
	writedata(0x00);

	writecommand(0xF1);		// ?????
	writedata(0x36);
	writedata(0xA4);

	writecommand(0xF8);		// ?????
	writedata(0x21);
	writedata(0x04);

	writecommand(0xF9);		// ?????
	writedata(0x00);
	writedata(0x08);
*/
	writecommand(0xC0);		// Power Control 1
	writedata(0x0d);
	writedata(0x0d);

	writecommand(0xC1);		// Power Control 2
	writedata(0x43);
	writedata(0x00);

	writecommand(0xC2);		// Power Control 3
	writedata(0x00);        // was 0x44

    writecommand(0xC5);     // VCOM Control
    writedata(0x00);
    writedata(0x48);

	writecommand(0xB6);		// Display Function Control
	writedata(0x00);
	writedata(0x22);		// 0x42 = Rotate display 180 deg.
	writedata(0x3B);

	writecommand(0xE0);		// PGAMCTRL (Positive Gamma Control)
	writedata(0x0f);
	writedata(0x24);
	writedata(0x1c);
	writedata(0x0a);
	writedata(0x0f);
	writedata(0x08);
	writedata(0x43);
	writedata(0x88);
	writedata(0x32);
	writedata(0x0f);
	writedata(0x10);
	writedata(0x06);
	writedata(0x0f);
	writedata(0x07);
	writedata(0x00);

	writecommand(0xE1);		// NGAMCTRL (Negative Gamma Control)
	writedata(0x0F);
	writedata(0x38);
	writedata(0x30);
	writedata(0x09);
	writedata(0x0f);
	writedata(0x0f);
	writedata(0x4e);
	writedata(0x77);
	writedata(0x3c);
	writedata(0x07);
	writedata(0x10);
	writedata(0x05);
	writedata(0x23);
	writedata(0x1b);
	writedata(0x00); 
/*
	writecommand(0xE2);		// NGAMCTRL (Negative Gamma Control)
	writedata(0x0F);
	writedata(0x38);
	writedata(0x30);
	writedata(0x09);
	writedata(0x0f);
	writedata(0x0f);
	writedata(0x4e);
	writedata(0x77);
	writedata(0x3c);
	writedata(0x07);
	writedata(0x10);
	writedata(0x05);
	writedata(0x23);
	writedata(0x1b);
	writedata(0x00); 
*/
	writecommand(0x20);		// Display Inversion OFF, 0x21 = ON

	writecommand(0x3A);		// Interface Pixel Format
	writedata(0x55);

	writecommand(0x36);     // Memory Access Control
	writedata(0x0A);


	writecommand(0x11);		// Sleep OUT
	delay(50);
	
	writecommand(0x29);		// Display ON
    delay(25);

#elif defined ILI9486
// from https://developer.mbed.org/teams/GraphicsDisplay/code/UniGraphic/file/12ba0ecc2c1f/Inits/ILI9486.cpp
/*
    writecommand(0xF1);
    writedata(0x36);
    writedata(0x04);
    writedata(0x00);
    writedata(0x3C);
    writedata(0x0F);
    writedata(0x8F);
 
 
    writecommand(0xF2);
    writedata(0x18);
    writedata(0xA3);
    writedata(0x12);
    writedata(0x02);
    writedata(0xb2);
    writedata(0x12);
    writedata(0xFF);
    writedata(0x10);
    writedata(0x00);
 
    writecommand(0xF8);
    writedata(0x21);
    writedata(0x04);
 
    writecommand(0xF9);
    writedata(0x00);
    writedata(0x08);  
 */
    writecommand(0xC0);
    writedata(0x0f); //13
    writedata(0x0f); //10
 
    writecommand(0xC1);
    writedata(0x42); //43
 
    writecommand(0xC2);
    writedata(0x22);
 
    writecommand(0xC5);
    writedata(0x01); //00
    writedata(0x29); //4D
    writedata(0x80);
 
    writecommand(0xB6);
    writedata(0x00);
    writedata(0x02); //42
    writedata(0x3b);
 
    writecommand(0xB1);
    writedata(0xB0); //C0
    writedata(0x11);
 
    writecommand(0xB4);
    writedata(0x02); //01
 
    writecommand(0xE0);
    writedata(0x0F);
    writedata(0x18);
    writedata(0x15);
    writedata(0x09);
    writedata(0x0B);
    writedata(0x04);
    writedata(0x49);
    writedata(0x64);
    writedata(0x3D);
    writedata(0x08);
    writedata(0x15);
    writedata(0x06);
    writedata(0x12);
    writedata(0x07);
    writedata(0x00);
 
    writecommand(0xE1);
    writedata(0x0F);
    writedata(0x38);
    writedata(0x35);
    writedata(0x0a);
    writedata(0x0c);
    writedata(0x03);
    writedata(0x4A);
    writedata(0x42);
    writedata(0x36);
    writedata(0x04);
    writedata(0x0F);
    writedata(0x03);
    writedata(0x1F);
    writedata(0x1B);
    writedata(0x00);
 
    writecommand(0x20);                     // display inversion OFF
  
    writecommand(0x36);      // MEMORY_ACCESS_CONTROL (orientation stuff)
    writedata(0x48);
     
    writecommand(0x3A);      // COLMOD_PIXEL_FORMAT_SET
    writedata(0x55);     // 16 bit pixel 
 
    writecommand(0x13); // Nomal Displaymode
    
    writecommand(0x11);                     // sleep out
    delay(150);
     
    writecommand(0x29);                     // display on
    delay(150);

#elif defined (ILI9486_2)
// From https://github.com/notro/fbtft/blob/master/fb_ili9486.c

    //writecommand(0x01); // SW reset
    //delay(120);
	
    writecommand(0x11); // Sleep out, also SW reset
    delay(120);

    writecommand(0x3A);
    writedata(0x55);
 
    writecommand(0xC2);
    writedata(0x44);

    writecommand(0xC5);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);

    writecommand(0xE0);
    writedata(0x0F);
    writedata(0x1F);
    writedata(0x1C);
    writedata(0x0C);
    writedata(0x0F);
    writedata(0x08);
    writedata(0x48);
    writedata(0x98);
    writedata(0x37);
    writedata(0x0A);
    writedata(0x13);
    writedata(0x04);
    writedata(0x11);
    writedata(0x0D);
    writedata(0x00);
 
    writecommand(0xE1);
    writedata(0x0F);
    writedata(0x32);
    writedata(0x2E);
    writedata(0x0B);
    writedata(0x0D);
    writedata(0x05);
    writedata(0x47);
    writedata(0x75);
    writedata(0x37);
    writedata(0x06);
    writedata(0x10);
    writedata(0x03);
    writedata(0x24);
    writedata(0x20);
    writedata(0x00);
 
    writecommand(0x20);                     // display inversion OFF
  
    writecommand(0x36);
    writedata(0x0A);
     
    writecommand(0x29);                     // display on
    delay(150);


#elif defined (R61581)
// R61581:
		writecommand(0xB0);		
		writedata(0x1E);	    

		writecommand(0xB0);
		writedata(0x00);

		writecommand(0xB3);
		writedata(0x02);
		writedata(0x00);
		writedata(0x00);
		writedata(0x10);

		writecommand(0xB4);
		writedata(0x00);//0X10

// 		writecommand(0xB9); //PWM Settings for Brightness Control
// 		writedata(0x01);// Disabled by default. 
// 		writedata(0xFF); //0xFF = Max brightness
// 		writedata(0xFF);
// 		writedata(0x18);

		writecommand(0xC0);
		writedata(0x03);
		writedata(0x3B);//
		writedata(0x00);
		writedata(0x00);
		writedata(0x00);
		writedata(0x01);
		writedata(0x00);//NW
		writedata(0x43);

		writecommand(0xC1);
		writedata(0x08);
		writedata(0x15);//CLOCK
		writedata(0x08);
		writedata(0x08);

		writecommand(0xC4);
		writedata(0x15);
		writedata(0x03);
		writedata(0x03);
		writedata(0x01);

		writecommand(0xC6);
		writedata(0x02);

		writecommand(0xC8);
		writedata(0x0c);
		writedata(0x05);
		writedata(0x0A);//0X12
		writedata(0x6B);//0x7D
		writedata(0x04);
		writedata(0x06);//0x08
		writedata(0x15);//0x0A
		writedata(0x10);
		writedata(0x00);
		writedata(0x60);//0x23
 
		writecommand(0x36);
		writedata(0x0A);

		writecommand(0x0C);
		writedata(0x55);

		writecommand(0x3A);
		writedata(0x55);

		writecommand(0x38);
 
		writecommand(0xD0);
		writedata(0x07);
		writedata(0x07);//VCI1
		writedata(0x14);//VRH 0x1D
		writedata(0xA2);//BT 0x06

		writecommand(0xD1);
		writedata(0x03);
		writedata(0x5A);//VCM  0x5A
		writedata(0x10);//VDV

		writecommand(0xD2);
		writedata(0x03);
		writedata(0x04);//0x24
		writedata(0x04);

		writecommand(0x11);
		delay(150);

		writecommand(0x2A);
		writedata(0x00);
		writedata(0x00);
		writedata(0x01);
		writedata(0xDF);//320

		writecommand(0x2B);
		writedata(0x00);
		writedata(0x00);
		writedata(0x01);
		writedata(0x3F);//480

 
		delay(100);

		writecommand(0x29);
		delay(30);

		writecommand(0x2C);
		delay(30);
#elif defined (HX8357B)
    //Serial.println("linux HX8357B"); 
    // seqpower
    writecommand(HX8357B_SETPOWER);
    writedata(0x44);
    writedata(0x41); 
    writedata(0x06);
    // seq_vcom
    writecommand(HX8357B_SETVCOM);
    writedata(0x40); 
    writedata(0x10);
    // seq_power_normal
    writecommand(HX8357B_SETPWRNORMAL);
    writedata(0x05); 
    writedata(0x12);
    // seq_panel_driving
    writecommand(HX8357B_SET_PANEL_DRIVING);
    writedata(0x14); 
    writedata(0x3b);
    writedata(0x00);
    writedata(0x02);
    writedata(0x11);
    // seq_display_frame
    writecommand(HX8357B_SETDISPLAYFRAME);
    writedata(0x0c);  // 6.8mhz
    // seq_panel_related
    writecommand(HX8357B_SETPANELRELATED);
    writedata(0x01);  // BGR
    // seq_undefined1
    writecommand(0xEA);
    writedata(0x03);
    writedata(0x00);
    writedata(0x00); 
    // undef2
    writecommand(0xEB);
    writedata(0x40);
    writedata(0x54);
    writedata(0x26); 
    writedata(0xdb);
    // seq_gamma
    writecommand(HX8357B_SETGAMMA); // 0xC8
    writedata(0x00);
    writedata(0x15);
    writedata(0x00); 
    writedata(0x22);
    writedata(0x00);
    writedata(0x08);
    writedata(0x77); 
    writedata(0x26);
    writedata(0x66);
    writedata(0x22);
    writedata(0x04); 
    writedata(0x00);

    // seq_addr mode
    writecommand(HX8357_MADCTL);
    writedata(0xC0);
    // pixel format
    writecommand(HX8357_COLMOD);
    writedata(0x55);
    
    // set up whole address box
    // paddr
    writecommand(HX8357_PASET);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01); 
    writedata(0xDF);
    // caddr
    writecommand(HX8357_CASET);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01); 
    writedata(0x3F);

    // display mode
    writecommand(HX8357B_SETDISPMODE);
    writedata(0x00); // CPU (DBI) and internal oscillation ??
    // exit sleep
    writecommand(HX8357_SLPOUT);

    delay(120);
    // main screen turn on
    writecommand(HX8357_DISPON);
    delay(10);

#elif defined (HX8257D)
    writecommand(HX8357_SWRESET);
    delay(10);

    // setextc
    writecommand(HX8357D_SETC);
    writedata(0xFF);
    writedata(0x83);
    writedata(0x57);
    delay(300);

    // setRGB which also enables SDO
    writecommand(HX8357_SETRGB); 
    writedata(0x80);  //enable SDO pin!
//    writedata(0x00);  //disable SDO pin!
    writedata(0x0);
    writedata(0x06);
    writedata(0x06);

    writecommand(HX8357D_SETCOM);
    writedata(0x25);  // -1.52V
    
    writecommand(HX8357_SETOSC);
    writedata(0x68);  // Normal mode 70Hz, Idle mode 55 Hz
    
    writecommand(HX8357_SETPANEL); //Set Panel
    writedata(0x05);  // BGR, Gate direction swapped
    
    writecommand(HX8357_SETPWR1);
    writedata(0x00);  // Not deep standby
    writedata(0x15);  //BT
    writedata(0x1C);  //VSPR
    writedata(0x1C);  //VSNR
    writedata(0x83);  //AP
    writedata(0xAA);  //FS

    writecommand(HX8357D_SETSTBA);  
    writedata(0x50);  //OPON normal
    writedata(0x50);  //OPON idle
    writedata(0x01);  //STBA
    writedata(0x3C);  //STBA
    writedata(0x1E);  //STBA
    writedata(0x08);  //GEN
    
    writecommand(HX8357D_SETCYC);  
    writedata(0x02);  //NW 0x02
    writedata(0x40);  //RTN
    writedata(0x00);  //DIV
    writedata(0x2A);  //DUM
    writedata(0x2A);  //DUM
    writedata(0x0D);  //GDON
    writedata(0x78);  //GDOFF

    writecommand(HX8357D_SETGAMMA); 
    writedata(0x02);
    writedata(0x0A);
    writedata(0x11);
    writedata(0x1d);
    writedata(0x23);
    writedata(0x35);
    writedata(0x41);
    writedata(0x4b);
    writedata(0x4b);
    writedata(0x42);
    writedata(0x3A);
    writedata(0x27);
    writedata(0x1B);
    writedata(0x08);
    writedata(0x09);
    writedata(0x03);
    writedata(0x02);
    writedata(0x0A);
    writedata(0x11);
    writedata(0x1d);
    writedata(0x23);
    writedata(0x35);
    writedata(0x41);
    writedata(0x4b);
    writedata(0x4b);
    writedata(0x42);
    writedata(0x3A);
    writedata(0x27);
    writedata(0x1B);
    writedata(0x08);
    writedata(0x09);
    writedata(0x03);
    writedata(0x00);
    writedata(0x01);
    
    writecommand(HX8357_COLMOD);
    writedata(0x55);  // 16 bit
    
    writecommand(HX8357_MADCTL);  
    writedata(0xC0); 
    
    writecommand(HX8357_TEON);  // TE off
    writedata(0x00); 
    
    writecommand(HX8357_TEARLINE);  // tear line
    writedata(0x00); 
    writedata(0x02);

    writecommand(HX8357_SLPOUT); //Exit Sleep
    delay(150);
    writecommand(HX8357_DISPON);  // display on
    delay(50);

#elif defined(WAVESHARE32B)

// Waveshare32b
    writecommand(0xCB);
    writedata(0x39);
    writedata(0x2C);
    writedata(0x00);
    writedata(0x34);
    writedata(0x02);

    writecommand(0xCF);
    writedata(0x00);
    writedata(0xC1);
    writedata(0x30);

    writecommand(0xE8);
    writedata(0x85);
    writedata(0x00);
    writedata(0x78);

    writecommand(0xEA);
    writedata(0x00);
    writedata(0x00);

    writecommand(0xED);
    writedata(0x64);
    writedata(0x03);
    writedata(0x12);
    writedata(0x81);	

    writecommand(0xF7);
    writedata(0x20);

    writecommand(0xC0);
    writedata(0x23);

    writecommand(0xC1);
    writedata(0x10);

    writecommand(0xC5);
    writedata(0x3E);
    writedata(0x28);
	
    writecommand(0xC7);
    writedata(0x86);

    writecommand(0x36);
    writedata(0x28);

    writecommand(0x3A);
    writedata(0x55);

    writecommand(0xB1);
    writedata(0x00);
    writedata(0x18);

    writecommand(0xB6);
    writedata(0x08);
    writedata(0x82);
    writedata(0x27);

    writecommand(0xF2);
    writedata(0x00);

    writecommand(0x26);
    writedata(0x01);

    writecommand(0xE0);
    writedata(0x0F);
    writedata(0x31);
    writedata(0x2B);
    writedata(0x0C);
    writedata(0x0E);
    writedata(0x08);
    writedata(0x4E);
    writedata(0xF1);
    writedata(0x37);
    writedata(0x07);
    writedata(0x10);
    writedata(0x03);
    writedata(0x0E);
    writedata(0x09);
    writedata(0x00);

    writecommand(0xE1);
    writedata(0x00);
    writedata(0x0E);
    writedata(0x14);
    writedata(0x03);
    writedata(0x11);
    writedata(0x07);
    writedata(0x31);
    writedata(0xC1);
    writedata(0x48);
    writedata(0x08);
    writedata(0x0F);
    writedata(0x0C);
    writedata(0x31);
    writedata(0x36);
    writedata(0x0F);

    writecommand(0x11);

    delay(120);
    writecommand(0x29);
    //writecommand(0x2C);
    delay(25);

#elif defined (TINYLCD2)

// Configure TINYLCD display
    writecommand(0x11);
    delay(20);

    writecommand(0xB0);
    writedata(0x80);

    writecommand(0xC0);
    writedata(0x0A);
    writedata(0x0A);

    writecommand(0xC1);
    writedata(0x45);
    writedata(0x07);

    writecommand(0xC2);
    writedata(0x33);

    writecommand(0xC5);
    writedata(0x00);
    writedata(0x42);
    writedata(0x80);

    writecommand(0xB1);
    writedata(0xD0);
    writedata(0x11);

	writecommand(0xB4);
    writedata(0x02);

    writecommand(0xB6);
    writedata(0x00);
    writedata(0x22);
    writedata(0x3B);

	writecommand(0xB7);
    writedata(0x07);

	writecommand(0x36);
    writedata(0x58);

	writecommand(0xF0);
    writedata(0x36);
    writedata(0xA5);
    writedata(0xD3);
	
    writecommand(0xE5);
    writedata(0x80);

	writecommand(0xE5);
    writedata(0x01);

    writecommand(0xB3);
    writedata(0x00);

    writecommand(0xE5);
    writedata(0x00);

	writecommand(0xF0);
    writedata(0x36);
    writedata(0xA5);
    writedata(0x53);

    writecommand(0xE0);
    writedata(0x00);
    writedata(0x35);
    writedata(0x33);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x35);
    writedata(0x33);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);


    writecommand(0x3A);
    writedata(0x55);

    delay(120);
    writecommand(0x29);

    delay(25);
// End of TINYLCD display configuration

#elif defined (HX8357C)

// HX8357-C display initialisation

	writecommand(0xB9); // Enable extension command
	writedata(0xFF);
	writedata(0x83);
	writedata(0x57);
	delay(50);
	
	writecommand(0xB6); //Set VCOM voltage
	writedata(0x2C);    //0x52 for HSD 3.0"
	
	writecommand(0x11); // Sleep off
	delay(200);
	
	writecommand(0x35); // Tearing effect on
	writedata(0x00);    // Added parameter

	writecommand(0x3A); // Interface pixel format
	writedata(0x55);    // 16 bits per pixel

	//writecommand(0xCC); // Set panel characteristic
	//writedata(0x09);    // S960>S1, G1>G480, R-G-B, normally black

	//writecommand(0xB3); // RGB interface
	//writedata(0x43);
	//writedata(0x00);
	//writedata(0x06);
	//writedata(0x06);

	writecommand(0xB1); // Power control
	writedata(0x00);
	writedata(0x15);
	writedata(0x0D);
	writedata(0x0D);
	writedata(0x83);
	writedata(0x48);
	
	
	writecommand(0xC0); // Does this do anything?
	writedata(0x24);
	writedata(0x24);
	writedata(0x01);
	writedata(0x3C);
	writedata(0xC8);
	writedata(0x08);
	
	writecommand(0xB4); // Display cycle
	writedata(0x02);
	writedata(0x40);
	writedata(0x00);
	writedata(0x2A);
	writedata(0x2A);
	writedata(0x0D);
	writedata(0x4F);
	
	writecommand(0xE0); // Gamma curve
	writedata(0x00);
	writedata(0x15);
	writedata(0x1D);
	writedata(0x2A);
	writedata(0x31);
	writedata(0x42);
	writedata(0x4C);
	writedata(0x53);
	writedata(0x45);
	writedata(0x40);
	writedata(0x3B);
	writedata(0x32);
	writedata(0x2E);
	writedata(0x28);
	
	writedata(0x24);
	writedata(0x03);
	writedata(0x00);
	writedata(0x15);
	writedata(0x1D);
	writedata(0x2A);
	writedata(0x31);
	writedata(0x42);
	writedata(0x4C);
	writedata(0x53);
	writedata(0x45);
	writedata(0x40);
	writedata(0x3B);
	writedata(0x32);
	
	writedata(0x2E);
	writedata(0x28);
	writedata(0x24);
	writedata(0x03);
	writedata(0x00);
	writedata(0x01);

	writecommand(0x36); // MADCTL Memory access control
	writedata(0x48);
	delay(20);

	writecommand(0x21); //Display inversion on
	delay(20);

	writecommand(0x29); // Display on
	
	delay(120);

#elif defined (ILI9481) // Must be an ILI9481

// Configure ILI9481 display

    writecommand(0x11);
    delay(20);
    writecommand(0xD0);
    writedata(0x07);
    writedata(0x42);
    writedata(0x18);

    writecommand(0xD1);
    writedata(0x00);
    writedata(0x07);
    writedata(0x10);

    writecommand(0xD2);
    writedata(0x01);
    writedata(0x02);

    writecommand(0xC0);
    writedata(0x10);
    writedata(0x3B);
    writedata(0x00);
    writedata(0x02);
    writedata(0x11);

    writecommand(0xC5);
    writedata(0x03);

    writecommand(0xC8);
    writedata(0x00);
    writedata(0x32);
    writedata(0x36);
    writedata(0x45);
    writedata(0x06);
    writedata(0x16);
    writedata(0x37);
    writedata(0x75);
    writedata(0x77);
    writedata(0x54);
    writedata(0x0C);
    writedata(0x00);

    writecommand(0x36);
    writedata(0x0A);

    writecommand(0x3A);
    writedata(0x55);

    writecommand(0x2A);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01);
    writedata(0x3F);

    writecommand(0x2B);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01);
    writedata(0xDF);

    delay(120);
    writecommand(0x29);

    delay(25);
// End of ILI9481 display configuration
#endif

}