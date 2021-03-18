#include <iostream>
#include <list>
#include <cstring>
#include <string>
#include <cmath>
#include <bitset>
#include <algorithm>
#include <unordered_map>
#include <assert.h>

using namespace std;

enum SLAB_RETURN {
    SLAB_OK,
    SLAB_ERROR,
    SLAB_KV_EXIST,
    SLAB_KV_NOT_EXIST,
    SLAB_OBJ_FULL,
};

struct SlabArray;

typedef struct SlabObj {
    static const int capacity = 4096;
    uint64_t slabNo;
    SlabArray *parent;
    int readCnt;
    int KVSize;
    int ppn; //if this slab is in flash
    list<int> freeList;
    unordered_map<string, pair<string, int>> KVs; // first:key ; second:value and offset in slab

    SlabObj():slabNo(0), KVSize(0), parent(NULL), readCnt(0){}
    SlabObj(uint64_t slabNo, int KVSize, SlabArray *parent):slabNo(slabNo), KVSize(KVSize), parent(parent), readCnt(0) {
        for(int i = 0; i < capacity; i += KVSize){
            freeList.push_back(i);
        }
    }
    ~SlabObj(){
        freeList.clear();
        KVs.clear();
    }
    void Reset(SlabArray *_parent, int _KVSize);
    bool FullObj();
    SLAB_RETURN InsertObj(const string &key, const string &val, int &offset);
    SLAB_RETURN GetObj(const string &key, string &val, int &offset);
} SlabObj;

typedef struct SlabArray {
    int slabKVSize;
    SlabObj buf;
    list<SlabObj> full;
    list<SlabObj> partial;
    unordered_map<string, list<SlabObj>::iterator> map;

    void Init(int kvSize);
    SLAB_RETURN InsertArray(const string &key, const string &val, int &offset);
    SLAB_RETURN GetArray(const string &key, string &val, int &offset);
} SlabArray;

typedef struct SlabCache {
    SlabArray sArray[8];

    static int OffCount(int kvSize){
        switch(kvSize) {
            case 8: return 0;
            case 16: return 1;
            case 32: return 2;
            case 64: return 3;
            case 128: return 4;
            case 256: return 5;
            case 512: return 6;
            case 1024: return 7;
        }
        return -1;
    }

    SlabCache(){
        for(int i = 8; i <= 1024; i *= 2){
            sArray[OffCount(i)].Init(i);
        }
    }

    SLAB_RETURN InsertCache(const string &key, const string &val, int &offset, const int &kvSize);
    SLAB_RETURN GetCache(const string &key, string &val, int &offset, const int &kvSize);
    SLAB_RETURN UpdateCache(string key, string val, int kvSize){}
    SLAB_RETURN DeleteCache(string key, int kvSize){}
} SlabCache;
