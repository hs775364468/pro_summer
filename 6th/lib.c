#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

/* ./rtp_h264 <protocol_type> <text_file> <output_file>*/

static int g_iFdTextFile;
static int TextFileLength;
static unsigned char *g_pucTextFileMem;
static unsigned char *g_pucTextFileMemEnd;

static int g_iFdOutPutFile;
static unsigned char *g_pucOutPutFileMem;
static unsigned char *g_pucOutPutFileMemEnd;

static unsigned char *Ssrc;
static unsigned int Number_Package;


int OpenTextFile(char *pcFileName)
{
	struct stat tStat;
	
	g_iFdTextFile = open(pcFileName, O_RDONLY);
	if (0 > g_iFdTextFile)
	{
		printf("can't open text file %s\n", pcFileName);
		return -1;
	}

	if(fstat(g_iFdTextFile, &tStat))
	{
		printf("can't get fstat\n");
		return -1;
	}
	
	g_pucTextFileMem = (unsigned char *)mmap(NULL , tStat.st_size, PROT_READ, MAP_SHARED, g_iFdTextFile, 0);
	if (g_pucTextFileMem == (unsigned char *)-1)
	{
		printf("can't mmap for text file\n");
		return -1;
	}
	g_pucTextFileMemEnd = g_pucTextFileMem + tStat.st_size;
	TextFileLength =(int)tStat.st_size;
	
	printf("textfile size is:%d\n",TextFileLength);
	//printf("textfile content the first 0x**is:%s",g_pucTextFileMem[0]);
	//printf("textfile content the last 0x**is:%s",g_pucTextFileMem[(int)tStat.st_size-1]);
	
	return 0;
}

int OpenOutPutFile(char *pcFileName)
{
	//char w_buf[] = "123456789";
	//struct stat tStat;
	
	g_iFdOutPutFile = open(pcFileName, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
	if (0 > g_iFdOutPutFile)
	{
		printf("can't open output file %s\n", pcFileName);
		return -1;
	}
	/*
	write(g_iFdOutPutFile,w_buf,sizeof(w_buf));
	
	if(fstat(g_iFdOutPutFile, &tStat))
	{
		printf("can't get fstat\n");
		return -1;
	}
	g_pucOutPutFileMem = (unsigned char *)mmap(NULL , tStat.st_size, PROT_READ, MAP_SHARED, g_iFdOutPutFile, 0);
	if (g_pucOutPutFileMem == (unsigned char *)-1)
	{
		printf("can't mmap for output file\n");
		return -1;
	}
	g_pucOutPutFileMemEnd = g_pucOutPutFileMem + tStat.st_size;

	printf("output file size is:%d\n",(int)tStat.st_size);
	printf("output file content is:%s\n",g_pucOutPutFileMem);
	*/
	return 0;
}

static unsigned int Count_Package_Number(void)
{
	unsigned char *Temp_Mem;
	unsigned int length;
	Temp_Mem = g_pucTextFileMem;
	length = TextFileLength;

	unsigned int count=0;
	unsigned int i=0;

	while(i<=length){
		if(strncmp(Ssrc,Temp_Mem,4)==0){
			count++;
		}
		Temp_Mem+=4;
		i+=4;
	}
	printf("count is:%d\n",count);
	return count;
}

static unsigned char *Find_SSRC_Position(unsigned int i)
{
	unsigned char *Temp_SSRC_Position;
	
	unsigned char *Temp_Mem;
	unsigned int length;
	Temp_Mem = g_pucTextFileMem;
	length = TextFileLength;

	unsigned int count=0;
	unsigned int j=0;

	while(j<=length){
		if(strncmp(Ssrc,Temp_Mem,4)==0){
			count++;
		}
		if(count==i){
			Temp_SSRC_Position = Temp_Mem;
			break;
		}
		Temp_Mem+=4;
		j+=4;
	}
	return Temp_SSRC_Position;
}

int AnalyzeFile(void)
{
	unsigned char *Temp_Mem_Start;
	unsigned char *Temp_Mem_End;
	unsigned int Temp_Size;
	unsigned char *Temp_SSRC_Position;
	unsigned char *Temp_SSRC_Next_Position;

	unsigned int Padding;
	unsigned char *Temp_Write_Start_Position;
	unsigned char *Temp_Write_End_Position;

	Temp_Mem_Start = g_pucTextFileMem;
	unsigned int i;

	/*1.record the ssrc*/
	Ssrc =&Temp_Mem_Start[8];
	/*2.the number of the package using ssrc*/
	Number_Package = Count_Package_Number();

	for(i=1;i<=Number_Package;i++){
	/*3.1 find SSRC position*/
	Temp_SSRC_Position = Find_SSRC_Position(i);
	Temp_SSRC_Next_Position = Find_SSRC_Position(i+1);
	
	/*3.2 get the package size*/
	Temp_Size = Temp_SSRC_Next_Position - Temp_SSRC_Position;
	Temp_Mem_Start = Temp_SSRC_Next_Position-8-Temp_Size;
	Temp_Mem_End = Temp_Mem_Start+Temp_Size-1;
	
	Temp_Write_Start_Position = Temp_Mem_Start;
	Temp_Write_End_Position = Temp_Mem_End;
	
	/*3.3 check the pdding bit*/
	if(Temp_Mem_Start[0]&0X20){
		Padding = 1;
	}
	if(Padding==1){
		Temp_Write_End_Position-=(unsigned int)Temp_Write_End_Position[0];
		Padding = 0;
	}
	/*3.4 remove the rtp_header 12btye */
	Temp_Write_Start_Position += 12;

	write(g_iFdOutPutFile,Temp_Write_Start_Position,(unsigned int)(Temp_Write_End_Position-Temp_Write_Start_Position+1));
	
	}

	return 0;
}
