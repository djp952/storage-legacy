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
#include "DiskAsyncResult.h"			// Include DiskAsyncResult decls

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// DiskAsyncResult Constructor (internal)
//
// Arguments:
//
//	handle		- Open DiskSafeHandle instance
//	state		- IAsyncResult state object

DiskAsyncResult::DiskAsyncResult(DiskSafeHandle^ handle, Object^ state) : 
	m_handle(handle), m_state(state), m_completed(false)
{
	// Can't check this
	// if(handle == nullptr) throw gcnew ArgumentNullException("handle");

	m_event = gcnew ManualResetEvent(false);
	m_overlapped = gcnew Overlapped(0, 0, m_event->SafeWaitHandle->DangerousGetHandle(), this);
	m_pNativeOverlapped = m_overlapped->Pack(nullptr, nullptr);
}

//---------------------------------------------------------------------------
// DiskAsyncResult Finalizer

DiskAsyncResult::!DiskAsyncResult()
{
	if(m_pNativeOverlapped) Overlapped::Free(m_pNativeOverlapped);
	m_pNativeOverlapped = NULL;
}

//---------------------------------------------------------------------------
// DiskAsyncResult::EndOperation (internal, static)
//
// Completes the asynchronous I/O operation
//
// Arguments:
//
//	asyncResult		- IAsyncResult object instance

void DiskAsyncResult::EndOperation(IAsyncResult^ asyncResult)
{
	DiskAsyncResult^ instance = safe_cast<DiskAsyncResult^>(asyncResult);
	instance->EndOperation();
}

//---------------------------------------------------------------------------
// DiskAsyncResult::EndOperation (private)
//
// Completes the asynchronous I/O operation
//
// Arguments:
//
//	NONE

void DiskAsyncResult::EndOperation(void)
{
	DWORD					cbTransferred;				// Bytes transferred

	m_event->WaitOne();									// Wait for the I/O operation to complete

	// Complete the asynchronous operation
	if(!GetOverlappedResult(m_handle, reinterpret_cast<LPOVERLAPPED>(m_pNativeOverlapped), &cbTransferred, FALSE))
		throw gcnew Win32Exception(GetLastError());

	Interlocked::Exchange(m_completed, 1);				// Set the completion flag
	Overlapped::Free(m_pNativeOverlapped);				// Release native OVERLAPPED structure
	m_pNativeOverlapped = NULL;							// Reset pointer to NULL
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
