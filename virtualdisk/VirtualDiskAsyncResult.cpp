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

#include "stdafx.h"						// Include project pre-compiled headers
#include "VirtualDiskAsyncResult.h"		// Include VirtualDiskAsyncResult decls

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult Constructor (internal)
//
// Arguments:
//
//	handle		- Open VirtualDiskSafeHandle instance
//	state		- IAsyncResult state object

VirtualDiskAsyncResult::VirtualDiskAsyncResult(VirtualDiskSafeHandle^ handle, Object^ state) : 
	m_handle(handle), m_state(state), m_completed(false)
{
	// Can't check this
	// if(handle == nullptr) throw gcnew ArgumentNullException("handle");

	m_event = gcnew ManualResetEvent(false);
	m_overlapped = gcnew Overlapped(0, 0, m_event->SafeWaitHandle->DangerousGetHandle(), this);
	m_pNativeOverlapped = m_overlapped->Pack(nullptr, nullptr);
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult Finalizer

VirtualDiskAsyncResult::!VirtualDiskAsyncResult()
{
	if(m_pNativeOverlapped) Overlapped::Free(m_pNativeOverlapped);
	m_pNativeOverlapped = NULL;
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::EndOperation (internal, static)
//
// Completes the asynchronous I/O operation
//
// Arguments:
//
//	asyncResult		- IAsyncResult object instance

VirtualDiskAsyncStatus VirtualDiskAsyncResult::EndOperation(IAsyncResult^ asyncResult)
{
	VirtualDiskAsyncResult^ instance = safe_cast<VirtualDiskAsyncResult^>(asyncResult);
	return instance->EndOperation();
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::EndOperation (private)
//
// Completes the asynchronous I/O operation
//
// Arguments:
//
//	NONE

VirtualDiskAsyncStatus VirtualDiskAsyncResult::EndOperation(void)
{
	VIRTUAL_DISK_PROGRESS			progress;			// Operation progress information
	DWORD							dwResult;			// Result from function call

	m_event->WaitOne();									// Wait for the I/O operation to complete

	// Get the final operation progress so that it can be returned by this function
	dwResult = GetVirtualDiskOperationProgress(m_handle, reinterpret_cast<LPOVERLAPPED>(m_pNativeOverlapped), &progress);
	if(dwResult != ERROR_SUCCESS) throw gcnew Win32Exception(dwResult);

	Interlocked::Exchange(m_completed, 1);				// Set the completion flag
	Overlapped::Free(m_pNativeOverlapped);				// Release native OVERLAPPED structure
	m_pNativeOverlapped = NULL;							// Reset pointer to NULL

	return VirtualDiskAsyncStatus(&progress);
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
