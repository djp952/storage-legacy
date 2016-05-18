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

#ifndef __VIRTUALDISKASYNCRESULT_H_
#define __VIRTUALDISKASYNCRESULT_H_
#pragma once

#include "VirtualDiskAsyncStatus.h"
#include "VirtualDiskSafeHandle.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::ComponentModel;
using namespace System::Threading;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class VirtualDiskAsyncResult (internal)
//
// IAsyncResult-based object used to manage the asynchronous OVERLAPPED calls
// to the virtual disk functions
//---------------------------------------------------------------------------

ref class VirtualDiskAsyncResult sealed : IAsyncResult
{
public:

	//-----------------------------------------------------------------------
	// Properties

	// AsyncState (IAsyncResult)
	//
	// Gets a user-defined object that qualifies or contains information about 
	// the asynchronous operation
	property Object^ AsyncState
	{
		virtual Object^ get(void) { return m_state; }
	}

	// AsyncWaitHandle (IAsyncResult)
	//
	// Gets a WaitHandle that is used to wait for an asynchronous operation to complete
	property WaitHandle^ AsyncWaitHandle
	{
		virtual WaitHandle^ get(void) { return m_event; }
	}

	// CompletedSynchronously (IAsyncResult)
	//
	// Gets a value that indicates whether the asynchronous operation completed 
	// synchronously
	property bool CompletedSynchronously
	{
		virtual bool get(void) { return false; }
	}

	// IsCompleted (IAsyncResult)
	//
	// Gets a value that indicates whether the asynchronous operation has completed
	property bool IsCompleted
	{
		virtual bool get(void) { return (m_completed != 0); }
	}

internal:

	// INTERNAL CONSTRUCTORS
	VirtualDiskAsyncResult(VirtualDiskSafeHandle^ handle, Object^ state);

	//-----------------------------------------------------------------------
	// Internal Overloaded Operators

	operator LPOVERLAPPED() { return reinterpret_cast<LPOVERLAPPED>(m_pNativeOverlapped); }

	//-----------------------------------------------------------------------
	// Internal Member Functions

	// EndOperation
	//
	// Completes the asynchronous I/O operation
	static VirtualDiskAsyncStatus EndOperation(IAsyncResult^ asyncResult);

private:

	// DESTRUCTOR / FINALIZER
	~VirtualDiskAsyncResult() { this->!VirtualDiskAsyncResult(); GC::SuppressFinalize(this); }
	!VirtualDiskAsyncResult();

	//-----------------------------------------------------------------------
	// Private Member Functions

	// EndOperation
	//
	// Completes the asynchronous I/O operation
	VirtualDiskAsyncStatus EndOperation(void);

	//-----------------------------------------------------------------------
	// Member Variables

	initonly VirtualDiskSafeHandle^		m_handle;
	initonly Object^					m_state;
	int									m_completed;
	ManualResetEvent^					m_event;
	Overlapped^							m_overlapped;
	NativeOverlapped*					m_pNativeOverlapped;
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKASYNCRESULT_H_
