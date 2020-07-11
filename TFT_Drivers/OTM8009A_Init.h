// This is the command sequence that initialises the OTM8009A driver
// Configure OTM8009A display

//3.97inch OTM8009 Init 20190116
	/* Enter CMD2 */
	writecommand16(MCS_CMD2_ENA1);
	writedata16(0x80);
	writecommand16(MCS_CMD2_ENA1 + 1);
	writedata16(0x09);
	writecommand16(MCS_CMD2_ENA1 + 2);
	writedata16(0x01);
	
	/* Enter Orise Command2 */
	writecommand16(MCS_CMD2_ENA2);
	writedata16(0x80);
	writecommand16(MCS_CMD2_ENA2 + 1);
	writedata16(0x09);

	/* Command not documented */
	writecommand16(0xff03);
	writedata16(0x01); //??

	//add ==========20131216============================//
	/* Command not documented */
	writecommand16(0xf5b6); 
	writedata16(0x06); //??

	/* Source Driver Precharge Control */
	writecommand16(MCS_SD_PCH_CTRL); 
	writedata16(0x30); 
	
	/* Command not documented: 0xC48A */
	writecommand16(MCS_NO_DOC1); 
	writedata16(0x40); 
	//===================================================//
	
	/* Source Driver Timing Setting */
	writecommand16(MCS_SD_CTRL + 1);
	writedata16(0x1B);

	/* Command not documented */
	writecommand16(0xc0ba); //--> (0xc0b4); // column inversion //  2013.12.16 modify
	writedata16(0x50); 

	/* Oscillator Adjustment for Idle/Normal mode */
	writecommand16(MCS_OSC_ADJ);
	writedata16(0x66); /* 65Hz */

	/* RGB Video Mode Setting */
	writecommand16(MCS_RGB_VID_SET);
	writedata16(0x0E);

	/* Source Driver Precharge Control */
	writecommand16(MCS_SD_PCH_CTRL + 1);
	writedata16(0x83);

	/* Power Control Setting 1 */
	writecommand16(MCS_PWR_CTRL1 + 2);
	writedata16(0x83);

	/* Power Control Setting 2 for Normal Mode */
	writecommand16(MCS_PWR_CTRL2);
	writedata16(0x96);

	writecommand16(MCS_PWR_CTRL2 + 1);
	writedata16(0x2B);

	writecommand16(MCS_PWR_CTRL2 + 2);
	writedata16(0x01);

	writecommand16(MCS_PWR_CTRL2 + 4);
	writedata16(0x33);

	writecommand16(MCS_PWR_CTRL2 + 5);
	writedata16(0x34);

	/* Power Control Setting 4 for DC Voltage */
	writecommand16(MCS_PWR_CTRL4 + 1);
	writedata16(0xa9);

	/* GOA VST Setting */
	writecommand16(MCS_GOAVST);
	writedata16(0x86);
	writecommand16(MCS_GOAVST + 1);
	writedata16(0x01); 
	writecommand16(MCS_GOAVST + 2);
	writedata16(0x00); 
	writecommand16(MCS_GOAVST + 3);
	writedata16(0x85); 
	writecommand16(MCS_GOAVST + 4);
	writedata16(0x01); 
	writecommand16(MCS_GOAVST + 5);
	writedata16(0x00);
	writecommand16(MCS_GOAVST + 6);
	writedata16(0x00);
	writecommand16(MCS_GOAVST + 7);
	writedata16(0x00);
	writecommand16(MCS_GOAVST + 8);
	writedata16(0x00);
	writecommand16(MCS_GOAVST + 9);
	writedata16(0x00);
	writecommand16(MCS_GOAVST + 10);
	writedata16(0x00);
	writecommand16(MCS_GOAVST + 11);
	writedata16(0x00);

	/* GOA CLKA1 Setting */
	writecommand16(MCS_GOACLKA1);// cea1[7:0] : clka1_width[3:0], clka1_shift[11:8]                         
	writedata16(0x18); 
	writecommand16(MCS_GOACLKA1 + 1);// cea2[7:0] : clka1_shift[7:0]                                            
	writedata16(0x04); 
	writecommand16(MCS_GOACLKA1 + 2);// cea3[7:0] : clka1_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]   
	writedata16(0x03); 
	writecommand16(MCS_GOACLKA1 + 3);// cea4[7:0] : clka1_switch[7:0]                                               
	writedata16(0x21); 
	writecommand16(MCS_GOACLKA1 + 4);// cea5[7:0] : clka1_extend[7:0]                                           
	writedata16(0x00); 
	writecommand16(MCS_GOACLKA1 + 5);// cea6[7:0] : clka1_tchop[7:0]                                            
	writedata16(0x00); 
	writecommand16(MCS_GOACLKA1 + 6);// cea7[7:0] : clka1_tglue[7:0]                                            
	writedata16(0x00); 

	/* GOA CLKA2 Setting */
	writecommand16(MCS_GOACLKA2);// cea8[7:0] : clka2_width[3:0], clka2_shift[11:8]                         
	writedata16(0x18); 
	writecommand16(MCS_GOACLKA2 + 1);// cea9[7:0] : clka2_shift[7:0]                                            
	writedata16(0x03);
	writecommand16(MCS_GOACLKA2 + 2);// ceaa[7:0] : clka2_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]   
	writedata16(0x03); 
	writecommand16(MCS_GOACLKA2 + 3);// ceab[7:0] : clka2_switch[7:0]                                                
	writedata16(0x22);
	writecommand16(MCS_GOACLKA2 + 4);// ceac[7:0] : clka2_extend                                                
	writedata16(0x00); 
	writecommand16(MCS_GOACLKA2 + 5);// cead[7:0] : clka2_tchop                                                 
	writedata16(0x00); 
	writecommand16(MCS_GOACLKA2 + 6);// ceae[7:0] : clka2_tglue 
	writedata16(0x00);    

	/* GOA CLKA3 Setting */
	writecommand16(MCS_GOACLKA3);// ceb1[7:0] : clka3_width[3:0], clka3_shift[11:8]                          
	writedata16(0x18);
	writecommand16(MCS_GOACLKA3 + 1);// ceb2[7:0] : clka3_shift[7:0]                                             
	writedata16(0x02); 
	writecommand16(MCS_GOACLKA3 + 2);// ceb3[7:0] : clka3_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]    
	writedata16(0x03); 
	writecommand16(MCS_GOACLKA3 + 3);// ceb4[7:0] : clka3_switch[7:0]                                               
	writedata16(0x23); 
	writecommand16(MCS_GOACLKA3 + 4);// ceb5[7:0] : clka3_extend[7:0]                                            
	writedata16(0x00); 
	writecommand16(MCS_GOACLKA3 + 5);// ceb6[7:0] : clka3_tchop[7:0]                                             
	writedata16(0x00); 
	writecommand16(MCS_GOACLKA3 + 6);// ceb7[7:0] : clka3_tglue[7:0]                                             
	writedata16(0x00); 
	
	/* GOA CLKA4 Setting */
	writecommand16(MCS_GOACLKA4);// ceb8[7:0] : clka4_width[3:0], clka2_shift[11:8]                          
	writedata16(0x18);
	writecommand16(MCS_GOACLKA4 + 1);// ceb9[7:0] : clka4_shift[7:0]                                             
	writedata16(0x01); 
	writecommand16(MCS_GOACLKA4 + 2);// ceba[7:0] : clka4_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]    
	writedata16(0x03); 
	writecommand16(MCS_GOACLKA4 + 3);// cebb[7:0] : clka4_switch[7:0]                                                
	writedata16(0x24); 
	writecommand16(MCS_GOACLKA4 + 4);// cebc[7:0] : clka4_extend                                                 
	writedata16(0x00); 
	writecommand16(MCS_GOACLKA4 + 5);// cebd[7:0] : clka4_tchop                                                  
	writedata16(0x00); 
	writecommand16(MCS_GOACLKA4 + 6);// cebe[7:0] : clka4_tglue                                                  
	writedata16(0x00); 

	/* GOA ECLK Setting */
	writecommand16(MCS_GOAECLK);// cfc1[7:0] : eclk_normal_width[7:0]   
	writedata16(0x01); 
	writecommand16(MCS_GOAECLK + 1);// cfc2[7:0] : eclk_partial_width[7:0]                                                                                  
	writedata16(0x01); 
	writecommand16(MCS_GOAECLK + 2);// cfc3[7:0] : all_normal_tchop[7:0]                                                                                    
	writedata16(0x20); 
	writecommand16(MCS_GOAECLK + 3);// cfc4[7:0] : all_partial_tchop[7:0]                                                                                   
	writedata16(0x20); 
	writecommand16(MCS_GOAECLK + 4);// cfc5[7:0] : eclk1_follow[3:0], eclk2_follow[3:0]                                                                     
	writedata16(0x00); 
	writecommand16(MCS_GOAECLK + 5);// cfc6[7:0] : eclk3_follow[3:0], eclk4_follow[3:0]                                                                     
	writedata16(0x00); 
	
	/* GOA Other Options 1 */
	writecommand16(MCS_GOAOPT1);// cfc7[7:0] : 00, vstmask, vendmask, 00, dir1, dir2 (0=VGL, 1=VGH)                                                     
	writedata16(0x01); 

	/* GOA Signal Toggle Option Setting */
	writecommand16(MCS_GOATGOPT);// cfc8[7:0] : reg_goa_gnd_opt, reg_goa_dpgm_tail_set, reg_goa_f_gating_en, reg_goa_f_odd_gating, toggle_mod1, 2, 3, 4  
	writedata16(0x00);    // GND OPT1 (00-->80  2011/10/28)	
	writecommand16(MCS_GOATGOPT + 1);// cfc9[7:0] : duty_block[3:0], DGPM[3:0]                                                                               
	writedata16(0x00); 
	writecommand16(MCS_GOATGOPT + 2);// cfca[7:0] : reg_goa_gnd_period[7:0]                                                                                  
	writedata16(0x00);    // Gate PCH (CLK base) (00-->0a  2011/10/28)

	/* Command not documented: 0xCFD0 */
	writecommand16(MCS_NO_DOC2);// cfd1[7:0] : 0000000, reg_goa_frame_odd_high
	writedata16(0x00); 

	/* Panel Control Setting 5 */
	writecommand16(MCS_PANCTRLSET5);//cbc1[7:0] : enmode H-byte of sig1  (pwrof_0, pwrof_1, norm, pwron_4 )           
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET5 + 1);//cbc2[7:0] : enmode H-byte of sig2  (pwrof_0, pwrof_1, norm, pwron_4 )          
	writedata16(0x04); 
	writecommand16(MCS_PANCTRLSET5 + 2);//cbc3[7:0] : enmode H-byte of sig3  (pwrof_0, pwrof_1, norm, pwron_4 )           
	writedata16(0x04); 
	writecommand16(MCS_PANCTRLSET5 + 3);//cbc4[7:0] : enmode H-byte of sig4  (pwrof_0, pwrof_1, norm, pwron_4 )        
	writedata16(0x04); 
	writecommand16(MCS_PANCTRLSET5 + 4);//cbc5[7:0] : enmode H-byte of sig5  (pwrof_0, pwrof_1, norm, pwron_4 )             
	writedata16(0x04); 
	writecommand16(MCS_PANCTRLSET5 + 5);//cbc6[7:0] : enmode H-byte of sig6  (pwrof_0, pwrof_1, norm, pwron_4 )           
	writedata16(0x04); 
	writecommand16(MCS_PANCTRLSET5 + 6);//cbc7[7:0] : enmode H-byte of sig7  (pwrof_0, pwrof_1, norm, pwron_4 )           
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET5 + 7);//cbc8[7:0] : enmode H-byte of sig8  (pwrof_0, pwrof_1, norm, pwron_4 )           
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET5 + 8);//cbc9[7:0] : enmode H-byte of sig9  (pwrof_0, pwrof_1, norm, pwron_4 )           
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET5 + 9);//cbca[7:0] : enmode H-byte of sig10 (pwrof_0, pwrof_1, norm, pwron_4 )        
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET5 + 10);//cbcb[7:0] : enmode H-byte of sig11 (pwrof_0, pwrof_1, norm, pwron_4 )        
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET5 + 11);//cbcc[7:0] : enmode H-byte of sig12 (pwrof_0, pwrof_1, norm, pwron_4 )        
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET5 + 12);//cbcd[7:0] : enmode H-byte of sig13 (pwrof_0, pwrof_1, norm, pwron_4 )        
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET5 + 13);//cbce[7:0] : enmode H-byte of sig14 (pwrof_0, pwrof_1, norm, pwron_4 ) 
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET5 + 14);//cbcf[7:0] : enmode H-byte of sig15 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 

	/* Panel Control Setting 6 */
	writecommand16(MCS_PANCTRLSET6);//cbd1[7:0] : enmode H-byte of sig16 (pwrof_0, pwrof_1, norm, pwron_4 )           
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET6 + 1);//cbd2[7:0] : enmode H-byte of sig17 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET6 + 2);//cbd3[7:0] : enmode H-byte of sig18 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET6 + 3);//cbd4[7:0] : enmode H-byte of sig19 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET6 + 4);//cbd5[7:0] : enmode H-byte of sig20 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET6 + 5);//cbd6[7:0] : enmode H-byte of sig21 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET6 + 6);//cbd7[7:0] : enmode H-byte of sig22 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x04); 
	writecommand16(MCS_PANCTRLSET6 + 7);//cbd8[7:0] : enmode H-byte of sig23 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x04); 
	writecommand16(MCS_PANCTRLSET6 + 8);//cbd9[7:0] : enmode H-byte of sig24 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x04); 
	writecommand16(MCS_PANCTRLSET6 + 9);//cbda[7:0] : enmode H-byte of sig25 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x04); 
	writecommand16(MCS_PANCTRLSET6 + 10);//cbdb[7:0] : enmode H-byte of sig26 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x04); 
	writecommand16(MCS_PANCTRLSET6 + 11);//cbdc[7:0] : enmode H-byte of sig27 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET6 + 12);//cbdd[7:0] : enmode H-byte of sig28 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET6 + 13);//cbde[7:0] : enmode H-byte of sig29 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET6 + 14);//cbdf[7:0] : enmode H-byte of sig30 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 

	/* Panel Control Setting 7 */
	writecommand16(MCS_PANCTRLSET7);//cbe1[7:0] : enmode H-byte of sig31 (pwrof_0, pwrof_1, norm, pwron_4 )             
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET7 + 1);//cbe2[7:0] : enmode H-byte of sig32 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET7 + 2);//cbe3[7:0] : enmode H-byte of sig33 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET7 + 3);//cbe4[7:0] : enmode H-byte of sig34 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET7 + 4);//cbe5[7:0] : enmode H-byte of sig35 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET7 + 5);//cbe6[7:0] : enmode H-byte of sig36 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET7 + 6);//cbe7[7:0] : enmode H-byte of sig37 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET7 + 7);//cbe8[7:0] : enmode H-byte of sig38 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET7 + 8);//cbe9[7:0] : enmode H-byte of sig39 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00); 
	writecommand16(MCS_PANCTRLSET7 + 9);//cbea[7:0] : enmode H-byte of sig40 (pwrof_0, pwrof_1, norm, pwron_4 )
	writedata16(0x00);
	
	/* Panel U2D Setting 1 */  
	// cc8x 
	writecommand16(MCS_PANU2D1);//cc81[7:0] : reg setting for signal01 selection with u2d mode   
	writedata16(0x00); 
	writecommand16(MCS_PANU2D1 + 1);//cc82[7:0] : reg setting for signal02 selection with u2d mode 
	writedata16(0x26); 
	writecommand16(MCS_PANU2D1 + 2);//cc83[7:0] : reg setting for signal03 selection with u2d mode 
	writedata16(0x09); 
	writecommand16(MCS_PANU2D1 + 3);//cc84[7:0] : reg setting for signal04 selection with u2d mode 
	writedata16(0x0B); 
	writecommand16(MCS_PANU2D1 + 4);//cc85[7:0] : reg setting for signal05 selection with u2d mode 
	writedata16(0x01); 
	writecommand16(MCS_PANU2D1 + 5);//cc86[7:0] : reg setting for signal06 selection with u2d mode 
	writedata16(0x25); 
	writecommand16(MCS_PANU2D1 + 6);//cc87[7:0] : reg setting for signal07 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D1 + 7);//cc88[7:0] : reg setting for signal08 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D1 + 8);//cc89[7:0] : reg setting for signal09 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D1 + 9);//cc8a[7:0] : reg setting for signal10 selection with u2d mode 
	writedata16(0x00);  

	/* Panel U2D Setting 2 */
	// cc9x   
	writecommand16(MCS_PANU2D2);//cc91[7:0] : reg setting for signal11 selection with u2d mode   
	writedata16(0x00); 
	writecommand16(MCS_PANU2D2 + 1);//cc92[7:0] : reg setting for signal12 selection with u2d mode
	writedata16(0x00); 
	writecommand16(MCS_PANU2D2 + 2);//cc93[7:0] : reg setting for signal13 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D2 + 3);//cc94[7:0] : reg setting for signal14 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D2 + 4);//cc95[7:0] : reg setting for signal15 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D2 + 5);//cc96[7:0] : reg setting for signal16 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D2 + 6);//cc97[7:0] : reg setting for signal17 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D2 + 7);//cc98[7:0] : reg setting for signal18 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D2 + 8);//cc99[7:0] : reg setting for signal19 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D2 + 9);//cc9a[7:0] : reg setting for signal20 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D2 + 10);//cc9b[7:0] : reg setting for signal21 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D2 + 11);//cc9c[7:0] : reg setting for signal22 selection with u2d mode 
	writedata16(0x26); 
	writecommand16(MCS_PANU2D2 + 12);//cc9d[7:0] : reg setting for signal23 selection with u2d mode 
	writedata16(0x0A); 
	writecommand16(MCS_PANU2D2 + 13);//cc9e[7:0] : reg setting for signal24 selection with u2d mode 
	writedata16(0x0C); 
	writecommand16(MCS_PANU2D2 + 14);//cc9f[7:0] : reg setting for signal25 selection with u2d mode 
	writedata16(0x02);  		
	
	/* Panel U2D Setting 3 */
	// ccax   
	writecommand16(MCS_PANU2D3);//cca1[7:0] : reg setting for signal26 selection with u2d mode   
	writedata16(0x25); 
	writecommand16(MCS_PANU2D3 + 1);//cca2[7:0] : reg setting for signal27 selection with u2d mode
	writedata16(0x00); 
	writecommand16(MCS_PANU2D3 + 2);//cca3[7:0] : reg setting for signal28 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D3 + 3);//cca4[7:0] : reg setting for signal29 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D3 + 4);//cca5[7:0] : reg setting for signal20 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D3 + 5);//cca6[7:0] : reg setting for signal31 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D3 + 6);//cca7[7:0] : reg setting for signal32 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D3 + 7);//cca8[7:0] : reg setting for signal33 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D3 + 8);//cca9[7:0] : reg setting for signal34 selection with u2d mode 
	writedata16(0x00); 
	writecommand16(MCS_PANU2D3 + 9);//ccaa[7:0] : reg setting for signal35 selection with u2d mode 
	writedata16(0x00); 

  	/* Command not documented: 0x3A00 */
	writecommand16(0x3A00);//ccaa[7:0] : reg setting for signal35 selection with u2d mode 
	writedata16(0x55);//0x55
			
	/* Exit CMD2 - new! */
	writecommand16(MCS_CMD2_ENA1);
	writedata16(0xFF);
	writecommand16(MCS_CMD2_ENA1 + 1);
	writedata16(0xFF);
	writecommand16(MCS_CMD2_ENA1 + 2);
	writedata16(0xFF);
	
	/* Sleep out */
	writecommand(TFT_SLPOUT);
	writecommand(TFT_NOP);
	delay(100);

	/* Display on */
	writecommand(TFT_DISPON);
	writecommand(TFT_NOP);
	delay(50);

	/* Memory Write */
	//writecommand16(TFT_RAMWR); 
// End of OTM8009A display configuration