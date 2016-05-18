//-----------------------------------------------------------------------------
// Copyright (c) 2016 Michael G. Brehm
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//-----------------------------------------------------------------------------

#include "stdafx.h"					// Include project pre-compiled headers
#include "DiskUtil.h"				// Include DiskUtil declarations

using namespace System;

#pragma warning(push, 4)			// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// DiskUtil::AlignDown
//
// Aligns an offset down to the specified alignment
//
// Arguments:
//
//	offset		- Value to be aligned
//	alignment	- Alignment

__int64 DiskUtil::AlignDown(__int64 offset, __int64 alignment)
{
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if(alignment < 1) throw gcnew ArgumentOutOfRangeException("alignment");

	if(offset < alignment) return 0;
	else return AlignUp(offset - (alignment - 1), alignment);
}

//---------------------------------------------------------------------------
// DiskUtil::AlignUp
//
// Aligns an offset up to the specified alignment
//
// Arguments:
//
//	offset		- Value to be aligned
//	alignment	- Alignment

__int64 DiskUtil::AlignUp(__int64 offset, __int64 alignment)
{
	if(offset < 0) throw gcnew ArgumentOutOfRangeException("offset");
	if(alignment < 1) throw gcnew ArgumentOutOfRangeException("alignment");

	if(offset == 0) return 0;
	else return offset + ((alignment - (offset % alignment)) % alignment);
}

//---------------------------------------------------------------------------
// DiskUtil::FreePWSTR
//
// Releases a C-style string allocated by StringToPWSTR
//
// Arguments:
//
//	pwstr		- Unmanaged string pointer

PWSTR DiskUtil::FreePWSTR(PWSTR pwstr)
{
	if(!pwstr) return NULL;

	Marshal::FreeHGlobal(IntPtr(reinterpret_cast<void*>(pwstr)));
	return NULL;
}

//---------------------------------------------------------------------------
// DiskUtil::StringToPWSTR
//
// Converts a System::String into a unicode C-style string.  Release the 
// memory allocated for the string with Marshal::FreeHGlobal
//
// Arguments:
//
//	string		- String to be converted

PWSTR DiskUtil::StringToPWSTR(String^ string)
{
	if(string == nullptr) return NULL;
	return reinterpret_cast<PWSTR>(Marshal::StringToHGlobalUni(string).ToPointer());
}

//---------------------------------------------------------------------------
// DiskUtil::SysGuidToUUID
//
// Converts a managed System::Guid structure into a standard UUID structure
//
// Arguments:
//
//	guid		- The managed System::Guid to be converted

UUID DiskUtil::SysGuidToUUID(Guid guid)
{
	array<Byte>^ guidData = guid.ToByteArray();
	PinnedBytePtr data = &(guidData[0]);
	return *reinterpret_cast<UUID*>(data);
}

//---------------------------------------------------------------------------
// DiskUtil::UUIDToSysGuid
//
// Converts an unmanaged UUID structure into a managed System::Guid structure
//
// Arguments:
//
//	guid		- The unmanaged UUID structure to be converted

Guid DiskUtil::UUIDToSysGuid(const UUID& guid)
{
   return Guid(guid.Data1, guid.Data2, guid.Data3, guid.Data4[0],
	   guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5],
	   guid.Data4[6], guid.Data4[7]);
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)
