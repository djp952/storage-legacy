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

#ifndef __DISK_H_
#define __DISK_H_
#pragma once

#include "DiskApi.h"
#include "DiskAsyncResult.h"
#include "DiskMediaType.h"
#include "DiskSafeHandle.h"
#include "DiskUtil.h"
#include "PartitionStyle.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class Disk
//
// Disk Management Class
//---------------------------------------------------------------------------

public ref class Disk
{
public:

	//-----------------------------------------------------------------------
	// Member Functions

	// Clean
	//
	// Removes all partition and volume information from the disk
	static void Clean(String^ device);

	// InitializeGpt
	//
	// Initializes the disk as EFI/GPT
	static void InitializeGpt(String^ device) { InitializeGpt(device, Guid::NewGuid()); }
	static void InitializeGpt(String^ device, Guid diskId);

	// InitializeMbr
	//
	// Initializes the disk as MBR
	static void InitializeMbr(String^ device) { InitializeMbr(device, (gcnew Random())->Next()); }
	static void InitializeMbr(String^ device, int signature);

	// Open
	//
	// Opens a disk device; test and cast the return object to Disk, GptDisk or MbrDisk
	static Disk^ Open(String^ device) { return Open(device, System::IO::FileAccess::ReadWrite, System::IO::FileShare::ReadWrite); }
	static Disk^ Open(String^ device, System::IO::FileAccess access) { return Open(device, access, System::IO::FileShare::ReadWrite); }
	static Disk^ Open(String^ device, System::IO::FileAccess access, System::IO::FileShare share);

	// Open
	//
	// Opens a disk device; use [out] style argument to determine if result is Disk, GptDisk or MbrDisk
	static Disk^ Open(String^ device, [Out] PartitionStyle% style) { return Open(device, System::IO::FileAccess::ReadWrite, System::IO::FileShare::ReadWrite, style); }
	static Disk^ Open(String^ device, System::IO::FileAccess access, [Out] PartitionStyle% style) { return Open(device, access, System::IO::FileShare::ReadWrite, style); }
	static Disk^ Open(String^ device, System::IO::FileAccess access, System::IO::FileShare share, [Out] PartitionStyle% style);

	// SetOffline
	//
	// Sets or clears the disk OFFLINE flag
	void SetOffline(bool offline) { SetOffline(offline, false); }
	void SetOffline(bool offline, bool persist);

	// SetReadOnly
	//
	// Sets or clears the disk READ_ONLY flag
	void SetReadOnly(bool readOnly) { SetReadOnly(readOnly, false); }
	void SetReadOnly(bool readOnly, bool persist);

	//-----------------------------------------------------------------------
	// Properties

	// BytesPerSector
	//
	// Gets the number of bytes per sector
	property int BytesPerSector
	{
		int get(void) { return static_cast<int>(m_pGeometry->Geometry.BytesPerSector); }
	}

	// Cylinders
	//
	// Gets the number of cylinders
	property __int64 Cylinders
	{
		__int64 get(void) { return static_cast<__int64>(m_pGeometry->Geometry.Cylinders.QuadPart); }
	}

	// IsWritable
	//
	// Determines whether the disk is writable or not
	property bool IsWritable
	{
		bool get(void);
	}

	// Length
	// 
	// Gets the size of the disk in bytes
	property __int64 Length
	{
		__int64 get(void) { return static_cast<__int64>(m_pGeometry->DiskSize.QuadPart); }
	}

	// Offline
	//
	// Gets a flag indicating if the disk if offline
	property bool Offline
	{
		bool get(void);
	}

	// MediaType
	//
	// Gets the disk media type
	property DiskMediaType MediaType
	{
		DiskMediaType get(void) { return static_cast<DiskMediaType>(m_pGeometry->Geometry.MediaType); }
	}

	// ReadOnly
	//
	// Gets a flag indicating if the disk if read-only
	property bool ReadOnly
	{
		bool get(void);
	}

	// SectorsPerTrack
	//
	// Gets the number of sectors per track
	property int SectorsPerTrack
	{
		int get(void) { return static_cast<int>(m_pGeometry->Geometry.SectorsPerTrack); }
	}

	// TracksPerCylinder
	//
	// Gets the number of tracks per cylinder
	property int TracksPerCylinder
	{
		int get(void) { return static_cast<int>(m_pGeometry->Geometry.TracksPerCylinder); }
	}

internal:

	//-----------------------------------------------------------------------
	// Internal Operators
	
	// operator HANDLE
	//
	// Exposes the contained disk object handle
	static operator HANDLE(Disk^ rhs);

protected:

	// PROTECTED CONSTRUCTORS
	Disk(HANDLE handle, PDISK_GEOMETRY_EX pGeometry);

private:

	// DESTRUCTOR / FINALIZER
	~Disk();
	!Disk();

	//-----------------------------------------------------------------------
	// Private Member Functions

	// CompleteAsyncOperation
	//
	// Common asynchronous operation completion function
	static void CompleteAsyncOperation(IAsyncResult^ asyncResult);

	//-----------------------------------------------------------------------
	// Member Variables

	initonly DiskSafeHandle^	m_handle;			// Disk SafeHandle
	bool						m_disposed;			// Object disposal flag
	PDISK_GEOMETRY_EX			m_pGeometry;		// Disk geometry information
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __DISK_H_
