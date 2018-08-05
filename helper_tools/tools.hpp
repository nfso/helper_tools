#pragma once
#include "inc.hpp"

struct unique_handle_deleter { void operator( )( void * handle ) const noexcept { CloseHandle(handle); } }; using unique_handle = ::std::unique_ptr<void, unique_handle_deleter>;
namespace nt_wpr { extern "C" NTSTATUS NTAPI NtReadVirtualMemory(HANDLE, PVOID, PVOID, SIZE_T, PULONG); extern "C" NTSTATUS NTAPI NtWriteVirtualMemory(HANDLE, PVOID, PVOID, SIZE_T, PULONG); }
namespace mth { template<typename t> static constexpr t pi = reinterpret_cast<t>(3.14159265358979323846); template <typename t> static constexpr t pi_2 = reinterpret_cast<t>(1.57079632679489661923); }

namespace det {
	auto get_pid( const LPCWSTR prcs ) -> ::std::vector<::std::uint64_t> const {
		::std::vector<::std::uint64_t> pids = { NULL };
		unique_handle pid_snap(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL));
		PROCESSENTRY32 proc_entry; proc_entry.dwSize = sizeof( PROCESSENTRY32 );

		if ( pid_snap.get( ) == INVALID_HANDLE_VALUE || GetLastError( ) == ERROR_BAD_LENGTH ) return pids;
		if ( !Process32First( pid_snap.get( ), &proc_entry ) ) return pids;

		do {
			if (!lstrcmpW(proc_entry.szExeFile, prcs)) { pids.emplace_back(proc_entry.th32ProcessID); return pids; }
		} while ( Process32Next( pid_snap.get( ), &proc_entry ) );
		
		return pids;
	}

	auto get_base( ::std::uint64_t prcs_id, LPCWSTR prcs_mod ) -> ::std::uintptr_t const {
		unique_handle mod_snap( CreateToolhelp32Snapshot( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, prcs_id ) );

		if ( mod_snap.get( ) == INVALID_HANDLE_VALUE ) return NULL;

		MODULEENTRY32 mod_entry; mod_entry.dwSize = sizeof(MODULEENTRY32);

		if ( !Module32First( mod_snap.get( ), &mod_entry ) ) return NULL;

		do {
			if ( !lstrcmpW( mod_entry.szModule, prcs_mod )  )
				return reinterpret_cast<::std::uintptr_t>( mod_entry.modBaseAddr );
		} while ( Module32Next( mod_snap.get( ), &mod_entry ) );

		return NULL;
	}

	template<typename t> auto is_valid( t data ) -> bool const {
		return ( data && sizeof(t) ) ? true : false;
	}

	template<typename t> auto read_mem( ::std::uintptr_t ptr ) -> t const { // you can make it a std::tuple<t, NTSTATUS> if you want to return the NTSTATUS of the read (retarded)
		if ( !is_valid<HANDLE>( h_proc ) || !is_valid<::std::uintptr_t>( ptr ) ) return NULL;
		t buf; ::nt_wpr::NtReadVirtualMemory( h_proc, reinterpret_cast<LPVOID>( ptr ), &buf, sizeof( t ), NULL ); return buf;
	}

	template<typename t> auto write_mem( ::std::uintptr_t ptr, t& data ) -> NTSTATUS const {
		if ( !is_valid<HANDLE>( h_proc ) || !is_valid<::std::uintptr_t>( ptr ) ) return STATUS_INVALID_PARAMETER;
		return ::nt_wpr::NtWriteVirtualMemory( h_proc, reinterpret_cast<LPVOID>( ptr ), data, sizeof( t ), NULL );
	}
}