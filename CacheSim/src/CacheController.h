/*
	Cache Simulator (Starter Code) by Justin Goins
	Oregon State University
	Spring Term 2021
*/

#ifndef _CACHECONTROLLER_H_
#define _CACHECONTROLLER_H_

#include "CacheStuff.h"
#include "Cache.h"
#include <string>
#include <fstream>
#include <queue>

class CacheController {
	private:
		struct AddressInfo {
			unsigned long int tag;
			unsigned int setIndex;
		};
		unsigned int globalCycles;
		unsigned int globalHits;
		unsigned int globalMisses;
		unsigned int globalEvictions;
		unsigned int globalReads;
		unsigned int globalWrites;
		std::string inputFile, outputFile;

		CacheInfo ci;

		std::vector<unsigned long int> *cache;
		unsigned int max;
		std::vector<std::queue<unsigned int> > *lruoffset;

		// function to allow read or write access to the cache
		void cacheAccess(CacheResponse*, bool, unsigned long int, int);
		// function that can compute the index and tag matching a specific address
		AddressInfo getAddressInfo(CacheInfo, unsigned long int);
		// function to add entry into output file
		void logEntry(std::ofstream&, CacheResponse*);
		

	public:
		CacheController(CacheInfo, std::string);
		void runTracefile();
};

#endif //CACHECONTROLLER
