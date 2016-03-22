//Name: Xu Zhaoxuan
//Student No.: V00791274

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>  //mmap
#include <fcntl.h>  //open
#include <string.h>

unsigned int Entry(const unsigned char* p, int n){
	int current = 512;
	if  (n % 2 != 0){
		current += (int)((double)n * 1.5 +0.5);
		return (((p[current - 1]) &0xF0)>>4) + (p[current] << 4);

	}else{
		current += (int)((double)n * 1.5);
		return ((p[current]) + ((p[current + 1]&0x0F)<<8));

	}

}

void getOSName(char* osname, char * mmap)
{	
	int i;
	for(i=0;i<8;i++)
		osname[i] = mmap[3+i];
	
	//fseek(fp,3L,SEEK_SET);
	//fread(osname,1,8,fp);
}

int getTotalSize(char * mmap)
{
	int *tmp1 = malloc(sizeof(int));
	int *tmp2 = malloc(sizeof(int));
	int retVal;
	//fseek(fp,19L,SEEK_SET);
	//fread(tmp1,1,1,fp);
	//fread(tmp2,1,1,fp);
	* tmp1 = mmap[19];
	* tmp2 = mmap[20];

	// Use a for loop if necessary
	retVal = *tmp1+((*tmp2)<<8);
	//retVal = (mmap[19] << 0) | (mmap[20] << 8);
	free(tmp1);
	free(tmp2);
	return retVal;
};

double getFilesSize(char * mmap){
	int Entries;
	int start, i;
	double FreeSize=0;
	
	//start = 18 * 512 +1;
	for(i=2; i<2849; i++){				//check every entry
		if(Entry(mmap, i) == 0x000){
			FreeSize += 512;
		}	
	}

	return FreeSize;
}

unsigned int numberofFiles(const unsigned char* mmap, char* label){
	int j = 33* 512;	//the 1st byte for file name
	int current = 0;
	//int i = (14*512)/32;		//numbers of headers for directory
	//int j=0;
	int num=0;
	int i=0;
	
	//printf("i = %d\n", i);

	for(current=19*512; current<j; current+=32){
		//printf("s = %d\n", mmap[current]);
		

		if(mmap[current]!=0xE5 && mmap[current]!=0x00){
			if((mmap[current + 11] & 0x08) || (mmap[current + 11] & 0x10) || (mmap[current +11] &0x0F)){
				
				if(mmap[current+11]&0x08){
					if(mmap[current+11]&0x07)
						continue;
					//printf("IMHERERE\n");
					for(i=0; i<8; i++){
						label[i] = mmap[current+i];
					}
					
				}
				continue;
			}
			
			
			num++;
			
			
		}
		//current += 32;
		//printf("%d debug\n", num);
	}
	
	return num;
}

unsigned int numberofFats(const unsigned char* mmap){
	//printf("%d\n", mmap[16]);
	return(mmap[16]);

}

unsigned int sectorperFAT(const unsigned char* mmap){
	int *tmp1 = malloc(sizeof(int));
	int *tmp2 = malloc(sizeof(int));
	int retVal;
	//fseek(fp,19L,SEEK_SET);
	//fread(tmp1,1,1,fp);
	//fread(tmp2,1,1,fp);
	* tmp1 = mmap[22];
	* tmp2 = mmap[23];
	retVal = *tmp1+((*tmp2)<<8);
	free(tmp1);
	free(tmp2);
	return(retVal);

}




int main(int argc, char** argv)
{
	FILE *fp;

	int fd;
	double filesize;
	struct stat sf;
	char *p;
	char *osname = malloc(sizeof(char)*8);
	char *label = malloc(sizeof(char)*20);
	bzero(label, sizeof(label));
	int size;
	int fileno;

	if ((fd=open(argv[1], O_RDONLY)))
	{
		fstat(fd, &sf);

		// void * mmap(void * addr, size_t length, int prot, 
		//		int flags, int fd, off_t offset);

		p = mmap(NULL,sf.st_size, PROT_READ, MAP_SHARED, fd, 0);

		//printf("Successfully open the image file.\n");
		
		getOSName(osname, p);
		size = getTotalSize(p);
		filesize = getFilesSize(p);	
		fileno = numberofFiles(p, label);
		printf("OS Name: %s\n", osname);
		
		
		printf("Label of the disk: %s\n", label);
		
		//printf("Total Sectors: %d\n", size);
		printf("Total size of the disk: %d bytes\n", size * 512);	

		//get the size of all files occupies	
		printf("Free Size of the disk: %d bytes\n\n", (int)filesize);
		printf("=================\n");
		
		printf("The number of file in the root directory (not including subdirectory): %d\n\n", fileno);
		printf("=================\n");
		printf("NUmber of FAT copies: %d\n", numberofFats(p));  
		printf("Sectors per FAT: %d\n", sectorperFAT(p)); 

	}

	else
		printf("Fail to open the image file.\n");

	free(osname);
	close(fd);
	return 0;
}
