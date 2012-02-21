#include <string.h>
#include "utils.h"
#include "storage.h"

CRegAlloc::CRegAlloc(int size, const int *prealloc)
	:m_size(size)
{
	m_allocTable = new int[size];
	memset(m_allocTable, VACANT, (size * sizeof(int)));
	if (prealloc != NULL) {
		int i = 0;
		while (prealloc[i] != NOREG)
			m_allocTable[ prealloc[ i++ ] ] = NOREG;
	}
}

CRegAlloc::~CRegAlloc()
{
	delete m_allocTable;
}

int CRegAlloc::Acquire()
{
	for (int i = 0; i < m_size; ++i) {
		if (m_allocTable[i] == VACANT) {
			m_allocTable[i] = 1;
			return i;
		}
	}

	return NOREG;
}

int CRegAlloc::Acquire(int addr)
{
	BUG_IF((m_allocTable[addr] <= 0), "Attempt to acquire not allocated register: " + addr);
	++m_allocTable[addr];
	return addr;
}

int CRegAlloc::AcquireOrThrow()
{
	int reg = Acquire();
	if (reg == NOREG)
		throw CCompilationException("Well, this is embarrassing. We are out of registers.");

	return reg;
}

/* Used for collecting register usage information by mirroring allocations.
 */
int CRegAlloc::Mirror(int addr)
{
	++m_allocTable[addr];
	return addr;
}

bool CRegAlloc::Release(int addr)
{
	BUG_IF((m_allocTable[addr] <= 0), "Attempt to release not allocated register: " + addr);
	if ( --m_allocTable[addr] == VACANT )
		return true;

	return false;
}

int CRegAlloc::RefCount(int addr)
{
	return m_allocTable[addr];
}

int CRegAlloc::FirstAllocated()
{
	return NextAllocated(NOREG);
}

int CRegAlloc::NextAllocated(int current)
{
	for (int i = current + 1; i < m_size; ++i) {
		if (m_allocTable[i] > 0)
			return i;
	}

	return NOREG;
}

