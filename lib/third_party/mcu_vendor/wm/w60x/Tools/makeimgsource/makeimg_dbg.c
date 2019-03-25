#include <stdio.h>
#include <string.h>

#define DEBUG_IMG_HEADER_LEN    (0x100)

#define IMG_HEADER_MAGIC_NO   (0xA0FFFF9F)
#define ONCE_READ_LEN	1024

typedef struct __T_BOOTER
{
	unsigned int   	magic_no;
	unsigned short 	img_type;
	unsigned short 	zip_type;	
	unsigned int   	run_img_addr;
	unsigned int   	run_img_len;
	unsigned int	run_org_checksum;
	unsigned int    upd_img_addr;
	unsigned int    upd_img_len;
	unsigned int 	upd_checksum;
	unsigned int   	upd_no;
	unsigned char  	ver[16];
	unsigned int 	hd_checksum;
}T_BOOTER;

int file_size(char* filename)  
{  
    FILE *fp=fopen(filename,"r");  
    if(!fp) return -1;  
    fseek(fp,0L,SEEK_END);  
    int size=ftell(fp);  
    fclose(fp);
      
    return size;
} 

/********************************************************

Final openocd debug img File Like this structure:
	------------------------------------
	| T_BOOTER--------image header     |
	------------------------------------
	| 256-sizeof(T_BOOTER)-pad 0xFF    |
	------------------------------------
	| RUN-IMG                          |
	------------------------------------
*********************************************************/
int main(int argc,char *argv[])
{
	FILE *fpimg = NULL;
	FILE *fout = NULL;
	
	unsigned char buf[ONCE_READ_LEN + 1];	
	int readlen = 0;
	int i;
	T_BOOTER tbooter;

	int appimg_len = 0;
	int final_len = 0;
	int magic_word = 0;

	memset(&tbooter, 0, sizeof(T_BOOTER));

	switch (argc)
	{		
		case 3:
			break;
		default:
			printf("\nparam cnt error\n");
			printf("参数0:  exe\n");
			printf("参数1:  输入image文件\n");
			printf("参数2 : 输出文件\n");
			return -1;
			
	}



	fpimg = fopen(argv[1], "rb+");
	if(NULL == fpimg)
	{
		printf("\nopen img file error\n");
		return -4;
	}

	magic_word = 0;
	fread(&magic_word, 1, 4, fpimg);
	if(magic_word != IMG_HEADER_MAGIC_NO)
	{
		printf("\ninput %s file error \n",argv[1]);
		fclose(fpimg);
		return -4;
	}
	
	fout = fopen(argv[2], "wb+");

	appimg_len = file_size(argv[1]);

	//Write 0xFF to output file
	final_len = DEBUG_IMG_HEADER_LEN + (appimg_len - sizeof(T_BOOTER));
	for(i = 0; i < (final_len /ONCE_READ_LEN); i++)
	{
		memset(buf, 0xff, ONCE_READ_LEN);
		fwrite(buf, 1, ONCE_READ_LEN, fout);
	}
	memset(buf, 0xff, final_len % ONCE_READ_LEN);
	fwrite(buf, 1, final_len % ONCE_READ_LEN, fout);
	
	//Write sec img to output file
	//fseek(fpimg, sizeof(T_BOOTER), SEEK_SET);
	fseek(fpimg, 0, SEEK_SET);
	fread((unsigned char *)&tbooter, 1, sizeof(T_BOOTER), fpimg);

	fseek(fout, 0, SEEK_SET);
	fwrite(&tbooter, 1, sizeof(T_BOOTER), fout);
	fseek(fout, DEBUG_IMG_HEADER_LEN, SEEK_SET);

	while(!feof(fpimg))
	{
		readlen = fread(buf, 1, ONCE_READ_LEN, fpimg);
		fwrite(buf, 1, readlen, fout);
	}

	if(fpimg)
	{
		fclose(fpimg);
	}

	if(fout)
	{
		fclose(fout);
	}
	return 0;
}
