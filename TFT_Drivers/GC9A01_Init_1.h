
// This is the command sequence that initialises the GC9A01 driver

{
  
  writecommand(0xEF);	
  writecommand(0xEB);
  writedata(0x14);
  		
  writecommand(0xFE);
  writecommand(0xEF);

  writecommand(0xEB);		
	writedata(0x14);

  writecommand(0x84);		
	writedata(0x40);

  writecommand(0x85);		
	writedata(0xFF);

  writecommand(0x86);		
	writedata(0xFF);

  writecommand(0x87);		
	writedata(0xFF);

  writecommand(0x88);		
	writedata(0x0A);

  writecommand(0x89);		
	writedata(0x21);

  writecommand(0x8A);		
	writedata(0x00);

  writecommand(0x8B);		
	writedata(0x80);

  writecommand(0x8C);		
	writedata(0x01);

  writecommand(0x8D);		
	writedata(0x01);

  writecommand(0x8E);		
	writedata(0xFF);

  writecommand(0x8F);		
	writedata(0xFF);

  writecommand(0xB6);		
	writedata(0x00);
  writedata(0x20);
  
  writecommand(0x3A);		
	writedata(0x05);

  writecommand(0x90);
	writedata(0x08);
	writedata(0x08);
	writedata(0x08);
	writedata(0x08);

  writecommand(0xBD);		
	writedata(0x06);

  writecommand(0xBC);		
	writedata(0x00);

  writecommand(0xFF);		
	writedata(0x21);
  writedata(0x0C);
  writedata(0x02);

  writecommand(0xF0);
	writedata(0x45);
	writedata(0x09);
	writedata(0x08);
	writedata(0x08);
  writedata(0x26);
	writedata(0x2A);

  writecommand(0xF1); 
	writedata(0X43);
	writedata(0X70);
	writedata(0X72);
	writedata(0X36);
	writedata(0X37);
	writedata(0X6F);

  writecommand(0xF2); 
	writedata(0X45);
	writedata(0X09);
	writedata(0X08);
	writedata(0X08);
	writedata(0X26);
	writedata(0X2A);

  writecommand(0xF3); 
	writedata(0X43);
	writedata(0X70);
	writedata(0X72);
	writedata(0X36);
	writedata(0X37);
	writedata(0X6F);

writecommand(0xED); 
	writedata(0X18);
	writedata(0X0B);


writecommand(0xAE); 
	writedata(0X77);

  writecommand(0xCD); 
	writedata(0X63);


    writecommand(0x70); 
	writedata(0X07);
	writedata(0X07);
	writedata(0X04);
	writedata(0X0E);
	writedata(0X0F);
	writedata(0X09);
  	writedata(0X07);
	writedata(0X08);
	writedata(0X03);


    writecommand(0xEB); 
	writedata(0X34);

      writecommand(0x62); 
	writedata(0X18);
	writedata(0X0D);
	writedata(0X71);
	writedata(0XED);
	writedata(0X70);
	writedata(0X70);
  	writedata(0X18);
	writedata(0X0F);
	writedata(0X71);
    	writedata(0XEF);
	writedata(0X70);
	writedata(0X70);

        writecommand(0x63); 
	writedata(0X18);
	writedata(0X11);
	writedata(0X71);
	writedata(0XF1);
	writedata(0X70);
	writedata(0X70);
  	writedata(0X18);
	writedata(0X13);
	writedata(0X71);
    	writedata(0XF3);
	writedata(0X70);
	writedata(0X70);


          writecommand(0x64); 
	writedata(0X28);
	writedata(0X29);
	writedata(0XF1);
	writedata(0X01);
	writedata(0XF1);
	writedata(0X00);
  	writedata(0X07);

              writecommand(0x66); 
	writedata(0X3C);
	writedata(0X00);
	writedata(0XCD);
	writedata(0X67);
	writedata(0X45);
	writedata(0X45);
  	writedata(0X10);
    writedata(0X00);
    writedata(0X00);
    writedata(0X00);

    writecommand(0x67); 
	writedata(0X00);
	writedata(0X3C);
	writedata(0X00);
	writedata(0X00);
	writedata(0X00);
	writedata(0X01);
  	writedata(0X54);
    writedata(0X10);
    writedata(0X32);
    writedata(0X98);

    writecommand(0x74); 
	writedata(0X10);
	writedata(0X85);
	writedata(0X80);
	writedata(0X00);
	writedata(0X00);
	writedata(0X4E);
  	writedata(0X00);

    writecommand(0x98); 
	writedata(0X3E);
	writedata(0X07);

writecommand(0x35); 
writecommand(0x21); 

writecommand(0x11); 
delay(120);
	writecommand(0x29);
  delay(120);
            
///////
// 	writecommand(0xB0);		
// 	writedata(0xC0); 
// 	writecommand(0xB2);			
// 	writedata(0x2F); 
// 	writecommand(0xB3);		
// 	writedata(0x03);
// 	writecommand(0xB6);		
// 	writedata(0x19); 
// 	writecommand(0xB7);		
// 	writedata(0x01);  
	
// 	writecommand(0xAC);
// 	writedata(0xCB);
// 	writecommand(0xAB); 
// 	writedata(0x0e);
		
// 	writecommand(0xB4);	
// 	writedata(0x04);
	
// 	writecommand(0xA8);
// 	writedata(0x19);

// 	writecommand(0x3A);		
// 	writedata(0x05); 

// 	writecommand(0x36);
// 	writedata(0xC8);// if(USE_HORIZONTAL==0)writedata(0xC8);
// 	// else if(USE_HORIZONTAL==1)writedata(0x08);
// 	// else if(USE_HORIZONTAL==2)writedata(0x68);
// 	// else writedata(0xA8); 

// 	writecommand(0xb8);
// 	writedata(0x08);
 
// 	writecommand(0xE8);
// 	writedata(0x24);

// 	writecommand(0xE9);
// 	writedata(0x48);

// 	writecommand(0xea);	
// 	writedata(0x22);

 			
// 	writecommand(0xC6);
// 	writedata(0x30);
// 	writecommand(0xC7);
// 	writedata(0x18);

// 	writecommand(0xF0);
// 	writedata(0x1F);
// 	writedata(0x28);
// 	writedata(0x04);
// 	writedata(0x3E);
// 	writedata(0x2A);
// 	writedata(0x2E);
// 	writedata(0x20);
// 	writedata(0x00);
// 	writedata(0x0C);
// 	writedata(0x06);
// 	writedata(0x00);
// 	writedata(0x1C);
// 	writedata(0x1F);
// 	writedata(0x0f);

// 	writecommand(0xF1); 
// 	writedata(0X00);
// 	writedata(0X2D);
// 	writedata(0X2F);
// 	writedata(0X3C);
// 	writedata(0X6F);
// 	writedata(0X1C);
// 	writedata(0X0B);
// 	writedata(0X00);
// 	writedata(0X00);
// 	writedata(0X00);
// 	writedata(0X07);
// 	writedata(0X0D);
// 	writedata(0X11);
// 	writedata(0X0f);

// 	writecommand(0x21);
//   delay(120);
// 	writecommand(0x11);
//   delay(120);
// 	writecommand(0x29);
//   delay(120);

}
