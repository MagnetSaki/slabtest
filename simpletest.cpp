#include "slab.cpp"

int main(){
    SlabCache cache;
    string key, val;
    int offset;
    int testnum;
    cout<<"Input testnum : ";
    cin>>testnum;
    for(int i = 0; i < testnum; i++){
        key = to_string(i);
        val = to_string(i);
        cache.InsertCache(key, val, offset, 64);
    }
    for(int i = 0; i < testnum; i++){
        key = to_string(i);
        cache.GetCache(key, val, offset, 64);
        cout<<val<<" ";
    }
    return 0;
}
