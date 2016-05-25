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
//	waithandle		- Wait handle for the operation (ManualResetEvent)
//	overlapped		- NativeOverlapped pointer (takes ownership)
//	cancellation	- CancellationToken instance
//	progress		- Progress callback

VirtualDiskAsyncResult::VirtualDiskAsyncResult(VirtualDiskSafeHandle^ handle, ManualResetEvent^ waithandle, NativeOverlapped* overlapped,
	CancellationToken cancellation, IProgress<int>^ progress) : m_handle(handle), m_waithandle(waithandle), m_overlapped(overlapped),
	m_progress(progress)
{
	if(Object::ReferenceEquals(handle, nullptr)) throw gcnew ArgumentNullException("handle");
	if(Object::ReferenceEquals(waithandle, nullptr)) throw gcnew ArgumentNullException("waithandle");
	if(overlapped == __nullptr) throw gcnew ArgumentNullException("overlapped");

	// Verify the cancellation token and register to be notified if triggered
	cancellation.ThrowIfCancellationRequested();
	if(cancellation.CanBeCanceled) m_cancelreg = cancellation.Register(gcnew Action(this, &VirtualDiskAsyncResult::Cancel));

	// If progress of the operation is requested, start a background thread to monitor and report it
	if(!Object::ReferenceEquals(progress, nullptr)) {

		m_progressthread = gcnew Thread(gcnew ThreadStart(this, &VirtualDiskAsyncResult::ProgressThread));
		m_progressthread->IsBackground = true;
		m_progressthread->Start();
	}
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::AsyncState::get
//
// Gets a user-defined object that qualifies the asynchronous operation

Object^ VirtualDiskAsyncResult::AsyncState::get(void)
{
	// Return the virtual disk safe handle through AsyncState
	return m_handle;
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::AsyncWaitHandle::get
//
// Gets a WaitHandle that is used to wait for an asynchronous operation to complete

WaitHandle^ VirtualDiskAsyncResult::AsyncWaitHandle::get(void)
{
	return m_waithandle;
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
	// Attempt to cancel the asynchronous operation, don't propagate any exception that occurs
	try { CancelIoEx(VirtualDiskSafeHandle::Reference(m_handle), reinterpret_cast<LPOVERLAPPED>(m_overlapped)); }
	catch(Exception^) { /* DO NOTHING */ }
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::CompleteAsynchronously (static)
//
// Completes the operation asynchronously
//
// Arguments:
//
//	asyncresult		- IAsyncResult instance to be completed

void VirtualDiskAsyncResult::CompleteAsynchronously(IAsyncResult^ asyncresult)
{
	safe_cast<VirtualDiskAsyncResult^>(asyncresult)->Complete();
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::Complete (private)
//
// Completes the operation
//
// Arguments:
//
//	NONE

void VirtualDiskAsyncResult::Complete(void)
{
	VIRTUAL_DISK_PROGRESS			progress;			// Final operation progress

	// Flag the operation as completed and prevent multiple completions
	if(Interlocked::CompareExchange(m_completed, TRUE, FALSE) == TRUE) throw gcnew InvalidOperationException();

	m_waithandle->WaitOne();						// Wait for the operation to complete

	// If a progress worker thread was created wait for it to terminate normally
	if(!Object::ReferenceEquals(m_progressthread, nullptr)) m_progressthread->Join();

	// Get the result from the asynchronous operation prior to releasing the OVERLAPPED structure
	DWORD result = GetVirtualDiskOperationProgress(VirtualDiskSafeHandle::Reference(m_handle), reinterpret_cast<LPOVERLAPPED>(m_overlapped), &progress);

	Overlapped::Free(m_overlapped);					// Release NativeOverlapped
	m_waithandle->Close();							// Close the wait handle

	// The cancellation registration has to be disposed of, but there is no way to do that directly
	// in C++/CLI, so sadly the Dispose() method has to be invoked dynamically
	//
	// m_cancelreg.Dispose();
	CancellationTokenRegistration::typeid->GetMethod("Dispose")->Invoke(m_cancelreg, nullptr);

	// Check if an error occurred getting the virtual disk operation status
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

	// If the operation was cancelled, throw an OperationCancelledException
	if(progress.OperationStatus == ERROR_OPERATION_ABORTED) throw gcnew OperationCanceledException();

	// Send a final completion progress if the operation was not cancelled
	if(!Object::ReferenceEquals(m_progress, nullptr)) m_progress->Report(100);

	// If the operation completed unsuccessfully, throw the status code as an exception
	if(progress.OperationStatus != ERROR_SUCCESS) throw gcnew Win32Exception(progress.OperationStatus);
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
	// Flag the operation as completed and prevent multiple completions
	if(Interlocked::CompareExchange(m_completed, TRUE, FALSE) == TRUE) throw gcnew InvalidOperationException();

	m_waithandle->Set();					// Signal the event object

	// If a progress worker thread was created wait for it to terminate normally
	if(!Object::ReferenceEquals(m_progressthread, nullptr)) m_progressthread->Join();

	m_synchronous = true;					// Operation completed synchronously
	Overlapped::Free(m_overlapped);			// Release NativeOverlapped
	m_waithandle->Close();					// Close the wait handle

	// The cancellation registration has to be disposed of, but there is no way to do that directly
	// in C++/CLI, so sadly the Dispose() method has to be invoked dynamically
	//
	// m_cancelreg.Dispose();
	CancellationTokenRegistration::typeid->GetMethod("Dispose")->Invoke(m_cancelreg, nullptr);

	// If the provided status code indicates an error condition, throw it as an exception
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
	return (m_completed == TRUE);
}

//---------------------------------------------------------------------------
// VirtualDiskAsyncResult::ProgressThread (private)
//
// Worker thread used to implement IProgress<> against the async result object
//
// Arguments:
//
//	NONE

void VirtualDiskAsyncResult::ProgressThread(void)
{
	VIRTUAL_DISK_PROGRESS		progress;		// Async operation progress

	// If there was no IProgress<int> registered, there is no reason to have this thread
	if(Object::ReferenceEquals(m_progress, nullptr)) return;

	try {

		// Report progress once every second until the operation has completed
		while(m_waithandle->WaitOne(1000) == false) {

			// Get the result from the asynchronous operation prior to releasing the OVERLAPPED structure
			DWORD result = GetVirtualDiskOperationProgress(VirtualDiskSafeHandle::Reference(m_handle), reinterpret_cast<LPOVERLAPPED>(m_overlapped), &progress);
			if(result == ERROR_SUCCESS) {
			
				// If the completion value is 100, the current value can be reported as-is
				if(progress.CompletionValue == 100ULL) m_progress->Report(static_cast<int>(progress.CurrentValue));

				// If the completion value is non-zero, report the current value as a percentage
				else if(progress.CompletionValue > 0) m_progress->Report(static_cast<int>((100ULL * progress.CurrentValue) / progress.CompletionValue));
			}
		}
	}

	catch(Exception^) { /* DO NOTHING - EXIT THE THREAD */ }
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
