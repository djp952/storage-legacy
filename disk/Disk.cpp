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
#include "Disk.h"						// Include Disk declarations
#include "GptDisk.h"					// Include GptDisk declarations
#include "MbrDisk.h"					// Include MbrDisk declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki.storage)

//-----------------------------------------------------------------------------
// Disk Constructor (internal)
//
// Arguments:
//
//	handle			- Handle to the open disk object
//	pGeometry		- DISK_GEOMETRY_EX information for the disk
//	pLayout			- DRIVE_LAYOUT_INFORMATION_EX data for the disk

Disk::Disk(HANDLE handle, PDISK_GEOMETRY_EX pGeometry) 
	: m_handle(gcnew DiskSafeHandle(handle)), m_disposed(false), m_pGeometry(NULL)
{
	if(handle == INVALID_HANDLE_VALUE) throw gcnew ArgumentNullException("handle");
	if(!pGeometry) throw gcnew ArgumentNullException("pGeometry");

	// GEOMETRY
	m_pGeometry = new DISK_GEOMETRY_EX();
	if(!m_pGeometry) throw gcnew OutOfMemoryException();
	memcpy(m_pGeometry, pGeometry, sizeof(DISK_GEOMETRY_EX));
}

//---------------------------------------------------------------------------
// Disk Destructor

Disk::~Disk()
{
	delete m_handle; 
	this->!Disk(); 
	m_disposed = true; 
}

//---------------------------------------------------------------------------
// Disk Finalizer

Disk::!Disk()
{
	if(m_pGeometry) delete m_pGeometry;
	m_pGeometry = NULL;
}

//---------------------------------------------------------------------------
// Disk::operator HANDLE (static)

Disk::operator HANDLE(Disk^ rhs) 
{ 
	CHECK_DISPOSED(rhs->m_disposed);
	return rhs->m_handle; 
}

//-----------------------------------------------------------------------------
// Disk::Clean (static)
//
// Removes the boot signature from the master boot record
//
// Arguments:
//
//	device			- Disk device to clean

void Disk::Clean(String^ device)
{
	PinnedStringPtr					pinDevice;				// Pinned device path name
	HANDLE							handle;					// Disk object handle
	DWORD							cbReturned;				// Bytes returned

	// Pin the device name string so it can be passed into CreateFile()
	if(String::IsNullOrEmpty(device)) throw gcnew ArgumentNullException("device");
	pinDevice = PtrToStringChars(device);

	// Attempt to open the specified disk device
	handle = CreateFile(pinDevice, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if(handle == INVALID_HANDLE_VALUE) throw gcnew Win32Exception(GetLastError());

	try { 

		// Attempt to clean the specified disk device
		if(!DeviceIoControl(handle, IOCTL_DISK_DELETE_DRIVE_LAYOUT, NULL, 0, NULL, 0, &cbReturned, NULL))
			throw gcnew Win32Exception(GetLastError());
	}

	finally { CloseHandle(handle); }
}

//-----------------------------------------------------------------------------
// Disk::CompleteAsyncOperation (private, static)
//
// Completes any asynchronous operation started with a BeginXXX function
//
// Arguments:
//
//	asyncResult		- AsyncResult object returned by BeginXXX function

void Disk::CompleteAsyncOperation(IAsyncResult^ asyncResult)
{
	return DiskAsyncResult::EndOperation(asyncResult);
}

//-----------------------------------------------------------------------------
// Disk::InitializeGpt (static)
//
// Initializes the disk and parition table as GPT
//
// Arguments:
//
//	device				- Disk device to initialize
//	diskId				- GPT disk identifier

void Disk::InitializeGpt(String^ device, Guid diskId)
{
	PinnedStringPtr				pinDevice;			// Pinned device path name
	CREATE_DISK					createDisk;			// Disk initialization parameters
	HANDLE						handle;				// Disk object handle
	DWORD						cbReturned;			// Bytes returned

	// Pin the device name string so it can be passed into CreateFile()
	if(String::IsNullOrEmpty(device)) throw gcnew ArgumentNullException("device");
	pinDevice = PtrToStringChars(device);

	// Initialize the CREATE_DISK structure
	ZeroMemory(&createDisk, sizeof(CREATE_DISK));
	createDisk.PartitionStyle = PARTITION_STYLE_GPT;
	createDisk.Gpt.DiskId = DiskUtil::SysGuidToUUID(diskId);
	createDisk.Gpt.MaxPartitionCount = 128;	// <--- always defaults to 128 no matter what you set here

	// Attempt to open the specified disk device
	handle = CreateFile(pinDevice, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if(handle == INVALID_HANDLE_VALUE) throw gcnew Win32Exception(GetLastError());

	try { 

		// Attempt to initialize the specified disk device
		if(!DeviceIoControl(handle, IOCTL_DISK_CREATE_DISK, &createDisk, sizeof(CREATE_DISK), NULL, 0, &cbReturned, NULL))
			throw gcnew Win32Exception(GetLastError());
	}

	finally { CloseHandle(handle); }
}

//-----------------------------------------------------------------------------
// Disk::InitializeMbr (static)
//
// Initializes the disk and parition table as MBR
//
// Arguments:
//
//	device				- Disk device to be initialized
//	signature			- 32-bit MBR signature

void Disk::InitializeMbr(String^ device, int signature)
{
	PinnedStringPtr				pinDevice;			// Pinned device path name
	CREATE_DISK					createDisk;			// Disk initialization parameters
	HANDLE						handle;				// Disk object handle
	DWORD						cbReturned;			// Bytes returned

	// Pin the device name string so it can be passed into CreateFile()
	if(String::IsNullOrEmpty(device)) throw gcnew ArgumentNullException("device");
	pinDevice = PtrToStringChars(device);

	// Initialize the CREATE_DISK structure
	ZeroMemory(&createDisk, sizeof(CREATE_DISK));
	createDisk.PartitionStyle = PARTITION_STYLE_MBR;
	createDisk.Mbr.Signature = static_cast<DWORD>(signature);

	// Attempt to open the specified disk device
	handle = CreateFile(pinDevice, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if(handle == INVALID_HANDLE_VALUE) throw gcnew Win32Exception(GetLastError());

	try { 

		// Attempt to initialize the specified disk device
		if(!DeviceIoControl(handle, IOCTL_DISK_CREATE_DISK, &createDisk, sizeof(CREATE_DISK), NULL, 0, &cbReturned, NULL))
			throw gcnew Win32Exception(GetLastError());
	}

	finally { CloseHandle(handle); }
}

//-----------------------------------------------------------------------------
// Disk::IsWritable::get
//
// Determines if the disk is writable or not

bool Disk::IsWritable::get(void)
{
	CHECK_DISPOSED(m_disposed);
	return DiskApi::IsWritable(m_handle);
}

//-----------------------------------------------------------------------------
// Disk::Offline::get
//
// Determines if the disk is offline

bool Disk::Offline::get(void)
{
	PGET_DISK_ATTRIBUTES		pAttributes;			// Disk attributes

	CHECK_DISPOSED(m_disposed);

	// Get the disk attributes to determine if the disk is offline
	DiskApi::GetDiskAttributes(m_handle, &pAttributes);
	try { return ((pAttributes->Attributes & DISK_ATTRIBUTE_OFFLINE) == DISK_ATTRIBUTE_OFFLINE); }
	finally { pAttributes = DiskApi::FreeGetDiskAttributes(pAttributes); }
}

//-----------------------------------------------------------------------------
// Disk::Open (static)
//
// Opens an disk device
//
// Arguments:
//
//	device			- Path to the disk device
//	access			- Access mode flags
//	share			- Share mode flags

Disk^ Disk::Open(String^ device, System::IO::FileAccess access, System::IO::FileShare share)
{
	PartitionStyle	style;				// Required [out] argument for full version

	// Just call into the full version of the function and discard the [out] argument
	return Open(device, access, share, style);
}

//-----------------------------------------------------------------------------
// Disk::Open (static)
//
// Opens an disk device
//
// Arguments:
//
//	device			- Path to the disk device
//	access			- Access mode flags
//	share			- Share mode flags
//	style			- Returns the disk partition style flag

Disk^ Disk::Open(String^ device, System::IO::FileAccess access, System::IO::FileShare share, PartitionStyle% style)
{
	PinnedStringPtr					pinDevice;				// Pinned device path name
	HANDLE							handle;					// Disk object handle
	PDISK_GEOMETRY_EX				pGeometry = NULL;		// Disk geometry information
	PDRIVE_LAYOUT_INFORMATION_EX	pLayout = NULL;			// Disk layout information

	// Pin the device name string so it can be passed into CreateFile()
	if(String::IsNullOrEmpty(device)) throw gcnew ArgumentNullException("device");
	pinDevice = PtrToStringChars(device);

	// Attempt to open the specified disk device
	handle = CreateFile(pinDevice, static_cast<DWORD>(access), static_cast<DWORD>(share), NULL,
		OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if(handle == INVALID_HANDLE_VALUE) throw gcnew Win32Exception(GetLastError());

	try {

		// Get the DISK_GEOMETRY_EX and DRIVE_LAYOUT_INFORMATION_EX for the disk
		DiskApi::GetDiskGeometryEx(handle, &pGeometry);
		DiskApi::GetDriveLayoutInformationEx(handle, &pLayout);

		// Set the [out] disk partition type flag; allows the caller to cast the result
		style = static_cast<PartitionStyle>(pLayout->PartitionStyle);

		// The partition style code dictates which Disk class to instantiate
		switch(style) {

			case PartitionStyle::Gpt : return gcnew GptDisk(handle, pGeometry, pLayout);
			case PartitionStyle::Mbr : return gcnew MbrDisk(handle, pGeometry, pLayout);
			default: return gcnew Disk(handle, pGeometry);
		}
	}

	catch(Exception^) { 
		
		CloseHandle(handle);				// Handle will not be consumed; close it
		throw;								// Re-throw the original exception
	}

	finally { 
		
		pLayout = DiskApi::FreeDriveLayoutInformationEx(pLayout);
		pGeometry = DiskApi::FreeDiskGeometryEx(pGeometry); 
	}
}

//-----------------------------------------------------------------------------
// Disk::ReadOnly::get
//
// Determines if the disk is read-only

bool Disk::ReadOnly::get(void)
{
	PGET_DISK_ATTRIBUTES		pAttributes;			// Disk attributes

	CHECK_DISPOSED(m_disposed);

	// Get the disk attributes to determine if the disk is offline
	DiskApi::GetDiskAttributes(m_handle, &pAttributes);
	try { return ((pAttributes->Attributes & DISK_ATTRIBUTE_READ_ONLY) == DISK_ATTRIBUTE_READ_ONLY); }
	finally { pAttributes = DiskApi::FreeGetDiskAttributes(pAttributes); }
}

//---------------------------------------------------------------------------
// Disk::SetOffline
//
// Sets or clears the disk OFFLINE flag
//
// Arguments:
//
//	offline		- Boolean OFFLINE flag
//	persist		- TRUE to persist this setting across reboots

void Disk::SetOffline(bool offline, bool persist)
{
	SET_DISK_ATTRIBUTES			attributes;			// Disk attribute data

	CHECK_DISPOSED(m_disposed);

	// Initialize the SET_DISK_ATTRIBUTES structure
	ZeroMemory(&attributes, sizeof(SET_DISK_ATTRIBUTES));
	attributes.Version = sizeof(SET_DISK_ATTRIBUTES);
	attributes.Persist = (persist) ? TRUE : FALSE;
	attributes.Attributes = (offline) ? DISK_ATTRIBUTE_OFFLINE : 0;
	attributes.AttributesMask = DISK_ATTRIBUTE_OFFLINE;

	// Attempt to apply the SET_DISK_ATTRIBUTES to the device
	DiskApi::SetDiskAttributes(m_handle, &attributes);
}

//---------------------------------------------------------------------------
// Disk::SetReadOnly
//
// Sets or clears the disk READ_ONLY flag
//
// Arguments:
//
//	readOnly	- Boolean READ_ONLY flag
//	persist		- TRUE to persist this setting across reboots

void Disk::SetReadOnly(bool readOnly, bool persist)
{
	SET_DISK_ATTRIBUTES			attributes;			// Disk attribute data

	CHECK_DISPOSED(m_disposed);

	// Initialize the SET_DISK_ATTRIBUTES structure
	ZeroMemory(&attributes, sizeof(SET_DISK_ATTRIBUTES));
	attributes.Version = sizeof(SET_DISK_ATTRIBUTES);
	attributes.Persist = (persist) ? TRUE : FALSE;
	attributes.Attributes = (readOnly) ? DISK_ATTRIBUTE_READ_ONLY : 0;
	attributes.AttributesMask = DISK_ATTRIBUTE_READ_ONLY;

	// Attempt to apply the SET_DISK_ATTRIBUTES to the device
	DiskApi::SetDiskAttributes(m_handle, &attributes);
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)
