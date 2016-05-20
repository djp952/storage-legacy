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

#ifndef __VIRTUALDISK_H_
#define __VIRTUALDISK_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::ComponentModel;
using namespace System::Security::Permissions;
using namespace System::Threading;
using namespace System::Threading::Tasks;

BEGIN_ROOT_NAMESPACE(zuki::storage)

// FORWARD DECLARATIONS
//
enum class	VirtualDiskAccess;
enum class	VirtualDiskCompactFlags;
enum class	VirtualDiskOpenFlags;
ref class	VirtualDiskSafeHandle;
value class	VirtualDiskType;

//---------------------------------------------------------------------------
// Class VirtualDisk
//
// TODO
//---------------------------------------------------------------------------

public ref class VirtualDisk sealed
{
public:

	//-----------------------------------------------------------------------
	// Member Functions

	// BeginCompact
	//
	// Begins an asynchronous compact operation
	IAsyncResult^ BeginCompact(AsyncCallback^ callback, Object^ state);
	IAsyncResult^ BeginCompact(VirtualDiskCompactFlags flags, AsyncCallback^ callback, Object^ state);

	// Compact
	//
	// Synchronously compacts the virtual disk
	void Compact(void);
	void Compact(VirtualDiskCompactFlags flags);

	// CompactAsync
	//
	// Asynchronously compacts the virtual disk
	Task^ CompactAsync(void);
	Task^ CompactAsync(VirtualDiskCompactFlags flags);
	Task^ CompactAsync(CancellationToken cancellation);
	Task^ CompactAsync(VirtualDiskCompactFlags flags, CancellationToken cancellation);
	
	// EndCompact
	//
	// Completes an asynchronous compact operation
	static void EndCompact(IAsyncResult^ asyncresult);

	// Open
	//
	// Opens an existing virtual disk
	static VirtualDisk^ Open(String^ path);
	static VirtualDisk^ Open(String^ path, VirtualDiskAccess access);
	static VirtualDisk^ Open(String^ path, VirtualDiskAccess access, VirtualDiskOpenFlags flags);
	static VirtualDisk^ Open(VirtualDiskType type, String^ path);
	static VirtualDisk^ Open(VirtualDiskType type, String^ path, VirtualDiskAccess access);
	static VirtualDisk^ Open(VirtualDiskType type, String^ path, VirtualDiskAccess access, VirtualDiskOpenFlags flags);
	// todo: VirtualDiskOpenParameters^

	//-----------------------------------------------------------------------
	// Properties

	// AttachedDevicePath
	//
	// Gets the physical device object path for the virtual disk
	property String^ AttachedDevicePath
	{
		String^ get(void);
	}

	// BlockSize
	//
	// Gets the block size of the virtual disk
	property unsigned int BlockSize
	{
		unsigned int get(void);
	}

	// FragmentationLevel
	//
	// Gets the fragmentation level percentage for the virtual disk
	property int FragmentationLevel
	{
		int get(void);
	}

	// PhysicalSize
	//
	// Gets the physical size of the virtual disk
	property unsigned __int64 PhysicalSize
	{
		unsigned __int64 get(void);
	}

	// SectorSize
	//
	// Gets the sector size of the virtual disk
	property unsigned int SectorSize
	{
		unsigned int get(void);
	}

	// Type
	//
	// Gets the virtual disk type
	property VirtualDiskType Type
	{
		VirtualDiskType get(void);
	}

	// UniqueIdentifier
	//
	// Gets/sets the unique identifer UUID of the virtual disk
	property Guid UniqueIdentifier
	{
		Guid get(void);
		void set(Guid value);
	}

	// VirtualSize
	//
	// Gets the virtual size of the virtual disk
	property unsigned __int64 VirtualSize
	{
		unsigned __int64 get(void);
	}

	//// OLD OLD OLD
	////
	//static VirtualDiskAsyncStatus AsyncOperationStatus(IAsyncResult^ asyncResult);
	//
	////-----------------------------------------------------------------------
	//// ASYNCHRONOUS VIRTUAL DISK API FUNCTIONS
	////-----------------------------------------------------------------------

	//// BeginAttach / EndAttach
	////
	//// Attaches the virtual disk
	//IAsyncResult^ BeginAttach(void) { return BeginAttach(gcnew VirtualDiskAttachParameters(), nullptr); }
	//IAsyncResult^ BeginAttach(VirtualDiskAttachParameters^ attachParams) { return BeginAttach(attachParams, nullptr); }
	//IAsyncResult^ BeginAttach(VirtualDiskAttachParameters^ attachParams, FileSecurity^ securityDescriptor);
	//VirtualDiskAsyncStatus EndAttach(IAsyncResult^ asyncResult) { return CompleteAsyncOperation(asyncResult); }

	//// BeginCompact / EndCompact
	////
	//// Compacts the virtual disk
	////IAsyncResult^ BeginCompact(void);
	////VirtualDiskAsyncStatus EndCompact(IAsyncResult^ asyncResult) { return CompleteAsyncOperation(asyncResult); }

	//// BeginCreate / EndCreate
	////
	//// Creates a new virtual disk
	//static IAsyncResult^ BeginCreate(String^ path, __int64 maximumSize) { return BeginCreate(gcnew VirtualDiskCreateParameters(path, VirtualDiskType::Unknown, maximumSize), nullptr); }
	//static IAsyncResult^ BeginCreate(String^ path, VirtualDiskType type, __int64 maximumSize) { return BeginCreate(gcnew VirtualDiskCreateParameters(path, type, maximumSize), nullptr); }
	//static IAsyncResult^ BeginCreate(String^ path, __int64 maximumSize, FileSecurity^ securityDescriptor) { return BeginCreate(gcnew VirtualDiskCreateParameters(path, VirtualDiskType::Unknown, maximumSize), securityDescriptor); }
	//static IAsyncResult^ BeginCreate(String^ path, VirtualDiskType type, __int64 maximumSize, FileSecurity^ securityDescriptor) { return BeginCreate(gcnew VirtualDiskCreateParameters(path, type, maximumSize), securityDescriptor); }
	//static IAsyncResult^ BeginCreate(VirtualDiskCreateParameters^ createParams) { return BeginCreate(createParams, nullptr); }
	//static IAsyncResult^ BeginCreate(VirtualDiskCreateParameters^ createParams, FileSecurity^ securityDescriptor);
	//static VirtualDisk^ EndCreate(IAsyncResult^ asyncResult);

	//// testing
	//static IAsyncResult^ BeginCreate2(String^ path);
	//static VirtualDisk^ EndCreate2(IAsyncResult^ asyncresult);

	//static VirtualDisk^ Create2(String^ path);
	//
	//[HostProtectionAttribute(SecurityAction::LinkDemand, ExternalThreading = true)]
	//static Task<VirtualDisk^>^ CreateAsync(String^ path);

	//// BeginExpand / EndExpand
	////
	//// Expands the virtual disk to the specified size
	//IAsyncResult^ BeginExpand(__int64 newSize);
	//VirtualDiskAsyncStatus EndExpand(IAsyncResult^ asyncResult) { return CompleteAsyncOperation(asyncResult); }
	//
	//// BeginResize / EndResize
	////
	//// Resizes the virtual disk to the specified size
	//IAsyncResult^ BeginResize(__int64 newSize) { return BeginResize(newSize, false); }
	//IAsyncResult^ BeginResize(__int64 newSize, bool allowTruncate);
	//VirtualDiskAsyncStatus EndResize(IAsyncResult^ asyncResult) { return CompleteAsyncOperation(asyncResult); }

	////-----------------------------------------------------------------------
	//// SYNCHRONOUS VIRTUAL DISK API FUNCTIONS
	////-----------------------------------------------------------------------

	//// Attach
	////
	//// Attaches the virtual disk
	//// NOTE: REQUIRES ELEVATED PRIVILEGES
	//void Attach(void) { Attach(gcnew VirtualDiskAttachParameters(), nullptr); }
	//void Attach(VirtualDiskAttachParameters^ attachParams) { Attach(attachParams, nullptr); }
	//void Attach(VirtualDiskAttachParameters^ attachParams, FileSecurity^ securityDescriptor);

	//// Compact
	////
	//// Compacts the virtual disk
	////void Compact(void);

	//// Create
	////
	//// Creates a new virtual disk
	//static VirtualDisk^ Create(String^ path, __int64 maximumSize) { return Create(gcnew VirtualDiskCreateParameters(path, VirtualDiskType::Unknown, maximumSize), nullptr); }
	//static VirtualDisk^ Create(String^ path, VirtualDiskType type, __int64 maximumSize) { return Create(gcnew VirtualDiskCreateParameters(path, type, maximumSize), nullptr); }
	//static VirtualDisk^ Create(String^ path, __int64 maximumSize, FileSecurity^ securityDescriptor) { return Create(gcnew VirtualDiskCreateParameters(path, VirtualDiskType::Unknown, maximumSize), securityDescriptor); }
	//static VirtualDisk^ Create(String^ path, VirtualDiskType type, __int64 maximumSize, FileSecurity^ securityDescriptor) { return Create(gcnew VirtualDiskCreateParameters(path, type, maximumSize), securityDescriptor); }
	//static VirtualDisk^ Create(VirtualDiskCreateParameters^ createParams) { return Create(createParams, nullptr); }
	//static VirtualDisk^ Create(VirtualDiskCreateParameters^ createParams, FileSecurity^ securityDescriptor);

	//// Detach
	////
	//// Detaches the virtual disk
	//// NOTE: REQUIRES ELEVATED PRIVILEGES
	//void Detach(void);

	//// Expand
	////
	//// Expands the virtual disk to the specified size
	//void Expand(__int64 newSize);
	//
	//// Open
	////
	//// Opens an existing virtual disk
	//static VirtualDisk^ Open(String^ path) { return Open(gcnew VirtualDiskOpenParameters(path, VirtualDiskType::Unknown)); }
	//static VirtualDisk^ Open(String^ path, VirtualDiskType type) { return Open(gcnew VirtualDiskOpenParameters(path, type)); }
	//static VirtualDisk^ Open(VirtualDiskOpenParameters^ openParams);

	//// Resize
	////
	//// Resizes the virtual disk to the specified size
	//void Resize(__int64 newSize) { Resize(newSize, false); }
	//void Resize(__int64 newSize, bool allowTruncate);

private:

	// Instance Constructor
	//
	VirtualDisk(VirtualDiskSafeHandle^ handle);

	// Destructor
	//
	~VirtualDisk();

	//-----------------------------------------------------------------------
	// Member Variables

	bool						m_disposed;		// Object disposal flag
	VirtualDiskSafeHandle^		m_handle;		// Virtual Disk SafeHandle
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISK_H_
