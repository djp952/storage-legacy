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
#include "VirtualDisk.h"				// Include VirtualDisk declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki.storage)

// (static)
// TODO: Description
VirtualDiskAsyncStatus VirtualDisk::AsyncOperationStatus(IAsyncResult^ asyncResult)
{
	VIRTUAL_DISK_PROGRESS			progress;			// Operation progress
	DWORD							dwResult;			// Result from function call

	// Cast the IAsyncResult object back into a VirtualDiskAsyncResult object
	VirtualDiskAsyncResult^ async = safe_cast<VirtualDiskAsyncResult^>(asyncResult);
	VirtualDisk^ disk = safe_cast<VirtualDisk^>(async->AsyncState);

	CHECK_DISPOSED(disk->m_disposed);

	// Get the current asynchronous operation status information
	dwResult = GetVirtualDiskOperationProgress(disk->m_handle, async, &progress);
	if(dwResult != ERROR_SUCCESS) throw gcnew Win32Exception(dwResult);

	// Convert the information into a value class and return it
	return VirtualDiskAsyncStatus(&progress);
}

//---------------------------------------------------------------------------
// VirtualDisk::Attach
//
// Attaches the virtual disk
//
// Arguments:
//
//	params				- Virtual disk attach parameters
//	securityDescriptor	- Optional security descriptor to use for the attached disk

void VirtualDisk::Attach(VirtualDiskAttachParameters^ params, FileSecurity^ securityDescriptor)
{
	CHECK_DISPOSED(m_disposed);

	// Execute the asynchronous version of the operation and throw if an error occurs
	VirtualDiskAsyncStatus state = CompleteAsyncOperation(BeginAttach(params, securityDescriptor));
	if(state.Status != ERROR_SUCCESS) throw gcnew Win32Exception(state.Status);
}

//-----------------------------------------------------------------------------
// VirtualDisk::BeginAttach (static)
//
// Attaches the virtual hard disk
//
// Arguments:
//
//	params				- Virtual disk creation parameters
//	securityDescriptor	- Security descriptor for the attached disk

IAsyncResult^ VirtualDisk::BeginAttach(VirtualDiskAttachParameters^ params, FileSecurity^ securityDescriptor)
{
	PATTACH_VIRTUAL_DISK_PARAMETERS	pAttachParams;		// Attach parameters
	PSECURITY_DESCRIPTOR			psd = NULL;			// Security descriptor
	PinnedBytePtr					pinSD;				// Pinned security descriptor data
	DWORD							dwResult;			// Result from function call

	if(params == nullptr) throw gcnew ArgumentNullException("params");

	// Pin and convert managed types into unmanaged types and structures
	params->ToATTACH_VIRTUAL_DISK_PARAMETERS(&pAttachParams);

	try {

		// If a managed security descriptor was provided, convert it into a binary security descriptor
		// and pin the array into memory for the API call
		if(securityDescriptor != nullptr) {

			pinSD = &(securityDescriptor->GetSecurityDescriptorBinaryForm()[0]);
			psd = reinterpret_cast<PSECURITY_DESCRIPTOR>(pinSD);
		}

		// Create and initialize the OVERLAPPED asynchronous result object
		VirtualDiskAsyncResult^ asyncResult = gcnew VirtualDiskAsyncResult(m_handle, this);

		// Attempt to attach the virtual disk using the provided parameters
		dwResult = AttachVirtualDisk(m_handle, psd, params->ToATTACH_VIRTUAL_DISK_FLAG(), 0,
			pAttachParams, asyncResult);
		if(dwResult != ERROR_IO_PENDING) { delete asyncResult; throw gcnew Win32Exception(dwResult); }

		return asyncResult;
	}

	// Always release the allocated ATTACH_VIRTUAL_DISK_PARAMETERS
	finally { VirtualDiskAttachParameters::FreeATTACH_VIRTUAL_DISK_PARAMETERS(pAttachParams); }
}

//-----------------------------------------------------------------------------
// VirtualDisk::BeginCompact
//
// Compacts the virtual disk
//
// Arguments:
//
//	NONE

IAsyncResult^ VirtualDisk::BeginCompact(void)
{
	COMPACT_VIRTUAL_DISK_PARAMETERS		params;
	DWORD								dwResult;

	CHECK_DISPOSED(m_disposed);

	// Initialize the compact parameters structure
	ZeroMemory(&params, sizeof(COMPACT_VIRTUAL_DISK_PARAMETERS));
	params.Version = COMPACT_VIRTUAL_DISK_VERSION_1;

	// Create and initialize the OVERLAPPED asynchronous result object
	VirtualDiskAsyncResult^ asyncResult = gcnew VirtualDiskAsyncResult(m_handle, this);

	// Attempt to compact the virtual disk
	dwResult = CompactVirtualDisk(m_handle, COMPACT_VIRTUAL_DISK_FLAG_NONE, &params, asyncResult);
	if(dwResult != ERROR_IO_PENDING) { delete asyncResult; throw gcnew Win32Exception(dwResult); }

	return asyncResult;
}

//-----------------------------------------------------------------------------
// VirtualDisk::BeginCreate (static)
//
// Creates a new virtual hard disk image
//
// Arguments:
//
//	params				- Virtual disk creation parameters
//	securityDescriptor	- Created disk object file security descriptor

IAsyncResult^ VirtualDisk::BeginCreate(VirtualDiskCreateParameters^ params, FileSecurity^ securityDescriptor)
{
	VIRTUAL_STORAGE_TYPE			storageType;		// Virtual storage type
	PinnedStringPtr					pinPath;			// Pinned storage path name
	PCREATE_VIRTUAL_DISK_PARAMETERS	pCreateParams;		// Virtual disk parameters
	PSECURITY_DESCRIPTOR			psd = NULL;			// Security descriptor
	PinnedBytePtr					pinSD;				// Pinned security descriptor data
	HANDLE							handle;				// Virtual disk object handle
	DWORD							dwResult;			// Result from function call

	if(params == nullptr) throw gcnew ArgumentNullException("params");

	// Pin and convert managed types into unmanaged types and structures
	pinPath = PtrToStringChars(params->Path);
	params->Type.ToVIRTUAL_STORAGE_TYPE(&storageType);
	params->ToCREATE_VIRTUAL_DISK_PARAMETERS(&pCreateParams);

	try {

		// If a managed security descriptor was provided, convert it into a binary security descriptor
		// and pin the array into memory for the API call
		if(securityDescriptor != nullptr) {

			pinSD = &(securityDescriptor->GetSecurityDescriptorBinaryForm()[0]);
			psd = reinterpret_cast<PSECURITY_DESCRIPTOR>(pinSD);
		}

		// The order of operations here is to create the SafeHandle without an actual underyling
		// HANDLE and create a VirtualDisk instance against it.  This allows construction of the
		// necessary VirtualDiskAsyncResult OVERLAPPED instance
		VirtualDiskSafeHandle^ safeHandle = gcnew VirtualDiskSafeHandle();
		VirtualDisk^ disk = gcnew VirtualDisk(safeHandle);
		VirtualDiskAsyncResult^ asyncResult = gcnew VirtualDiskAsyncResult(safeHandle, disk);

		// Attempt to create the virtual disk using the provided information
		dwResult = CreateVirtualDisk(&storageType, pinPath, VIRTUAL_DISK_ACCESS_NONE, psd, 
			params->ToCREATE_VIRTUAL_DISK_FLAG(), 0, pCreateParams, asyncResult, &handle);
		if(dwResult != ERROR_IO_PENDING) { delete asyncResult; delete disk; throw gcnew Win32Exception(dwResult); }

		// The operation was successful, assign the underlying HANDLE to be tracked by the SafeHandle
		safeHandle->SetHandle(handle);

		return asyncResult;
	}

	// Always release the allocated CREATE_VIRTUAL_DISK_PARAMETERS
	finally { VirtualDiskCreateParameters::FreeCREATE_VIRTUAL_DISK_PARAMETERS(pCreateParams); }
}

//-----------------------------------------------------------------------------
// VirtualDisk::BeginExpand
//
// Expands the virtual disk
//
// Arguments:
//
//	newSize			- Requested new virtual disk size

IAsyncResult^ VirtualDisk::BeginExpand(__int64 newSize)
{
	EXPAND_VIRTUAL_DISK_PARAMETERS		params;
	DWORD								dwResult;

	CHECK_DISPOSED(m_disposed);

	// Initialize the expand parameters structure
	ZeroMemory(&params, sizeof(EXPAND_VIRTUAL_DISK_PARAMETERS));
	params.Version = EXPAND_VIRTUAL_DISK_VERSION_1;
	params.Version1.NewSize = static_cast<ULONGLONG>(newSize);

	// Create and initialize the OVERLAPPED asynchronous result object
	VirtualDiskAsyncResult^ asyncResult = gcnew VirtualDiskAsyncResult(m_handle, this);

	// Expand the virtual disk
	dwResult = ExpandVirtualDisk(m_handle, EXPAND_VIRTUAL_DISK_FLAG_NONE, &params, asyncResult);
	if(dwResult != ERROR_IO_PENDING) { delete asyncResult; throw gcnew Win32Exception(dwResult); }

	return asyncResult;
}

//-----------------------------------------------------------------------------
// VirtualDisk::BeginResize
//
// Resizes the virtual disk
//
// Arguments:
//
//	newSize			- New size for the virtual disk
//	allowTruncate	- Flag indicating if data truncation/loss is acceptable

IAsyncResult^ VirtualDisk::BeginResize(__int64 newSize, bool allowTruncate)
{
	RESIZE_VIRTUAL_DISK_PARAMETERS		params;
	RESIZE_VIRTUAL_DISK_FLAG			flags;
	DWORD								dwResult;

	CHECK_DISPOSED(m_disposed);

	// Initialize the resize parameters structure
	ZeroMemory(&params, sizeof(RESIZE_VIRTUAL_DISK_PARAMETERS));
	params.Version = RESIZE_VIRTUAL_DISK_VERSION_1;
	params.Version1.NewSize = static_cast<ULONGLONG>(newSize);

	// Set up the RESIZE_VIRTUAL_DISK_FLAG
	flags = (allowTruncate) ? RESIZE_VIRTUAL_DISK_FLAG_ALLOW_UNSAFE_VIRTUAL_SIZE : RESIZE_VIRTUAL_DISK_FLAG_NONE;
	if((newSize == 0) && (!allowTruncate)) flags = RESIZE_VIRTUAL_DISK_FLAG_RESIZE_TO_SMALLEST_SAFE_VIRTUAL_SIZE;

	// Create and initialize the OVERLAPPED asynchronous result object
	VirtualDiskAsyncResult^ asyncResult = gcnew VirtualDiskAsyncResult(m_handle, this);

	// Resize the virtual disk, optionally allowing for data truncation
	dwResult = ResizeVirtualDisk(m_handle, flags, &params, asyncResult);
	if(dwResult != ERROR_IO_PENDING) { delete asyncResult; throw gcnew Win32Exception(dwResult); }

	return asyncResult;
}

//---------------------------------------------------------------------------
// VirtualDisk::Compact
//
// Compacts the virtual disk
//
// Arguments:
//
//	NONE

void VirtualDisk::Compact(void)
{
	CHECK_DISPOSED(m_disposed);

	// Execute the asynchronous version of the operation and throw if an error occurs
	VirtualDiskAsyncStatus state = CompleteAsyncOperation(BeginCompact());
	if(state.Status != ERROR_SUCCESS) throw gcnew Win32Exception(state.Status);
}

//-----------------------------------------------------------------------------
// VirtualDisk::CompleteAsyncOperation (private, static)
//
// Completes any asynchronous operation started with a BeginXXX function
//
// Arguments:
//
//	asyncResult		- AsyncResult object returned by BeginXXX function

VirtualDiskAsyncStatus VirtualDisk::CompleteAsyncOperation(IAsyncResult^ asyncResult)
{
	return VirtualDiskAsyncResult::EndOperation(asyncResult);
}

//-----------------------------------------------------------------------------
// VirtualDisk::Create (static)
//
// Creates a new virtual hard disk image
//
// Arguments:
//
//	params				- Virtual disk creation parameters
//	securityDescriptor	- Created disk object file security descriptor

VirtualDisk^ VirtualDisk::Create(VirtualDiskCreateParameters^ params, FileSecurity^ securityDescriptor)
{
	// EndCreate() will throw an exception on failure, so just chain it to BeginCreate()
	return EndCreate(BeginCreate(params, securityDescriptor));
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
	DWORD				dwResult;					// Result from API call

	CHECK_DISPOSED(m_disposed);

	dwResult = DetachVirtualDisk(m_handle, DETACH_VIRTUAL_DISK_FLAG_NONE, 0);
	if(dwResult != ERROR_SUCCESS) throw gcnew Win32Exception(dwResult);
}

//---------------------------------------------------------------------------
// VirtualDisk::EndCreate (static)
//
// Completes an asynchronous virtual disk creation operation
//
// Arguments:
//
//	asyncResult		- IAsyncResult instance returned by BeginCreate()

VirtualDisk^ VirtualDisk::EndCreate(IAsyncResult^ asyncResult)
{
	VirtualDisk^ disk = safe_cast<VirtualDisk^>(asyncResult->AsyncState);

	// Complete the asynchronous operation, but unlike the other async methods
	// throw an exception if completion failed
	VirtualDiskAsyncStatus status = CompleteAsyncOperation(asyncResult);
	if(status.Status != ERROR_SUCCESS) { delete disk; throw gcnew Win32Exception(status.Status); }

	return disk;
}
	
//---------------------------------------------------------------------------
// VirtualDisk::Expand
//
// Expands the virtual disk
//
// Arguments:
//
//	NONE

void VirtualDisk::Expand(__int64 newSize)
{
	CHECK_DISPOSED(m_disposed);

	// Execute the asynchronous version of the operation and throw if an error occurs
	VirtualDiskAsyncStatus state = CompleteAsyncOperation(BeginExpand(newSize));
	if(state.Status != ERROR_SUCCESS) throw gcnew Win32Exception(state.Status);
}

//---------------------------------------------------------------------------
// VirtualDisk::FragmentationLevel::get
//
// Gets the fragmentation level percentage of the virtual disk

int VirtualDisk::FragmentationLevel::get(void)
{
	GET_VIRTUAL_DISK_INFO				info;
	ULONG								cbInfo = sizeof(GET_VIRTUAL_DISK_INFO);
	ULONG								cbUsed = 0;
	DWORD								dwResult;

	CHECK_DISPOSED(m_disposed);

	// Initialize the GET_VIRTUAL_DISK_INFO structure
	ZeroMemory(&info, sizeof(GET_VIRTUAL_DISK_INFO));
	info.Version = GET_VIRTUAL_DISK_INFO_FRAGMENTATION;

	// Get the requested information 
	dwResult = GetVirtualDiskInformation(m_handle, &cbInfo, &info, &cbUsed);
	if(dwResult != ERROR_SUCCESS) throw gcnew Win32Exception(dwResult);

	return static_cast<int>(info.FragmentationPercentage);
}

//-----------------------------------------------------------------------------
// VirtualDisk::Open (static)
//
// Opens an existing virtual disk
//
// Arguments:
//
//	params				- Virtual disk open parameters

VirtualDisk^ VirtualDisk::Open(VirtualDiskOpenParameters^ params)
{
	VIRTUAL_STORAGE_TYPE			storageType;		// Virtual storage type
	PinnedStringPtr					pinPath;			// Pinned storage path name
	POPEN_VIRTUAL_DISK_PARAMETERS	pOpenParams;		// Virtual disk parameters
	HANDLE							handle;				// Virtual disk object handle
	DWORD							dwResult;			// Result from function call

	if(params == nullptr) throw gcnew ArgumentNullException("params");

	// Pin and convert managed types into unmanaged types and structures
	pinPath = PtrToStringChars(params->Path);
	params->Type.ToVIRTUAL_STORAGE_TYPE(&storageType);
	params->ToOPEN_VIRTUAL_DISK_PARAMETERS(&pOpenParams);

	try {

		// Attempt to open the virtual disk using the provided information
		dwResult = OpenVirtualDisk(&storageType, pinPath, VIRTUAL_DISK_ACCESS_NONE, 
			params->ToOPEN_VIRTUAL_DISK_FLAG(), pOpenParams, &handle);
		if(dwResult != ERROR_SUCCESS) throw gcnew Win32Exception(dwResult);

		// Construct and return the new VirtualDisk instance if successful
		return gcnew VirtualDisk(gcnew VirtualDiskSafeHandle(handle));
	}

	// Always release the allocated OPEN_VIRTUAL_DISK_PARAMETERS
	finally { VirtualDiskOpenParameters::FreeOPEN_VIRTUAL_DISK_PARAMETERS(pOpenParams); }
}

//---------------------------------------------------------------------------
// VirtualDisk::PhysicalPath::get
//
// Gets the physical device object path for the virtual disk

String^ VirtualDisk::PhysicalPath::get(void)
{
	ULONG			cbPath = 0;					// Path length in bytes
	PWCHAR			pwszPath;					// Physical device path
	DWORD			dwResult;					// Result from function call

	CHECK_DISPOSED(m_disposed);

	// Attempt to get the size of the physical path
	dwResult = GetVirtualDiskPhysicalPath(m_handle, &cbPath, NULL);

	// If the disk has not been attached, this will fail with ERROR_DEV_NOT_EXIST
	if(dwResult == ERROR_DEV_NOT_EXIST) return String::Empty;
	else if(dwResult != ERROR_INSUFFICIENT_BUFFER) throw gcnew Win32Exception(dwResult);

	// Allocate the unmanaged buffer for the string
	pwszPath = new WCHAR[cbPath + 1];
	if(!pwszPath) throw gcnew OutOfMemoryException();

	try {

		// Now that the proper buffer exists, attempt the API call again
		dwResult = GetVirtualDiskPhysicalPath(m_handle, &cbPath, pwszPath);
		if(dwResult != ERROR_SUCCESS) throw gcnew Win32Exception(dwResult);

		// Convert the string into a managed String object
		return Marshal::PtrToStringUni(IntPtr(reinterpret_cast<void*>(pwszPath)));
	}

	finally { delete[] pwszPath; }
}

//---------------------------------------------------------------------------
// VirtualDisk::Resize
//
// Resizes the virtual disk
//
// Arguments:
//
//	newSize			- New size for the virtual disk
//	allowTruncate	- Flag indicating if data truncation/loss is acceptable

void VirtualDisk::Resize(__int64 newSize, bool allowTruncate)
{
	CHECK_DISPOSED(m_disposed);

	// Execute the asynchronous version of the operation and throw if an error occurs
	VirtualDiskAsyncStatus state = CompleteAsyncOperation(BeginResize(newSize, allowTruncate));
	if(state.Status != ERROR_SUCCESS) throw gcnew Win32Exception(state.Status);
}

//---------------------------------------------------------------------------
// VirtualDisk::Type::get
//
// Gets the virtual disk storage type

VirtualDiskStorageType VirtualDisk::Type::get(void)
{
	GET_VIRTUAL_DISK_INFO				info;
	ULONG								cbInfo = sizeof(GET_VIRTUAL_DISK_INFO);
	ULONG								cbUsed = 0;
	DWORD								dwResult;

	CHECK_DISPOSED(m_disposed);

	// Initialize the GET_VIRTUAL_DISK_INFO structure
	ZeroMemory(&info, sizeof(GET_VIRTUAL_DISK_INFO));
	info.Version = GET_VIRTUAL_DISK_INFO_VIRTUAL_STORAGE_TYPE;

	// Get the requested information 
	dwResult = GetVirtualDiskInformation(m_handle, &cbInfo, &info, &cbUsed);
	if(dwResult != ERROR_SUCCESS) throw gcnew Win32Exception(dwResult);

	return VirtualDiskStorageType::FromVIRTUAL_STORAGE_TYPE(&info.VirtualStorageType);
}

//---------------------------------------------------------------------------
// VirtualDisk::UniqueIdentifier::get
//
// Gets the unique identifier of the virtual disk

Guid VirtualDisk::UniqueIdentifier::get(void)
{
	GET_VIRTUAL_DISK_INFO				info;
	ULONG								cbInfo = sizeof(GET_VIRTUAL_DISK_INFO);
	ULONG								cbUsed = 0;
	DWORD								dwResult;

	CHECK_DISPOSED(m_disposed);

	// Initialize the GET_VIRTUAL_DISK_INFO structure
	ZeroMemory(&info, sizeof(GET_VIRTUAL_DISK_INFO));
	info.Version = GET_VIRTUAL_DISK_INFO_IDENTIFIER;

	// Get the requested information 
	dwResult = GetVirtualDiskInformation(m_handle, &cbInfo, &info, &cbUsed);
	if(dwResult != ERROR_SUCCESS) throw gcnew Win32Exception(dwResult);

	return VirtualDiskUtil::UUIDToSysGuid(info.Identifier);
}

//---------------------------------------------------------------------------
// VirtualDisk::UniqueIdentifier::set
//
// Sets the unique identifier of the virtual disk

void VirtualDisk::UniqueIdentifier::set(Guid value)
{
	SET_VIRTUAL_DISK_INFO			info;
	DWORD							dwResult;

	CHECK_DISPOSED(m_disposed);

	// Initialize the SET_VIRTUAL_DISK_INFO structure
	ZeroMemory(&info, sizeof(SET_VIRTUAL_DISK_INFO));
	info.Version = SET_VIRTUAL_DISK_INFO_IDENTIFIER;
	info.UniqueIdentifier = VirtualDiskUtil::SysGuidToUUID(value);

	// Attempt to set the specified virtual disk information
	dwResult = SetVirtualDiskInformation(m_handle, &info);
	if(dwResult != ERROR_SUCCESS) throw gcnew Win32Exception(dwResult);
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)
