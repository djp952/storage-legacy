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

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::ComponentModel;
using namespace System::Threading;

BEGIN_ROOT_NAMESPACE(zuki::storage)

// FORWARD DECLARATIONS
//
enum class	VirtualDiskAsyncOperation;
ref class	VirtualDiskSafeHandle;

//---------------------------------------------------------------------------
// Class VirtualDiskAsyncResult (internal)
//
// IAsyncResult-based object used to manage the asynchronous OVERLAPPED calls
// to the virtual disk functions
//---------------------------------------------------------------------------

ref class VirtualDiskAsyncResult : IAsyncResult
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
		virtual Object^ get(void);
	}

	// AsyncWaitHandle (IAsyncResult)
	//
	// Gets a WaitHandle that is used to wait for an asynchronous operation to complete
	property WaitHandle^ AsyncWaitHandle
	{
		virtual WaitHandle^ get(void);
	}

	// CompletedSynchronously (IAsyncResult)
	//
	// Gets a value that indicates whether the asynchronous operation completed 
	// synchronously
	property bool CompletedSynchronously
	{
		virtual bool get(void);
	}

	// IsCompleted (IAsyncResult)
	//
	// Gets a value that indicates whether the asynchronous operation has completed
	property bool IsCompleted
	{
		virtual bool get(void);
	}

internal:

	// Constructor
	//
	VirtualDiskAsyncResult(VirtualDiskAsyncOperation operation, WaitHandle^ waithandle, NativeOverlapped* overlapped, 
		VirtualDiskSafeHandle^ safehandle, Object^ state);

	//-----------------------------------------------------------------------
	// Internal Properties

	// Operation
	//
	// Gets the VirtualDiskAsyncOperation value of this async result
	property VirtualDiskAsyncOperation Operation
	{
		VirtualDiskAsyncOperation get(void);
	}

	// VirtualDiskHandle
	//
	// Gets the VirtualDiskSafeHandle associated with this operation
	property VirtualDiskSafeHandle^ VirtualDiskHandle
	{
		VirtualDiskSafeHandle^ get(void);
	}

	//-----------------------------------------------------------------------
	// Internal Member Functions

	// Complete (static)
	//
	// Completes the asynchronous operation
	static VIRTUAL_DISK_PROGRESS Complete(VirtualDiskAsyncResult^ asyncresult);

private:

	// Destructor / Finalizer
	//
	~VirtualDiskAsyncResult();
	!VirtualDiskAsyncResult();

	//-----------------------------------------------------------------------
	// Member Variables

	bool						m_disposed;			// Object disposal flag
	VirtualDiskAsyncOperation	m_operation;		// Operation type
	WaitHandle^					m_waithandle;		// Operation wait handle
	NativeOverlapped*			m_overlapped;		// Native OVERLAPPED data
	VirtualDiskSafeHandle^		m_safehandle;		// Virtual disk safe handle
	Object^						m_state;			// User-defined state object
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKASYNCRESULT_H_
