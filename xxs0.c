
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define DBG(msg) printf("%s\n",msg);
#define DBGO(fmt,code...) printf(fmt,code);
#define FOR(n,code...) {int _n=n,i; for(i=0; i<_n; i++) { code; }}

typedef uint8_t xbyte;
typedef uint16_t xword;
typedef uint32_t xlong;

#define xtNULL 1
#define xtCHAR 2
#define xtINT 3
#define xtLIST 4
#define xtNULLF 20
#define xtUNARYF 21
#define xtBINARYF 22

#define MAXNAMES 256
#define NULLID 0
typedef xbyte xid_t;    // an id is a pointer into the scopes names/types/len array - must be big enough to hold MAXNAMES
typedef xlong xname_t; // we store names as hashes
extern uint32_t crc32(uint32_t crc, const void *buf, size_t size);
#define NAME_HASH(str,len) crc32(0,str,len==0?strlen(str):len)
typedef xbyte xtype_t;  // types are presented as integers; how many do we need?
typedef xword xlen_t;
typedef xlong xpool_ptr_t;  // pointer into the pool. should be big enough to point anywhere in your buffer
typedef struct { 
	xname_t name[MAXNAMES];
	xtype_t type[MAXNAMES];
	xlen_t len[MAXNAMES];
	xpool_ptr_t ptr[MAXNAMES];  // where is the actual value? offsets into the pool buffer 
	xid_t maxid;
} xscope_t;
#define WS(w) (w->scope)
#define LEN(id,w) (WS(w).len[id])
#define PTR(id,w) ((xbyte*)&w->pool.buf[w->scope.ptr[id]])
// need to figure out the concept of a "free map"
// maybe this:
#define MAXFREEMAP 256
#define FREEMAPSENTINEL 255
typedef struct {
	xbyte* buf;
	size_t buf_sz;
	xpool_ptr_t free_map[MAXFREEMAP];
	size_t avail_size[MAXFREEMAP];
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

void diaf(const char* msg) {
	printf("%s",msg); exit(1);
}

void init_scope(xscope_t* s) {
	DBG("is");
	memset(s, 0, sizeof(xscope_t));
}

void init_pool(xpool_t* p, xbyte* buf, size_t buf_sz) {
	DBG("ip");
	memset(p, 0, sizeof(xpool_t));
	p->buf = buf;
	p->buf_sz = buf_sz;
	memset(p->free_map, FREEMAPSENTINEL, MAXFREEMAP);
	p->free_map[0] = 0;
	p->avail_size[0] = buf_sz;
}

xid_t set(char* name, size_t namelen, xtype_t valtype, xbyte* value, xlen_t valsz, xsys_t* world) {
	DBG("set");
	if (value != NULL && valsz == 0) valsz = strlen(value); // cstrings
	int alloc_size = (valsz < 4 ? 4 : valsz), i;
	// now we have to find free space using the freemap
	size_t* avails = world->pool.avail_size;
	xpool_ptr_t* freemap = world->pool.free_map;
	xpool_ptr_t newptr = -1;
	for (i=0; i<MAXFREEMAP; i++) {
		if(avails[i] >= alloc_size) {
			newptr = freemap[i];
			avails[i] -= alloc_size;
			freemap[i] += alloc_size;
			break;
		}
	}
	if (newptr==-1) diaf("pool full");
	xscope_t* scope = &(world->scope);
	xid_t id = scope->maxid++;
	if ((int)id >= MAXNAMES-1) diaf("names table full");
	xname_t nh = NAME_HASH(name, namelen);
	// todo: cuckoo hash on collisions
	scope->name[id] = nh;
	scope->type[id] = valtype;
	scope->len[id] = valsz;
	scope->ptr[id] = newptr;
	xbyte* ptr=&world->pool.buf[newptr];
	printf("set ptr %u vs %d\n", ptr, valsz);
	if(value == NULL) memset(ptr,0,valsz);
	else memcpy(ptr, value, valsz);
	return id;
}

xid_t get(char* name, size_t namelen, xsys_t* world) {
	// return the id if its in NAMES
	DBG("get"); DBG(name);
	xname_t nh = NAME_HASH(name, namelen);
	xname_t* n = world->scope.name;
	FOR(world->scope.maxid, {if (n[i] == nh) { return i; }});
	return NULLID;
}

xbyte* ptr(const xid_t id, const xsys_t* world) {
	// TODO blah blah immediate value-type'd id blah blah
	return PTR(id, world);
}

xid_t interp(const char* code, size_t codelen, int start, xsys_t* world) {
	// parse states
	// 0 start
	// 1 comment
	// 2 quote
	// 3 number
	// 4 fractional part
	// 5 identifier
	xid_t states = set("states", 0, xtCHAR, "0cqnfi", 0, world); 
	xid_t transitions = set("transitions", 0, xtINT, NULL, LEN(states,world)*('~'-' '), world);
	return 0;
}

#define POOLSZ 1024*256
int main(void) {
	xbyte buf[POOLSZ];
	xpool_t p;
	init_pool(&p, buf, POOLSZ);
	xscope_t scope;
	init_scope(&scope);
	xsys_t world;
	world.scope = scope;
	world.pool = p;
	set("null", 4, xtNULL, "(null)", 0, &world);
	set("self", 4, xtCHAR, "hello", 5, &world);
	set("x", 1, xtCHAR, "xxs", 0, &world);
	xid_t id;
	id = get("self", 4, &world);
	printf("%d\n", id);
	id = get("x", 1, &world);
	printf("%d\n", id);
	char* pp = ptr(id, &world);
	printf("%u\n", pp);
	printf("%s\n", (char*)ptr(id, &world));
	return 0;
}

// IDEA: knit for ragged data structures (for n items, maintain a separate length index, pretty easy)
// IDEA: apter trees

