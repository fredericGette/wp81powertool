#pragma once

// See https://github.com/tandasat/SecRuntimeSample/blob/master/SecRuntimeSampleNative/Win32Api.h

// In case of error "error LNK2019: unresolved external symbol __imp_SetConsoleCtrlHandler referenced in function "
// see https://stackoverflow.com/a/19455274
extern "C" {
	WINBASEAPI HMODULE WINAPI GetModuleHandleW(LPCWSTR lpModuleName);
	WINBASEAPI HMODULE WINAPI LoadLibraryA(LPCSTR lpLibFileName);
}

#define WIN32API_TOSTRING(x) #x

// Link exported function
#define WIN32API_INIT_PROC(Module, Name)  \
  Name(reinterpret_cast<decltype(&::Name)>( \
      ::GetProcAddress((Module), WIN32API_TOSTRING(Name))))

// Convenientmacro to declare function
#define WIN32API_DEFINE_PROC(Name) const decltype(&::Name) Name

class Win32Api {

private:
	// Returns a base address of KernelBase.dll
	static HMODULE GetKernelBase() {
		return GetBaseAddress(&::DisableThreadLibraryCalls);
	}

	// Returns a base address of the given address
	static HMODULE GetBaseAddress(const void *Address) {
		MEMORY_BASIC_INFORMATION mbi = {};
		if (!::VirtualQuery(Address, &mbi, sizeof(mbi))) {
			return nullptr;
		}
		const auto mz = *reinterpret_cast<WORD *>(mbi.AllocationBase);
		if (mz != IMAGE_DOS_SIGNATURE) {
			return nullptr;
		}
		return reinterpret_cast<HMODULE>(mbi.AllocationBase);
	}

public:
	const HMODULE m_Kernelbase;
	WIN32API_DEFINE_PROC(GetModuleHandleW);
	const HMODULE m_KERNEL32LEGACY;
	WIN32API_DEFINE_PROC(LoadLibraryA);

	Win32Api()
		: m_Kernelbase(GetKernelBase()),
		WIN32API_INIT_PROC(m_Kernelbase, GetModuleHandleW),
		m_KERNEL32LEGACY(GetModuleHandleW(L"KERNEL32LEGACY.DLL")),
		WIN32API_INIT_PROC(m_KERNEL32LEGACY, LoadLibraryA)
	{};

};