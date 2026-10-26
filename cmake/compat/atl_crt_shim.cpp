// ==============================================================================
//  atl_crt_shim.cpp
//
//  The prebuilt nvDXTlib.lib (texture compression, VC6 era) references
//  ATL::_AtlInitializeCriticalSectionEx, which modern ATL only provides through
//  a different runtime library.  This small shim supplies the symbol so the
//  renderer DLLs can be linked.
// ==============================================================================

#include <windows.h>

namespace ATL
{
	int __cdecl _AtlInitializeCriticalSectionEx(LPCRITICAL_SECTION pcs, DWORD dwSpinCount, DWORD dwFlags)
	{
		if (!pcs)
			return 0;
		// InitializeCriticalSectionEx() needs _WIN32_WINNT >= 0x0600, but only
		// the plain initialisation is needed by the libraries using this shim.
		InitializeCriticalSection(pcs);
		return 1;
	}
}
