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

#include "VirtualDiskAsyncResult.h"
#include "VirtualDiskAsyncStatus.h"
#include "VirtualDiskAttachParameters.h"
#include "VirtualDiskCreateParameters.h"
#include "VirtualDiskOpenParameters.h"
#include "VirtualDiskSafeHandle.h"
#include "VirtualDiskStorageType.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::ComponentModel;
using namespace System::Runtime::InteropServices;
using namespace System::Security::AccessControl;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class VirtualDisk
//
// TODO
//---------------------------------------------------------------------------

public ref class VirtualDisk sealed
{
public:

	// TODO
	static VirtualDiskAsyncStatus AsyncOperationStatus(IAsyncResult^ asyncResult);
	
	//-----------------------------------------------------------------------
	// ASYNCHRONOUS VIRTUAL DISK API FUNCTIONS
	//-----------------------------------------------------------------------

	// BeginAttach / EndAttach
	//
	// Attaches the virtual disk
	IAsyncResult^ BeginAttach(void) { return BeginAttach(gcnew VirtualDiskAttachParameters(), nullptr); }
	IAsyncResult^ BeginAttach(VirtualDiskAttachParameters^ attachParams) { return BeginAttach(attachParams, nullptr); }
	IAsyncResult^ BeginAttach(VirtualDiskAttachParameters^ attachParams, FileSecurity^ securityDescriptor);
	VirtualDiskAsyncStatus EndAttach(IAsyncResult^ asyncResult) { return CompleteAsyncOperation(asyncResult); }

	// BeginCompact / EndCompact
	//
	// Compacts the virtual disk
	IAsyncResult^ BeginCompact(void);
	VirtualDiskAsyncStatus EndCompact(IAsyncResult^ asyncResult) { return CompleteAsyncOperation(asyncResult); }

	// BeginCreate / EndCreate
	//
	// Creates a new virtual disk
	static IAsyncResult^ BeginCreate(String^ path, __int64 maximumSize) { return BeginCreate(gcnew VirtualDiskCreateParameters(path, VirtualDiskStorageType::Unknown, maximumSize), nullptr); }
	static IAsyncResult^ BeginCreate(String^ path, VirtualDiskStorageType type, __int64 maximumSize) { return BeginCreate(gcnew VirtualDiskCreateParameters(path, type, maximumSize), nullptr); }
	static IAsyncResult^ BeginCreate(String^ path, __int64 maximumSize, FileSecurity^ securityDescriptor) { return BeginCreate(gcnew VirtualDiskCreateParameters(path, VirtualDiskStorageType::Unknown, maximumSize), securityDescriptor); }
	static IAsyncResult^ BeginCreate(String^ path, VirtualDiskStorageType type, __int64 maximumSize, FileSecurity^ securityDescriptor) { return BeginCreate(gcnew VirtualDiskCreateParameters(path, type, maximumSize), securityDescriptor); }
	static IAsyncResult^ BeginCreate(VirtualDiskCreateParameters^ createParams) { return BeginCreate(createParams, nullptr); }
	static IAsyncResult^ BeginCreate(VirtualDiskCreateParameters^ createParams, FileSecurity^ securityDescriptor);
	static VirtualDisk^ EndCreate(IAsyncResult^ asyncResult);

	// BeginExpand / EndExpand
	//
	// Expands the virtual disk to the specified size
	IAsyncResult^ BeginExpand(__int64 newSize);
	VirtualDiskAsyncStatus EndExpand(IAsyncResult^ asyncResult) { return CompleteAsyncOperation(asyncResult); }
	
	// BeginResize / EndResize
	//
	// Resizes the virtual disk to the specified size
	IAsyncResult^ BeginResize(__int64 newSize) { return BeginResize(newSize, false); }
	IAsyncResult^ BeginResize(__int64 newSize, bool allowTruncate);
	VirtualDiskAsyncStatus EndResize(IAsyncResult^ asyncResult) { return CompleteAsyncOperation(asyncResult); }

	//-----------------------------------------------------------------------
	// SYNCHRONOUS VIRTUAL DISK API FUNCTIONS
	//-----------------------------------------------------------------------

	// Attach
	//
	// Attaches the virtual disk
	// NOTE: REQUIRES ELEVATED PRIVILEGES
	void Attach(void) { Attach(gcnew VirtualDiskAttachParameters(), nullptr); }
	void Attach(VirtualDiskAttachParameters^ attachParams) { Attach(attachParams, nullptr); }
	void Attach(VirtualDiskAttachParameters^ attachParams, FileSecurity^ securityDescriptor);

	// Compact
	//
	// Compacts the virtual disk
	void Compact(void);

	// Create
	//
	// Creates a new virtual disk
	static VirtualDisk^ Create(String^ path, __int64 maximumSize) { return Create(gcnew VirtualDiskCreateParameters(path, VirtualDiskStorageType::Unknown, maximumSize), nullptr); }
	static VirtualDisk^ Create(String^ path, VirtualDiskStorageType type, __int64 maximumSize) { return Create(gcnew VirtualDiskCreateParameters(path, type, maximumSize), nullptr); }
	static VirtualDisk^ Create(String^ path, __int64 maximumSize, FileSecurity^ securityDescriptor) { return Create(gcnew VirtualDiskCreateParameters(path, VirtualDiskStorageType::Unknown, maximumSize), securityDescriptor); }
	static VirtualDisk^ Create(String^ path, VirtualDiskStorageType type, __int64 maximumSize, FileSecurity^ securityDescriptor) { return Create(gcnew VirtualDiskCreateParameters(path, type, maximumSize), securityDescriptor); }
	static VirtualDisk^ Create(VirtualDiskCreateParameters^ createParams) { return Create(createParams, nullptr); }
	static VirtualDisk^ Create(VirtualDiskCreateParameters^ createParams, FileSecurity^ securityDescriptor);

	// Detach
	//
	// Detaches the virtual disk
	// NOTE: REQUIRES ELEVATED PRIVILEGES
	void Detach(void);

	// Expand
	//
	// Expands the virtual disk to the specified size
	void Expand(__int64 newSize);
	
	// Open
	//
	// Opens an existing virtual disk
	static VirtualDisk^ Open(String^ path) { return Open(gcnew VirtualDiskOpenParameters(path, VirtualDiskStorageType::Unknown)); }
	static VirtualDisk^ Open(String^ path, VirtualDiskStorageType type) { return Open(gcnew VirtualDiskOpenParameters(path, type)); }
	static VirtualDisk^ Open(VirtualDiskOpenParameters^ openParams);

	// Resize
	//
	// Resizes the virtual disk to the specified size
	void Resize(__int64 newSize) { Resize(newSize, false); }
	void Resize(__int64 newSize, bool allowTruncate);

	//-----------------------------------------------------------------------
	// Properties

	// FragmentationLevel
	//
	// Gets the fragmentation level percentage for the virtual disk
	property int FragmentationLevel
	{
		int get(void);
	}

	// PhysicalPath
	//
	// Gets the physical device object path for the virtual disk
	property String^ PhysicalPath
	{
		String^ get(void);
	}

	// Type
	//
	// Gets the virtual disk type
	property VirtualDiskStorageType Type
	{
		VirtualDiskStorageType get(void);
	}

	// UniqueIdentifier
	//
	// Gets/sets the unique identifer UUID of the virtual disk
	property Guid UniqueIdentifier
	{
		Guid get(void);
		void set(Guid value);
	}

private:

	// PRIVATE CONSTRUCTOR
	VirtualDisk(VirtualDiskSafeHandle^ handle) : m_disposed(false), m_handle(handle) {}

	// DESTRUCTOR
	~VirtualDisk() { delete m_handle; m_disposed = true; }

	//-----------------------------------------------------------------------
	// Private Member Functions

	// CompleteAsyncOperation
	//
	// Common asynchronous operation completion function
	static VirtualDiskAsyncStatus CompleteAsyncOperation(IAsyncResult^ asyncResult);

	//-----------------------------------------------------------------------
	// Member Variables

	bool					m_disposed;			// Object disposal flag
	VirtualDiskSafeHandle^	m_handle;			// Virtual Disk SafeHandle
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISK_H_
