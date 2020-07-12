// This is the command sequence that initialises the NT35510 driver

//************* NT35510初始化**********//	
	/* MAUCCTR: Manufacture Command Set enable */
	//#Enable Page1
	writecommand16(0xF000); writedata16(0x55);
	writecommand16(0xF001); writedata16(0xAA);
	writecommand16(0xF002); writedata16(0x52);
	writecommand16(0xF003); writedata16(0x08);
	writecommand16(0xF004); writedata16(0x01);
	
	/* BT1CTR: BT1 Power Control for AVDD */
	//# AVDD: manual); 
	writecommand16(0xB600); writedata16(0x34);
	writecommand16(0xB601); writedata16(0x34);
	writecommand16(0xB602); writedata16(0x34);

	/* SETAVDD: Setting AVDD Voltage */
	writecommand16(0xB000); writedata16(0x0D);//09
	writecommand16(0xB001); writedata16(0x0D);
	writecommand16(0xB002); writedata16(0x0D);
	
	/* BT2CTR: BT2 Power Control for AVEE */
	//# AVEE: manual); -6V
	writecommand16(0xB700); writedata16(0x24);
	writecommand16(0xB701); writedata16(0x24);
	writecommand16(0xB702); writedata16(0x24);

	/* SETAVEE: Setting AVEE Voltage */
	writecommand16(0xB100); writedata16(0x0D);
	writecommand16(0xB101); writedata16(0x0D);
	writecommand16(0xB102); writedata16(0x0D);
	
	/* BT3CTR: BT3 Power Control for VCL */
	writecommand16(0xB800); writedata16(0x24);
	writecommand16(0xB801); writedata16(0x24);
	writecommand16(0xB802); writedata16(0x24);

	/* SETVCL: Setting VCL Voltage */
	writecommand16(0xB200); writedata16(0x00);

	/* BT4CTR: BT4 Power Control for VGH */
	//# VGH: Clamp Enable); 
	writecommand16(0xB900); writedata16(0x24);
	writecommand16(0xB901); writedata16(0x24);
	writecommand16(0xB902); writedata16(0x24);

	/* SETVGH: Setting VGH Voltage */
	writecommand16(0xB300); writedata16(0x05);
	writecommand16(0xB301); writedata16(0x05);
	writecommand16(0xB302); writedata16(0x05);

	/* VGHCTR: VGH Output Voltage - commented out */
	///writecommand16(0xBF00); writedata16(0x01);

	/* BT5CTR: BT5 Power Control for VGLX */
	//# VGL(LVGL):
	writecommand16(0xBA00); writedata16(0x34);
	writecommand16(0xBA01); writedata16(0x34);
	writecommand16(0xBA02); writedata16(0x34);

	/* SETVGL_REG: Setting VGL_REG Voltage */
	//# VGL_REG(VGLO)
	writecommand16(0xB500); writedata16(0x0B);
	writecommand16(0xB501); writedata16(0x0B);
	writecommand16(0xB502); writedata16(0x0B);

	/* SETVGP: Setting VGMP and VGSP Voltage */
	//# VGMP/VGSP:
	writecommand16(0xBC00); writedata16(0X00);
	writecommand16(0xBC01); writedata16(0xA3);
	writecommand16(0xBC02); writedata16(0X00);

	/* SETVGN: Setting VGMN and VGSN Voltage */
	//# VGMN/VGSN
	writecommand16(0xBD00); writedata16(0x00);
	writecommand16(0xBD01); writedata16(0xA3);
	writecommand16(0xBD02); writedata16(0x00);

	/* SETVCMOFF: Setting VCOM Offset Voltage */
	//# VCOM=-0.1
	writecommand16(0xBE00); writedata16(0x00);
	writecommand16(0xBE01); writedata16(0x63);//4f
	//  VCOMH+0x01;
  
  	/* GMRCTR1: Setting Gamma 2.2 Correction for Red (Positive) */
	//#R+
	writecommand16(0xD100); writedata16(0x00);
	writecommand16(0xD101); writedata16(0x37);
	writecommand16(0xD102); writedata16(0x00);
	writecommand16(0xD103); writedata16(0x52);
	writecommand16(0xD104); writedata16(0x00);
	writecommand16(0xD105); writedata16(0x7B);
	writecommand16(0xD106); writedata16(0x00);
	writecommand16(0xD107); writedata16(0x99);
	writecommand16(0xD108); writedata16(0x00);
	writecommand16(0xD109); writedata16(0xB1);
	writecommand16(0xD10A); writedata16(0x00);
	writecommand16(0xD10B); writedata16(0xD2);
	writecommand16(0xD10C); writedata16(0x00);
	writecommand16(0xD10D); writedata16(0xF6);
	writecommand16(0xD10E); writedata16(0x01);
	writecommand16(0xD10F); writedata16(0x27);
	writecommand16(0xD110); writedata16(0x01);
	writecommand16(0xD111); writedata16(0x4E);
	writecommand16(0xD112); writedata16(0x01);
	writecommand16(0xD113); writedata16(0x8C);
	writecommand16(0xD114); writedata16(0x01);
	writecommand16(0xD115); writedata16(0xBE);
	writecommand16(0xD116); writedata16(0x02);
	writecommand16(0xD117); writedata16(0x0B);
	writecommand16(0xD118); writedata16(0x02);
	writecommand16(0xD119); writedata16(0x48);
	writecommand16(0xD11A); writedata16(0x02);
	writecommand16(0xD11B); writedata16(0x4A);
	writecommand16(0xD11C); writedata16(0x02);
	writecommand16(0xD11D); writedata16(0x7E);
	writecommand16(0xD11E); writedata16(0x02);
	writecommand16(0xD11F); writedata16(0xBC);
	writecommand16(0xD120); writedata16(0x02);
	writecommand16(0xD121); writedata16(0xE1);
	writecommand16(0xD122); writedata16(0x03);
	writecommand16(0xD123); writedata16(0x10);
	writecommand16(0xD124); writedata16(0x03);
	writecommand16(0xD125); writedata16(0x31);
	writecommand16(0xD126); writedata16(0x03);
	writecommand16(0xD127); writedata16(0x5A);
	writecommand16(0xD128); writedata16(0x03);
	writecommand16(0xD129); writedata16(0x73);
	writecommand16(0xD12A); writedata16(0x03);
	writecommand16(0xD12B); writedata16(0x94);
	writecommand16(0xD12C); writedata16(0x03);
	writecommand16(0xD12D); writedata16(0x9F);
	writecommand16(0xD12E); writedata16(0x03);
	writecommand16(0xD12F); writedata16(0xB3);
	writecommand16(0xD130); writedata16(0x03);
	writecommand16(0xD131); writedata16(0xB9);
	writecommand16(0xD132); writedata16(0x03);
	writecommand16(0xD133); writedata16(0xC1);
	
	/* GMGCTR1: Setting Gamma 2.2 Correction for Green (Positive) */
	//#G+
	writecommand16(0xD200); writedata16(0x00);
	writecommand16(0xD201); writedata16(0x37);
	writecommand16(0xD202); writedata16(0x00);
	writecommand16(0xD203); writedata16(0x52);
	writecommand16(0xD204); writedata16(0x00);
	writecommand16(0xD205); writedata16(0x7B);
	writecommand16(0xD206); writedata16(0x00);
	writecommand16(0xD207); writedata16(0x99);
	writecommand16(0xD208); writedata16(0x00);
	writecommand16(0xD209); writedata16(0xB1);
	writecommand16(0xD20A); writedata16(0x00);
	writecommand16(0xD20B); writedata16(0xD2);
	writecommand16(0xD20C); writedata16(0x00);
	writecommand16(0xD20D); writedata16(0xF6);
	writecommand16(0xD20E); writedata16(0x01);
	writecommand16(0xD20F); writedata16(0x27);
	writecommand16(0xD210); writedata16(0x01);
	writecommand16(0xD211); writedata16(0x4E);
	writecommand16(0xD212); writedata16(0x01);
	writecommand16(0xD213); writedata16(0x8C);
	writecommand16(0xD214); writedata16(0x01);
	writecommand16(0xD215); writedata16(0xBE);
	writecommand16(0xD216); writedata16(0x02);
	writecommand16(0xD217); writedata16(0x0B);
	writecommand16(0xD218); writedata16(0x02);
	writecommand16(0xD219); writedata16(0x48);
	writecommand16(0xD21A); writedata16(0x02);
	writecommand16(0xD21B); writedata16(0x4A);
	writecommand16(0xD21C); writedata16(0x02);
	writecommand16(0xD21D); writedata16(0x7E);
	writecommand16(0xD21E); writedata16(0x02);
	writecommand16(0xD21F); writedata16(0xBC);
	writecommand16(0xD220); writedata16(0x02);
	writecommand16(0xD221); writedata16(0xE1);
	writecommand16(0xD222); writedata16(0x03);
	writecommand16(0xD223); writedata16(0x10);
	writecommand16(0xD224); writedata16(0x03);
	writecommand16(0xD225); writedata16(0x31);
	writecommand16(0xD226); writedata16(0x03);
	writecommand16(0xD227); writedata16(0x5A);
	writecommand16(0xD228); writedata16(0x03);
	writecommand16(0xD229); writedata16(0x73);
	writecommand16(0xD22A); writedata16(0x03);
	writecommand16(0xD22B); writedata16(0x94);
	writecommand16(0xD22C); writedata16(0x03);
	writecommand16(0xD22D); writedata16(0x9F);
	writecommand16(0xD22E); writedata16(0x03);
	writecommand16(0xD22F); writedata16(0xB3);
	writecommand16(0xD230); writedata16(0x03);
	writecommand16(0xD231); writedata16(0xB9);
	writecommand16(0xD232); writedata16(0x03);
	writecommand16(0xD233); writedata16(0xC1);
	
	/* GMBCTR1: Setting Gamma 2.2 Correction for Blue (Positive) */
	//#B+
	writecommand16(0xD300); writedata16(0x00);
	writecommand16(0xD301); writedata16(0x37);
	writecommand16(0xD302); writedata16(0x00);
	writecommand16(0xD303); writedata16(0x52);
	writecommand16(0xD304); writedata16(0x00);
	writecommand16(0xD305); writedata16(0x7B);
	writecommand16(0xD306); writedata16(0x00);
	writecommand16(0xD307); writedata16(0x99);
	writecommand16(0xD308); writedata16(0x00);
	writecommand16(0xD309); writedata16(0xB1);
	writecommand16(0xD30A); writedata16(0x00);
	writecommand16(0xD30B); writedata16(0xD2);
	writecommand16(0xD30C); writedata16(0x00);
	writecommand16(0xD30D); writedata16(0xF6);
	writecommand16(0xD30E); writedata16(0x01);
	writecommand16(0xD30F); writedata16(0x27);
	writecommand16(0xD310); writedata16(0x01);
	writecommand16(0xD311); writedata16(0x4E);
	writecommand16(0xD312); writedata16(0x01);
	writecommand16(0xD313); writedata16(0x8C);
	writecommand16(0xD314); writedata16(0x01);
	writecommand16(0xD315); writedata16(0xBE);
	writecommand16(0xD316); writedata16(0x02);
	writecommand16(0xD317); writedata16(0x0B);
	writecommand16(0xD318); writedata16(0x02);
	writecommand16(0xD319); writedata16(0x48);
	writecommand16(0xD31A); writedata16(0x02);
	writecommand16(0xD31B); writedata16(0x4A);
	writecommand16(0xD31C); writedata16(0x02);
	writecommand16(0xD31D); writedata16(0x7E);
	writecommand16(0xD31E); writedata16(0x02);
	writecommand16(0xD31F); writedata16(0xBC);
	writecommand16(0xD320); writedata16(0x02);
	writecommand16(0xD321); writedata16(0xE1);
	writecommand16(0xD322); writedata16(0x03);
	writecommand16(0xD323); writedata16(0x10);
	writecommand16(0xD324); writedata16(0x03);
	writecommand16(0xD325); writedata16(0x31);
	writecommand16(0xD326); writedata16(0x03);
	writecommand16(0xD327); writedata16(0x5A);
	writecommand16(0xD328); writedata16(0x03);
	writecommand16(0xD329); writedata16(0x73);
	writecommand16(0xD32A); writedata16(0x03);
	writecommand16(0xD32B); writedata16(0x94);
	writecommand16(0xD32C); writedata16(0x03);
	writecommand16(0xD32D); writedata16(0x9F);
	writecommand16(0xD32E); writedata16(0x03);
	writecommand16(0xD32F); writedata16(0xB3);
	writecommand16(0xD330); writedata16(0x03);
	writecommand16(0xD331); writedata16(0xB9);
	writecommand16(0xD332); writedata16(0x03);
	writecommand16(0xD333); writedata16(0xC1);

	/* GMRCTR2: Setting Gamma 2.2 Correction for Red (Negative) */
	//#R-///////////////////////////////////////////
	writecommand16(0xD400); writedata16(0x00);
	writecommand16(0xD401); writedata16(0x37);
	writecommand16(0xD402); writedata16(0x00);
	writecommand16(0xD403); writedata16(0x52);
	writecommand16(0xD404); writedata16(0x00);
	writecommand16(0xD405); writedata16(0x7B);
	writecommand16(0xD406); writedata16(0x00);
	writecommand16(0xD407); writedata16(0x99);
	writecommand16(0xD408); writedata16(0x00);
	writecommand16(0xD409); writedata16(0xB1);
	writecommand16(0xD40A); writedata16(0x00);
	writecommand16(0xD40B); writedata16(0xD2);
	writecommand16(0xD40C); writedata16(0x00);
	writecommand16(0xD40D); writedata16(0xF6);
	writecommand16(0xD40E); writedata16(0x01);
	writecommand16(0xD40F); writedata16(0x27);
	writecommand16(0xD410); writedata16(0x01);
	writecommand16(0xD411); writedata16(0x4E);
	writecommand16(0xD412); writedata16(0x01);
	writecommand16(0xD413); writedata16(0x8C);
	writecommand16(0xD414); writedata16(0x01);
	writecommand16(0xD415); writedata16(0xBE);
	writecommand16(0xD416); writedata16(0x02);
	writecommand16(0xD417); writedata16(0x0B);
	writecommand16(0xD418); writedata16(0x02);
	writecommand16(0xD419); writedata16(0x48);
	writecommand16(0xD41A); writedata16(0x02);
	writecommand16(0xD41B); writedata16(0x4A);
	writecommand16(0xD41C); writedata16(0x02);
	writecommand16(0xD41D); writedata16(0x7E);
	writecommand16(0xD41E); writedata16(0x02);
	writecommand16(0xD41F); writedata16(0xBC);
	writecommand16(0xD420); writedata16(0x02);
	writecommand16(0xD421); writedata16(0xE1);
	writecommand16(0xD422); writedata16(0x03);
	writecommand16(0xD423); writedata16(0x10);
	writecommand16(0xD424); writedata16(0x03);
	writecommand16(0xD425); writedata16(0x31);
	writecommand16(0xD426); writedata16(0x03);
	writecommand16(0xD427); writedata16(0x5A);
	writecommand16(0xD428); writedata16(0x03);
	writecommand16(0xD429); writedata16(0x73);
	writecommand16(0xD42A); writedata16(0x03);
	writecommand16(0xD42B); writedata16(0x94);
	writecommand16(0xD42C); writedata16(0x03);
	writecommand16(0xD42D); writedata16(0x9F);
	writecommand16(0xD42E); writedata16(0x03);
	writecommand16(0xD42F); writedata16(0xB3);
	writecommand16(0xD430); writedata16(0x03);
	writecommand16(0xD431); writedata16(0xB9);
	writecommand16(0xD432); writedata16(0x03);
	writecommand16(0xD433); writedata16(0xC1);

	/* GMGCTR2: Setting Gamma 2.2 Correction for Green (Negative) */
	//#G-//////////////////////////////////////////////
	writecommand16(0xD500); writedata16(0x00);
	writecommand16(0xD501); writedata16(0x37);
	writecommand16(0xD502); writedata16(0x00);
	writecommand16(0xD503); writedata16(0x52);
	writecommand16(0xD504); writedata16(0x00);
	writecommand16(0xD505); writedata16(0x7B);
	writecommand16(0xD506); writedata16(0x00);
	writecommand16(0xD507); writedata16(0x99);
	writecommand16(0xD508); writedata16(0x00);
	writecommand16(0xD509); writedata16(0xB1);
	writecommand16(0xD50A); writedata16(0x00);
	writecommand16(0xD50B); writedata16(0xD2);
	writecommand16(0xD50C); writedata16(0x00);
	writecommand16(0xD50D); writedata16(0xF6);
	writecommand16(0xD50E); writedata16(0x01);
	writecommand16(0xD50F); writedata16(0x27);
	writecommand16(0xD510); writedata16(0x01);
	writecommand16(0xD511); writedata16(0x4E);
	writecommand16(0xD512); writedata16(0x01);
	writecommand16(0xD513); writedata16(0x8C);
	writecommand16(0xD514); writedata16(0x01);
	writecommand16(0xD515); writedata16(0xBE);
	writecommand16(0xD516); writedata16(0x02);
	writecommand16(0xD517); writedata16(0x0B);
	writecommand16(0xD518); writedata16(0x02);
	writecommand16(0xD519); writedata16(0x48);
	writecommand16(0xD51A); writedata16(0x02);
	writecommand16(0xD51B); writedata16(0x4A);
	writecommand16(0xD51C); writedata16(0x02);
	writecommand16(0xD51D); writedata16(0x7E);
	writecommand16(0xD51E); writedata16(0x02);
	writecommand16(0xD51F); writedata16(0xBC);
	writecommand16(0xD520); writedata16(0x02);
	writecommand16(0xD521); writedata16(0xE1);
	writecommand16(0xD522); writedata16(0x03);
	writecommand16(0xD523); writedata16(0x10);
	writecommand16(0xD524); writedata16(0x03);
	writecommand16(0xD525); writedata16(0x31);
	writecommand16(0xD526); writedata16(0x03);
	writecommand16(0xD527); writedata16(0x5A);
	writecommand16(0xD528); writedata16(0x03);
	writecommand16(0xD529); writedata16(0x73);
	writecommand16(0xD52A); writedata16(0x03);
	writecommand16(0xD52B); writedata16(0x94);
	writecommand16(0xD52C); writedata16(0x03);
	writecommand16(0xD52D); writedata16(0x9F);
	writecommand16(0xD52E); writedata16(0x03);
	writecommand16(0xD52F); writedata16(0xB3);
	writecommand16(0xD530); writedata16(0x03);
	writecommand16(0xD531); writedata16(0xB9);
	writecommand16(0xD532); writedata16(0x03);
	writecommand16(0xD533); writedata16(0xC1);
	
	/* GMBCTR2: Setting Gamma 2.2 Correction for Blue (Negative) */
	//#B-///////////////////////////////
	writecommand16(0xD600); writedata16(0x00);
	writecommand16(0xD601); writedata16(0x37);
	writecommand16(0xD602); writedata16(0x00);
	writecommand16(0xD603); writedata16(0x52);
	writecommand16(0xD604); writedata16(0x00);
	writecommand16(0xD605); writedata16(0x7B);
	writecommand16(0xD606); writedata16(0x00);
	writecommand16(0xD607); writedata16(0x99);
	writecommand16(0xD608); writedata16(0x00);
	writecommand16(0xD609); writedata16(0xB1);
	writecommand16(0xD60A); writedata16(0x00);
	writecommand16(0xD60B); writedata16(0xD2);
	writecommand16(0xD60C); writedata16(0x00);
	writecommand16(0xD60D); writedata16(0xF6);
	writecommand16(0xD60E); writedata16(0x01);
	writecommand16(0xD60F); writedata16(0x27);
	writecommand16(0xD610); writedata16(0x01);
	writecommand16(0xD611); writedata16(0x4E);
	writecommand16(0xD612); writedata16(0x01);
	writecommand16(0xD613); writedata16(0x8C);
	writecommand16(0xD614); writedata16(0x01);
	writecommand16(0xD615); writedata16(0xBE);
	writecommand16(0xD616); writedata16(0x02);
	writecommand16(0xD617); writedata16(0x0B);
	writecommand16(0xD618); writedata16(0x02);
	writecommand16(0xD619); writedata16(0x48);
	writecommand16(0xD61A); writedata16(0x02);
	writecommand16(0xD61B); writedata16(0x4A);
	writecommand16(0xD61C); writedata16(0x02);
	writecommand16(0xD61D); writedata16(0x7E);
	writecommand16(0xD61E); writedata16(0x02);
	writecommand16(0xD61F); writedata16(0xBC);
	writecommand16(0xD620); writedata16(0x02);
	writecommand16(0xD621); writedata16(0xE1);
	writecommand16(0xD622); writedata16(0x03);
	writecommand16(0xD623); writedata16(0x10);
	writecommand16(0xD624); writedata16(0x03);
	writecommand16(0xD625); writedata16(0x31);
	writecommand16(0xD626); writedata16(0x03);
	writecommand16(0xD627); writedata16(0x5A);
	writecommand16(0xD628); writedata16(0x03);
	writecommand16(0xD629); writedata16(0x73);
	writecommand16(0xD62A); writedata16(0x03);
	writecommand16(0xD62B); writedata16(0x94);
	writecommand16(0xD62C); writedata16(0x03);
	writecommand16(0xD62D); writedata16(0x9F);
	writecommand16(0xD62E); writedata16(0x03);
	writecommand16(0xD62F); writedata16(0xB3);
	writecommand16(0xD630); writedata16(0x03);
	writecommand16(0xD631); writedata16(0xB9);
	writecommand16(0xD632); writedata16(0x03);
	writecommand16(0xD633); writedata16(0xC1);

	/* MAUCCTR: Manufacture Command Set enable */
	//#Enable Page0
	writecommand16(0xF000); writedata16(0x55);
	writecommand16(0xF001); writedata16(0xAA);
	writecommand16(0xF002); writedata16(0x52);
	writecommand16(0xF003); writedata16(0x08);
	writecommand16(0xF004); writedata16(0x00);
	
	/* RGBCTR: RGB Interface Signals Control */
	//# RGB I/F Setting
	writecommand16(0xB000); writedata16(0x08);
	writecommand16(0xB001); writedata16(0x05);
	writecommand16(0xB002); writedata16(0x02);
	writecommand16(0xB003); writedata16(0x05);
	writecommand16(0xB004); writedata16(0x02);
	
	/* SDHDTCTR: Source Output Data Hold Time Control */
	//## SDT:
	writecommand16(0xB600); writedata16(0x08);

	/* DPRSLCTR: Display Resolution Control */
	writecommand16(0xB500); writedata16(0x50);//0x6b ???? 480x854       0x50 ???? 480x800

	/* GSEQCTR: EQ Control Function for Gate Signals */
	//## Gate EQ:
	writecommand16(0xB700); writedata16(0x00);
	writecommand16(0xB701); writedata16(0x00);

	/* SDEQCTR: EQ Control Function for Source Driver */
	//## Source EQ:
	writecommand16(0xB800); writedata16(0x01);
	writecommand16(0xB801); writedata16(0x05);
	writecommand16(0xB802); writedata16(0x05);
	writecommand16(0xB803); writedata16(0x05);

	/* INVCTR: Inversion Driving Control */
	//# Inversion: Column inversion (NVT)
	writecommand16(0xBC00); writedata16(0x00);
	writecommand16(0xBC01); writedata16(0x00);
	writecommand16(0xBC02); writedata16(0x00);

	/* DPTMCTR12: Display Timing Control 12 */
	//# BOE's Setting(default)
	writecommand16(0xCC00); writedata16(0x03);
	writecommand16(0xCC01); writedata16(0x00);
	writecommand16(0xCC02); writedata16(0x00);

	/* DPFRCTR1: Display Timing Control in Normal / Idle Off Mode */
	//# Display Timing:
	writecommand16(0xBD00); writedata16(0x01);
	writecommand16(0xBD01); writedata16(0x84);
	writecommand16(0xBD02); writedata16(0x07);
	writecommand16(0xBD03); writedata16(0x31);
	writecommand16(0xBD04); writedata16(0x00);

	/* SDVPCTR: Source Control in Vertical Porch Time */
	writecommand16(0xBA00); writedata16(0x01);

	/* MAUCCTR: Manufacture Command Set disable (?) */
	writecommand16(0xFF00); writedata16(0xAA);
	writecommand16(0xFF01); writedata16(0x55);
	writecommand16(0xFF02); writedata16(0x25);
	writecommand16(0xFF03); writedata16(0x01);

	/* TEON: Tearing Effect Line ON */
	writecommand16(0x3500); writedata16(0x00);

	/* MADCTL: Memory Data Access Control */
	writecommand16(TFT_MADCTL); writedata16(0x00);

	/* COLMOD: Interface Pixel Format */ 
	/* 	“0101” = 16-bit/pixel
		“0110” = 18-bit/pixel
		“0111” = 24-bit/pixel */
	writecommand16(0x3a00); writedata16(0x55);  ////55=16?/////66=18?
	
	/* SLPOUT: Sleep Out */
	writecommand16(TFT_SLPOUT);	
	delay(120); 

	/* DISPON: Display On */
	writecommand16(TFT_DISPON); 
	
	/* RAMWR: Memory Write */
	//writecommand16(TFT_RAMWR);

// End of NT35510 display configuration