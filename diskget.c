#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/mman.h>  //mmap
#include<fcntl.h>  //open
#include<string.h>

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

unsigned int Entry(const unsigned char* p, int n){		//get FAT table
	int current = 512;
	if  (n % 2 != 0){
		current += (int)((double)n * 1.5 +0.5);
		return (((p[current - 1]) &0xF0)>>4) + (p[current] << 4);

	}else{
		current += (int)((double)n * 1.5);
		return ((p[current]) + ((p[current + 1]&0x0F)<<8));

	}

}


int getFile(unsigned char* mmap, char* Naked, char* Extension){
	int current = 0;
	int j = 33*512;
	int i=0;
	int k=0;
	char *diskFile = malloc(sizeof(char)*8);
	char *diskExtension = malloc(sizeof(char)*3);
	char *totalName = malloc(sizeof(char)*12);
	unsigned int firstCluster=0;
	unsigned int physicalCluster = 0;
	unsigned int FATvalue = 0;
	unsigned int FileSize=0;
	int EOC = 1;
	int cc=0;

	for(current=19*512; current<j; current+=32){
		if(mmap[current]!=0xE5 && mmap[current]!=0x00){
			if((mmap[current + 11] & 0x08) || (mmap[current +11] &0x0F) || (mmap[current+11] &0x10))
				continue;

			for(i=0; i<8; i++){				//get naked file name
					
				diskFile[i] = mmap[current+i];
			}

			for(i=0; i<3; i++){				//get extension 
					
				diskExtension[i] = mmap[current+8+i];
			}

			if(!(strncmp(Naked, diskFile, 8)) && !(strncmp(Extension, diskExtension, 3))){		//find the correct file 

				//printf("%s.%s\n", diskFile, diskExtension);
				strncat(diskFile, ".", 1);
				strncat(diskFile, diskExtension, 3);
				//printf("%s\n", diskFile);
					//get file size here
				FileSize = mmap[current+28] + (mmap[current +29]<<8) + (mmap[current+30]<<16) + (mmap[current+31]<<24);

				FILE *ffp = fopen(diskFile, "wb");
    	    	
    	    	if (!ffp) {
    	    	   	perror("Error: fopen malfunction!\n");
    	    	    	exit(EXIT_FAILURE);
                }

                firstCluster = mmap[current+26] + (mmap[current+27]<<8);
                while(EOC){
                	
                	
                	physicalCluster = 33+firstCluster-2;
                	firstCluster = Entry(mmap, firstCluster);

                	if(firstCluster<0x001 || firstCluster>=0xFF0)
                		EOC=0;


                	for(k=0; k<((FileSize>512)?512:FileSize) ; k++){
                		fputc(mmap[512*physicalCluster + k], ffp);
                	}
                	//printf("BUGBUGBUGBUGBGU\n");
                	FileSize-=512;
                }
                       

                //printf("%d  FIRSTCluster\n", firstCluster);
				
                fclose(ffp);
                return 1;
			}

			//compare the input and the disk file name

			//printf("%s disk\n", diskExtension);
		}
	}
	free(diskFile);
	printf("File Not Found\n");
	exit(EXIT_FAILURE);

}


int main(int argc, char** argv)
{
	FILE *fp;

	int fd;
	struct stat sf;
	char *p;
	char *osname = malloc(sizeof(char)*8);
	int size;
	char* Filename;
	char Naked[8];
	char Extension[3];
	int i=0;
	int j=0;

	if(argc!=3){
		printf("Incorrect Input\n");
		exit(EXIT_FAILURE);
	}
		

	if ((fd=open(argv[1], O_RDONLY)))
	{
		fstat(fd, &sf);

		// void * mmap(void * addr, size_t length, int prot, 
		//		int flags, int fd, off_t offset);

		p = mmap(NULL,sf.st_size, PROT_READ, MAP_SHARED, fd, 0);

		printf("Successfully open the image file.\n");
		
		getOSName(osname, p);

		Filename = argv[2];
		while(Filename[i] != '.'){
			Naked[i] = Filename[i];
			i++;
			//printf("%d\n",i );
			
		}

		if((8-i)>0){
			for(j=0; j<(8-i); j++){
				//printf("%s DEBUG\n", Naked);
				strcat(Naked, " ");		
			}
		}
		//printf("HERERERERERERER\n");
		for(j=0; j<3; j++){
			Extension[j] = Filename[i+1];
			i++;
			//printf("%s\n", Extension);
		}
		//printf("INPUT %s.", Naked);
		//printf("%s\n", Extension);

		getFile(p, Naked, Extension);


		
		
			
	}

	else
		printf("Fail to open the image file.\n");

	free(osname);
	close(fd);
	return 0;
}
