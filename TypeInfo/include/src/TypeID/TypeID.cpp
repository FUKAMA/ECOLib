#include "TypeID.h"

namespace utl
{
	void TypeIDAllocator::Allocate(const char* a_keyName)
	{
		TypeID nID = ++m_counter;
		m_umNameToTypeID[a_keyName] = nID;
	}
}