#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <lib.h>

/* ./rtp_h264 <protocol_type> <text_file> <output_file>*/
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
	iRet = AnalyzeFile();
	if (iRet)
	{
		printf("AnalyzeFile error!\n");
		return -1;
	}	
	return 0;
}
