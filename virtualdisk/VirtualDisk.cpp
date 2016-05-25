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
#include "VirtualDisk.h"

#include "zeroinit.h"
#include "GuidUtil.h"
#include "VirtualDiskAccess.h"
#include "VirtualDiskAttachFlags.h"
#include "VirtualDiskAttachParameters.h"
#include "VirtualDiskAsyncResult.h"
#include "VirtualDiskCompactFlags.h"
#include "VirtualDiskCreateFlags.h"
#include "VirtualDiskDetachFlags.h"
#include "VirtualDiskExpandFlags.h"
#include "VirtualDiskOpenFlags.h"
#include "VirtualDiskResizeFlags.h"
#include "VirtualDiskSafeHandle.h"
#include "VirtualDiskType.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// VirtualDisk Constructor (private)
//
// Arguments:
//
//	handle		- VirtualDiskSafeHandle instance

VirtualDisk::VirtualDisk(VirtualDiskSafeHandle^ handle) : m_handle(handle)
{
	if(Object::ReferenceEquals(handle, nullptr)) throw gcnew ArgumentNullException("handle");
}

//---------------------------------------------------------------------------
// VirtualDisk Destructor

VirtualDisk::~VirtualDisk()
{
	if(m_disposed) return;

	delete m_handle;
	m_disposed = true;
}

//---------------------------------------------------------------------------
// VirtualDisk::Attach
//
// Synchronously attaches the virtual disk
//
// Arguments:
//
//	NONE

void VirtualDisk::Attach(void)
{
	CHECK_DISPOSED(m_disposed);

	// Synchronously attach the virtual disk using a default set of parameters
	Attach(gcnew VirtualDiskAttachParameters());
}

//---------------------------------------------------------------------------
// VirtualDisk::Attach
//
// Synchronously attaches the virtual disk
//
// Arguments:
//
//	flags			- Flags to control the attach operation

void VirtualDisk::Attach(VirtualDiskAttachFlags flags)
{
	CHECK_DISPOSED(m_disposed);

	// Create a new VirtualDiskAttachParameters instance and set the flags
	VirtualDiskAttachParameters^ params = gcnew	VirtualDiskAttachParameters();
	params->Flags = flags;

	// Synchronously attach the virtual disk using the generated parameters
	Attach(params);
}

//---------------------------------------------------------------------------
// VirtualDisk::Attach
//
// Synchronously attaches the virtual disk
//
// Arguments:
//
//	params			- Attach operation parameters and flags

void VirtualDisk::Attach(VirtualDiskAttachParameters^ params)
{
	CHECK_DISPOSED(m_disposed);

	// Synchronously attach the virtual disk using the provided parameters
	VirtualDiskAsyncResult::CompleteAsynchronously(BeginAttach(params, CancellationToken::None, nullptr));
}

//---------------------------------------------------------------------------
// VirtualDisk::AttachAsync
//
// Asynchronously attaches the virtual disk
//
// Arguments:
//
//	params			- Attach operation parameters and flags
//	cancellation	- Token to monitor for cancellation requests
//	progress		- Optional IProgress<int> on which to report progress

Task^ VirtualDisk::AttachAsync(VirtualDiskAttachParameters^ params, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);
	return Task::Factory->FromAsync(BeginAttach(params, cancellation, progress), gcnew Action<IAsyncResult^>(&VirtualDiskAsyncResult::CompleteAsynchronously));
}

//---------------------------------------------------------------------------
// VirtualDisk::AttachedDevicePath::get
//
// Gets the physical device object path for the virtual disk

String^ VirtualDisk::AttachedDevicePath::get(void)
{
	ULONG				pathlen = 0;				// Path length in bytes
	uint8_t*			pathbuffer;					// Physical device path
	DWORD				result;						// Result from function call

	CHECK_DISPOSED(m_disposed);

	// Attempt to get the size of the physical path string in bytes
	result = GetVirtualDiskPhysicalPath(VirtualDiskSafeHandle::Reference(m_handle), &pathlen, NULL);

	// If the disk has not been attached, this will fail with ERROR_DEV_NOT_EXIST
	if(result == ERROR_DEV_NOT_EXIST) return String::Empty;
	else if(result != ERROR_INSUFFICIENT_BUFFER) throw gcnew Win32Exception(result);

	// Attempt to allocate an unmanaged heap buffer for the string
	try { pathbuffer = new uint8_t[pathlen + sizeof(wchar_t)]; }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }

	try {

		result = GetVirtualDiskPhysicalPath(VirtualDiskSafeHandle::Reference(m_handle), &pathlen, reinterpret_cast<wchar_t*>(pathbuffer));
		if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

		return gcnew String(reinterpret_cast<const wchar_t*>(pathbuffer));
	}

	finally { delete[] pathbuffer; }
}

//-----------------------------------------------------------------------------
// VirtualDisk::BeginAttach (private)
//
// Starts an asynchronous attach operation
//
// Arguments:
//
//	params			- Attach operation parameters
//	cancellation	- CancellationToken for the asynchronous operation
//	progress		- IProgress<int> on which to report operation progress

IAsyncResult^ VirtualDisk::BeginAttach(VirtualDiskAttachParameters^ params, CancellationToken cancellation, IProgress<int>^ progress)
{
	pin_ptr<uint8_t>			pinsd = __nullptr;			// Pinned security descriptor

	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(params, nullptr)) throw gcnew ArgumentNullException("params");

	zero_init<ATTACH_VIRTUAL_DISK_PARAMETERS> attachparams;
	attachparams.Version = ATTACH_VIRTUAL_DISK_VERSION_1;

	// Create a new event-based NativeOverlapped structure (VirtualDiskAsyncResult will take ownership of these)
	ManualResetEvent^ waithandle = gcnew ManualResetEvent(false);
	NativeOverlapped* overlapped = Overlapped(0, 0, waithandle->SafeWaitHandle->DangerousGetHandle(), nullptr).Pack(nullptr, nullptr);

	// If a managed security descriptor was provided, convert it into a pinned binary security descriptor
	if(params->SecurityDescriptor != nullptr) pinsd = &(params->SecurityDescriptor->GetSecurityDescriptorBinaryForm()[0]);

	// Begin the asynchronous virtual disk operation 
	DWORD result = AttachVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), reinterpret_cast<PSECURITY_DESCRIPTOR>(pinsd), 
		static_cast<ATTACH_VIRTUAL_DISK_FLAG>(params->Flags), params->ProviderFlags, &attachparams, reinterpret_cast<LPOVERLAPPED>(overlapped));

	// Transfer ownership of all asynchronous resources to a VirtualDiskAsyncResult instance before checking the result
	VirtualDiskAsyncResult^ asyncresult = gcnew VirtualDiskAsyncResult(m_handle, waithandle, overlapped, cancellation, progress);

	// If the request to create the disk failed, complete the operation synchronously to clean everything up
	if(result != ERROR_IO_PENDING) asyncresult->CompleteSynchronously(result);

	return asyncresult;
}

//-----------------------------------------------------------------------------
// VirtualDisk::BeginCompact (private)
//
// Starts an asynchronous compact operation
//
// Arguments:
//
//	flags			- Compact operation flags
//	cancellation	- CancellationToken for the asynchronous operation
//	progress		- IProgress<int> on which to report operation progress

IAsyncResult^ VirtualDisk::BeginCompact(VirtualDiskCompactFlags flags, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);

	zero_init<COMPACT_VIRTUAL_DISK_PARAMETERS> params;
	params.Version = COMPACT_VIRTUAL_DISK_VERSION_1;

	// Create a new event-based NativeOverlapped structure (VirtualDiskAsyncResult will take ownership of these)
	ManualResetEvent^ waithandle = gcnew ManualResetEvent(false);
	NativeOverlapped* overlapped = Overlapped(0, 0, waithandle->SafeWaitHandle->DangerousGetHandle(), nullptr).Pack(nullptr, nullptr);

	// Begin the asynchronous virtual disk operation 
	DWORD result = CompactVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), static_cast<COMPACT_VIRTUAL_DISK_FLAG>(flags), &params, reinterpret_cast<LPOVERLAPPED>(overlapped));

	// Transfer ownership of all asynchronous resources to a VirtualDiskAsyncResult instance before checking the result
	VirtualDiskAsyncResult^ asyncresult = gcnew VirtualDiskAsyncResult(m_handle, waithandle, overlapped, cancellation, progress);

	// If the request to create the disk failed, complete the operation synchronously to clean everything up
	if(result != ERROR_IO_PENDING) asyncresult->CompleteSynchronously(result);

	return asyncresult;
}

//-----------------------------------------------------------------------------
// VirtualDisk::BeginCreate (private, static)
//
// Starts an asynchronous create operation
//
// Arguments:
//
//	path			- Fully qualified path to the virtual disk file
//	flags			- Flags to control the behavior of the disk creation
//	cancellation	- Asynchronous operation cancellation token
//	progress		- Optional IProgress<> on which to report operation progress

IAsyncResult^ VirtualDisk::BeginCreate(String^ path, VirtualDiskCreateFlags flags, CancellationToken cancellation, IProgress<int>^ progress)
{
	if(Object::ReferenceEquals(path, nullptr)) throw gcnew ArgumentNullException("path");

	// todo
	zero_init<CREATE_VIRTUAL_DISK_PARAMETERS> params;
	params.Version = CREATE_VIRTUAL_DISK_VERSION_1;
	params.Version1.MaximumSize = 4194304 * 400;		// 1600MB
	params.Version1.SectorSizeInBytes = 512;

	// todo
	zero_init<VIRTUAL_STORAGE_TYPE> type;
	type.DeviceId = VIRTUAL_STORAGE_TYPE_DEVICE_VHD;
	type.VendorId = VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT;

	// Create a new event-based NativeOverlapped structure (VirtualDiskAsyncResult will take ownership of these)
	ManualResetEvent^ waithandle = gcnew ManualResetEvent(false);
	NativeOverlapped* overlapped = Overlapped(0, 0, waithandle->SafeWaitHandle->DangerousGetHandle(), nullptr).Pack(nullptr, nullptr);

	pin_ptr<const wchar_t> pinpath = PtrToStringChars(path);		// Destination path
	HANDLE handle = __nullptr;										// Virtual disk handle

	// Begin the asynchronous virtual disk operation
	DWORD result = CreateVirtualDisk(&type, pinpath, VIRTUAL_DISK_ACCESS_ALL, __nullptr, static_cast<CREATE_VIRTUAL_DISK_FLAG>(flags),
		0, &params, reinterpret_cast<LPOVERLAPPED>(overlapped), &handle);

	// Transfer ownership of all asynchronous resources to a VirtualDiskAsyncResult instance before checking the result
	VirtualDiskAsyncResult^ asyncresult = gcnew VirtualDiskAsyncResult(gcnew VirtualDiskSafeHandle(handle), waithandle,
		overlapped, cancellation, progress);

	// If the request to create the disk failed, complete the operation synchronously to clean everything up
	if(result != ERROR_IO_PENDING) asyncresult->CompleteSynchronously(result);

	return asyncresult;
}

//-----------------------------------------------------------------------------
// VirtualDisk::BeginExpand (private)
//
// Starts an asynchronous expand operation
//
// Arguments:
//
//	newsize			- Requested new size of the virtual disk
//	flags			- Expand operation flags
//	cancellation	- CancellationToken for the asynchronous operation
//	progress		- IProgress<int> on which to report operation progress

IAsyncResult^ VirtualDisk::BeginExpand(__int64 newsize, VirtualDiskExpandFlags flags, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);

	zero_init<EXPAND_VIRTUAL_DISK_PARAMETERS> params;
	params.Version = EXPAND_VIRTUAL_DISK_VERSION_1;
	params.Version1.NewSize = static_cast<ULONGLONG>(newsize);

	// Create a new event-based NativeOverlapped structure (VirtualDiskAsyncResult will take ownership of these)
	ManualResetEvent^ waithandle = gcnew ManualResetEvent(false);
	NativeOverlapped* overlapped = Overlapped(0, 0, waithandle->SafeWaitHandle->DangerousGetHandle(), nullptr).Pack(nullptr, nullptr);

	// Begin the asynchronous virtual disk operation 
	DWORD result = ExpandVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), static_cast<EXPAND_VIRTUAL_DISK_FLAG>(flags), &params, reinterpret_cast<LPOVERLAPPED>(overlapped));

	// Transfer ownership of all asynchronous resources to a VirtualDiskAsyncResult instance before checking the result
	VirtualDiskAsyncResult^ asyncresult = gcnew VirtualDiskAsyncResult(m_handle, waithandle, overlapped, cancellation, progress);

	// If the request to create the disk failed, complete the operation synchronously to clean everything up
	if(result != ERROR_IO_PENDING) asyncresult->CompleteSynchronously(result);

	return asyncresult;
}

//-----------------------------------------------------------------------------
// VirtualDisk::BeginResize (private)
//
// Starts an asynchronous resize operation
//
// Arguments:
//
//	newsize			- Requested new size of the virtual disk
//	flags			- Resize operation flags
//	cancellation	- CancellationToken for the asynchronous operation
//	progress		- IProgress<int> on which to report operation progress

IAsyncResult^ VirtualDisk::BeginResize(__int64 newsize, VirtualDiskResizeFlags flags, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);

	zero_init<RESIZE_VIRTUAL_DISK_PARAMETERS> params;
	params.Version = RESIZE_VIRTUAL_DISK_VERSION_1;
	params.Version1.NewSize = static_cast<ULONGLONG>(newsize);

	// Create a new event-based NativeOverlapped structure (VirtualDiskAsyncResult will take ownership of these)
	ManualResetEvent^ waithandle = gcnew ManualResetEvent(false);
	NativeOverlapped* overlapped = Overlapped(0, 0, waithandle->SafeWaitHandle->DangerousGetHandle(), nullptr).Pack(nullptr, nullptr);

	// Begin the asynchronous virtual disk operation 
	DWORD result = ResizeVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), static_cast<RESIZE_VIRTUAL_DISK_FLAG>(flags), &params, reinterpret_cast<LPOVERLAPPED>(overlapped));

	// Transfer ownership of all asynchronous resources to a VirtualDiskAsyncResult instance before checking the result
	VirtualDiskAsyncResult^ asyncresult = gcnew VirtualDiskAsyncResult(m_handle, waithandle, overlapped, cancellation, progress);

	// If the request to create the disk failed, complete the operation synchronously to clean everything up
	if(result != ERROR_IO_PENDING) asyncresult->CompleteSynchronously(result);

	return asyncresult;
}

//---------------------------------------------------------------------------
// VirtualDisk::BlockSize::get
//
// Gets the virtual disk block size

unsigned int VirtualDisk::BlockSize::get(void)
{
	ULONG infolen = sizeof(GET_VIRTUAL_DISK_INFO);		// Information length
	ULONG infoused = 0;									// Used information bytes

	CHECK_DISPOSED(m_disposed);

	// Initialize a GET_VIRTUAL_DISK_INFO structure for the operation
	zero_init<GET_VIRTUAL_DISK_INFO> info;
	info.Version = GET_VIRTUAL_DISK_INFO_SIZE;

	// Get the requested information about the virtual disk
	DWORD result = GetVirtualDiskInformation(VirtualDiskSafeHandle::Reference(m_handle), &infolen, &info, &infoused);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

	return info.Size.BlockSize;
}

//---------------------------------------------------------------------------
// VirtualDisk::Compact
//
// Synchronously compacts the virtual disk
//
// Arguments:
//
//	NONE

void VirtualDisk::Compact(void)
{
	CHECK_DISPOSED(m_disposed);
	Compact(VirtualDiskCompactFlags::None);
}

//---------------------------------------------------------------------------
// VirtualDisk::Compact
//
// Synchronously compacts the virtual disk
//
// Arguments:
//
//	flags			- Flags to control the compaction operation

void VirtualDisk::Compact(VirtualDiskCompactFlags flags)
{
	CHECK_DISPOSED(m_disposed);
	VirtualDiskAsyncResult::CompleteAsynchronously(BeginCompact(flags, CancellationToken::None, nullptr));
}

//---------------------------------------------------------------------------
// VirtualDisk::CompactAsync
//
// Asynchronously compacts the virtual disk
//
// Arguments:
//
//	flags			- Flags to control the compaction operation
//	cancellation	- Token to monitor for cancellation requests
//	progress		- Optional IProgress<int> on which to report progress

Task^ VirtualDisk::CompactAsync(VirtualDiskCompactFlags flags, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);
	return Task::Factory->FromAsync(BeginCompact(flags, cancellation, progress), gcnew Action<IAsyncResult^>(&VirtualDiskAsyncResult::CompleteAsynchronously));
}

//---------------------------------------------------------------------------
// VirtualDisk::Create (static)
//
// Synchronously creates a virtual disk
//
// Arguments:
//
//	path			- Fully qualified path to the virtual disk file

VirtualDisk^ VirtualDisk::Create(String^ path)
{
	return Create(path, VirtualDiskCreateFlags::None);
}

//---------------------------------------------------------------------------
// VirtualDisk::Create (static)
//
// Synchronously creates a virtual disk
//
// Arguments:
//
//	path			- Fully qualified path to the virtual disk file
//	flags			- Flags to control the behavior of the disk creation

VirtualDisk^ VirtualDisk::Create(String^ path, VirtualDiskCreateFlags flags)
{
	return EndCreate(BeginCreate(path, flags, CancellationToken::None, nullptr));
}

//---------------------------------------------------------------------------
// VirtualDisk::CreateAsync (static)
//
// Asynchronously creates a virtual disk
//
// Arguments:
//
//	path			- Fully qualified path to the virtual disk file
//	flags			- Flags to control the behavior of the disk creation
//	cancellation	- Asynchronous operation cancellation token
//	progress		- Optional IProgress<> on which to report operation progress

Task<VirtualDisk^>^ VirtualDisk::CreateAsync(String^ path, VirtualDiskCreateFlags flags, CancellationToken cancellation, IProgress<int>^ progress)
{
	return Task<VirtualDisk^>::Factory->FromAsync(BeginCreate(path, flags, cancellation, progress), gcnew Func<IAsyncResult^, VirtualDisk^>(&EndCreate));
}

//---------------------------------------------------------------------------
// VirtualDisk::Detach
//
// Detaches the virtual disk
//
// Arguments:
//
//	NONE

void VirtualDisk::Detach(void)
{
	CHECK_DISPOSED(m_disposed);
	Detach(VirtualDiskDetachFlags::None, 0U);
}

//---------------------------------------------------------------------------
// VirtualDisk::Detach
//
// Detaches the virtual disk
//
// Arguments:
//
//	providerflags		- Provider-specific detach operation flags

void VirtualDisk::Detach(unsigned int providerflags)
{
	CHECK_DISPOSED(m_disposed);
	Detach(VirtualDiskDetachFlags::None, providerflags);
}

//---------------------------------------------------------------------------
// VirtualDisk::Detach
//
// Detaches the virtual disk
//
// Arguments:
//
//	flags			- Flags to control the detach operation

void VirtualDisk::Detach(VirtualDiskDetachFlags flags)
{
	CHECK_DISPOSED(m_disposed);
	Detach(flags, 0U);
}

//---------------------------------------------------------------------------
// VirtualDisk::Detach
//
// Detaches the virtual disk
//
// Arguments:
//
//	flags			- Flags to control the detach operation
//	providerflags	- Provider-specific detach operation flags

void VirtualDisk::Detach(VirtualDiskDetachFlags flags, unsigned int providerflags)
{
	CHECK_DISPOSED(m_disposed);

	// This method is not available asynchronously, it's always a synchronous operation
	DWORD result = DetachVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), static_cast<DETACH_VIRTUAL_DISK_FLAG>(flags), providerflags);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);
}

//---------------------------------------------------------------------------
// VirtualDisk::EndCreate (private, static)
//
// Completes an asynchronous create operation
//
// Arguments:
//
//	asyncresult		- IAsyncResult returned from BeginCreate

VirtualDisk^ VirtualDisk::EndCreate(IAsyncResult^ asyncresult)
{
	// Complete the asynchronous operation, and if successful return a VirtualDisk
	VirtualDiskAsyncResult::CompleteAsynchronously(asyncresult);
	return gcnew VirtualDisk(safe_cast<VirtualDiskSafeHandle^>(asyncresult->AsyncState));
}

//---------------------------------------------------------------------------
// VirtualDisk::ExpandAsync
//
// Asynchronously expands the virtual disk
//
// Arguments:
//
//	newsize			- Requested new size of the virtual disk
//	flags			- Flags to control the expansion operation
//	cancellation	- Token to monitor for cancellation requests
//	progress		- Optional IProgress<int> on which to report progress

Task^ VirtualDisk::ExpandAsync(__int64 newsize, VirtualDiskExpandFlags flags, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);
	return Task::Factory->FromAsync(BeginExpand(newsize, flags, cancellation, progress), gcnew Action<IAsyncResult^>(&VirtualDiskAsyncResult::CompleteAsynchronously));
}

//---------------------------------------------------------------------------
// VirtualDisk::FragmentationLevel::get
//
// Gets the fragmentation level percentage of the virtual disk

int VirtualDisk::FragmentationLevel::get(void)
{
	ULONG infolen = sizeof(GET_VIRTUAL_DISK_INFO);		// Information length
	ULONG infoused = 0;									// Used information bytes

	CHECK_DISPOSED(m_disposed);

	// Initialize a GET_VIRTUAL_DISK_INFO structure for the operation
	zero_init<GET_VIRTUAL_DISK_INFO> info;
	info.Version = GET_VIRTUAL_DISK_INFO_FRAGMENTATION;

	// Get the requested information about the virtual disk
	DWORD result = GetVirtualDiskInformation(VirtualDiskSafeHandle::Reference(m_handle), &infolen, &info, &infoused);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

	return static_cast<int>(info.FragmentationPercentage);
}

//-----------------------------------------------------------------------------
// VirtualDisk::Open (static)
//
// Opens an existing virtual disk
//
// Arguments:
//
//	path			- Path to the virtual disk file

VirtualDisk^ VirtualDisk::Open(String^ path)
{
	return Open(VirtualDiskType::Unknown, path, VirtualDiskAccess::All, VirtualDiskOpenFlags::None);
}

//-----------------------------------------------------------------------------
// VirtualDisk::Open (static)
//
// Opens an existing virtual disk
//
// Arguments:
//
//	path			- Path to the virtual disk file
//	access			- Access mask for the opened virtual disk

VirtualDisk^ VirtualDisk::Open(String^ path, VirtualDiskAccess access)
{
	return Open(VirtualDiskType::Unknown, path, access, VirtualDiskOpenFlags::None);
}

//-----------------------------------------------------------------------------
// VirtualDisk::Open (static)
//
// Opens an existing virtual disk
//
// Arguments:
//
//	path			- Path to the virtual disk file
//	access			- Access mask for the opened virtual disk
//	flags			- Flags to control the open operation

VirtualDisk^ VirtualDisk::Open(String^ path, VirtualDiskAccess access, VirtualDiskOpenFlags flags)
{
	return Open(VirtualDiskType::Unknown, path, access, flags);
}

//-----------------------------------------------------------------------------
// VirtualDisk::Open (static)
//
// Opens an existing virtual disk
//
// Arguments:
//
//	type			- Type of virtual disk being opened
//	path			- Path to the virtual disk file

VirtualDisk^ VirtualDisk::Open(VirtualDiskType type, String^ path)
{
	return Open(type, path, VirtualDiskAccess::All, VirtualDiskOpenFlags::None);
}

//-----------------------------------------------------------------------------
// VirtualDisk::Open (static)
//
// Opens an existing virtual disk
//
// Arguments:
//
//	type			- Type of virtual disk being opened
//	path			- Path to the virtual disk file
//	access			- Access mask for the opened virtual disk

VirtualDisk^ VirtualDisk::Open(VirtualDiskType type, String^ path, VirtualDiskAccess access)
{
	return Open(type, path, access, VirtualDiskOpenFlags::None);
}

//-----------------------------------------------------------------------------
// VirtualDisk::Open (static)
//
// Opens an existing virtual disk
//
// Arguments:
//
//	type			- Type of virtual disk being opened
//	path			- Path to the virtual disk file
//	access			- Access mask for the opened virtual disk
//	flags			- Flags to control the open operation

VirtualDisk^ VirtualDisk::Open(VirtualDiskType type, String^ path, VirtualDiskAccess access, VirtualDiskOpenFlags flags)
{
	VIRTUAL_STORAGE_TYPE			storagetype;		// Virtual storage type
	HANDLE							handle;				// Virtual disk object handle

	if(Object::ReferenceEquals(path, nullptr)) throw gcnew ArgumentNullException("path");

	// Pin and convert managed types into unmanaged types and structures
	pin_ptr<const wchar_t> pinpath = PtrToStringChars(path);
	type.ToVIRTUAL_STORAGE_TYPE(&storagetype);

	// Attempt to open the virtual disk using the provided information
	DWORD result = OpenVirtualDisk(&storagetype, pinpath, static_cast<VIRTUAL_DISK_ACCESS_MASK>(access), static_cast<OPEN_VIRTUAL_DISK_FLAG>(flags), __nullptr, &handle);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

	return gcnew VirtualDisk(gcnew VirtualDiskSafeHandle(handle));
}

//---------------------------------------------------------------------------
// VirtualDisk::PhysicalSize::get
//
// Gets the virtual disk physical size

unsigned __int64 VirtualDisk::PhysicalSize::get(void)
{
	ULONG infolen = sizeof(GET_VIRTUAL_DISK_INFO);		// Information length
	ULONG infoused = 0;									// Used information bytes

	CHECK_DISPOSED(m_disposed);

	// Initialize a GET_VIRTUAL_DISK_INFO structure for the operation
	zero_init<GET_VIRTUAL_DISK_INFO> info;
	info.Version = GET_VIRTUAL_DISK_INFO_SIZE;

	// Get the requested information about the virtual disk
	DWORD result = GetVirtualDiskInformation(VirtualDiskSafeHandle::Reference(m_handle), &infolen, &info, &infoused);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

	return info.Size.PhysicalSize;
}

//---------------------------------------------------------------------------
// VirtualDisk::Resize
//
// Synchronously resizes the virtual disk
//
// Arguments:
//
//	newsize			- Requested new size of the virtual disk

void VirtualDisk::Resize(__int64 newsize)
{
	CHECK_DISPOSED(m_disposed);
	Resize(newsize, VirtualDiskResizeFlags::None);
}

//---------------------------------------------------------------------------
// VirtualDisk::Resize
//
// Synchronously compacts the virtual disk
//
// Arguments:
//
//	newsize			- Requested new size of the virtual disk
//	flags			- Flags to control the resize operation

void VirtualDisk::Resize(__int64 newsize, VirtualDiskResizeFlags flags)
{
	CHECK_DISPOSED(m_disposed);
	VirtualDiskAsyncResult::CompleteAsynchronously(BeginResize(newsize, flags, CancellationToken::None, nullptr));
}

//---------------------------------------------------------------------------
// VirtualDisk::ResizeAsync
//
// Asynchronously compacts the virtual disk
//
// Arguments:
//
//	newsize			- Requested new size of the virtual disk
//	flags			- Flags to control the resize operation
//	cancellation	- Token to monitor for cancellation requests
//	progress		- Optional IProgress<int> on which to report progress

Task^ VirtualDisk::ResizeAsync(__int64 newsize, VirtualDiskResizeFlags flags, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);
	return Task::Factory->FromAsync(BeginResize(newsize, flags, cancellation, progress), gcnew Action<IAsyncResult^>(&VirtualDiskAsyncResult::CompleteAsynchronously));
}

//---------------------------------------------------------------------------
// VirtualDisk::SectorSize::get
//
// Gets the virtual disk sector size

unsigned int VirtualDisk::SectorSize::get(void)
{
	ULONG infolen = sizeof(GET_VIRTUAL_DISK_INFO);		// Information length
	ULONG infoused = 0;									// Used information bytes

	CHECK_DISPOSED(m_disposed);

	// Initialize a GET_VIRTUAL_DISK_INFO structure for the operation
	zero_init<GET_VIRTUAL_DISK_INFO> info;
	info.Version = GET_VIRTUAL_DISK_INFO_SIZE;

	// Get the requested information about the virtual disk
	DWORD result = GetVirtualDiskInformation(VirtualDiskSafeHandle::Reference(m_handle), &infolen, &info, &infoused);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

	return info.Size.SectorSize;
}

//---------------------------------------------------------------------------
// VirtualDisk::Type::get
//
// Gets the virtual disk storage type

VirtualDiskType VirtualDisk::Type::get(void)
{
	ULONG infolen = sizeof(GET_VIRTUAL_DISK_INFO);		// Information length
	ULONG infoused = 0;									// Used information bytes

	CHECK_DISPOSED(m_disposed);

	// Initialize a GET_VIRTUAL_DISK_INFO structure for the operation
	zero_init<GET_VIRTUAL_DISK_INFO> info;
	info.Version = GET_VIRTUAL_DISK_INFO_VIRTUAL_STORAGE_TYPE;

	// Get the requested information about the virtual disk
	DWORD result = GetVirtualDiskInformation(VirtualDiskSafeHandle::Reference(m_handle), &infolen, &info, &infoused);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

	return VirtualDiskType(info.VirtualStorageType);
}

//---------------------------------------------------------------------------
// VirtualDisk::UniqueIdentifier::get
//
// Gets the unique identifier of the virtual disk

Guid VirtualDisk::UniqueIdentifier::get(void)
{
	ULONG infolen = sizeof(GET_VIRTUAL_DISK_INFO);		// Information length
	ULONG infoused = 0;									// Used information bytes

	CHECK_DISPOSED(m_disposed);

	// Initialize a GET_VIRTUAL_DISK_INFO structure for the operation
	zero_init<GET_VIRTUAL_DISK_INFO> info;
	info.Version = GET_VIRTUAL_DISK_INFO_IDENTIFIER;

	// Get the requested information about the virtual disk
	DWORD result = GetVirtualDiskInformation(VirtualDiskSafeHandle::Reference(m_handle), &infolen, &info, &infoused);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

	return GuidUtil::UUIDToSysGuid(info.Identifier);
}

//---------------------------------------------------------------------------
// VirtualDisk::UniqueIdentifier::set
//
// Sets the unique identifier of the virtual disk

void VirtualDisk::UniqueIdentifier::set(Guid value)
{
	CHECK_DISPOSED(m_disposed);

	// Initialize the SET_VIRTUAL_DISK_INFO structure for this operation
	zero_init<SET_VIRTUAL_DISK_INFO> info;
	info.Version = SET_VIRTUAL_DISK_INFO_IDENTIFIER;
	info.UniqueIdentifier = GuidUtil::SysGuidToUUID(value);

	// Attempt to set the specified virtual disk information
	DWORD result = SetVirtualDiskInformation(VirtualDiskSafeHandle::Reference(m_handle), &info);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);
}

//---------------------------------------------------------------------------
// VirtualDisk::VirtualSize::get
//
// Gets the virtual disk virtual size

unsigned __int64 VirtualDisk::VirtualSize::get(void)
{
	ULONG infolen = sizeof(GET_VIRTUAL_DISK_INFO);		// Information length
	ULONG infoused = 0;									// Used information bytes

	CHECK_DISPOSED(m_disposed);

	// Initialize a GET_VIRTUAL_DISK_INFO structure for the operation
	zero_init<GET_VIRTUAL_DISK_INFO> info;
	info.Version = GET_VIRTUAL_DISK_INFO_SIZE;

	// Get the requested information about the virtual disk
	DWORD result = GetVirtualDiskInformation(VirtualDiskSafeHandle::Reference(m_handle), &infolen, &info, &infoused);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

	return info.Size.VirtualSize;
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
