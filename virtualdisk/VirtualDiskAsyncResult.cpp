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

#include "stdafx.h"
#include "VirtualDiskAsyncResult.h"

#include "VirtualDiskAsyncOperation.h"
#include "VirtualDiskSafeHandle.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult Constructor (internal)
//
// Arguments:
//
//	operation		- Asynchronous operation being performed
//	waithandle		- WaitHandle instance for the async operation
//	overlapped		- Packed NativeOverlapped pointer
//	safehandle		- VirtualDiskSafeHandle instance
//	state			- Caller-provided state object for the operation

VirtualDiskAsyncResult::VirtualDiskAsyncResult(VirtualDiskAsyncOperation operation, WaitHandle^ waithandle, NativeOverlapped* overlapped, 
	VirtualDiskSafeHandle^ safehandle, Object^ state) : m_operation(operation), m_waithandle(waithandle), m_overlapped(overlapped), m_safehandle(safehandle),
	m_state(state)
{
	if(Object::ReferenceEquals(waithandle, nullptr)) throw gcnew ArgumentNullException("waithandle");
	if(overlapped == __nullptr) throw gcnew ArgumentNullException("overlapped");
	if(Object::ReferenceEquals(safehandle, nullptr)) throw gcnew ArgumentNullException("safehandle");
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult Destructor

VirtualDiskAsyncResult::~VirtualDiskAsyncResult()
{
	if(m_disposed) return;

	delete m_waithandle;
	this->!VirtualDiskAsyncResult();
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult Finalizer

VirtualDiskAsyncResult::!VirtualDiskAsyncResult()
{
	if(m_overlapped) Overlapped::Free(m_overlapped);
	m_overlapped = __nullptr;
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::AsyncState::get
//
// Gets a user-defined object that qualifies or contains information about 
// the asynchronous operation

Object^ VirtualDiskAsyncResult::AsyncState::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_state;
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::AsyncWaitHandle::get
//
// Gets a WaitHandle that is used to wait for an asynchronous operation to complete

WaitHandle^ VirtualDiskAsyncResult::AsyncWaitHandle::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_waithandle;
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::Complete (static, internal)
//
// Completes the asynchronous operation
//
// Arguments:
//
//	asyncresult		- VirtualDiskAsyncResult instance to complete

VIRTUAL_DISK_PROGRESS VirtualDiskAsyncResult::Complete(VirtualDiskAsyncResult^ asyncresult)
{
	VIRTUAL_DISK_PROGRESS			progress;			// Operation progress information
	DWORD							result;				// Result from function call

	if(Object::ReferenceEquals(asyncresult, nullptr)) throw gcnew ArgumentNullException("asyncresult");

	asyncresult->m_waithandle->WaitOne();				// Wait for the async operation to complete

	// Get the final operation progress to return to the caller
	result = GetVirtualDiskOperationProgress(VirtualDiskSafeHandle::Reference(asyncresult->m_safehandle), 
		reinterpret_cast<LPOVERLAPPED>(asyncresult->m_overlapped), &progress);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

	// Return the final progress as an unmanaged VIRTUAL_DISK_PROGRESS
	return progress;
}
	
//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::CompletedSynchronously::get
//
// Gets a value that indicates whether the asynchronous operation completed 
// synchronously

bool VirtualDiskAsyncResult::CompletedSynchronously::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return false;
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::IsCompleted::get
//
// Gets a value that indicates whether the asynchronous operation has completed

bool VirtualDiskAsyncResult::IsCompleted::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_waithandle->WaitOne(0);
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::Operation::get
//
// Gets the VirtualDiskAsyncOperation value for this async result

VirtualDiskAsyncOperation VirtualDiskAsyncResult::Operation::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_operation;
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::VirtualDiskHandle::get (internal)
//
// Gets the virtual disk safe handle associated with this operation

VirtualDiskSafeHandle^ VirtualDiskAsyncResult::VirtualDiskHandle::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_safehandle;
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
