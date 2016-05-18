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

#ifndef __DISKAPI_H_
#define __DISKAPI_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::ComponentModel;

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// Class DiskApi (internal)
//
// Helper class that wraps the underlying disk IOCTL calls to keep some
// clutter out of the main class library
//---------------------------------------------------------------------------

ref class DiskApi
{
public:

	// FreeDiskGeometryEx
	//
	// Releases the pointer allocated by GetDiskGeometryEx()
	static PDISK_GEOMETRY_EX FreeDiskGeometryEx(PDISK_GEOMETRY_EX pGeometry);

	// FreeDriveLayoutInformationEx
	//
	// Releases the pointer allocated by GetDriveLayoutInformationEx()
	static PDRIVE_LAYOUT_INFORMATION_EX FreeDriveLayoutInformationEx(PDRIVE_LAYOUT_INFORMATION_EX pLayout);

	// FreeGetDiskAttributes
	//
	// Releases the pointer allocated by GetDiskAttributes()
	static PGET_DISK_ATTRIBUTES FreeGetDiskAttributes(PGET_DISK_ATTRIBUTES pAttributes);

	// GetDiskAttributes
	//
	// Retrieves the GET_DISK_ATTRIBUTES data for the disk object
	static void GetDiskAttributes(HANDLE handle, PGET_DISK_ATTRIBUTES* ppAttributes);

	// GetDiskGeometryEx
	//
	// Retrieves the DISK_GEOMETRY_EX data for the disk object
	static void GetDiskGeometryEx(HANDLE handle, PDISK_GEOMETRY_EX* ppGeometry);
	static void GetDiskGeometryEx(HANDLE handle, DWORD* pcbGeometry, PDISK_GEOMETRY_EX* ppGeometry);

	// GetDriveLayoutInformationEx
	//
	// Retrieves the DRIVE_LAYOUT_INFORMATION_EX data for the disk object
	static void GetDriveLayoutInformationEx(HANDLE handle, PDRIVE_LAYOUT_INFORMATION_EX* ppLayout);
	static void GetDriveLayoutInformationEx(HANDLE handle, DWORD* pcbLayout, PDRIVE_LAYOUT_INFORMATION_EX* ppLayout);

	// IsWritable
	//
	// Determines if the disk object is writable
	static bool IsWritable(HANDLE handle);

	// SetDiskAttributes
	//
	// Applies SET_DISK_ATTRIBUTES information for the disk
	static void SetDiskAttributes(HANDLE handle, PSET_DISK_ATTRIBUTES pAttributes);

	// SetDriveLayoutInformationEx
	//
	// Applies the DRIVE_LAYOUT_INFORMATION_EX data for the disk object
	static void SetDriveLayoutInformationEx(HANDLE handle, DWORD cbLayout, PDRIVE_LAYOUT_INFORMATION_EX pLayout);

	// SetDriveLayoutInformationGpt
	//
	// Sets the DRIVE_LAYOUT_INFORMATION_GPT for a disk object
	static void SetDriveLayoutInformationGpt(HANDLE handle, PDRIVE_LAYOUT_INFORMATION_GPT pInfo);
	
	// SetDriveLayoutInformationMbr
	//
	// Sets the DRIVE_LAYOUT_INFORMATION_MBR for a disk object
	static void SetDriveLayoutInformationMbr(HANDLE handle, PDRIVE_LAYOUT_INFORMATION_MBR pInfo);

	// UpdateProperties
	//
	// Synchronizes the system view of the disk with it's updated information
	static void UpdateProperties(HANDLE handle);

	// TryUpdateProperties
	//
	// Invokes UpdateProperties without throwing an exception on failure
	static bool TryUpdateProperties(HANDLE handle);
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)

#endif	// __DISKAPI_H_
