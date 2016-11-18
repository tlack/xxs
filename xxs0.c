
#include <stdlib.h>

#define MAXNAMES 256
typedef uint8_t xid_t;    // an id is a pointer into the scopes names/types/len array - must be big enough to hold MAXNAMES
typedef uint32_t xname_t; // we store names as hashes
#define NAME_HASH(str,len) crc32(0,str,len)

typedef uint8_t xtype_t;  // types are presented as integers; how many do we need?
typedef uint16_t xlen_t;
typedef uint16_t xpool_ptr_t;  // pointer into the pool. should be big
typedef struct { 
	xname_t names[MAXNAMES];
	xtype_t types[MAXNAMES];
	xlen_t len[MAXNAMES];
	xpool_ptr_t ptr[MAXNAMES];
	xid_t maxid;
} xscope_t;

// need to figure out the concept of a "free map"
// maybe this:
//

#define MAXFREEMAP 256
#define FREEMAPSENTINEL 255
typedef struct {
	uint8_t* buf;
	size_t buf_sz;
	xpool_ptr_t free_map[MAXFREEMAP] = {FREEMAPSENTINEL};
	size_t avail_size[MAXFREEMAP] = {0};
} xpool_t;

// when we allocate buf..
//
// so free map starts empty - i.e., totally free
// at that time, avail_size[0] = buf_sz
//
// as we allocate, change the start of the free map
// as we deallocate, add items to end of free map
// then have process to occasionally shorten and compress it

typedef struct {
	xscope_t scope;
	xpool_t pool;
} xsys_t;

void init_pool(xpool_t* p, uint8_t* buf, size_t buf_sz) {
	p.buf = buf;
	p.buf_sz = buf_sz;
	memset(p.freemap, FREEMAPSENTINEL, MAXFREEMAP);
}

xid_t set(char* name, size_t namelen, xtype_t valtype, xlen_t valsz, uint8* value, xsys_t world) {
	xscope_t scope = world.scope;
	xid_t id = scope.maxid++;
	xname_t nh = NAME_HASH(name, namelen);
	// todo: cuckoo hash on collisions
	scope.names[id] = nh;
	scope.types[id] = valtype;
	scope.len[id] = valsz;
	
	// now we have to find free space using the freemap
}

xid_t get(char* name, size_t namelen) {
	// return the id if its in NAMES
}

xid_t interp(const char* code, size_t codelen) {
	size_t i = 0;
	while (i < codelen) 
	// scan for space or end of code
	// for each token, 
	// do lame "looks like a name" thing, 
	// - find it with get
	// - do something? lol
	// otherwise,
	// assign to "cur_left_value" somehow
	// uhm..
}

#define POOLSZ 1024*256
int main(void) {
	uint8_t buf[POOLSZ];
	xpool_t p;
	init_pool(p, buf, POOLSZ);
	xscope_t scope;
	xsys_t world;
	world.scope = scope;
	world.pool = p;

	return 0;
}

