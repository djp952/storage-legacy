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
// VirtualDiskAsyncResult Constructor
//
// Arguments:
//
//	operation	- Asynchronous operation being performed
//	handle		- VirtualDiskSafeHandle instance
//	callback	- User-provided AsyncCallback instance
//	state		- User-provided state object

VirtualDiskAsyncResult::VirtualDiskAsyncResult(VirtualDiskAsyncOperation operation, VirtualDiskSafeHandle^ handle, AsyncCallback^ callback, 
	Object^ state) : m_operation(operation), m_handle(handle), m_callback(callback), m_state(state)
{
	if(Object::ReferenceEquals(handle, nullptr)) throw gcnew ArgumentNullException("handle");

	// Construct a new manual reset event to act as the wait handle for the asynchronous operation
	m_event = gcnew ManualResetEvent(false);

	// Construct a new IOCompletionCallback for this operation and the associated NativeOverlapped pointer
	IOCompletionCallback^ completioncallback = gcnew IOCompletionCallback(&VirtualDiskAsyncResult::CompletionCallback);
	m_overlapped = Overlapped(0, 0, m_event->SafeWaitHandle->DangerousGetHandle(), this).Pack(completioncallback, nullptr);

	ThreadPool::UnsafeQueueNativeOverlapped(m_overlapped);
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult LPOVERLAPPED conversion operator

VirtualDiskAsyncResult::operator LPOVERLAPPED()
{
	return reinterpret_cast<LPOVERLAPPED>(m_overlapped);
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::AsyncState::get
//
// Gets a user-defined object that qualifies the asynchronous operation

Object^ VirtualDiskAsyncResult::AsyncState::get(void)
{
	return m_state;
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::AsyncWaitHandle::get
//
// Gets a WaitHandle that is used to wait for an asynchronous operation to complete

WaitHandle^ VirtualDiskAsyncResult::AsyncWaitHandle::get(void)
{
	return m_event;
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::Cancel (internal)
//
// Attempts to cancel the operation
//
// Arguments:
//
//	NONE

void VirtualDiskAsyncResult::Cancel(void)
{
	// Attempt to cancel the asynchronous operation
	if(!CancelIoEx(VirtualDiskSafeHandle::Reference(m_handle), reinterpret_cast<LPOVERLAPPED>(m_overlapped))) throw gcnew Win32Exception(GetLastError());
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::Complete (internal)
//
// Completes the operation
//
// Arguments:
//
//	NONE

void VirtualDiskAsyncResult::Complete(void)
{
	m_event->WaitOne();				// Wait for the operation to complete
	m_event->Close();				// Close the manual reset event

	// If an error status was set during completion, throw it as a Win32Exception
	if(m_status != ERROR_SUCCESS) throw gcnew Win32Exception(m_status);
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::CompleteSynchronous (internal)
//
// Completes the operation synchronously
//
// Arguments:
//
//	status			- Status code for the operation

void VirtualDiskAsyncResult::CompleteSynchronous(unsigned int status)
{
	m_event->Set();						// Set the manual reset event
	m_synchronous = true;				// Operation completed synchronously

	// Invoke the completion callback to finish the operation
	CompletionCallback(status, 0, m_overlapped);
}
	
//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::CompletionCallback (static)
//
// Callback method invoked when the operation has completed
//
// Arguments:
//
//	errorcode		- Operation error/status code
//	numbytes		- Number of bytes transferred
//	overlapped		- NativeOverlapped instance for the operation

void VirtualDiskAsyncResult::CompletionCallback(unsigned int errorcode, unsigned int numbytes, NativeOverlapped* overlapped)
{
	VIRTUAL_DISK_PROGRESS			progress;			// Final operation progress

	UNREFERENCED_PARAMETER(errorcode);
	UNREFERENCED_PARAMETER(numbytes);

	// Unpack the NativeOverlapped and retrieve the VirtualDiskAsyncResult instance from it
	VirtualDiskAsyncResult^ asyncresult = safe_cast<VirtualDiskAsyncResult^>(Overlapped::Unpack(overlapped)->AsyncResult);

	// There is no completion callback from the virtual disk API calls, this callback is generated
	// with the I/O completion port thread pool; wait for the event handle to be signaled
	asyncresult->AsyncWaitHandle->WaitOne();

	// The errorcode argument is worthless here, get the actual operation status.  This should always succeed
	DWORD result = GetVirtualDiskOperationProgress(VirtualDiskSafeHandle::Reference(asyncresult->m_handle), reinterpret_cast<LPOVERLAPPED>(overlapped), &progress);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);
	
	asyncresult->m_status = progress.OperationStatus;	// Get the operation status
	Overlapped::Free(overlapped);						// Release the NativeOverlapped instance

	// Invoke the user-provided AsyncCallback to complete the asynchronous operation
	if(!Object::ReferenceEquals(asyncresult->m_callback, nullptr)) asyncresult->m_callback(asyncresult);
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::CompletedSynchronously::get
//
// Gets a value that indicates whether the asynchronous operation completed synchronously

bool VirtualDiskAsyncResult::CompletedSynchronously::get(void)
{
	return m_synchronous;
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::Handle::get
//
// Gets the VirtualDiskSafeHandle instance for this operation

VirtualDiskSafeHandle^ VirtualDiskAsyncResult::Handle::get(void)
{
	return m_handle;
}
	
//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::IsCompleted::get
//
// Gets a value that indicates whether the asynchronous operation has completed

bool VirtualDiskAsyncResult::IsCompleted::get(void)
{
	return m_event->WaitOne(0);
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::Operation::get (internal)
//
// Gets the VirtualDiskAsyncOperation value for this async result

VirtualDiskAsyncOperation VirtualDiskAsyncResult::Operation::get(void)
{
	return m_operation;
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
