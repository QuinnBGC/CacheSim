#include "Cache.h"
#include "CacheController.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

Cache::Cache(CacheInfo cacheInfo) {
    Block block;

    block.tag = 0;
    block.dirtyBit = 0;
    block.validBit = 0;

    std::vector<std::vector<Block> > blocks(cacheInfo.numberSets, std::vector<Block> (cacheInfo.associativity, block));
    this->blocks = blocks;
};
