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
enum class	VirtualDiskAttachFlags;
ref struct	VirtualDiskAttachParameters;
enum class	VirtualDiskCompactFlags;
ref struct	VirtualDiskCompactParameters;
enum class	VirtualDiskCreateFlags;
ref struct	VirtualDiskCreateParameters;
enum class	VirtualDiskDetachFlags;
ref struct	VirtualDiskDetachParameters;
enum class	VirtualDiskExpandFlags;
ref struct	VirtualDiskExpandParameters;
ref class	VirtualDiskMetadataCollection;
enum class	VirtualDiskOpenFlags;
ref struct	VirtualDiskOpenParameters;
enum class	VirtualDiskResizeFlags;
ref struct	VirtualDiskResizeParameters;
ref class	VirtualDiskSafeHandle;
value class	VirtualDiskType;

//---------------------------------------------------------------------------
// Class VirtualDisk
//
// Managed wrapper class around the Windows Virtual Disk API
//---------------------------------------------------------------------------

public ref class VirtualDisk sealed
{
public:

	//-----------------------------------------------------------------------
	// Member Functions

	// AddParentDisk
	//
	// Adds a parent to a virtual disk opened with CustomDifferencingChain
	void AddParentDisk(String^ path);

	// Attach
	//
	// Synchronously attaches the virtual disk
	void Attach(void);
	void Attach(VirtualDiskAttachFlags flags);
	void Attach(VirtualDiskAttachParameters^ params);

	// AttachAsync
	//
	// Asynchronously attaches the virtual disk
	//Task^ AttachAsync(CancellationToken cancellation, IProgress<int>^ progress);
	//Task^ AttachAsync(VirtualDiskAttachFlags flags, CancellationToken cancellation, IProgress<int>^ progress);
	Task^ AttachAsync(VirtualDiskAttachParameters^ params, CancellationToken cancellation, IProgress<int>^ progress);

	// Compact
	//
	// Synchronously compacts the virtual disk
	void Compact(void);
	void Compact(VirtualDiskCompactFlags flags);
	void Compact(VirtualDiskCompactParameters^ params);

	// CompactAsync
	//
	// Asynchronously compacts the virtual disk
	//Task^ CompactAsync(void);
	//Task^ CompactAsync(VirtualDiskCompactFlags flags);
	//Task^ CompactAsync(CancellationToken cancellation);
	//Task^ CompactAsync(VirtualDiskCompactFlags flags, CancellationToken cancellation);
	//Task^ CompactAsync(IProgress<int>^ progress);
	//Task^ CompactAsync(VirtualDiskCompactFlags flags, IProgress<int>^ progress);
	//Task^ CompactAsync(CancellationToken cancellation, IProgress<int>^ progress);
	Task^ CompactAsync(VirtualDiskCompactParameters^ params, CancellationToken cancellation, IProgress<int>^ progress);

	// Create
	//
	// Synchronously creates a new virtual disk
	static VirtualDisk^ Create(String^ path, VirtualDiskType type, unsigned __int64 maxsize);
	static VirtualDisk^ Create(String^ path, VirtualDiskType type, unsigned __int64 maxsize, VirtualDiskCreateFlags flags);
	static VirtualDisk^ Create(VirtualDiskCreateParameters^ params);

	// CreateAsync
	//
	// Asynchronously creates a new virtual disk
	static Task<VirtualDisk^>^ CreateAsync(VirtualDiskCreateParameters^ params, CancellationToken cancellation, IProgress<int>^ progress);

	// Detach
	//
	// Detaches the virtual disk
	void Detach(void);
	void Detach(VirtualDiskDetachFlags flags);
	void Detach(VirtualDiskDetachParameters^ params);

	// Expand
	//
	// Synchronously expands the virtual disk
	void Expand(unsigned __int64 newsize);
	void Expand(unsigned __int64 newsize, VirtualDiskExpandFlags flags);
	void Expand(VirtualDiskExpandParameters^ params);

	// ExpandAsync
	//
	// Asynchronously expands the virtual disk
	//Task^ ExpandAsync(__int64 newsize);
	//Task^ ExpandAsync(__int64 newsize, VirtualDiskExpandFlags flags);
	//Task^ ExpandAsync(__int64 newsize, CancellationToken cancellation);
	//Task^ ExpandAsync(__int64 newsize, VirtualDiskExpandFlags flags, CancellationToken cancellation);
	//Task^ ExpandAsync(__int64 newsize, IProgress<int>^ progress);
	//Task^ ExpandAsync(__int64 newsize, VirtualDiskExpandFlags flags, IProgress<int>^ progress);
	//Task^ ExpandAsync(__int64 newsize, CancellationToken cancellation, IProgress<int>^ progress);
	Task^ ExpandAsync(VirtualDiskExpandParameters^ params, CancellationToken cancellation, IProgress<int>^ progress);

	// Open (static)
	//
	// Opens an existing virtual disk
	static VirtualDisk^ Open(String^ path);
	static VirtualDisk^ Open(String^ path, VirtualDiskAccess access);
	static VirtualDisk^ Open(String^ path, VirtualDiskAccess access, VirtualDiskOpenFlags flags);
	static VirtualDisk^ Open(VirtualDiskOpenParameters^ params);

	// Resize
	//
	// Synchronously resizes the virtual disk
	void Resize(unsigned __int64 newsize);
	void Resize(unsigned __int64 newsize, VirtualDiskResizeFlags flags);
	void Resize(VirtualDiskResizeParameters^ params);

	// ResizeAsync
	//
	// Asynchronously resizes the virtual disk
	//Task^ ResizeAsync(__int64 newsize);
	//Task^ ResizeAsync(__int64 newsize, VirtualDiskResizeFlags flags);
	//Task^ ResizeAsync(__int64 newsize, CancellationToken cancellation);
	//Task^ ResizeAsync(__int64 newsize, VirtualDiskResizeFlags flags, CancellationToken cancellation);
	//Task^ ResizeAsync(__int64 newsize, IProgress<int>^ progress);
	//Task^ ResizeAsync(__int64 newsize, VirtualDiskResizeFlags flags, IProgress<int>^ progress);
	//Task^ ResizeAsync(__int64 newsize, CancellationToken cancellation, IProgress<int>^ progress);
	Task^ ResizeAsync(VirtualDiskResizeParameters^ params, CancellationToken cancellation, IProgress<int>^ progress);

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

	// Metadata
	//
	// Gets a reference to the metadata collection for this virtual disk
	property VirtualDiskMetadataCollection^ Metadata
	{
		VirtualDiskMetadataCollection^ get(void);
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

private:

	// Instance Constructor
	//
	VirtualDisk(VirtualDiskSafeHandle^ handle);

	// Destructor
	//
	~VirtualDisk();

	//-----------------------------------------------------------------------
	// Private Member Functions

	// BeginAttach
	//
	// Begins an asynchronous attach operation
	IAsyncResult^ BeginAttach(VirtualDiskAttachParameters^ params, CancellationToken cancellation, IProgress<int>^ progress);

	// BeginCompact
	//
	// Begins an asynchronous compact operation
	IAsyncResult^ BeginCompact(VirtualDiskCompactParameters^ params, CancellationToken cancellation, IProgress<int>^ progress);

	// BeginCreate (static)
	//
	// Begins an asynchronous create operation
	static IAsyncResult^ BeginCreate(VirtualDiskCreateParameters^ params, CancellationToken cancellation, IProgress<int>^ progress);

	// BeginExpand
	//
	// Begins an asynchronous expand operation
	IAsyncResult^ BeginExpand(VirtualDiskExpandParameters^ params, CancellationToken cancellation, IProgress<int>^ progress);

	// BeginResize
	//
	// Begins an asynchronous resize operation
	IAsyncResult^ BeginResize(VirtualDiskResizeParameters^ params, CancellationToken cancellation, IProgress<int>^ progress);

	// EndCreate (static)
	//
	// Completes an asynchronous create operation
	static VirtualDisk^ EndCreate(IAsyncResult^ asyncresult);

	//-----------------------------------------------------------------------
	// Member Variables

	bool							m_disposed;		// Object disposal flag
	VirtualDiskSafeHandle^			m_handle;		// Virtual Disk SafeHandle
	VirtualDiskMetadataCollection^	m_metadata;		// Metadata collection
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISK_H_
