#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

/* ./rtp_h264 <protocol_type> <text_file> */

static int g_iFdTextFile;
static unsigned char *g_pucTextFileMem;
static unsigned char *g_pucTextFileMemEnd;

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

	printf("pcFileName size is:%d\n",(int)tStat.st_size);
	printf("pcFileName content is:%s",g_pucTextFileMem);
	return 0;
}

int main(int argc, char *argv[])
{
	char protocol_type[10];
	char text_file[30];
	int iRet;
	
	if(argc!=3){
		printf("Usage: %s <protocol_type> <text_file>\n", argv[0]);
		return -1;
	}
	if(strcmp(argv[1],"udp")){
		printf("protocol_type only support udp!\n");
		printf("Usage: %s <protocol_type> <text_file>\n", argv[0]);
		return -1;
	}
	strncpy(protocol_type, argv[1],10);
	strncpy(text_file, argv[2],30);

	printf("protocol_type is :%s\n",protocol_type);
	printf("text_file is :%s\n",text_file);

	iRet = OpenTextFile(text_file);
	if (iRet)
	{
		printf("OpenTextFile error!\n");
		return -1;
	}
	return 0;
}
