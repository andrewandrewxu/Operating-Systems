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

void writeFAT( unsigned char* p, int i, int j){	//i is value to store
																// j is index of FAT to be stored
	int current = 512;
	if  (j % 2 != 0){
		current += (int)((double)j * 1.5 +0.5);
		p[current-1] = (char)(((i<<4)&0xF0) | (p[current-1] & 0x0F));
		p[current] = (char)((i>>4)&0xFF);


		//return (((p[current - 1]) &0xF0)>>4) + (p[current] << 4);

	}else{
		current += (int)((double)j * 1.5);

		p[current] = (char)(i&0xFF);
		p[current+1] = (char)((p[current+1]&0xF0) | (i>>8));
		//return ((p[current]) + ((p[current + 1]&0x0F)<<8));

	}
	//printf("hereereFAT\n" );

}

void writeEOC( unsigned char* p, int j){		//get FAT table
	int current = 512;
	if  (j % 2 != 0){
		current += (int)((double)j * 1.5 +0.5);
		p[current-1] = ((0xF0) | (p[current-1] & 0x0F));
		p[current] = (0xFF);


		//return (((p[current - 1]) &0xF0)>>4) + (p[current] << 4);

	}else{
		current += (int)((double)j * 1.5);

		p[current] = (0xFF);
		p[current+1] = ((p[current+1]&0xF0) | (0x0F));
		//return ((p[current]) + ((p[current + 1]&0x0F)<<8));

	}
	//printf("hereereFAT\n" );

}

int AssignFirst(unsigned char* mmap, int Firstone, int FIleSize, char* Naked, char* Extension){
	int current=0;
	int j=33*512;
	int i=0;
	int fs=0;

	//printf("%d\n", FIleSize);
	for(current=19*512; current<j; current+=32){
		if(mmap[current]==0x00){
			mmap[current+26]=(Firstone&0xFF);		//update the first cluster
			mmap[current+27]=((Firstone>>8)&0xFF);

			mmap[current+28]=(FIleSize&0xFF);		//update File Size
			mmap[current+29]=((FIleSize>>8)&0xFF);
			mmap[current+30]=((FIleSize>>16)&0xFF);
			mmap[current+31]=((FIleSize>>24)&0xFF);
			

			fs = mmap[current+28] + (mmap[current +29]<<8) + (mmap[current+30]<<16) + (mmap[current+31]<<24);
			printf("%d\n", fs);
			for(i=0; i<8; i++){
				mmap[current+i] = Naked[i];			// update file name
			}
			for(i=0; i<3; i++){
				mmap[current+8+i] = Extension[i];	//update Extension
			}
	
			return;
		}
	}

}


int WriteDisk(unsigned char* mmap, char* Filename){
	int current = 1*512;
	int i=0;
	unsigned int freeFAT=0;
	unsigned int PhysicalSector=0;
	long LeftSize=0;	
	int k=0;			
	int j=0;
	int FirstLOgical=1;
	char Naked[8];
	char Extension[3];


	//printf("%lu\n", LeftSize);
	FILE *ffp = fopen(Filename, "r");		//open file
	//printf("%lu\n", LeftSize);
	fseek(ffp, 0, SEEK_END);				//get file size here
	LeftSize = ftell(ffp);
	fseek(ffp, 0, SEEK_SET);

	//printf("%lu\n", LeftSize);
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





	for(i=2; i<2849; i++){
		freeFAT = Entry(mmap, i);
		if( freeFAT == 0x00){
			if(FirstLOgical==1){				//the first one 
				
				AssignFirst(mmap, i, LeftSize, Naked, Extension);

				//mmap[19*512]=(i&0xFF)
				FirstLOgical=0;
				
			}

				if(LeftSize>0){
					writeFAT(mmap, i, j); 	//update update table

				}
				else{
					writeEOC(mmap, i);			//reach the end of file, and update EOC
					//printf("%lu\n",LeftSize );
					fclose(ffp);
					return;
				}

				PhysicalSector = i+33-2;					//get disk sector here
				//printf("%lu\n",LeftSize );
				for(k=0; k<((LeftSize>512)?512:LeftSize); k++){			//read byte in
					//printf("%d\n",LeftSize );
					mmap[PhysicalSector*512 + k] = fgetc(ffp);
					//printf("%lu\n",LeftSize );
					//printf("%c\n", mmap[PhysicalSector*512 +k]);
				}

				LeftSize-=512;

				j=i;				//store the previous FAT index into j
			



		}
	}
	if(LeftSize>0){
		//printf("%d\n",LeftSize );
		printf("NOt enough free space\n");
		exit(EXIT_FAILURE);

	}


	fclose(ffp);
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

	if ((fd=open(argv[1], O_RDWR)))
	{
		fstat(fd, &sf);

		// void * mmap(void * addr, size_t length, int prot, 
		//		int flags, int fd, off_t offset);

		p = mmap(NULL,sf.st_size, PROT_WRITE, MAP_SHARED, fd, 0);

		printf("Successfully open the image file.\n");
		
		Filename = argv[2];

		WriteDisk(p, Filename);
		
		
	}

	else
		printf("Fail to open the image file.\n");

	free(osname);
	close(fd);
	return 0;
}
