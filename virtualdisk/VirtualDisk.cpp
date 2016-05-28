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
#include "VirtualDiskAttachFlags.h"
#include "VirtualDiskAttachParameters.h"
#include "VirtualDiskAsyncResult.h"
#include "VirtualDiskCompactFlags.h"
#include "VirtualDiskCompactParameters.h"
#include "VirtualDiskCreateFlags.h"
#include "VirtualDiskCreateParameters.h"
#include "VirtualDiskDetachFlags.h"
#include "VirtualDiskDetachParameters.h"
#include "VirtualDiskExpandFlags.h"
#include "VirtualDiskExpandParameters.h"
#include "VirtualDiskMetadataCollection.h"
#include "VirtualDiskMergeFlags.h"
#include "VirtualDiskMergeParameters.h"
#include "VirtualDiskMirrorFlags.h"
#include "VirtualDiskMirrorParameters.h"
#include "VirtualDiskOpenFlags.h"
#include "VirtualDiskOpenParameters.h"
#include "VirtualDiskResizeFlags.h"
#include "VirtualDiskResizeParameters.h"
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
	m_metadata = gcnew VirtualDiskMetadataCollection(handle);
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
// VirtualDisk::AddParentDisk
//
// Adds a parent to a virtual disk opened with CustomDifferencingChain
//
// Arguments:
//
//	path		- Path to the parent virtual disk to be added

void VirtualDisk::AddParentDisk(String^ path)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(path, nullptr)) throw gcnew ArgumentNullException("path");

	pin_ptr<const wchar_t> pinpath = PtrToStringChars(path);
	DWORD result = AddVirtualDiskParent(VirtualDiskSafeHandle::Reference(m_handle), pinpath);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);
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
	Attach(gcnew VirtualDiskAttachParameters(flags));
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
//	params			- Compact operation parameters
//	cancellation	- CancellationToken for the asynchronous operation
//	progress		- IProgress<int> on which to report operation progress

IAsyncResult^ VirtualDisk::BeginCompact(VirtualDiskCompactParameters^ params, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(params, nullptr)) throw gcnew ArgumentNullException("params");

	zero_init<COMPACT_VIRTUAL_DISK_PARAMETERS> compactparams;
	compactparams.Version = COMPACT_VIRTUAL_DISK_VERSION_1;

	// Create a new event-based NativeOverlapped structure (VirtualDiskAsyncResult will take ownership of these)
	ManualResetEvent^ waithandle = gcnew ManualResetEvent(false);
	NativeOverlapped* overlapped = Overlapped(0, 0, waithandle->SafeWaitHandle->DangerousGetHandle(), nullptr).Pack(nullptr, nullptr);

	// Begin the asynchronous virtual disk operation 
	DWORD result = CompactVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), static_cast<COMPACT_VIRTUAL_DISK_FLAG>(params->Flags), 
		&compactparams, reinterpret_cast<LPOVERLAPPED>(overlapped));

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
//	params			- Create operation parameters
//	cancellation	- Asynchronous operation cancellation token
//	progress		- Optional IProgress<> on which to report operation progress

IAsyncResult^ VirtualDisk::BeginCreate(VirtualDiskCreateParameters^ params, CancellationToken cancellation, IProgress<int>^ progress)
{
	VIRTUAL_STORAGE_TYPE			storagetype;			// Virtual storage type
	pin_ptr<uint8_t>				pinsd = __nullptr;		// Pinned security descriptor
	HANDLE							handle = __nullptr;		// Virtual disk object handle

	if(Object::ReferenceEquals(params, nullptr)) throw gcnew ArgumentNullException("params");
	if(Object::ReferenceEquals(params->Path, nullptr)) throw gcnew ArgumentNullException("params.Path");

	// Pin and convert managed types into unmanaged types and structures
	pin_ptr<const wchar_t> pinpath = PtrToStringChars(params->Path);
	params->Type.ToVIRTUAL_STORAGE_TYPE(&storagetype);

	pin_ptr<const wchar_t> pinparentpath = (String::IsNullOrEmpty(params->ParentDiskPath)) ? __nullptr : PtrToStringChars(params->ParentDiskPath);
	pin_ptr<const wchar_t> pinsourcepath = (String::IsNullOrEmpty(params->SourceDiskPath)) ? __nullptr : PtrToStringChars(params->SourceDiskPath);
	pin_ptr<const wchar_t> pinsourcelimitpath = (String::IsNullOrEmpty(params->SourceLimitPath)) ? __nullptr : PtrToStringChars(params->SourceLimitPath);

	// todo: these can be manipulated a bit at runtime to clean up bad parameters - see MSDN documentation
	// todo: throw exceptions for known bad parameters; can do that for open and other operations too,
	// consider adding a .Validate method to the parameter classes
	zero_init<CREATE_VIRTUAL_DISK_PARAMETERS> createparams;
	createparams.Version = CREATE_VIRTUAL_DISK_VERSION_2;
	createparams.Version2.UniqueId = GuidUtil::SysGuidToUUID(params->UniqueIdentifier);
	createparams.Version2.MaximumSize = params->MaximumSize;
	createparams.Version2.BlockSizeInBytes = params->BlockSize;
	createparams.Version2.SectorSizeInBytes = params->SectorSize;
    createparams.Version2.PhysicalSectorSizeInBytes = 0;	// <-- TODO: NOT DOCUMENTED?
	createparams.Version2.ParentPath = pinparentpath;
	createparams.Version2.SourcePath = pinsourcepath;
	createparams.Version2.OpenFlags = static_cast<OPEN_VIRTUAL_DISK_FLAG>(params->OpenFlags);
	params->ParentDiskType.ToVIRTUAL_STORAGE_TYPE(&createparams.Version2.ParentVirtualStorageType);
	params->SourceDiskType.ToVIRTUAL_STORAGE_TYPE(&createparams.Version2.SourceVirtualStorageType);
	createparams.Version2.ResiliencyGuid = GuidUtil::SysGuidToUUID(params->ResiliencyGuid);

	if(IsWindows10OrGreater()) {

		createparams.Version = CREATE_VIRTUAL_DISK_VERSION_3;
        createparams.Version3.SourceLimitPath = pinsourcelimitpath;
		params->BackingStorageType.ToVIRTUAL_STORAGE_TYPE(&createparams.Version3.BackingStorageType);
	}

	// Create a new event-based NativeOverlapped structure (VirtualDiskAsyncResult will take ownership of these)
	ManualResetEvent^ waithandle = gcnew ManualResetEvent(false);
	NativeOverlapped* overlapped = Overlapped(0, 0, waithandle->SafeWaitHandle->DangerousGetHandle(), nullptr).Pack(nullptr, nullptr);

	// If a managed security descriptor was provided, convert it into a pinned binary security descriptor
	if(params->SecurityDescriptor != nullptr) pinsd = &(params->SecurityDescriptor->GetSecurityDescriptorBinaryForm()[0]);

	// Begin the asynchronous virtual disk operation
	DWORD result = CreateVirtualDisk(&storagetype, pinpath, VIRTUAL_DISK_ACCESS_NONE, reinterpret_cast<PSECURITY_DESCRIPTOR>(pinsd), 
		static_cast<CREATE_VIRTUAL_DISK_FLAG>(params->Flags), params->ProviderFlags, &createparams, reinterpret_cast<LPOVERLAPPED>(overlapped), &handle);

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
//	params			- Expand operation parameters
//	cancellation	- CancellationToken for the asynchronous operation
//	progress		- IProgress<int> on which to report operation progress

IAsyncResult^ VirtualDisk::BeginExpand(VirtualDiskExpandParameters^ params, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(params, nullptr)) throw gcnew ArgumentNullException("params");

	zero_init<EXPAND_VIRTUAL_DISK_PARAMETERS> expandparams;
	expandparams.Version = EXPAND_VIRTUAL_DISK_VERSION_1;
	expandparams.Version1.NewSize = params->NewSize;

	// Create a new event-based NativeOverlapped structure (VirtualDiskAsyncResult will take ownership of these)
	ManualResetEvent^ waithandle = gcnew ManualResetEvent(false);
	NativeOverlapped* overlapped = Overlapped(0, 0, waithandle->SafeWaitHandle->DangerousGetHandle(), nullptr).Pack(nullptr, nullptr);

	// Begin the asynchronous virtual disk operation 
	DWORD result = ExpandVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), static_cast<EXPAND_VIRTUAL_DISK_FLAG>(params->Flags), 
		&expandparams, reinterpret_cast<LPOVERLAPPED>(overlapped));

	// Transfer ownership of all asynchronous resources to a VirtualDiskAsyncResult instance before checking the result
	VirtualDiskAsyncResult^ asyncresult = gcnew VirtualDiskAsyncResult(m_handle, waithandle, overlapped, cancellation, progress);

	// If the request to create the disk failed, complete the operation synchronously to clean everything up
	if(result != ERROR_IO_PENDING) asyncresult->CompleteSynchronously(result);

	return asyncresult;
}

//-----------------------------------------------------------------------------
// VirtualDisk::BeginMerge (private)
//
// Starts an asynchronous merge operation
//
// Arguments:
//
//	params			- Merge operation parameters
//	cancellation	- CancellationToken for the asynchronous operation
//	progress		- IProgress<int> on which to report operation progress

IAsyncResult^ VirtualDisk::BeginMerge(VirtualDiskMergeParameters^ params, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(params, nullptr)) throw gcnew ArgumentNullException("params");

	zero_init<MERGE_VIRTUAL_DISK_PARAMETERS> mergeparams;
	mergeparams.Version = MERGE_VIRTUAL_DISK_VERSION_2;
	mergeparams.Version2.MergeSourceDepth = params->SourceDepth;
	mergeparams.Version2.MergeTargetDepth = params->TargetDepth;

	// Create a new event-based NativeOverlapped structure (VirtualDiskAsyncResult will take ownership of these)
	ManualResetEvent^ waithandle = gcnew ManualResetEvent(false);
	NativeOverlapped* overlapped = Overlapped(0, 0, waithandle->SafeWaitHandle->DangerousGetHandle(), nullptr).Pack(nullptr, nullptr);

	// Begin the asynchronous virtual disk operation 
	DWORD result = MergeVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), static_cast<MERGE_VIRTUAL_DISK_FLAG>(params->Flags), 
		&mergeparams, reinterpret_cast<LPOVERLAPPED>(overlapped));

	// Transfer ownership of all asynchronous resources to a VirtualDiskAsyncResult instance before checking the result
	VirtualDiskAsyncResult^ asyncresult = gcnew VirtualDiskAsyncResult(m_handle, waithandle, overlapped, cancellation, progress);

	// If the request to create the disk failed, complete the operation synchronously to clean everything up
	if(result != ERROR_IO_PENDING) asyncresult->CompleteSynchronously(result);

	return asyncresult;
}

//-----------------------------------------------------------------------------
// VirtualDisk::BeginMirror (private)
//
// Starts an asynchronous mirror operation
//
// Arguments:
//
//	params			- Mirror operation parameters
//	cancellation	- CancellationToken for the asynchronous operation
//	progress		- IProgress<int> on which to report operation progress

IAsyncResult^ VirtualDisk::BeginMirror(VirtualDiskMirrorParameters^ params, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(params, nullptr)) throw gcnew ArgumentNullException("params");
	if(Object::ReferenceEquals(params->TargetPath, nullptr)) throw gcnew ArgumentNullException("params.TargetPath");

	pin_ptr<const wchar_t> pinpath = PtrToStringChars(params->TargetPath);

	zero_init<MIRROR_VIRTUAL_DISK_PARAMETERS> mirrorparams;
	mirrorparams.Version = MIRROR_VIRTUAL_DISK_VERSION_1;
	mirrorparams.Version1.MirrorVirtualDiskPath = pinpath;

	// Create a new event-based NativeOverlapped structure (VirtualDiskAsyncResult will take ownership of these)
	ManualResetEvent^ waithandle = gcnew ManualResetEvent(false);
	NativeOverlapped* overlapped = Overlapped(0, 0, waithandle->SafeWaitHandle->DangerousGetHandle(), nullptr).Pack(nullptr, nullptr);

	// Begin the asynchronous virtual disk operation 
	DWORD result = MirrorVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), static_cast<MIRROR_VIRTUAL_DISK_FLAG>(params->Flags), 
		&mirrorparams, reinterpret_cast<LPOVERLAPPED>(overlapped));

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
//	params			- Resize operation parameters
//	cancellation	- CancellationToken for the asynchronous operation
//	progress		- IProgress<int> on which to report operation progress

IAsyncResult^ VirtualDisk::BeginResize(VirtualDiskResizeParameters^ params, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(params, nullptr)) throw gcnew ArgumentNullException("params");

	zero_init<RESIZE_VIRTUAL_DISK_PARAMETERS> resizeparams;
	resizeparams.Version = RESIZE_VIRTUAL_DISK_VERSION_1;
	resizeparams.Version1.NewSize = params->NewSize;

	// Create a new event-based NativeOverlapped structure (VirtualDiskAsyncResult will take ownership of these)
	ManualResetEvent^ waithandle = gcnew ManualResetEvent(false);
	NativeOverlapped* overlapped = Overlapped(0, 0, waithandle->SafeWaitHandle->DangerousGetHandle(), nullptr).Pack(nullptr, nullptr);

	// Begin the asynchronous virtual disk operation 
	DWORD result = ResizeVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), static_cast<RESIZE_VIRTUAL_DISK_FLAG>(params->Flags), 
		&resizeparams, reinterpret_cast<LPOVERLAPPED>(overlapped));

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
// VirtualDisk::BreakMirror
//
// Breaks the virtual disk mirror
//
// Arguments:
//
//	NONE

void VirtualDisk::BreakMirror(void)
{
	CHECK_DISPOSED(m_disposed);

	DWORD result = BreakMirrorVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle));
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);
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
	Compact(gcnew VirtualDiskCompactParameters(flags));
}

//---------------------------------------------------------------------------
// VirtualDisk::Compact
//
// Synchronously compacts the virtual disk
//
// Arguments:
//
//	params			- Parameters to control the compaction operation

void VirtualDisk::Compact(VirtualDiskCompactParameters^ params)
{
	CHECK_DISPOSED(m_disposed);
	VirtualDiskAsyncResult::CompleteAsynchronously(BeginCompact(params, CancellationToken::None, nullptr));
}

//---------------------------------------------------------------------------
// VirtualDisk::CompactAsync
//
// Asynchronously compacts the virtual disk
//
// Arguments:
//
//	params			- Parameters to control the compaction operation
//	cancellation	- Token to monitor for cancellation requests
//	progress		- Optional IProgress<int> on which to report progress

Task^ VirtualDisk::CompactAsync(VirtualDiskCompactParameters^ params, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);
	return Task::Factory->FromAsync(BeginCompact(params, cancellation, progress), gcnew Action<IAsyncResult^>(&VirtualDiskAsyncResult::CompleteAsynchronously));
}

//---------------------------------------------------------------------------
// VirtualDisk::Create (static)
//
// Synchronously creates a virtual disk
//
// Arguments:
//
//	path			- Fully qualified path to the virtual disk file
//	type			- Virtual disk type to be created
//	maxsize			- Maximum size of the created virtual disk

VirtualDisk^ VirtualDisk::Create(String^ path, VirtualDiskType type, unsigned __int64 maxsize)
{
	return Create(gcnew VirtualDiskCreateParameters(path, type, maxsize));
}

//---------------------------------------------------------------------------
// VirtualDisk::Create (static)
//
// Synchronously creates a virtual disk
//
// Arguments:
//
//	path			- Fully qualified path to the virtual disk file
//	type			- Virtual disk type to be created
//	maxsize			- Maximum size of the created virtual disk
//	flags			- Flags to control the behavior of the disk creation

VirtualDisk^ VirtualDisk::Create(String^ path, VirtualDiskType type, unsigned __int64 maxsize, VirtualDiskCreateFlags flags)
{
	return Create(gcnew VirtualDiskCreateParameters(path, type, maxsize, flags));
}

//---------------------------------------------------------------------------
// VirtualDisk::Create (static)
//
// Synchronously creates a virtual disk
//
// Arguments:
//
//	params		- Parameters and flags for the create operation

VirtualDisk^ VirtualDisk::Create(VirtualDiskCreateParameters^ params)
{
	return EndCreate(BeginCreate(params, CancellationToken::None, nullptr));
}

//---------------------------------------------------------------------------
// VirtualDisk::CreateAsync (static)
//
// Asynchronously creates a virtual disk
//
// Arguments:
//
//	params			- Create operation parameters
//	cancellation	- Asynchronous operation cancellation token
//	progress		- Optional IProgress<> on which to report operation progress

Task<VirtualDisk^>^ VirtualDisk::CreateAsync(VirtualDiskCreateParameters^ params, CancellationToken cancellation, IProgress<int>^ progress)
{
	return Task<VirtualDisk^>::Factory->FromAsync(BeginCreate(params, cancellation, progress), gcnew Func<IAsyncResult^, VirtualDisk^>(&EndCreate));
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
	Detach(gcnew VirtualDiskDetachParameters());
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
	Detach(gcnew VirtualDiskDetachParameters(flags));
}

//---------------------------------------------------------------------------
// VirtualDisk::Detach
//
// Detaches the virtual disk
//
// Arguments:
//
//	params			- Parameters and flags for the operation

void VirtualDisk::Detach(VirtualDiskDetachParameters^ params)
{
	CHECK_DISPOSED(m_disposed);

	if(Object::ReferenceEquals(params, nullptr)) throw gcnew ArgumentNullException("params");

	// This method is not available asynchronously, it's always a synchronous operation
	DWORD result = DetachVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), static_cast<DETACH_VIRTUAL_DISK_FLAG>(params->Flags), params->ProviderFlags);
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
// VirtualDisk::Expand
//
// Synchronously expands the virtual disk
//
// Arguments:
//
//	newsize			- Requested new size of the virtual disk

void VirtualDisk::Expand(unsigned __int64 newsize)
{
	CHECK_DISPOSED(m_disposed);
	Expand(gcnew VirtualDiskExpandParameters(newsize));
}

//---------------------------------------------------------------------------
// VirtualDisk::Expand
//
// Synchronously expands the virtual disk
//
// Arguments:
//
//	newsize			- Requested new size of the virtual disk
//	flags			- Flags to control the expand operation

void VirtualDisk::Expand(unsigned __int64 newsize, VirtualDiskExpandFlags flags)
{
	CHECK_DISPOSED(m_disposed);
	Expand(gcnew VirtualDiskExpandParameters(newsize, flags));
}

//---------------------------------------------------------------------------
// VirtualDisk::Expand
//
// Synchronously expands the virtual disk
//
// Arguments:
//
//	params		- Expand operation parameters

void VirtualDisk::Expand(VirtualDiskExpandParameters^ params)
{
	CHECK_DISPOSED(m_disposed);
	VirtualDiskAsyncResult::CompleteAsynchronously(BeginExpand(params, CancellationToken::None, nullptr));
}

//---------------------------------------------------------------------------
// VirtualDisk::ExpandAsync
//
// Asynchronously expands the virtual disk
//
// Arguments:
//
//	params			- Expand operation parameters
//	cancellation	- Token to monitor for cancellation requests
//	progress		- Optional IProgress<int> on which to report progress

Task^ VirtualDisk::ExpandAsync(VirtualDiskExpandParameters^ params, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);
	return Task::Factory->FromAsync(BeginExpand(params, cancellation, progress), gcnew Action<IAsyncResult^>(&VirtualDiskAsyncResult::CompleteAsynchronously));
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

//---------------------------------------------------------------------------
// VirtualDisk::GetAllAttachedVirtualDisks (static)
//
// Gets an enumerable collection of all attached virtual disks, these are standard
// DOS paths (D:\blah.vhdx), not physical device paths as the API name impies
//
// Arguments:
//
//	NONE

IReadOnlyList<String^>^ VirtualDisk::GetAllAttachedVirtualDisks(void)
{
	ULONG				cb = 0;					// Required buffer length in bytes
	array<Byte>^		buffer;					// Managed output buffer

	// Determine how much space is required to hold all the path strings, and
	// if there are none return an empty string collection
	DWORD result = GetAllAttachedVirtualDiskPhysicalPaths(&cb, __nullptr);
	if(result == ERROR_SUCCESS) return gcnew List<String^>();
	else if(result != ERROR_INSUFFICIENT_BUFFER) throw gcnew Win32Exception(result);

	// This is one of those really annoying API calls that doesn't tell you the right
	// buffer size unless you call it multiple times in a loop ...
	while(result == ERROR_INSUFFICIENT_BUFFER) {
		
		buffer = gcnew array<Byte>(cb);
		pin_ptr<uint8_t> pinbuffer = &buffer[0];
		result = GetAllAttachedVirtualDiskPhysicalPaths(&cb, reinterpret_cast<wchar_t*>(pinbuffer));
	}

	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

	// The returned buffer is a UTF-16 "multi sz", containing multiple null terminated strings
	// followed by a trailing null terminator.  Convert into a String^ and split it up for the List<>
	return gcnew List<String^>(Encoding::Unicode->GetString(buffer)->Split(gcnew array<Char>{'\0'}, StringSplitOptions::RemoveEmptyEntries));
}

//---------------------------------------------------------------------------
// VirtualDisk::Merge
//
// Synchronously merges a disk within the differencing chain
//
// Arguments:
//
//	sourcedepth		- Source disk depth in the differencing chain
//	targetdepth		- Target disk depth in the differencing chain

void VirtualDisk::Merge(unsigned int sourcedepth, unsigned int targetdepth)
{
	CHECK_DISPOSED(m_disposed);
	Merge(gcnew VirtualDiskMergeParameters(sourcedepth, targetdepth));
}

//---------------------------------------------------------------------------
// VirtualDisk::Merge
//
// Synchronously merges a disk within the differencing chain
//
// Arguments:
//
//	sourcedepth		- Source disk depth in the differencing chain
//	targetdepth		- Target disk depth in the differencing chain
//	flags			- Flags to control the merge operation

void VirtualDisk::Merge(unsigned int sourcedepth, unsigned int targetdepth, VirtualDiskMergeFlags flags)
{
	CHECK_DISPOSED(m_disposed);
	Merge(gcnew VirtualDiskMergeParameters(sourcedepth, targetdepth, flags));
}

//---------------------------------------------------------------------------
// VirtualDisk::Merge
//
// Synchronously merges a disk within the differencing chain
//
// Arguments:
//
//	params			- Merge operation parameters

void VirtualDisk::Merge(VirtualDiskMergeParameters^ params)
{
	CHECK_DISPOSED(m_disposed);
	VirtualDiskAsyncResult::CompleteAsynchronously(BeginMerge(params, CancellationToken::None, nullptr));
}

//---------------------------------------------------------------------------
// VirtualDisk::MergeAsync
//
// Asynchronously merges a disk within the differencing chain
//
// Arguments:
//
//	params			- Merge operation parameters
//	cancellation	- Token to monitor for cancellation requests
//	progress		- Optional IProgress<int> on which to report progress

Task^ VirtualDisk::MergeAsync(VirtualDiskMergeParameters^ params, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);
	return Task::Factory->FromAsync(BeginMerge(params, cancellation, progress), gcnew Action<IAsyncResult^>(&VirtualDiskAsyncResult::CompleteAsynchronously));
}

//---------------------------------------------------------------------------
// VirtualDisk::Metadata::get
//
// Gets a reference to the metadata collection for this virtual disk

VirtualDiskMetadataCollection^ VirtualDisk::Metadata::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return m_metadata;
}
	
//---------------------------------------------------------------------------
// VirtualDisk::Mirror
//
// Synchronously mirrors the virtual disk
//
// Arguments:
//
//	targetpath		- Target virtual disk file path

void VirtualDisk::Mirror(String^ targetpath)
{
	CHECK_DISPOSED(m_disposed);
	Mirror(gcnew VirtualDiskMirrorParameters(targetpath));
}

//---------------------------------------------------------------------------
// VirtualDisk::Mirror
//
// Synchronously mirrors the virtual disk
//
// Arguments:
//
//	targetpath		- Target virtual disk file path
//	flags			- Flags to control the mirror operation

void VirtualDisk::Mirror(String^ targetpath, VirtualDiskMirrorFlags flags)
{
	CHECK_DISPOSED(m_disposed);
	Mirror(gcnew VirtualDiskMirrorParameters(targetpath, flags));
}

//---------------------------------------------------------------------------
// VirtualDisk::Mirror
//
// Synchronously mirrors the virtual disk
//
// Arguments:
//
//	params			- Mirror operation parameters

void VirtualDisk::Mirror(VirtualDiskMirrorParameters^ params)
{
	CHECK_DISPOSED(m_disposed);
	VirtualDiskAsyncResult::CompleteAsynchronously(BeginMirror(params, CancellationToken::None, nullptr));
}

//---------------------------------------------------------------------------
// VirtualDisk::MirrorAsync
//
// Asynchronously mirrors the virtual disk
//
// Arguments:
//
//	params			- Mirror operation parameters
//	cancellation	- Token to monitor for cancellation requests
//	progress		- Optional IProgress<int> on which to report progress

Task^ VirtualDisk::MirrorAsync(VirtualDiskMirrorParameters^ params, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);
	return Task::Factory->FromAsync(BeginMirror(params, cancellation, progress), gcnew Action<IAsyncResult^>(&VirtualDiskAsyncResult::CompleteAsynchronously));
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
	return Open(gcnew VirtualDiskOpenParameters(path));
}

//-----------------------------------------------------------------------------
// VirtualDisk::Open (static)
//
// Opens an existing virtual disk
//
// Arguments:
//
//	path			- Path to the virtual disk file
//	flags			- Flags to control the open operation

VirtualDisk^ VirtualDisk::Open(String^ path, VirtualDiskOpenFlags flags)
{
	return Open(gcnew VirtualDiskOpenParameters(path, flags));
}

//-----------------------------------------------------------------------------
// VirtualDisk::Open (static)
//
// Opens an existing virtual disk
//
// Arguments:
//
//	params		- Open operation parameters

VirtualDisk^ VirtualDisk::Open(VirtualDiskOpenParameters^ params)
{
	VIRTUAL_STORAGE_TYPE			storagetype;		// Virtual storage type
	HANDLE							handle;				// Virtual disk object handle

	if(Object::ReferenceEquals(params, nullptr)) throw gcnew ArgumentNullException("params");
	if(Object::ReferenceEquals(params->Path, nullptr)) throw gcnew ArgumentNullException("params.Path");

	// Pin and convert managed types into unmanaged types and structures
	pin_ptr<const wchar_t> pinpath = PtrToStringChars(params->Path);
	params->Type.ToVIRTUAL_STORAGE_TYPE(&storagetype);

	// Open operation parameters (zero-initialized)
	zero_init<OPEN_VIRTUAL_DISK_PARAMETERS> openparams;

	// Access to the virtual disk is automatic here, always NONE for the V2/V3 API
	VIRTUAL_DISK_ACCESS_MASK access = VIRTUAL_DISK_ACCESS_NONE;

	// .ISO files can only be opened in read-only mode using OPEN_VIRTUAL_DISK_VERSION_1
	if((params->Type == VirtualDiskType::ISO) || 
		((params->Type == VirtualDiskType::Unknown) && System::IO::Path::GetExtension(params->Path)->ToLower() == ".iso")) {

		access = VIRTUAL_DISK_ACCESS_READ;
		openparams.Version = OPEN_VIRTUAL_DISK_VERSION_1;
	}

	// .VHD/.VHDX files can be opened using OPEN_VIRTUAL_DISK_VERSION_2/OPEN_VIRTUAL_DISK_VERSION_3
	else {

		openparams.Version = OPEN_VIRTUAL_DISK_VERSION_2;
		openparams.Version2.GetInfoOnly = (params->InformationOnly) ? TRUE : FALSE;
		openparams.Version2.ReadOnly = (params->ReadOnlyBackingStore) ? TRUE : FALSE;
		openparams.Version2.ResiliencyGuid = GuidUtil::SysGuidToUUID(params->ResiliencyGuid);

		// Windows 10 and higher supports OPEN_VIRTUAL_DISK_VERSION_3
		if(IsWindows10OrGreater()) {

			openparams.Version = OPEN_VIRTUAL_DISK_VERSION_3;
			openparams.Version3.SnapshotId = GuidUtil::SysGuidToUUID(params->SnapshotIdentifier);
		}
	}

	// Attempt to open the virtual disk using the provided information
	DWORD result = OpenVirtualDisk(&storagetype, pinpath, access, static_cast<OPEN_VIRTUAL_DISK_FLAG>(params->Flags), 
		&openparams, &handle);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

	// Wrap the resultant HANDLE into a VirtualDiskSafeHandle instance
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

void VirtualDisk::Resize(unsigned __int64 newsize)
{
	CHECK_DISPOSED(m_disposed);
	Resize(gcnew VirtualDiskResizeParameters(newsize));
}

//---------------------------------------------------------------------------
// VirtualDisk::Resize
//
// Synchronously resizes the virtual disk
//
// Arguments:
//
//	newsize			- Requested new size of the virtual disk
//	flags			- Flags to control the resize operation

void VirtualDisk::Resize(unsigned __int64 newsize, VirtualDiskResizeFlags flags)
{
	CHECK_DISPOSED(m_disposed);
	Resize(gcnew VirtualDiskResizeParameters(newsize, flags));
}

//---------------------------------------------------------------------------
// VirtualDisk::Resize
//
// Synchronously resizes the virtual disk
//
// Arguments:
//
//	params		- Resize operation parameters

void VirtualDisk::Resize(VirtualDiskResizeParameters^ params)
{
	CHECK_DISPOSED(m_disposed);
	VirtualDiskAsyncResult::CompleteAsynchronously(BeginResize(params, CancellationToken::None, nullptr));
}

//---------------------------------------------------------------------------
// VirtualDisk::ResizeAsync
//
// Asynchronously compacts the virtual disk
//
// Arguments:
//
//	params			- Resize operation parameters
//	cancellation	- Token to monitor for cancellation requests
//	progress		- Optional IProgress<int> on which to report progress

Task^ VirtualDisk::ResizeAsync(VirtualDiskResizeParameters^ params, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);
	return Task::Factory->FromAsync(BeginResize(params, cancellation, progress), gcnew Action<IAsyncResult^>(&VirtualDiskAsyncResult::CompleteAsynchronously));
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
// VirtualDisk::SmallestSafeVirtualSize::get
//
// Gets the smallest safe virtual disk size

unsigned __int64 VirtualDisk::SmallestSafeVirtualSize::get(void)
{
	ULONG infolen = sizeof(GET_VIRTUAL_DISK_INFO);		// Information length
	ULONG infoused = 0;									// Used information bytes

	CHECK_DISPOSED(m_disposed);

	// Initialize a GET_VIRTUAL_DISK_INFO structure for the operation
	zero_init<GET_VIRTUAL_DISK_INFO> info;
	info.Version = GET_VIRTUAL_DISK_INFO_SMALLEST_SAFE_VIRTUAL_SIZE;

	// Get the requested information about the virtual disk
	DWORD result = GetVirtualDiskInformation(VirtualDiskSafeHandle::Reference(m_handle), &infolen, &info, &infoused);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

	return info.SmallestSafeVirtualSize;
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
