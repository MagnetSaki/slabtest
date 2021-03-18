#include "slab.h"

uint64_t nextSerial;
uint64_t NextSerial(){
    nextSerial++;
    return nextSerial;
}

void SlabObj::Reset(SlabArray *_parent, int _KVSize){
    parent = _parent;
    KVSize = _KVSize;
    readCnt = 0;
    KVs.clear();
    freeList.clear();
    for(int i = 0; i < capacity; i += KVSize){
        freeList.push_back(i);
    }
}
bool SlabObj::FullObj(){
    return freeList.size() == 0;
}
SLAB_RETURN SlabObj::InsertObj(const string &key, const string &val, int &offset){
    //if(FullObj()) return SLAB_OBJ_FULL;
    //if(KVs.find(key) != KVs.end()) return SLAB_KV_EXIST;
    offset = *freeList.begin();
    freeList.pop_front();
    KVs[key] = pair<string, int>(val, offset);
    if(FullObj()){ // turn into full list
        auto it = parent->partial.begin();
        parent->full.splice(parent->full.begin(), parent->partial, it);
        parent->map[key] = parent->full.begin();
    }
    else{
        parent->map[key] = parent->partial.begin();
    }
    return SLAB_OK;
}
SLAB_RETURN SlabObj::GetObj(const string &key, string &val, int &offset){
    assert(KVs.find(key) != KVs.end()); // assert key exist
    val = KVs[key].first;
    offset = KVs[key].second;
    readCnt++;
    // to do
    return SLAB_OK;
}

void SlabArray::Init(int kvSize){
    slabKVSize = kvSize;
    buf.Reset(this, kvSize);
}
SLAB_RETURN SlabArray::InsertArray(const string &key, const string &val, int &offset){
    if(partial.size() == 0){
        SlabObj newSlab(NextSerial(), slabKVSize, this); // new a slab to insert, and add this slab to partial list
        partial.push_front(newSlab);
    }
    return partial.begin()->InsertObj(key, val, offset); // insert kv into partial's first slab 
}
SLAB_RETURN SlabArray::GetArray(const string &key, string &val, int &offset){
    assert(map.find(key) != map.end());
    return map[key]->GetObj(key, val, offset);
}

SLAB_RETURN SlabCache::InsertCache(const string &key, const string &val, int &offset, const int &kvSize){
    SlabArray *arr = &sArray[OffCount(kvSize)];
    return arr->InsertArray(key, val, offset);
}
SLAB_RETURN SlabCache::GetCache(const string &key, string &val, int &offset, const int &kvSize){
    SlabArray *arr = &sArray[OffCount(kvSize)];
    return arr->GetArray(key, val, offset);
}
