#ifndef _CACHE_H
#define _CACHE_H

#include "CacheStuff.h"
#include "CacheController.h"
#include <vector>

class Cache {
    private:
        std::vector<std::vector<Block> > blocks;
        ReplacementPolicy rp;
	    WritePolicy wp;
    public:
        Cache(CacheInfo);
};

#endif //CACHEH