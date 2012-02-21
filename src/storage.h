#ifndef _STORAGE_H
#define _STORAGE_H

class CRegAlloc
{
public:
	enum {
		NOREG = -1,
		VACANT = 0
	};

	CRegAlloc(int size, const int *prealloc = NULL);
	~CRegAlloc();

	int Acquire();
	int Acquire(int addr);
	int AcquireOrThrow();
	int Mirror(int addr);
	bool Release(int addr);
	int RefCount(int addr);

	int FirstAllocated();
	int NextAllocated(int current);

private:
	int m_size;
	int *m_allocTable;
};

#endif	// _STORAGE_H

