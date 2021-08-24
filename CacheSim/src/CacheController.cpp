/*
	Cache Simulator (Starter Code) by Justin Goins
	Oregon State University
	Spring Term 2021
*/

#include "CacheController.h"
#include "Cache.h"
#include "CacheStuff.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <cmath>
#include <queue>

using namespace std;

CacheController::CacheController(CacheInfo ci, string tracefile) {
	// store the configuration info
	this->ci = ci;
	this->inputFile = tracefile;
	this->outputFile = this->inputFile + ".out";
	// compute the other cache parameters
	this->ci.numByteOffsetBits = log2(ci.blockSize);
	this->ci.numSetIndexBits = log2(ci.numberSets);
	// initialize the counters
	this->globalCycles = 0;
	this->globalHits = 0;
	this->globalMisses = 0;
	this->globalEvictions = 0;
	this->globalReads = 0;
	this->globalWrites = 0;
	
	// create your cache structure
	// ...
	int numBlocks = this->ci.numberSets * this->ci.associativity;
	this->max = 999999999;
	static vector<unsigned long int> cache;
	this->cache = &cache;
	for (int i = 0; i < numBlocks; i++){
		cache.push_back(this->max);
	}

	static vector<std::queue<unsigned int> > lruOffset;
	for (unsigned int i = 0; i < this->ci.numberSets; i++){
		lruOffset.push_back(queue<unsigned int>());
		lruOffset[i].push(0);
	}
	this->lruoffset = &lruOffset;
	
	

	// manual test code to see if the cache is behaving properly
	// will need to be changed slightly to match the function prototype
	/*
	cacheAccess(false, 0);
	cacheAccess(false, 128);
	cacheAccess(false, 256);

	cacheAccess(false, 0);
	cacheAccess(false, 128);
	cacheAccess(false, 256);
	*/
}

/*
	Starts reading the tracefile and processing memory operations.
*/
void CacheController::runTracefile() {
	cout << "Input tracefile: " << inputFile << endl;
	cout << "Output file name: " << outputFile << endl;
	
	// process each input line
	string line;
	// define regular expressions that are used to locate commands
	regex commentPattern("==.*");
	regex instructionPattern("I .*");
	regex loadPattern(" (L )(.*)(,)([[:digit:]]+)$");
	regex storePattern(" (S )(.*)(,)([[:digit:]]+)$");
	regex modifyPattern(" (M )(.*)(,)([[:digit:]]+)$");

	// open the output file
	ofstream outfile(outputFile);
	// open the output file
	ifstream infile(inputFile);
	// parse each line of the file and look for commands
	while (getline(infile, line)) {
		// these strings will be used in the file output
		string opString, activityString;
		smatch match; // will eventually hold the hexadecimal address string
		unsigned long int address;
		// create a struct to track cache responses
		CacheResponse response;
		response.hits=0;
		response.misses=0;
		response.evictions=0;
		response.dirtyEvictions=0;
		response.cycles=0;

		// ignore comments
		if (std::regex_match(line, commentPattern) || std::regex_match(line, instructionPattern)) {
			// skip over comments and CPU instructions
			continue;
		} else if (std::regex_match(line, match, loadPattern)) {
			cout << "Found a load op!" << endl;
			istringstream hexStream(match.str(2));
			hexStream >> std::hex >> address;
			outfile << match.str(1) << match.str(2) << match.str(3) << match.str(4);
			cacheAccess(&response, false, address, stoi(match.str(4)));
			logEntry(outfile, &response);
			
		} else if (std::regex_match(line, match, storePattern)) {
			cout << "Found a store op!" << endl;
			istringstream hexStream(match.str(2));
			hexStream >> std::hex >> address;
			outfile << match.str(1) << match.str(2) << match.str(3) << match.str(4);
			cacheAccess(&response, true, address, stoi(match.str(4)));
			logEntry(outfile, &response);
		} else if (std::regex_match(line, match, modifyPattern)) {
			cout << "Found a modify op!" << endl;
			istringstream hexStream(match.str(2));
			// first process the read operation
			hexStream >> std::hex >> address;
			outfile << match.str(1) << match.str(2) << match.str(3) << match.str(4);
			cacheAccess(&response, false, address, stoi(match.str(4)));
			logEntry(outfile, &response);
			outfile << endl;
			// now process the write operation
			hexStream >> std::hex >> address;
			outfile << match.str(1) << match.str(2) << match.str(3) << match.str(4);
			cacheAccess(&response, true, address, stoi(match.str(4)));
			logEntry(outfile, &response);
		} else {
			throw runtime_error("Encountered unknown line format in tracefile.");
		}
		outfile << endl;
	}
	// add the final cache statistics
	outfile << "L1 Cache: ";
	outfile << "Hits: " << globalHits << " Misses: " << globalMisses << " Evictions: " << globalEvictions << endl;
	outfile << "Cycles: " << globalCycles << " Reads: " << globalReads << " Writes: " << globalWrites << endl;

	infile.close();
	outfile.close();
}

/*
	Report the results of a memory access operation.
*/
void CacheController::logEntry(ofstream& outfile, CacheResponse* response) {
	outfile << " " << response->cycles;
	outfile << " L1";
	if (response->hits > 0)
		outfile << " hit";
	if (response->misses > 0)
		outfile << " miss";
	if (response->evictions > 0)
		outfile << " eviction";
}

/*
	Calculate the block index and tag for a specified address.
*/
CacheController::AddressInfo CacheController::getAddressInfo(CacheInfo ci, unsigned long int address) {
	AddressInfo ai;
	// this code should be changed to assign the proper index and tag
	ai.tag = address >> (ci.numByteOffsetBits+ci.numSetIndexBits);
	ai.setIndex = (address >> ci.numByteOffsetBits) & ((1 << ci.numSetIndexBits)-1);
	return ai;
}

/*
	This function allows us to read or write to the cache.
	The read or write is indicated by isWrite.
	address is the initial memory address
	numByte is the number of bytes involved in the access
*/
void CacheController::cacheAccess(CacheResponse* response, bool isWrite, unsigned long int address, int numBytes) {
	// determine the index and tag
	AddressInfo ai = getAddressInfo(ci, address);

	cout << "\tSet index: " << ai.setIndex << ", tag: " << ai.tag << endl;
	
	// your code should also calculate the proper number of cycles that were used for the operation
	
	
	// your code needs to update the global counters that track the number of hits, misses, and evictions
	vector<unsigned int> emptyBlocks;
	vector<unsigned int> occupiedBlocks;
	unsigned int blockNumber = this->ci.associativity * ai.setIndex;
	unsigned int offset = 0;
	unsigned int i;
	response->evictions = 0;
	response->dirtyEvictions = 0;
	response->hits = 0;
	response->misses = 0;
	int c = 0;
	for (i = 0; i < this->ci.associativity; i++)
	{
		if (this->cache->data()[blockNumber + i] == this->max){
			cout << "MISS" << endl;
			emptyBlocks.push_back(i);
			c += 1;
		}
		else if (this->cache->data()[blockNumber + i] == ai.tag){
			cout << "HIT" << endl;
			response->hits++;
			c=0;
			break;
		}
		else {
			cout << "MISS?" << endl;
			occupiedBlocks.push_back(i);
			c += 1;
		}
	}
	//cout << "Out For" << endl;
	if (!response->hits )
	{
		//cout << "no hits" << endl;
		response->misses++;
		if (!emptyBlocks.empty()){
			//cout << "emptyblocks" << endl;
			offset = emptyBlocks.front();
		}
		else if (!occupiedBlocks.empty()){
			//cout << "occupiedblocks" << endl;
			if (this->ci.rp == ReplacementPolicy::Random){
				//cout << "time?" << endl;
				srand(time(NULL));
				cout << rand() << endl;
				int r = rand() % this->ci.associativity;
				//cout << "RANDOM BLOCK OFFSET: " << r << endl;
				offset = r;
			}
			else{
				//cout << "not rp" << endl;
				offset = this->lruoffset->data()[ai.setIndex].front();
			}
			//cout << "evics" << endl;
			if (occupiedBlocks.size() == this->ci.associativity){
				//cout << "OFFSET: " << endl;
				response->evictions = true;
				if (isWrite)
					response->dirtyEvictions = true;
			}
		}

		blockNumber = (blockNumber + offset);
		

		//cout << "cache[" << blockNumber << "] = " << cache->data()[blockNumber] << endl;
		this->cache->data()[blockNumber] = ai.tag;
		//cout << "Stored " << ai.tag << " at cache[" << blockNumber << "]" << endl;
	}
	//cout << "L1" << endl;
	if (this->lruoffset->data()[ai.setIndex].size() == this->ci.associativity){
		this->lruoffset->data()[ai.setIndex].pop();
	}
	//cout << "L2" << endl;
	if (this->lruoffset->data()[ai.setIndex].back() != offset){
		this->lruoffset->data()[ai.setIndex].push(offset);
	}
	

	
	//cout << "cycle start" << endl;

	if (isWrite){
		globalWrites++;
		//cout << "isWrite" << endl;
		if (this->ci.wp == WritePolicy::WriteThrough)
		{
			//cout << "ci.wp" << endl;
			c += this->ci.cacheAccessCycles + this->ci.memoryAccessCycles;
		} else {
			c += this->ci.cacheAccessCycles;
		}
		//cout << "change global" << endl;
		if (response->hits){
			this->globalHits++;
		} else {
			this->globalMisses++;
		}
	} else { 
		globalReads++;
		//cout << "notWrite" << endl;
		//cout << "change global" << endl;
		if (response->hits){
			c += this->ci.cacheAccessCycles;
			this->globalHits++;
		} else {
			c += this->ci.cacheAccessCycles + this->ci.memoryAccessCycles;
			this->globalMisses++;
		}
	}

	if (response->evictions > 0 || response->dirtyEvictions > 0){
		c += this->ci.cacheAccessCycles + this->ci.memoryAccessCycles;
		this->globalEvictions++;
	}

	response->cycles = c;
	this->globalCycles += response->cycles;
	
	

	if (response->hits > 0)
		cout << "Operation at address " << std::hex << address << " caused " << response->hits << " hit(s)." << std::dec << endl;
	if (response->misses > 0)
		cout << "Operation at address " << std::hex << address << " caused " << response->misses << " miss(es)." << std::dec << endl;

	cout << "-----------------------------------------" << endl;

	return;
}
