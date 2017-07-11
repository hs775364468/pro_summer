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
static unsigned char *g_pucTextFileMem;
static unsigned char *g_pucTextFileMemEnd;

static int g_iFdOutPutFile;
static unsigned char *g_pucOutPutFileMem;
static unsigned char *g_pucOutPutFileMemEnd;

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

	printf("textfile size is:%d\n",(int)tStat.st_size);
	printf("textfile content is:%s",g_pucTextFileMem);
	return 0;
}

int OpenOutPutFile(char *pcFileName)
{
	char w_buf[] = "123456789";
	struct stat tStat;
	
	g_iFdOutPutFile = open(pcFileName, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
	if (0 > g_iFdOutPutFile)
	{
		printf("can't open output file %s\n", pcFileName);
		return -1;
	}
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
	return 0;
}
int main(int argc, char *argv[])
{
	char protocol_type[10];
	char text_file[30];
	char output_file[30];
	int iRet;

	if(argc!=4){
		printf("Usage: %s <protocol_type> <text_file> <output_file>\n", argv[0]);
		return -1;
	}
	if(strcmp(argv[1],"udp")){
		printf("protocol_type only support udp!\n");
		printf("Usage: %s <protocol_type> <text_file>\n", argv[0]);
		return -1;
	}
	strncpy(protocol_type, argv[1],10);
	strncpy(text_file, argv[2],30);
	strncpy(output_file, argv[3],30);
	
	printf("protocol_type is :%s\n",protocol_type);
	printf("text_file is :%s\n",text_file);
	printf("output_file is :%s\n",output_file);
	
	iRet = OpenTextFile(text_file);
	if (iRet)
	{
		printf("OpenTextFile error!\n");
		return -1;
	}
	iRet = OpenOutPutFile(output_file);
	if (iRet)
	{
		printf("OpenOutPutFile error!\n");
		return -1;
	}
	return 0;
}
