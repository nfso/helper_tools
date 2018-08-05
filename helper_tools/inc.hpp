#pragma once

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <tuple>
#include <vector>
#include <memory>
#include <iostream>

#include <Windows.h>
#include <TlHelp32.h>

#include "xor_str.hpp"
#include "tools.hpp"

#pragma comment(lib, "ntdll.lib")

HANDLE h_proc = INVALID_HANDLE_VALUE;