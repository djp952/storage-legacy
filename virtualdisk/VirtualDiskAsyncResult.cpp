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

#include "VirtualDiskSafeHandle.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult Constructor
//
// Arguments:
//
//	handle			- VirtualDiskSafeHandle instance for the operation
//	cancellation	- CancellationToken instance
//	progress		- Progress callback

VirtualDiskAsyncResult::VirtualDiskAsyncResult(VirtualDiskSafeHandle^ handle, CancellationToken cancellation, IProgress<int>^ progress) 
	: m_handle(handle), m_progress(progress)
{
	if(Object::ReferenceEquals(handle, nullptr)) throw gcnew ArgumentNullException("handle");
	cancellation.ThrowIfCancellationRequested();

	m_event = gcnew ManualResetEvent(false);
	m_overlapped = Overlapped(0, 0, m_event->SafeWaitHandle->DangerousGetHandle(), this).Pack(nullptr, nullptr);
	if(cancellation.CanBeCanceled) cancellation.Register(gcnew Action(this, &VirtualDiskAsyncResult::Cancel));

	// If progress of the operation is requested, start a thread pool thread to handle that
	if(!Object::ReferenceEquals(progress, nullptr)) ThreadPool::QueueUserWorkItem(gcnew WaitCallback(this, &VirtualDiskAsyncResult::ProgressThread));
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
	// This is a dumbed-down AsyncResult class, there is no state object
	return nullptr;
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
// VirtualDiskAsyncResult::Cancel (private)
//
// Attempts to cancel the operation
//
// Arguments:
//
//	NONE

void VirtualDiskAsyncResult::Cancel(void)
{
	// Attempt to cancel the asynchronous operation
	try { CancelIoEx(VirtualDiskSafeHandle::Reference(m_handle), reinterpret_cast<LPOVERLAPPED>(m_overlapped)); }
	catch(Exception^) { /* DO NOTHING */ }
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::Complete (static)
//
// Completes an asynchronous operation
//
// Arguments:
//
//	asyncresult		- IAsyncResult instance to be completed

void VirtualDiskAsyncResult::Complete(IAsyncResult^ asyncresult)
{
	VIRTUAL_DISK_PROGRESS			progress;			// Final operation progress

	// Cast the IAsyncResult interface back into a VirtualDiskAsyncResult instance
	VirtualDiskAsyncResult^ vdiskresult = safe_cast<VirtualDiskAsyncResult^>(asyncresult);

	// Wait for the event to be signaled and close the handle
	vdiskresult->m_event->WaitOne();
	vdiskresult->m_event->Close();

	// Get the result from the asynchronous operation prior to releasing the OVERLAPPED structure
	DWORD result = GetVirtualDiskOperationProgress(VirtualDiskSafeHandle::Reference(vdiskresult->m_handle), reinterpret_cast<LPOVERLAPPED>(vdiskresult->m_overlapped), &progress);

	// Release and reset the NativeOverlapped pointer
	Overlapped::Free(vdiskresult->m_overlapped);
	vdiskresult->m_overlapped = __nullptr;

	// Send a final progress report if an IProgress<> was provided during construction
	if(!Object::ReferenceEquals(vdiskresult->m_progress, nullptr)) vdiskresult->m_progress->Report(100);

	// If an error occurred getting the operation status, throw that as an exception
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

	// If a cancellation/error status was set on the operation, throw an exception
	if(progress.OperationStatus == ERROR_OPERATION_ABORTED) throw gcnew OperationCanceledException();
	else if(progress.OperationStatus != ERROR_SUCCESS) throw gcnew Win32Exception(progress.OperationStatus);
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::CompleteSynchronously
//
// Completes the operation synchronously
//
// Arguments:
//
//	status			- Status code for the operation

void VirtualDiskAsyncResult::CompleteSynchronously(unsigned int status)
{
	m_synchronous = true;				// Operation completed synchronously
	m_event->Close();					// Close the event object handle

	Overlapped::Free(m_overlapped);		// Release the OVERLAPPED structure
	m_overlapped = __nullptr;			// Reset the OVERLAPPED pointer

	// If the provided status code is an error code, throw it as an exception
	if(status != ERROR_SUCCESS) throw gcnew Win32Exception(status);
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
// VirtualDiskAsyncResult::IsCompleted::get
//
// Gets a value that indicates whether the asynchronous operation has completed

bool VirtualDiskAsyncResult::IsCompleted::get(void)
{
	return m_event->WaitOne(0);
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::ProgressThread (private)
//
// Worker thread used to implement IProgress<> against the async result object
//
// Arguments:
//
//	state		- Caller-provided state object (unused)

void VirtualDiskAsyncResult::ProgressThread(Object^ state)
{
	VIRTUAL_DISK_PROGRESS		progress;		// Async operation progress

	UNREFERENCED_PARAMETER(state);

	// If there was no IProgress<int> registered, there is no reason to have this thread
	if(Object::ReferenceEquals(m_progress, nullptr)) return;

	// Report progress every second until the operation has completed
	while(m_event->WaitOne(1000) == false) {

		try {

			// Get the result from the asynchronous operation prior to releasing the OVERLAPPED structure
			DWORD result = GetVirtualDiskOperationProgress(VirtualDiskSafeHandle::Reference(m_handle), reinterpret_cast<LPOVERLAPPED>(m_overlapped), &progress);
			if((result == ERROR_SUCCESS) && (progress.CompletionValue > 0)) m_progress->Report((100ULL * progress.CurrentValue) / progress.CompletionValue);
		}
		
		catch(Exception^) { /* DO NOTHING */ }
	}
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
