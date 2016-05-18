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

#ifndef __DISKSAFEHANDLE_H_
#define __DISKSAFEHANDLE_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace Microsoft::Win32::SafeHandles;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class DiskSafeHandle (internal)
//
// A SafeHandle-dervied class for unmanaged disk handles
//---------------------------------------------------------------------------

ref class DiskSafeHandle sealed : public SafeHandleZeroOrMinusOneIsInvalid
{
public:

	// CONSTRUCTORS
	DiskSafeHandle() : SafeHandleZeroOrMinusOneIsInvalid(true) {}
	
	DiskSafeHandle(HANDLE handle) : SafeHandleZeroOrMinusOneIsInvalid(true) {
		SafeHandleZeroOrMinusOneIsInvalid::SetHandle(IntPtr(reinterpret_cast<void*>(handle))); 
	}

	DiskSafeHandle(IntPtr handle, bool ownsHandle) : SafeHandleZeroOrMinusOneIsInvalid(ownsHandle) {
		SafeHandleZeroOrMinusOneIsInvalid::SetHandle(handle);
	}

	DiskSafeHandle(HANDLE handle, bool ownsHandle) : SafeHandleZeroOrMinusOneIsInvalid(ownsHandle) {
		SafeHandleZeroOrMinusOneIsInvalid::SetHandle(IntPtr(reinterpret_cast<void*>(handle)));
	}

	//-----------------------------------------------------------------------
	// Overloaded Operators

	// HANDLE() --> Use this class as type HANDLE for Win32 API calls
	operator HANDLE() { return reinterpret_cast<HANDLE>(SafeHandleZeroOrMinusOneIsInvalid::handle.ToPointer()); }

	//-----------------------------------------------------------------------
	// Member Functions

	// ReleaseHandle (SafeHandleZeroOrMinusOneIsInvalid)
	//
	// Frees the unmanaged handle object
	virtual bool ReleaseHandle(void) override
	{
		return (CloseHandle(SafeHandleZeroOrMinusOneIsInvalid::handle.ToPointer()) != 0);
	}

	// SetHandle
	//
	// Sets the contained handle.  Only use this when it can't be set during construction
	void SetHandle(HANDLE _handle) { SafeHandleZeroOrMinusOneIsInvalid::SetHandle(IntPtr(reinterpret_cast<void*>(_handle))); }
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __DISKSAFEHANDLE_H_
