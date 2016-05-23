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

#include "VirtualDiskAccess.h"
#include "VirtualDiskAsyncResult.h"
#include "VirtualDiskCompactFlags.h"
#include "VirtualDiskOpenFlags.h"
#include "VirtualDiskSafeHandle.h"
#include "VirtualDiskType.h"
#include "VirtualDiskUtil.h"

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
// VirtualDisk::BeginCompact (private)
//
// Starts an asynchronous compact operation
//
// Arguments:
//
//	flags			- Compact operation flags
//	cancellation	- CancellationToken for the asynchronous operation
//	progress		- IProgress<int> on which to report operatioj progress

IAsyncResult^ VirtualDisk::BeginCompact(VirtualDiskCompactFlags flags, CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);

	// Initialize the compact parameters structure
	zero_init<COMPACT_VIRTUAL_DISK_PARAMETERS> params;
	params.Version = COMPACT_VIRTUAL_DISK_VERSION_1;

	// Construct the VirtualDiskAsyncResult instance for this operation
	VirtualDiskAsyncResult^ asyncresult = gcnew VirtualDiskAsyncResult(m_handle, cancellation, progress);

	// Attempt to compact the virtual disk asynchronously using the provided parameters
	DWORD result = CompactVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), static_cast<COMPACT_VIRTUAL_DISK_FLAG>(flags), &params, asyncresult);
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
	VirtualDiskAsyncResult::Complete(BeginCompact(flags, CancellationToken::None, nullptr));
}

//---------------------------------------------------------------------------
// VirtualDisk::CompactAsync
//
// Asynchronously compacts the virtual disk
//
// Arguments:
//
//	NONE

Task^ VirtualDisk::CompactAsync(void)
{
	CHECK_DISPOSED(m_disposed);
	return CompactAsync(VirtualDiskCompactFlags::None, CancellationToken::None, nullptr);
}

//---------------------------------------------------------------------------
// VirtualDisk::CompactAsync
//
// Asynchronously compacts the virtual disk
//
// Arguments:
//
//	flags			- Flags to control the compaction operation

Task^ VirtualDisk::CompactAsync(VirtualDiskCompactFlags flags)
{
	CHECK_DISPOSED(m_disposed);
	return CompactAsync(flags, CancellationToken::None, nullptr);
}

//---------------------------------------------------------------------------
// VirtualDisk::CompactAsync
//
// Asynchronously compacts the virtual disk
//
// Arguments:
//
//	cancellation	- Token to monitor for cancellation requests

Task^ VirtualDisk::CompactAsync(CancellationToken cancellation)
{
	CHECK_DISPOSED(m_disposed);
	return CompactAsync(VirtualDiskCompactFlags::None, cancellation, nullptr);
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

Task^ VirtualDisk::CompactAsync(VirtualDiskCompactFlags flags, CancellationToken cancellation)
{
	CHECK_DISPOSED(m_disposed);
	return CompactAsync(flags, cancellation, nullptr);
}

//---------------------------------------------------------------------------
// VirtualDisk::CompactAsync
//
// Asynchronously compacts the virtual disk
//
// Arguments:
//
//	progress		- Optional IProgress<int> on which to report progress

Task^ VirtualDisk::CompactAsync(IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);
	return CompactAsync(VirtualDiskCompactFlags::None, CancellationToken::None, progress);
}

//---------------------------------------------------------------------------
// VirtualDisk::CompactAsync
//
// Asynchronously compacts the virtual disk
//
// Arguments:
//
//	flags			- Flags to control the compaction operation
//	progress		- Optional IProgress<int> on which to report progress

Task^ VirtualDisk::CompactAsync(VirtualDiskCompactFlags flags, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);
	return CompactAsync(flags, CancellationToken::None, progress);
}

//---------------------------------------------------------------------------
// VirtualDisk::CompactAsync
//
// Asynchronously compacts the virtual disk
//
// Arguments:
//
//	cancellation	- Token to monitor for cancellation requests
//	progress		- Optional IProgress<int> on which to report progress

Task^ VirtualDisk::CompactAsync(CancellationToken cancellation, IProgress<int>^ progress)
{
	CHECK_DISPOSED(m_disposed);
	return CompactAsync(VirtualDiskCompactFlags::None, cancellation, progress);
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

	// Create a Task<> for this operation invoking the Begin method manually since it's non-standard
	return Task::Factory->FromAsync(BeginCompact(flags, cancellation, progress), gcnew Action<IAsyncResult^>(&VirtualDiskAsyncResult::Complete));
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
	// todo: OPEN_PARAMETERS
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

	return VirtualDiskUtil::UUIDToSysGuid(info.Identifier);
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
	info.UniqueIdentifier = VirtualDiskUtil::SysGuidToUUID(value);

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

//----------------------------------------------
// OLD OLD OLD
//
//
//
//
//
//
//
//// (static)
//// TODO: Description
//VirtualDiskAsyncStatus VirtualDisk::AsyncOperationStatus(IAsyncResult^ asyncResult)
//{
//	VIRTUAL_DISK_PROGRESS			progress;			// Operation progress
//	DWORD							dwResult;			// Result from function call
//
//	// Cast the IAsyncResult object back into a VirtualDiskAsyncResult object
//	VirtualDiskAsyncResult^ async = safe_cast<VirtualDiskAsyncResult^>(asyncResult);
//	VirtualDisk^ disk = safe_cast<VirtualDisk^>(async->AsyncState);
//
//	CHECK_DISPOSED(disk->m_disposed);
//
//	// Get the current asynchronous operation status information
//	dwResult = GetVirtualDiskOperationProgress(VirtualDiskSafeHandle::Reference(disk->m_handle), async, &progress);
//	if(dwResult != ERROR_SUCCESS) throw gcnew Win32Exception(dwResult);
//
//	// Convert the information into a value class and return it
//	return VirtualDiskAsyncStatus(&progress);
//}
//
////---------------------------------------------------------------------------
//// VirtualDisk::Attach
////
//// Attaches the virtual disk
////
//// Arguments:
////
////	params				- Virtual disk attach parameters
////	securityDescriptor	- Optional security descriptor to use for the attached disk
//
//void VirtualDisk::Attach(VirtualDiskAttachParameters^ params, FileSecurity^ securityDescriptor)
//{
//	CHECK_DISPOSED(m_disposed);
//
//	// Execute the asynchronous version of the operation and throw if an error occurs
//	VirtualDiskAsyncStatus state = CompleteAsyncOperation(BeginAttach(params, securityDescriptor));
//	if(state.Status != ERROR_SUCCESS) throw gcnew Win32Exception(state.Status);
//}
//
////-----------------------------------------------------------------------------
//// VirtualDisk::BeginAttach (static)
////
//// Attaches the virtual hard disk
////
//// Arguments:
////
////	params				- Virtual disk creation parameters
////	securityDescriptor	- Security descriptor for the attached disk
//
//IAsyncResult^ VirtualDisk::BeginAttach(VirtualDiskAttachParameters^ params, FileSecurity^ securityDescriptor)
//{
//	PATTACH_VIRTUAL_DISK_PARAMETERS	pAttachParams;		// Attach parameters
//	PSECURITY_DESCRIPTOR			psd = NULL;			// Security descriptor
//	PinnedBytePtr					pinSD;				// Pinned security descriptor data
//	DWORD							dwResult;			// Result from function call
//
//	if(params == nullptr) throw gcnew ArgumentNullException("params");
//
//	// Pin and convert managed types into unmanaged types and structures
//	params->ToATTACH_VIRTUAL_DISK_PARAMETERS(&pAttachParams);
//
//	try {
//
//		// If a managed security descriptor was provided, convert it into a binary security descriptor
//		// and pin the array into memory for the API call
//		if(securityDescriptor != nullptr) {
//
//			pinSD = &(securityDescriptor->GetSecurityDescriptorBinaryForm()[0]);
//			psd = reinterpret_cast<PSECURITY_DESCRIPTOR>(pinSD);
//		}
//
//		// Create and initialize the OVERLAPPED asynchronous result object
//		VirtualDiskAsyncResult^ asyncResult = gcnew VirtualDiskAsyncResult(m_handle, this);
//
//		// Attempt to attach the virtual disk using the provided parameters
//		dwResult = AttachVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), psd, params->ToATTACH_VIRTUAL_DISK_FLAG(), 0,
//			pAttachParams, asyncResult);
//		if(dwResult != ERROR_IO_PENDING) { delete asyncResult; throw gcnew Win32Exception(dwResult); }
//
//		return asyncResult;
//	}
//
//	// Always release the allocated ATTACH_VIRTUAL_DISK_PARAMETERS
//	finally { VirtualDiskAttachParameters::FreeATTACH_VIRTUAL_DISK_PARAMETERS(pAttachParams); }
//}
//
//// testing new async result / safe handle / etc
////
//
//VirtualDisk^ VirtualDisk::Create2(String^ path)
//{
//	return EndCreate2(BeginCreate2(path));
//}
//
//IAsyncResult^ VirtualDisk::BeginCreate2(String^ path)
//{
//	WaitHandle^					waithandle;
//	NativeOverlapped*			overlapped;
//
//	(path);
//
//	waithandle = gcnew ManualResetEvent(false);
//	overlapped = Overlapped(0, 0, waithandle->SafeWaitHandle->DangerousGetHandle(), nullptr).Pack(nullptr, nullptr);
//
//	CREATE_VIRTUAL_DISK_PARAMETERS params;
//	zero_init<> here  ... memset(&params, 0, sizeof(CREATE_VIRTUAL_DISK_PARAMETERS));
//	params.Version = CREATE_VIRTUAL_DISK_VERSION_1;
//	params.Version1.MaximumSize = 4194304;		// 4MB
//	params.Version1.SectorSizeInBytes = 512;
//
//	VIRTUAL_STORAGE_TYPE type;
//	type.DeviceId = VIRTUAL_STORAGE_TYPE_DEVICE_VHD;
//	type.VendorId = VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT;
//
//	HANDLE handle;
//	DWORD result = CreateVirtualDisk(&type, L"D:\\test.vhd", VIRTUAL_DISK_ACCESS_ALL, __nullptr, CREATE_VIRTUAL_DISK_FLAG_NONE,
//		0, &params, reinterpret_cast<LPOVERLAPPED>(overlapped), &handle);
//	if(result != ERROR_IO_PENDING) {
//
//		Overlapped::Free(overlapped);
//		throw gcnew Win32Exception(result);
//	}
//		
//	return gcnew VirtualDiskAsyncResult(VirtualDiskAsyncOperation::Create, waithandle, overlapped, gcnew VirtualDiskSafeHandle(handle), nullptr);
//}
//
////-----------------------------------------------------------------------------
//// VirtualDisk::BeginCreate (static)
////
//// Creates a new virtual hard disk image
////
//// Arguments:
////
////	params				- Virtual disk creation parameters
////	securityDescriptor	- Created disk object file security descriptor
//
//IAsyncResult^ VirtualDisk::BeginCreate(VirtualDiskCreateParameters^ params, FileSecurity^ securityDescriptor)
//{
//	VIRTUAL_STORAGE_TYPE			storageType;		// Virtual storage type
//	PinnedStringPtr					pinPath;			// Pinned storage path name
//	PCREATE_VIRTUAL_DISK_PARAMETERS	pCreateParams;		// Virtual disk parameters
//	PSECURITY_DESCRIPTOR			psd = NULL;			// Security descriptor
//	PinnedBytePtr					pinSD;				// Pinned security descriptor data
//	HANDLE							handle;				// Virtual disk object handle
//	DWORD							dwResult;			// Result from function call
//
//	if(params == nullptr) throw gcnew ArgumentNullException("params");
//
//	// Pin and convert managed types into unmanaged types and structures
//	pinPath = PtrToStringChars(params->Path);
//	params->Type.ToVIRTUAL_STORAGE_TYPE(&storageType);
//	params->ToCREATE_VIRTUAL_DISK_PARAMETERS(&pCreateParams);
//
//	try {
//
//		// If a managed security descriptor was provided, convert it into a binary security descriptor
//		// and pin the array into memory for the API call
//		if(securityDescriptor != nullptr) {
//
//			pinSD = &(securityDescriptor->GetSecurityDescriptorBinaryForm()[0]);
//			psd = reinterpret_cast<PSECURITY_DESCRIPTOR>(pinSD);
//		}
//
//		// The order of operations here is to create the SafeHandle without an actual underyling
//		// HANDLE and create a VirtualDisk instance against it.  This allows construction of the
//		// necessary VirtualDiskAsyncResult OVERLAPPED instance
//		VirtualDiskSafeHandle^ safeHandle = nullptr; throw gcnew Exception("NO LONGER IMPLEMENTED");// gcnew VirtualDiskSafeHandle();
//		VirtualDisk^ disk = gcnew VirtualDisk(safeHandle);
//		VirtualDiskAsyncResult^ asyncResult = gcnew VirtualDiskAsyncResult(safeHandle, disk);
//
//		// Attempt to create the virtual disk using the provided information
//		dwResult = CreateVirtualDisk(&storageType, pinPath, VIRTUAL_DISK_ACCESS_NONE, psd, 
//			params->ToCREATE_VIRTUAL_DISK_FLAG(), 0, pCreateParams, asyncResult, &handle);
//		if(dwResult != ERROR_IO_PENDING) { delete asyncResult; delete disk; throw gcnew Win32Exception(dwResult); }
//
//		// The operation was successful, assign the underlying HANDLE to be tracked by the SafeHandle
//		//////safeHandle->SetHandle(handle);
//
//		return asyncResult;
//	}
//
//	// Always release the allocated CREATE_VIRTUAL_DISK_PARAMETERS
//	finally { VirtualDiskCreateParameters::FreeCREATE_VIRTUAL_DISK_PARAMETERS(pCreateParams); }
//}
//
////-----------------------------------------------------------------------------
//// VirtualDisk::BeginExpand
////
//// Expands the virtual disk
////
//// Arguments:
////
////	newSize			- Requested new virtual disk size
//
//IAsyncResult^ VirtualDisk::BeginExpand(__int64 newSize)
//{
//	EXPAND_VIRTUAL_DISK_PARAMETERS		params;
//	DWORD								dwResult;
//
//	CHECK_DISPOSED(m_disposed);
//
//	// Initialize the expand parameters structure
//	ZeroMemory(&params, sizeof(EXPAND_VIRTUAL_DISK_PARAMETERS));
//	params.Version = EXPAND_VIRTUAL_DISK_VERSION_1;
//	params.Version1.NewSize = static_cast<ULONGLONG>(newSize);
//
//	// Create and initialize the OVERLAPPED asynchronous result object
//	VirtualDiskAsyncResult^ asyncResult = gcnew VirtualDiskAsyncResult(m_handle, this);
//
//	// Expand the virtual disk
//	dwResult = ExpandVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), EXPAND_VIRTUAL_DISK_FLAG_NONE, &params, asyncResult);
//	if(dwResult != ERROR_IO_PENDING) { delete asyncResult; throw gcnew Win32Exception(dwResult); }
//
//	return asyncResult;
//}
//
////-----------------------------------------------------------------------------
//// VirtualDisk::BeginResize
////
//// Resizes the virtual disk
////
//// Arguments:
////
////	newSize			- New size for the virtual disk
////	allowTruncate	- Flag indicating if data truncation/loss is acceptable
//
//IAsyncResult^ VirtualDisk::BeginResize(__int64 newSize, bool allowTruncate)
//{
//	RESIZE_VIRTUAL_DISK_PARAMETERS		params;
//	RESIZE_VIRTUAL_DISK_FLAG			flags;
//	DWORD								dwResult;
//
//	CHECK_DISPOSED(m_disposed);
//
//	// Initialize the resize parameters structure
//	ZeroMemory(&params, sizeof(RESIZE_VIRTUAL_DISK_PARAMETERS));
//	params.Version = RESIZE_VIRTUAL_DISK_VERSION_1;
//	params.Version1.NewSize = static_cast<ULONGLONG>(newSize);
//
//	// Set up the RESIZE_VIRTUAL_DISK_FLAG
//	flags = (allowTruncate) ? RESIZE_VIRTUAL_DISK_FLAG_ALLOW_UNSAFE_VIRTUAL_SIZE : RESIZE_VIRTUAL_DISK_FLAG_NONE;
//	if((newSize == 0) && (!allowTruncate)) flags = RESIZE_VIRTUAL_DISK_FLAG_RESIZE_TO_SMALLEST_SAFE_VIRTUAL_SIZE;
//
//	// Create and initialize the OVERLAPPED asynchronous result object
//	VirtualDiskAsyncResult^ asyncResult = gcnew VirtualDiskAsyncResult(m_handle, this);
//
//	// Resize the virtual disk, optionally allowing for data truncation
//	dwResult = ResizeVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), flags, &params, asyncResult);
//	if(dwResult != ERROR_IO_PENDING) { delete asyncResult; throw gcnew Win32Exception(dwResult); }
//
//	return asyncResult;
//}
//
////-----------------------------------------------------------------------------
//// VirtualDisk::CompleteAsyncOperation (private, static)
////
//// Completes any asynchronous operation started with a BeginXXX function
////
//// Arguments:
////
////	asyncResult		- AsyncResult object returned by BeginXXX function
//
//VirtualDiskAsyncStatus VirtualDisk::CompleteAsyncOperation(IAsyncResult^ asyncResult)
//{
//	return VirtualDiskAsyncResult::EndOperation(asyncResult);
//}
//
////-----------------------------------------------------------------------------
//// VirtualDisk::Create (static)
////
//// Creates a new virtual hard disk image
////
//// Arguments:
////
////	params				- Virtual disk creation parameters
////	securityDescriptor	- Created disk object file security descriptor
//
//VirtualDisk^ VirtualDisk::Create(VirtualDiskCreateParameters^ params, FileSecurity^ securityDescriptor)
//{
//	// EndCreate() will throw an exception on failure, so just chain it to BeginCreate()
//	return EndCreate(BeginCreate(params, securityDescriptor));
//}
//
////---------------------------------------------------------------------------
//// VirtualDisk::Detach
////
//// Detaches the virtual disk
////
//// Arguments:
////
////	NONE
//
//void VirtualDisk::Detach(void)
//{
//	DWORD				dwResult;					// Result from API call
//
//	CHECK_DISPOSED(m_disposed);
//
//	dwResult = DetachVirtualDisk(VirtualDiskSafeHandle::Reference(m_handle), DETACH_VIRTUAL_DISK_FLAG_NONE, 0);
//	if(dwResult != ERROR_SUCCESS) throw gcnew Win32Exception(dwResult);
//}
//
////---------------------------------------------------------------------------
//// VirtualDisk::EndCreate (static)
////
//// Completes an asynchronous virtual disk creation operation
////
//// Arguments:
////
////	asyncresult		- IAsyncResult instance returned by BeginCreate()
//
//VirtualDisk^ VirtualDisk::EndCreate2(IAsyncResult^ asyncresult)
//{
//	// The IAsyncResult reference must be a VirtualDiskAsyncResult from BeginCreate()
//	VirtualDiskAsyncResult^ result = safe_cast<VirtualDiskAsyncResult^>(asyncresult);
//	if(result->Operation != VirtualDiskAsyncOperation::Create) throw gcnew InvalidOperationException();
//
//	try {
//
//		// Complete the asynchronous operation and get the final progress information.  If any
//		// errors occurred during creation, throw the status code as a Win32Exception to the caller
//		VIRTUAL_DISK_PROGRESS progress = VirtualDiskAsyncResult::Complete(result);
//		if(progress.OperationStatus != ERROR_SUCCESS) throw gcnew Win32Exception(progress.OperationStatus);
//
//		// Create a new VirtualDisk instance to be returned to the caller
//		return gcnew VirtualDisk(result->VirtualDiskHandle);
//	}
//
//	finally { delete result; }					// Always dispose of the VirtualDiskAsyncResult
//}
//	
//Task<VirtualDisk^>^ VirtualDisk::CreateAsync(String^ path)
//{
//	Func<IAsyncResult^, VirtualDisk^>^ func = gcnew Func<IAsyncResult^, VirtualDisk^>(&VirtualDisk::EndCreate2);
//	return Task<VirtualDisk^>::Factory->FromAsync(BeginCreate2(path), func);
//}
//	
//
//VirtualDisk^ VirtualDisk::EndCreate(IAsyncResult^ asyncResult)
//{
//	VirtualDisk^ disk = safe_cast<VirtualDisk^>(asyncResult->AsyncState);
//
//	// Complete the asynchronous operation, but unlike the other async methods
//	// throw an exception if completion failed
//	VirtualDiskAsyncStatus status = CompleteAsyncOperation(asyncResult);
//	if(status.Status != ERROR_SUCCESS) { delete disk; throw gcnew Win32Exception(status.Status); }
//
//	return disk;
//}
//	
////---------------------------------------------------------------------------
//// VirtualDisk::Expand
////
//// Expands the virtual disk
////
//// Arguments:
////
////	NONE
//
//void VirtualDisk::Expand(__int64 newSize)
//{
//	CHECK_DISPOSED(m_disposed);
//
//	// Execute the asynchronous version of the operation and throw if an error occurs
//	VirtualDiskAsyncStatus state = CompleteAsyncOperation(BeginExpand(newSize));
//	if(state.Status != ERROR_SUCCESS) throw gcnew Win32Exception(state.Status);
//}
//
////-----------------------------------------------------------------------------
//// VirtualDisk::Open (static)
////
//// Opens an existing virtual disk
////
//// Arguments:
////
////	params				- Virtual disk open parameters
//
//VirtualDisk^ VirtualDisk::Open(VirtualDiskOpenParameters^ params)
//{
//	VIRTUAL_STORAGE_TYPE			storageType;		// Virtual storage type
//	PinnedStringPtr					pinPath;			// Pinned storage path name
//	POPEN_VIRTUAL_DISK_PARAMETERS	pOpenParams;		// Virtual disk parameters
//	HANDLE							handle;				// Virtual disk object handle
//	DWORD							dwResult;			// Result from function call
//
//	if(params == nullptr) throw gcnew ArgumentNullException("params");
//
//	// Pin and convert managed types into unmanaged types and structures
//	pinPath = PtrToStringChars(params->Path);
//	params->Type.ToVIRTUAL_STORAGE_TYPE(&storageType);
//	params->ToOPEN_VIRTUAL_DISK_PARAMETERS(&pOpenParams);
//
//	try {
//
//		// Attempt to open the virtual disk using the provided information
//		dwResult = OpenVirtualDisk(&storageType, pinPath, VIRTUAL_DISK_ACCESS_NONE, 
//			params->ToOPEN_VIRTUAL_DISK_FLAG(), pOpenParams, &handle);
//		if(dwResult != ERROR_SUCCESS) throw gcnew Win32Exception(dwResult);
//
//		// Construct and return the new VirtualDisk instance if successful
//		return gcnew VirtualDisk(gcnew VirtualDiskSafeHandle(handle));
//	}
//
//	// Always release the allocated OPEN_VIRTUAL_DISK_PARAMETERS
//	finally { VirtualDiskOpenParameters::FreeOPEN_VIRTUAL_DISK_PARAMETERS(pOpenParams); }
//}
//
////---------------------------------------------------------------------------
//// VirtualDisk::Resize
////
//// Resizes the virtual disk
////
//// Arguments:
////
////	newSize			- New size for the virtual disk
////	allowTruncate	- Flag indicating if data truncation/loss is acceptable
//
//void VirtualDisk::Resize(__int64 newSize, bool allowTruncate)
//{
//	CHECK_DISPOSED(m_disposed);
//
//	// Execute the asynchronous version of the operation and throw if an error occurs
//	VirtualDiskAsyncStatus state = CompleteAsyncOperation(BeginResize(newSize, allowTruncate));
//	if(state.Status != ERROR_SUCCESS) throw gcnew Win32Exception(state.Status);
//}
//
//

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
