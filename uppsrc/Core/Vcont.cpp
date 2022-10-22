#include "Core.h"

#define LLOG(x)

namespace Upp {

int64 NewInVectorSerial()
{
	static int64 x;
	INTERLOCKED {
		++x;
	}
	return x;
}


thread_local int64 invector_cache_serial_;
thread_local int   invector_cache_blki_;
thread_local int   invector_cache_offset_;
thread_local int   invector_cache_end_;

void SetInvectorCache__(int64 serial, int blki, int offset, int end)
{
	invector_cache_serial_ = serial;
	invector_cache_blki_ = blki;
	invector_cache_offset_ = offset;
	invector_cache_end_ = end;
}

void ClearInvectorCache__()
{
	invector_cache_serial_ = 0;
}

int FindInvectorCache__(int64 serial, int& pos, int& off)
{
	if(invector_cache_serial_ == serial && pos >= invector_cache_offset_ &&
	   pos < invector_cache_end_) {
		LLOG("Found in cache, serial: " << invector_cache_serial_ << ", offset: " << invector_cache_offset_ << ", end: " << invector_cache_end_);
		off = invector_cache_offset_;
		pos -= off;
		return invector_cache_blki_;
	}
	return -1;
}

void Bits::Clear()
{
	if(bp)
		MemoryFree(bp);
	alloc = 0;
	bp = NULL;
}

void Bits::Realloc(int nalloc)
{
	size_t sz = sizeof(dword) * nalloc;
	dword *nbp = (dword *)MemoryAllocSz(sz);
	nalloc = int(sz / sizeof(dword));
	if(bp) {
		Copy(nbp, bp, bp + min(alloc, nalloc));
		MemoryFree(bp);
	}
	if(nalloc > alloc)
		Fill(nbp + alloc, nbp + nalloc, (dword)0);
	bp = nbp;
	alloc = nalloc;
}

void Bits::Expand(int q)
{
	Realloc(3 * q / 2 + 1);
}

void Bits::Reserve(int nbits)
{
	int n = (nbits + 31) >> 5;
	if(n > alloc)
		Realloc(n);
}

int Bits::GetLast() const
{
	int lasti = alloc - 1;
	while(lasti > 0 && bp[lasti] == 0)
		lasti--;
	return lasti;
}

void Bits::Shrink()
{
	int nalloc = GetLast() + 1;
	if(nalloc != alloc)
		Realloc(nalloc);
}

dword *Bits::CreateRaw(int n_dwords)
{
	
	Clear();
	Realloc(n_dwords);
	return bp;
}

String Bits::ToString() const
{
	StringBuffer ss;
	for(int i = GetLast(); i >= 0; i--)
		ss << FormatIntHex(bp[i]);
	return String(ss);
}

void Bits::Serialize(Stream& s)
{
	int dwords;
	if(s.IsStoring())
		dwords = GetLast() + 1;
	s % dwords;
	if(dwords < 0)
		s.LoadError();
	if(s.IsLoading())
		CreateRaw(dwords);
	s.SerializeRaw(bp, dwords);
}

void * DoRawAlloc(int& n, size_t szT)
{
	size_t sz0 = n * szT;
	size_t sz = sz0;
	void *q = MemoryAllocSz(sz);
	n += (int)((sz - sz0) / szT);
	return q;
}

//template <class T>
//bool DoReAlloc(int newalloc, void *& vector,
//	int items, size_t szT, void (*cpy)(void *, const void*, size_t))
bool DoReAlloc(int newalloc, Vector<void*>& vec,
	size_t szT,void (*cpy)(void *, const void *, size_t))
{
	ASSERT(newalloc >= vec.items);
	size_t sz0 = (size_t)newalloc * szT;
	size_t sz = sz0;
	void *newvector;
	bool  alloced = true;
	if(MemoryTryRealloc(vec.vector, sz)) {
		newvector = vec.vector;
		vec.vector = NULL;
		alloced = false;
	}
	else
		newvector = newalloc ? MemoryAllocSz(sz) : NULL;
	vec.alloc = newalloc == INT_MAX ? INT_MAX // maximum alloc reached
	        : (int)((sz - sz0) / szT + newalloc); // adjust alloc to real memory size
	if(vec.vector && newvector)
		cpy(newvector, vec.vector, vec.items);
	vec.vector = (void**)newvector;
	return alloced;
}


}
