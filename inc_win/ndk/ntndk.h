/*++ NDK Version: 0098

Copyright (c) Alex Ionescu.  All rights reserved.

Header Name:

    ntndk.h

Abstract:

    Master include file for the Native Development Kit.

Author:

    Alex Ionescu (alexi@tinykrnl.org) - Updated - 27-Feb-2006

--*/

#ifndef _NTNDK_
#define _NTNDK_

// #undef STATUS_WAIT_0
#undef STATUS_ABANDONED_WAIT_0
#undef STATUS_USER_APC
#undef STATUS_TIMEOUT
#undef STATUS_PENDING
#undef DBG_EXCEPTION_HANDLED
#undef DBG_CONTINUE
#undef STATUS_SEGMENT_NOTIFICATION
#undef STATUS_FATAL_APP_EXIT
#undef DBG_REPLY_LATER
#undef DBG_TERMINATE_THREAD
#undef DBG_TERMINATE_PROCESS
#undef DBG_CONTROL_C
#undef DBG_PRINTEXCEPTION_C
#undef DBG_RIPEXCEPTION
#undef DBG_CONTROL_BREAK
#undef DBG_COMMAND_EXCEPTION
#undef DBG_PRINTEXCEPTION_WIDE_C
#undef STATUS_GUARD_PAGE_VIOLATION
#undef STATUS_DATATYPE_MISALIGNMENT
#undef STATUS_BREAKPOINT
#undef STATUS_SINGLE_STEP
#undef STATUS_LONGJUMP
#undef STATUS_UNWIND_CONSOLIDATE
#undef DBG_EXCEPTION_NOT_HANDLED
#undef STATUS_ACCESS_VIOLATION
#undef STATUS_IN_PAGE_ERROR
#undef STATUS_INVALID_HANDLE
#undef STATUS_INVALID_PARAMETER
#undef STATUS_NO_MEMORY
#undef STATUS_ILLEGAL_INSTRUCTION
#undef STATUS_NONCONTINUABLE_EXCEPTION
#undef STATUS_INVALID_DISPOSITION
#undef STATUS_ARRAY_BOUNDS_EXCEEDED
#undef STATUS_FLOAT_DENORMAL_OPERAND
#undef STATUS_FLOAT_DIVIDE_BY_ZERO
#undef STATUS_FLOAT_INEXACT_RESULT
#undef STATUS_FLOAT_INVALID_OPERATION
#undef STATUS_FLOAT_OVERFLOW
#undef STATUS_FLOAT_STACK_CHECK
#undef STATUS_FLOAT_UNDERFLOW
#undef STATUS_INTEGER_DIVIDE_BY_ZERO
#undef STATUS_INTEGER_OVERFLOW
#undef STATUS_PRIVILEGED_INSTRUCTION
#undef STATUS_STACK_OVERFLOW
#undef STATUS_DLL_NOT_FOUND
#undef STATUS_ORDINAL_NOT_FOUND
#undef STATUS_ENTRYPOINT_NOT_FOUND
#undef STATUS_CONTROL_C_EXIT
#undef STATUS_DLL_INIT_FAILED
#undef STATUS_FLOAT_MULTIPLE_FAULTS
#undef STATUS_FLOAT_MULTIPLE_TRAPS
#undef STATUS_REG_NAT_CONSUMPTION
#undef STATUS_HEAP_CORRUPTION
#undef STATUS_STACK_BUFFER_OVERRUN
#undef STATUS_INVALID_CRUNTIME_PARAMETER
#undef STATUS_ASSERTION_FAILURE
#undef STATUS_ENCLAVE_VIOLATION
#undef STATUS_SXS_EARLY_DEACTIVATION
#undef STATUS_SXS_INVALID_DEACTIVATION


//
// Disable some warnings that we'd get on /W4.
// Only active for compilers which support this feature.
//
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4001)
#pragma warning(disable:4201)
#pragma warning(disable:4115)
#pragma warning(disable:4214)
#endif
_CRT_BEGIN_C_HEADER
//
// Use dummy macros, if SAL 2 is not available
//
#include <sal.h>
#if (_SAL_VERSION < 20)
#include <no_sal2.h>
#endif

//
// Headers needed for NDK
//
#include <stdio.h>          // C Standard Header
#include <excpt.h>          // C Standard Header
#include <stdarg.h>         // C Standard Header
#include <ndk/umtypes.h>        // General Definitions

#ifndef _ANONYMOUS_UNION
#define _ANONYMOUS_UNION
#endif

//
// Type Headers
//
#include <ndk/cctypes.h>        // Cache Manager Types
#include <ndk/cmtypes.h>        // Configuration Manager Types
#include <ndk/dbgktypes.h>      // User-Mode Kernel Debugging Types
#include <ndk/extypes.h>        // Executive Types
#include <ndk/kdtypes.h>        // Kernel Debugger Types
#include <ndk/ketypes.h>        // Kernel Types
#include <ndk/haltypes.h>       // Hardware Abstraction Layer Types
#include <ndk/ifssupp.h>        // IFS Support Header
#include <ndk/iotypes.h>        // Input/Output Manager Types
#include <ndk/ldrtypes.h>       // Loader Types
#include <ndk/lpctypes.h>       // Local Procedure Call Types
#include <ndk/mmtypes.h>        // Memory Manager Types
#include <ndk/obtypes.h>        // Object Manager Types
#include <ndk/potypes.h>        // Power Manager Types
#include <ndk/pstypes.h>        // Process Manager Types
#include <ndk/rtltypes.h>       // Runtime Library Types
#include <ndk/setypes.h>        // Security Subsystem Types
#include <ndk/vftypes.h>        // Verifier Types

//
// Function Headers
//
#include <ndk/cmfuncs.h>        // Configuration Manager Functions
#include <ndk/dbgkfuncs.h>      // User-Mode Kernel Debugging Functions
#include <ndk/kdfuncs.h>        // Kernel Debugger Functions
#include <ndk/kefuncs.h>        // Kernel Functions
#include <ndk/exfuncs.h>        // Executive Functions
#include <ndk/halfuncs.h>       // Hardware Abstraction Layer Functions
#include <ndk/iofuncs.h>        // Input/Output Manager Functions
#include <ndk/inbvfuncs.h>      // Initialization Boot Video Functions
#include <ndk/ldrfuncs.h>       // Loader Functions
#include <ndk/lpcfuncs.h>       // Local Procedure Call Functions
#include <ndk/mmfuncs.h>        // Memory Manager Functions
#include <ndk/obfuncs.h>        // Object Manager Functions
#include <ndk/pofuncs.h>        // Power Manager Functions
#include <ndk/psfuncs.h>        // Process Manager Functions
#include <ndk/rtlfuncs.h>       // Runtime Library Functions
#include <ndk/sefuncs.h>        // Security Subsystem Functions
#include <ndk/umfuncs.h>        // User-Mode NT Library Functions
#include <ndk/vffuncs.h>        // Verifier Functions

//
// Assembly Support
//
#include <ndk/asm.h>            // Assembly Offsets

#ifdef _MSC_VER
#pragma warning(pop)
#endif
_CRT_END_C_HEADER

#endif // _NTNDK_
