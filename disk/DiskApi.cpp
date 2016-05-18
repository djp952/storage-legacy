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
#include "DiskApi.h"					// Include DiskApi declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// DiskApi::FreeDiskGeometryEx (static)
//
// Releases the pointer allocated by GetDiskGeometryEx()
//
// Arguments:
//
//	pGeometry		- Pointer to be released

PDISK_GEOMETRY_EX DiskApi::FreeDiskGeometryEx(PDISK_GEOMETRY_EX pGeometry)
{
	if(pGeometry) delete[] reinterpret_cast<BYTE*>(pGeometry);
	return NULL;
}

//---------------------------------------------------------------------------
// DiskApi::FreeDriveLayoutInformationEx (static)
//
// Releases the pointer allocated by GetDriveLayoutInformationEx()
//
// Arguments:
//
//	pLayout		- Pointer to be released

PDRIVE_LAYOUT_INFORMATION_EX DiskApi::FreeDriveLayoutInformationEx(PDRIVE_LAYOUT_INFORMATION_EX pLayout)
{
	if(pLayout) delete[] reinterpret_cast<BYTE*>(pLayout);
	return NULL;
}

//---------------------------------------------------------------------------
// DiskApi::FreeGetDiskAttributes (static)
//
// Releases the pointer allocated by GetDiskAttributes()
//
// Arguments:
//
//	pLayout		- Pointer to be released

PGET_DISK_ATTRIBUTES DiskApi::FreeGetDiskAttributes(PGET_DISK_ATTRIBUTES pAttributes)
{
	if(pAttributes) delete pAttributes;
	return NULL;
}

//---------------------------------------------------------------------------
// DiskApi::GetDiskAttributes (static)
//
// Gets the GET_DISK_ATTRIBUTES data for an open disk object handle
//
// Arguments:
//
//	handle			- Disk object handle from CreateFile
//	ppAttributes	- Pointer to receive allocated GET_DISK_ATTRIBUTES data

void DiskApi::GetDiskAttributes(HANDLE handle, PGET_DISK_ATTRIBUTES* ppAttributes)
{
	PGET_DISK_ATTRIBUTES	pAttributes;			// Disk attribute information
	DWORD					cbReturned;				// Bytes returned from IOCTL
	DWORD					dwResult;				// Result from function call

	if(handle == INVALID_HANDLE_VALUE) throw gcnew ArgumentNullException("handle");
	if(!ppAttributes) throw gcnew ArgumentNullException("ppAttributes");
	*ppAttributes = NULL;

	// Allocate the GET_DISK_ATTRIBUTES structure
	pAttributes = new GET_DISK_ATTRIBUTES();
	if(!pAttributes) throw gcnew OutOfMemoryException();

	// Attempt to get the requested inforation via DeviceIoControl
	if(!DeviceIoControl(handle, IOCTL_DISK_GET_DISK_ATTRIBUTES, NULL, 0, pAttributes, 
		sizeof(GET_DISK_ATTRIBUTES), &cbReturned, NULL)) {
		
		dwResult = GetLastError();
		pAttributes = FreeGetDiskAttributes(pAttributes);
		throw gcnew Win32Exception(dwResult);
	}

	*ppAttributes = pAttributes;
}

//---------------------------------------------------------------------------
// DiskApi::GetDiskGeometryEx (static)
//
// Gets the DISK_GEOMETRY_EX data for an open disk object handle
//
// Arguments:
//
//	handle		- Disk object handle from CreateFile
//	ppGeometry	- Pointer to receive allocated DISK_GEOMETRY_EX data

void DiskApi::GetDiskGeometryEx(HANDLE handle, PDISK_GEOMETRY_EX* ppGeometry)
{
	DWORD				cbGeometry;				// Unused [out] size argument

	// Invoke the main version of this routine
	GetDiskGeometryEx(handle, &cbGeometry, ppGeometry);
}

//---------------------------------------------------------------------------
// DiskApi::GetDiskGeometryEx (static)
//
// Gets the DISK_GEOMETRY_EX data for an open disk object handle
//
// Arguments:
//
//	handle		- Disk object handle from CreateFile
//	pcbGeometry	- Returns the size of the returned DISK_GEOMETRY_EX buffer
//	ppGeometry	- Pointer to receive allocated DISK_GEOMETRY_EX data

void DiskApi::GetDiskGeometryEx(HANDLE handle, DWORD* pcbGeometry, PDISK_GEOMETRY_EX* ppGeometry)
{
	PDISK_GEOMETRY_EX		pGeometry;				// Disk geometry information
	DWORD					cbGeometry = 4096;		// Size of geometry information
	DWORD					cbReturned;				// Bytes returned from IOCTL
	DWORD					dwResult;				// Result from function call

	if(handle == INVALID_HANDLE_VALUE) throw gcnew ArgumentNullException("handle");
	if(!pcbGeometry) throw gcnew ArgumentNullException("pcbGeometry");
	if(!ppGeometry) throw gcnew ArgumentNullException("ppGeometry");
	*pcbGeometry = 0;
	*ppGeometry = NULL;

	do {

		dwResult = ERROR_SUCCESS;					// Assume success

		// Allocate the buffer to hold the geometry information
		pGeometry = reinterpret_cast<PDISK_GEOMETRY_EX>(new BYTE[cbGeometry]);
		if(!pGeometry) throw gcnew OutOfMemoryException();

		// Request the geometry information for the disk
		if(!DeviceIoControl(handle, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0, pGeometry, cbGeometry, &cbReturned, NULL)) {
			
			dwResult = GetLastError();
			pGeometry = FreeDiskGeometryEx(pGeometry);
			pGeometry = NULL; 
		}
		
		// If the buffer was insufficient, increase it by another 4K to try again
		if(dwResult == ERROR_INSUFFICIENT_BUFFER) cbGeometry += 4096;

	} while(dwResult == ERROR_INSUFFICIENT_BUFFER);

	// The data could not be retrieved -- throw the underlying Win32 exception
	if(dwResult != ERROR_SUCCESS) throw gcnew Win32Exception(dwResult);

	*pcbGeometry = cbReturned;
	*ppGeometry = pGeometry;
}

//---------------------------------------------------------------------------
// DiskApi::GetDriveLayoutInformationEx (static)
//
// Gets the DRIVE_LAYOUT_INFORMATION_EX data for an open disk object handle
//
// Arguments:
//
//	handle		- Disk object handle from CreateFile
//	ppLayout	- Pointer to receive allocated DRIVE_LAYOUT_INFORMATION_EX data

void DiskApi::GetDriveLayoutInformationEx(HANDLE handle, PDRIVE_LAYOUT_INFORMATION_EX* ppLayout)
{
	DWORD				cbLayout;				// Unused [out] size argument

	// Invoke the main version of this routine
	GetDriveLayoutInformationEx(handle, &cbLayout, ppLayout);
}

//---------------------------------------------------------------------------
// DiskApi::GetDriveLayoutInformationEx (static)
//
// Gets the DRIVE_LAYOUT_INFORMATION_EX data for an open disk object handle
//
// Arguments:
//
//	handle		- Disk object handle from CreateFile
//	pcbLayout	- Size of the returned DRIVE_LAYOUT_INFORMATION_EX buffer
//	ppLayout	- Pointer to receive allocated DRIVE_LAYOUT_INFORMATION_EX data

void DiskApi::GetDriveLayoutInformationEx(HANDLE handle, DWORD* pcbLayout, PDRIVE_LAYOUT_INFORMATION_EX* ppLayout)
{
	PDRIVE_LAYOUT_INFORMATION_EX	pLayout;				// Disk layout information
	DWORD							cbLayout = 4096;		// Size of layout information
	DWORD							cbReturned;				// Bytes returned from IOCTL
	DWORD							dwResult;				// Result from function call

	if(handle == INVALID_HANDLE_VALUE) throw gcnew ArgumentNullException("handle");
	if(!pcbLayout) throw gcnew ArgumentNullException("pcbLayout");
	if(!ppLayout) throw gcnew ArgumentNullException("ppLayout");
	*pcbLayout = 0;
	*ppLayout = NULL;

	do {

		dwResult = ERROR_SUCCESS;					// Assume success

		// Allocate the buffer to hold the layout information
		pLayout = reinterpret_cast<PDRIVE_LAYOUT_INFORMATION_EX>(new BYTE[cbLayout]);
		if(!pLayout) throw gcnew OutOfMemoryException();

		// Request the geometry information for the disk
		if(!DeviceIoControl(handle, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, pLayout, cbLayout, &cbReturned, NULL)) {
			
			dwResult = GetLastError();
			pLayout = FreeDriveLayoutInformationEx(pLayout);
		}
		
		// If the buffer was insufficient, increase it by another 4K to try again
		if(dwResult == ERROR_INSUFFICIENT_BUFFER) cbLayout += 4096;

	} while(dwResult == ERROR_INSUFFICIENT_BUFFER);

	// The data could not be retrieved -- throw the underlying Win32 exception
	if(dwResult != ERROR_SUCCESS) throw gcnew Win32Exception(dwResult);

	*pcbLayout = cbReturned;
	*ppLayout = pLayout;
}

//---------------------------------------------------------------------------
// DiskApi::IsWritable (static)
//
// Gets a flag indicating if the disk object is writable or not
//
// Arguments:
//
//	 handle		- Disk object handle

bool DiskApi::IsWritable(HANDLE handle)
{
	DWORD					cb;				// Bytes returned from IOCTL
	DWORD					dwResult;		// Result from function call

	if(handle == INVALID_HANDLE_VALUE) throw gcnew ArgumentNullException("handle");

	// Issue the IOCTL
	if(DeviceIoControl(handle, IOCTL_DISK_IS_WRITABLE, NULL, 0, NULL, 0, &cb, NULL)) return true;

	// ERROR_WRITE_PROTECT = disk is read-only; anything else indicates an exception code
	dwResult = GetLastError();
	if(dwResult == ERROR_WRITE_PROTECT) return false;
	else throw gcnew Win32Exception(dwResult);
}

//---------------------------------------------------------------------------
// DiskApi::SetDiskAttributes (static)
//
// Applies SET_DISK_ATTRIBUTES data for the disk object
//
// Arguments:
//
//	handle		- Disk object handle
//	pAttributes	- SET_DISK_ATTRIBUTES data to be applied

void DiskApi::SetDiskAttributes(HANDLE handle, PSET_DISK_ATTRIBUTES pAttributes)
{
	DWORD					cbReturned;		// Bytes returned from DeviceIoControl

	if(handle == INVALID_HANDLE_VALUE) throw gcnew ArgumentNullException("handle");
	if(!pAttributes) throw gcnew ArgumentNullException("pAttributes");

	// Set the version in the structure in case the caller didn't
	pAttributes->Version = sizeof(SET_DISK_ATTRIBUTES);

	// Pretty simple operation; just pass our arguments to IOCTL_DISK_SET_DISK_ATTRIBUTES
	if(!DeviceIoControl(handle, IOCTL_DISK_SET_DISK_ATTRIBUTES, pAttributes, sizeof(SET_DISK_ATTRIBUTES), NULL, 0, &cbReturned, NULL))
		throw gcnew Win32Exception(GetLastError());
}

//---------------------------------------------------------------------------
// DiskApi::SetDriveLayoutInformationEx (static)
//
// Applies the DRIVE_LAYOUT_INFORMATION_EX data for the disk object
//
// Arguments:
//
//	handle		- Disk object handle
//	cbLayout	- Length of the PDRIVE_LAYOUT_INFORMATION_EX structure
//	pLayout		- Preconstructed DRIVE_LAYOUT_INFORMATION_EX structure

void DiskApi::SetDriveLayoutInformationEx(HANDLE handle, DWORD cbLayout, PDRIVE_LAYOUT_INFORMATION_EX pLayout)
{
	DWORD								cbReturned;		// Bytes returned from DeviceIoControl

	if(handle == INVALID_HANDLE_VALUE) throw gcnew ArgumentNullException("handle");
	if(!pLayout) throw gcnew ArgumentNullException("pLayout");

	// Pretty simple operation; just pass our arguments to IOCTL_DISK_SET_DRIVE_LAYOUT_EX
	if(!DeviceIoControl(handle, IOCTL_DISK_SET_DRIVE_LAYOUT_EX, pLayout, cbLayout, NULL, 0, &cbReturned, NULL))
		throw gcnew Win32Exception(GetLastError());
}

//---------------------------------------------------------------------------
// DiskApi::SetDriveLayoutInformationGpt (static)
//
// Sets the DRIVE_LAYOUT_INFORMATION_GPT for a disk object
//
// Arguments:
//
//	handle		- Disk object handle
//	pInfo		- Pointer to the new DRIVE_LAYOUT_INFORMATION_GPT data

void DiskApi::SetDriveLayoutInformationGpt(HANDLE handle, PDRIVE_LAYOUT_INFORMATION_GPT pInfo)
{
	DWORD								cbLayout;		// Size of the current disk layout
	PDRIVE_LAYOUT_INFORMATION_EX		pLayout;		// Current disk layout
	DWORD								cbReturned;		// Bytes returned from DeviceIoControl

	if(handle == INVALID_HANDLE_VALUE) throw gcnew ArgumentNullException("handle");
	if(!pInfo) throw gcnew ArgumentNullException("pInfo");

	// Start with the current DRIVE_LAYOUT_INFORMATION_EX information
	GetDriveLayoutInformationEx(handle, &cbLayout, &pLayout);
	
	try {

		// Check to make sure this is a GPT disk so we don't trash anything
		if(pLayout->PartitionStyle != PARTITION_STYLE_GPT)
			throw gcnew Exception("SetDriveLayoutInformationGpt can only be invoked against GPT disks");

		// Apply the updated GPT information for the disk
		memcpy(&pLayout->Gpt, pInfo, sizeof(DRIVE_LAYOUT_INFORMATION_GPT));
		if(!DeviceIoControl(handle, IOCTL_DISK_SET_DRIVE_LAYOUT_EX, pLayout, cbLayout, NULL, 0, &cbReturned, NULL))
			throw gcnew Win32Exception(GetLastError());
	}
	
	// Release the allocated DRIVE_LAYOUT_INFORMATION_EX structure
	finally { pLayout = FreeDriveLayoutInformationEx(pLayout); }
}

//---------------------------------------------------------------------------
// DiskApi::SetDriveLayoutInformationMbr (static)
//
// Sets the DRIVE_LAYOUT_INFORMATION_MBR for a disk object
//
// Arguments:
//
//	handle		- Disk object handle
//	pInfo		- Pointer to the new DRIVE_LAYOUT_INFORMATION_MBR data

void DiskApi::SetDriveLayoutInformationMbr(HANDLE handle, PDRIVE_LAYOUT_INFORMATION_MBR pInfo)
{
	DWORD								cbLayout;		// Size of the current disk layout
	PDRIVE_LAYOUT_INFORMATION_EX		pLayout;		// Current disk layout
	DWORD								cbReturned;		// Bytes returned from DeviceIoControl

	if(handle == INVALID_HANDLE_VALUE) throw gcnew ArgumentNullException("handle");
	if(!pInfo) throw gcnew ArgumentNullException("pInfo");

	// Start with the current DRIVE_LAYOUT_INFORMATION_EX information
	GetDriveLayoutInformationEx(handle, &cbLayout, &pLayout);
	
	try {

		// Check to make sure this is a MBR disk so we don't trash anything
		if(pLayout->PartitionStyle != PARTITION_STYLE_MBR)
			throw gcnew Exception("SetDriveLayoutInformationMbr can only be invoked against MBR disks");

		// Apply the updated MBR information for the disk
		memcpy(&pLayout->Mbr, pInfo, sizeof(DRIVE_LAYOUT_INFORMATION_MBR));
		if(!DeviceIoControl(handle, IOCTL_DISK_SET_DRIVE_LAYOUT_EX, pLayout, cbLayout, NULL, 0, &cbReturned, NULL))
			throw gcnew Win32Exception(GetLastError());
	}
	
	// Release the allocated DRIVE_LAYOUT_INFORMATION_EX structure
	finally { pLayout = FreeDriveLayoutInformationEx(pLayout); }
}

//---------------------------------------------------------------------------
// DiskApi::UpdateProperties (static)
//
// Synchronizes the system view of the disk with it's updated information
//
// Arguments:
//
//	handle		- Disk object handle

void DiskApi::UpdateProperties(HANDLE handle)
{
	DWORD					cbReturned;				// Bytes returned from DeviceIoControl

	if(!DeviceIoControl(handle, IOCTL_DISK_UPDATE_PROPERTIES, NULL, 0, NULL, 0, &cbReturned, NULL))
		throw gcnew Win32Exception(GetLastError());
}

//---------------------------------------------------------------------------
// DiskApi::TryUpdateProperties (static)
//
// Invokes UpdateProperties but eats any exception generated
//
// Arguments:
//
//	handl		- Disk object handle

bool DiskApi::TryUpdateProperties(HANDLE handle)
{
	try { UpdateProperties(handle); return true; }
	catch(Exception^) { return false; }
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)
