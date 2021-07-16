/*  
     VLW Compress v0.0.1 16/7/21
	 
  Used to transform 8bit VLW file into 4bit VLW file with no padding support.
	
  Note: The program is made by pure C ;it has no dependence.
  License:See license in root directory.
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

FILE* fin;
FILE* fout;
struct {
	uint32_t gCount;
	uint32_t gVer; //0xB(11) = unCompressed,0xC(12)=compressed.
	uint32_t fSize;
	uint32_t mboxY;
	uint32_t ascent;
	uint32_t descent;
	uint8_t* gX;
	uint8_t* gY;
} fInfo;

uint32_t readu32(){
	uint32_t temp=0;
	uint8_t tmp[4]={0};
	fread(tmp,1,4,fin);
	temp |= tmp[0] << 24;
	temp |= tmp[1] << 16;
	temp |= tmp[2] << 8;
	temp |= tmp[3] << 0;

	return temp;
}

uint8_t readu8(){
	uint8_t temp[1];
	fread(temp,1,1,fin);
	return temp[0];
}

int writeu32(uint32_t in){
	uint8_t tmp[4]={0};
	tmp[0]=in>>24;
	tmp[1]=(in & 0x00ff0000 )>>16;
	tmp[2]=(in & 0x0000ff00 )>>8;
	tmp[3]=(in & 0x000000ff );
	if(fwrite(tmp,1,4,fout)==4){
		return 0;
	}
	return 1;
}

int writeu16(uint16_t in){
	uint8_t tmp[2]={0};
	tmp[0]=in>>8;
	tmp[1]=(in & 0x00ff );
	if(fwrite(tmp,1,2,fout)==2){
		return 0;
	}
	return 1;
}

int writeu8(uint8_t in){
	if(fwrite(&in,1,1,fout)==1){
		return 0;
	}
	return 1;
}

uint8_t combine2u8(uint8_t H,uint8_t L){
	return (H & 0xf0 ) | (L >> 4); //All get High bits and combine
}


int getFontInfo(){
	fInfo.gCount=readu32();
	fInfo.gVer=readu32();
	fInfo.fSize=readu32();
	fInfo.mboxY=readu32();
	fInfo.ascent=readu32();
	fInfo.descent=readu32();
	return 0;
}

int writeHeader(){
	writeu32(fInfo.gCount);
	writeu32(12ULL);
	writeu32(fInfo.fSize);
	writeu32(fInfo.mboxY);
	writeu32(fInfo.ascent);
	writeu32(fInfo.descent);
	return 0;
	
}

int compressCharsTable(){
	for(uint32_t i=0;i<fInfo.gCount;i++){
		uint16_t unicode=(uint16_t)readu32();
		uint8_t  bh=(uint8_t)readu32();
		uint8_t  bw=(uint8_t)readu32();
		uint8_t  ga=(uint8_t)readu32();
		int16_t   dy=(int16_t)readu32();
		int8_t   dx=(int8_t)readu32();
		readu32(); //ignore padding
		fInfo.gX[i]=bw;
		fInfo.gY[i]=bh;
		writeu16(unicode);
		writeu8(bh);
		writeu8(bw);
		writeu8(ga);
		writeu16(dy);
		writeu8(dx);
	}
	return 0;
}

int compressCharsBitmap(){
	for(uint32_t i=0;i<fInfo.gCount;i++){
		if((fInfo.gX[i] & 1) !=0){//not fix 2
			for(uint8_t j=0;j<fInfo.gY[i];j++){
				for(uint8_t k=0;k<fInfo.gX[i]-1;k+=2){
					uint8_t t1=(uint8_t)readu8();
					uint8_t t2=(uint8_t)readu8();
					writeu8(combine2u8(t1,t2));
				}
				//process the last pixel of each line with 0 as low bits
				uint8_t t1=(uint8_t)readu8();
				writeu8(combine2u8(t1,0));
			}
		}else{
			for(uint16_t j=0;j<fInfo.gX[i] * fInfo.gY[i];j+=2){
				uint8_t t1=(uint8_t)readu8();
				uint8_t t2=(uint8_t)readu8();
				writeu8(combine2u8(t1,t2));
			}
		}
	}
}


int main(int argc,char* argv[]){
	printf("\n=== VLWCompress - 4bpp by Lucy2003 v0.0.1 === \n\n");
	if(argc<=2){
		printf("  usage: %s [input_path] [output_path]\n\n",argv[0]);
		return 0;
	}
	printf("Input file: %s\nOutput file: %s\n",argv[1],argv[2]);
	
	
	fin=fopen(argv[1],"rb");
	if(fin == NULL){
		printf("\nError: Can't open input file. \n\n");
		return 1;
	}
	fseek(fin,0,SEEK_SET);
	fout=fopen(argv[2],"w+b");
	if(fout==NULL){
		printf("\nError: Can't open output file. \n\n");
		return 2;
	}
	
	getFontInfo();
	printf("Font file info:\n");
	printf("  Char count: %d\n",fInfo.gCount);
	printf("  Font version: %d\n",fInfo.gVer);
	printf("  Font size: %d Bytes\n",fInfo.fSize);
	printf("  mboxY: %d\n  ascent:%d\n  descent:%d\n",fInfo.mboxY,fInfo.ascent,fInfo.descent);
	
	if(fInfo.gVer==12){
		printf("\nError: Input file already compressed.\n\n");
		return 3;
	}
	
	if(fInfo.gVer!=11){
		printf("\nError: Unsupport font version.\n\n");
		return 4;
	}
	
	fInfo.gX=NULL;
	fInfo.gY=NULL;
	fInfo.gX=(uint8_t*)calloc(fInfo.gCount,sizeof(uint8_t));
	fInfo.gY=(uint8_t*)calloc(fInfo.gCount,sizeof(uint8_t));
	if(fInfo.gX==NULL || fInfo.gY==NULL){
		printf("\nError: Malloc memory failed.\n\n");
		return 5;
	}
	//Compress
	writeHeader();
	compressCharsTable();
	compressCharsBitmap();
	
	free(fInfo.gX);
	free(fInfo.gY);
	
	while(!feof(fin)){
		writeu8(readu8());
	}
	
	fclose(fin);
	fclose(fout);
	printf("\nCompress Font successfully!\n\n");

	return 0;
}
