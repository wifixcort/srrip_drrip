#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <assert.h>

struct cacheBlock{
	int tag;
	bool valid;
	bool dirtyBit;
	int replacement;
};

//Funciones genéricas
bool isPowerOf2(int x);
bool isHit(struct cacheBlock tags[], int index, int tag, int assoc, int *way);

//Funciones para la cache LRU
void updateLRU(struct cacheBlock tags[], int index, int way, int assoc);
int getVictimLRU(struct cacheBlock tags[], int index, int assoc);

//Funciones para la cache LFU
void updateLFU(struct cacheBlock tags[], int index, int way, int assoc);
int getVictimLFU(struct cacheBlock tags[], int index, int assoc);

//Funciones para la cache LIFO
void updateLIFO(struct cacheBlock tags[], int index, int way, int assoc);
int getVictimLIFO(struct cacheBlock tags[], int index, int assoc);

//Funciones para la cache FIFO
void updateFIFO(struct cacheBlock tags[], int index, int way, int assoc);
int getVictimFIFO(struct cacheBlock tags[], int index, int assoc);

int main(int argc, char** argv)
{
	int cacheSize;
	int assoc;
	int blockSize;
	std::string addressesFile;
	int numTags;
	int numSets;
	int offsetBits;
	int indexBits;
	int indexMask;

    if(argc==5)
    {
        cacheSize=std::stoi(argv[1]);
    	assert(isPowerOf2(cacheSize)&&"cacheSize must be power of 2");

        assoc=std::stoi(argv[2]);
    	assert(isPowerOf2(assoc)&&"assoc must be power of 2");
    	assert(cacheSize>assoc&&"cacheSize must be greater than assoc");

        blockSize=std::stoi(argv[3]);
    	assert(isPowerOf2(blockSize)&&"blockSize must be power of 2");
    	assert(cacheSize>blockSize&&"cacheSize must be greater than blockSize");

        addressesFile.append(argv[4]);

        //std::cout<<cacheSize<<assoc<<blockSize<<addressesFile<<std::endl;
    }
    else{
    	std::cout<<"usage: ./cache cache_size Assoc Block_size Accesses_file"<<std::endl;
    	return -1;
    }

    numTags=cacheSize/blockSize;
    numSets=numTags/assoc;
    offsetBits=log2(blockSize);
    indexBits=log2(numSets);
    indexMask=numSets-1;

	std::cout<<"Simulando cache de "<<cacheSize<<" bytes, "<<numTags<<" bloques, "<<numSets<<" conjuntos, "<<assoc<<" vías, "<<offsetBits<<" bits de offset y "<<indexBits<<" bits de index"<<std::endl;

	struct cacheBlock *tagsLRU=new cacheBlock[numTags];
	struct cacheBlock *tagsLFU=new cacheBlock[numTags];
	struct cacheBlock *tagsLIFO=new cacheBlock[numTags];
	struct cacheBlock *tagsFIFO=new cacheBlock[numTags];

	for(int i=0; i<numSets; i++){
//		setsLRU[i]=0x00010203;
		for(int j=0;j<assoc;j++){
			tagsLRU[(i*assoc)+j].valid=0;
			tagsLRU[(i*assoc)+j].tag=0;
			tagsLRU[(i*assoc)+j].dirtyBit=0;
			tagsLRU[(i*assoc)+j].replacement=j;

			tagsLFU[(i*assoc)+j].valid=0;
			tagsLFU[(i*assoc)+j].tag=0;
			tagsLFU[(i*assoc)+j].dirtyBit=0;
			tagsLFU[(i*assoc)+j].replacement=0;

			tagsLIFO[(i*assoc)+j].valid=0;
			tagsLIFO[(i*assoc)+j].tag=0;
			tagsLIFO[(i*assoc)+j].dirtyBit=0;
			tagsLIFO[(i*assoc)+j].replacement=0;

			tagsFIFO[(i*assoc)+j].valid=0;
			tagsFIFO[(i*assoc)+j].tag=0;
			tagsFIFO[(i*assoc)+j].dirtyBit=0;
			tagsFIFO[(i*assoc)+j].replacement=0;
		}
	}
	std::cout<<"Inicializados los array"<<std::endl;

	int accesses=0;
	int missesLRU=0;
	int missesLFU=0;
	int missesLIFO=0;
	int missesFIFO=0;


    //Create an input file stream
    std::ifstream in(addressesFile,std::ios::in);

    /*
     As long as we haven't reached the end of the file, keep reading entries.
    */
    int address;  //Variable to hold each number as it is read
            //Read number using the extraction (>>) operator
    while (in >> address) {
		//int myAccess = accesses[acc];
		int way;
		accesses++;

		int index = (address>>offsetBits)&indexMask;
		int tag = (address>>(indexBits+offsetBits));

//Actualizo la cache LRU
		if(isHit(tagsLRU, index, tag, assoc, &way)==false)
		{
			way = getVictimLRU(tagsLRU, index, assoc);
			tagsLRU[(index*assoc)+way].tag=tag;
			tagsLRU[(index*assoc)+way].valid=true;
			missesLRU+=1;
		}
		updateLRU(tagsLRU, index, way, assoc);

//Actualizo la LFU
		if(isHit(tagsLFU, index, tag, assoc, &way)==false)
		{
			way = getVictimLFU(tagsLFU, index, assoc);
			tagsLFU[(index*assoc)+way].tag=tag;
			tagsLFU[(index*assoc)+way].valid=true;
			missesLFU+=1;
		}
		updateLFU(tagsLFU, index, way, assoc);

//Actualizo la LIFO
		if(isHit(tagsLIFO, index, tag, assoc, &way)==false)
		{
			way = getVictimLIFO(tagsLIFO, index, assoc);
			tagsLIFO[(index*assoc)+way].tag=tag;
			tagsLIFO[(index*assoc)+way].valid=true;
			missesLIFO+=1;
		}
		updateLIFO(tagsLIFO, index, way, assoc);

//Actualizo la FIFO
		if(isHit(tagsFIFO, index, tag, assoc, &way)==false)
		{
			way = getVictimFIFO(tagsFIFO, index, assoc);
			tagsFIFO[(index*assoc)+way].tag=tag;
			tagsFIFO[(index*assoc)+way].valid=true;
			missesFIFO+=1;
			updateFIFO(tagsFIFO, index, way, assoc);
		}
    }
    //Close the file stream
    in.close();

	std::cout<<"Resultados LRU ideal\n"<<"Accesses= "<<accesses<<" Misses= "<<missesLRU<<" Miss rate= "<<(float)missesLRU/accesses<<std::endl;
	std::cout<<"Resultados LFU\n"<<"Accesses= "<<accesses<<" Misses= "<<missesLFU<<" Miss rate= "<<(float)missesLFU/accesses<<std::endl;
	std::cout<<"Resultados LIFO\n"<<"Accesses= "<<accesses<<" Misses= "<<missesLIFO<<" Miss rate= "<<(float)missesLIFO/accesses<<std::endl;
	std::cout<<"Resultados FIFO\n"<<"Accesses= "<<accesses<<" Misses= "<<missesFIFO<<" Miss rate= "<<(float)missesFIFO/accesses<<std::endl;	

	delete [] tagsLRU;
	delete [] tagsLFU;
	delete [] tagsLIFO;
	delete [] tagsFIFO;

	return true;
}

bool isHit(struct cacheBlock tags[], int index, int tag, int assoc, int *way){
	for(int i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].tag==tag&&tags[(index*assoc)+i].valid==true)
		{
			*way=i;
			return(true);
		}
	}
	return(false);
}

bool isPowerOf2(int x)
{
	return !(x == 0) && !(x & (x - 1));
}

//LRU
void updateLRU(struct cacheBlock tags[], int index, int way, int assoc){
	for(int i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].replacement<(assoc-1))
			tags[(index*assoc)+i].replacement+=1;
	}
	tags[(index*assoc)+way].replacement=0;
}

int getVictimLRU(struct cacheBlock tags[], int index, int assoc){
	//check empty way
	int i;
	for(i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].valid==false)
		{
			return i;
		}
	}
	//look for the victim way
	for(i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].replacement==(assoc-1))
			return i;
	}
	return i;
}


//LFU
void updateLFU(struct cacheBlock tags[], int index, int way, int assoc){
	tags[(index*assoc)+way].replacement+=1;
}

int getVictimLFU(struct cacheBlock tags[], int index, int assoc){
	//check empty way
	int i;
	for(i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].valid==false)
		{
			return i;
		}
	}
	//look for the victim way

	return 0;
}

//LIFO
void updateLIFO(struct cacheBlock tags[], int index, int way, int assoc){
	int i;
	for(i=0; i<assoc;i++)
	{
		tags[(index*assoc)+i].replacement=0;
	}
	tags[(index*assoc)+way].replacement=1;
}

int getVictimLIFO(struct cacheBlock tags[], int index, int assoc){
	//check empty way
	int i;
	for(i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].valid==false)
		{
			return i;
		}
	}
	//look for the victim way
	for(i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].replacement==1)
			return i;
	}
	return i;
}

//FIFO
void updateFIFO(struct cacheBlock tags[], int index, int way, int assoc){
	int i;
	int greater=tags[(index*assoc)].replacement;
	for(i=1; i<assoc;i++)
	{
		if (greater<tags[(index*assoc)+i].replacement)
			greater=tags[(index*assoc)+i].replacement;
	}
	tags[(index*assoc)+way].replacement=greater+1;
}

int getVictimFIFO(struct cacheBlock tags[], int index, int assoc){
	//check empty way
	int i;
	for(i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].valid==false)
		{
			return i;
		}
	}
	//look for the victim way
	int lower=0;
	for(i=1; i<assoc;i++)
	{
		if(tags[(index*assoc)+lower].replacement>tags[(index*assoc)+i].replacement)
			lower=i;
	}
	return lower;
}
