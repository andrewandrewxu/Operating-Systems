#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/mman.h>  //mmap
#include<fcntl.h>  //open
#include<string.h>

void FileInfo(const unsigned char* mmap,  char* FileName, char* Extension){
	int j = 32* 512;	//the end for file name
	int current = 0;
	int num=0;
	int i=0;
	unsigned int FileSize=0;
	unsigned int Day=0;
	unsigned int Month=0;
	unsigned int Year=0;
	unsigned int Hours=0;
	unsigned int Minutes=0;

	for(current=19*512; current<j; current+=32){
		if(mmap[current]!=0xE5 && mmap[current]!=0x00){
			if((mmap[current + 11] & 0x08) || (mmap[current +11] &0x0F))
				continue;
			//for(i=0; i<4; i++){
			FileSize = mmap[current+28] + (mmap[current +29]<<8) + (mmap[current+30]<<16) + (mmap[current+31]<<24);
			//}


			for(i=0; i<8; i++){				//get file name here
				if(mmap[current+i]==0x00){
					break;
				}
					
				FileName[i] = mmap[current+i];
			}

			for(i=0; i<3; i++){				//get file extension here
				Extension[i] = mmap[current + 8 +i];
			}

						//get file creation date here
			Day = (mmap[current + 16]&0x1F);		//day
			Month = (mmap[current + 16] >>5) + ((mmap[current+17]<<3)&0x08);
			Year = ((mmap[current+17]>>1)&0x7F);
			Year += 1980;


			//get Time here
			Hours = (mmap[current+15]>>3);
			Minutes = (mmap[current+14]>>5) + ((mmap[current+15]<<3)&0x38);

			if(mmap[current+11]&0x10){		//is a file
				if(Minutes<10 && Hours>10){			//formating the output
					printf("D      %d         %10s.%s      %d-%d-%d  %d:0%d\n", FileSize, FileName, Extension, Year, Month, Day, Hours, Minutes);
				}
				else if(Hours<10 && Minutes>10){
					printf("D      %d         %10s.%s      %d-%d-%d  0%d:%d\n", FileSize, FileName, Extension, Year, Month, Day, Hours, Minutes);
				}
				else if(Hours<10 && Minutes<10){
					printf("D      %d         %10s.%s      %d-%d-%d  0%d:0%d\n", FileSize, FileName, Extension, Year, Month, Day, Hours, Minutes);
				}
				else{
					printf("D      %d         %10s.%s      %d-%d-%d  %d:%d\n", FileSize, FileName, Extension, Year, Month, Day, Hours, Minutes);
				}
				

			}else{	

				if(Minutes<10 && Hours>10){			//formating the output
					printf("F      %d         %10s.%s      %d-%d-%d  %d:0%d\n", FileSize, FileName, Extension, Year, Month, Day, Hours, Minutes);
				}
				else if(Hours<10 && Minutes>10){
					printf("F      %d         %10s.%s      %d-%d-%d  0%d:%d\n", FileSize, FileName, Extension, Year, Month, Day, Hours, Minutes);
				}
				else if(Hours<10 && Minutes<10){
					printf("F      %d         %10s.%s      %d-%d-%d  0%d:0%d\n", FileSize, FileName, Extension, Year, Month, Day, Hours, Minutes);
				}
				else{
					printf("F      %d         %10s.%s      %d-%d-%d  %d:%d\n", FileSize, FileName, Extension, Year, Month, Day, Hours, Minutes);
				}		// is a subdirectory
				
			}
			

			//free(FileName);
		}
		//current += 32;
		
	}

}


int main(int argc, char** argv)
{
	FILE *fp;

	int fd;
	struct stat sf;
	char *p;
	
	int size;

	char *FileSize = malloc(sizeof(char)*10);
	char *FileName = malloc(sizeof(char)*20);
	char *Extension = malloc(sizeof(char)*3);
	bzero(FileSize, sizeof(FileSize));
	bzero(FileName, sizeof(FileName));
	char* Date = malloc(sizeof(char)*2);
	char* Time = malloc(sizeof(char)*2);
	bzero(Date, sizeof(Date));
	bzero(Time, sizeof(Time));


	if ((fd=open(argv[1], O_RDONLY)))
	{
		fstat(fd, &sf);

		// void * mmap(void * addr, size_t length, int prot, 
		//		int flags, int fd, off_t offset);

		p = mmap(NULL,sf.st_size, PROT_READ, MAP_SHARED, fd, 0);

		//printf("Successfully open the image file.\n");
		FileInfo(p,  FileName, Extension);
					
	}

	else{
		printf("Fail to open the image file.\n");
	}
		

	free(FileSize);
	free(FileName);
	free(Extension);
	free(Date);
	free(Time);
	close(fd);
	return 0;
}
