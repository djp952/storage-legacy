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

#ifndef __VIRTUALDISKCREATEPARAMETERS_H_
#define __VIRTUALDISKCREATEPARAMETERS_H_
#pragma once

#include "VirtualDiskParentDiskParameters.h"
#include "VirtualDiskType.h"
#include "VirtualDiskSourceDiskParameters.h"
#include "VirtualDiskUtil.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::ComponentModel;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
using namespace System::Security::AccessControl;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class VirtualDiskCreateParameters
//
// Contains virtual hard disk (VHD) creation parameters, providing control over, 
// and information about, the newly created virtual disk
//---------------------------------------------------------------------------

public ref class VirtualDiskCreateParameters sealed
{
public:

	// Instance Constructors
	VirtualDiskCreateParameters() { Construct(String::Empty, VirtualDiskType::Unknown, 0); }
	VirtualDiskCreateParameters(String^ path) { Construct(path, VirtualDiskType::Unknown, 0); }
	VirtualDiskCreateParameters(String^ path, VirtualDiskType type) { Construct(path, type, 0); }
	VirtualDiskCreateParameters(String^ path, __int64 maximumSize) { Construct(path, VirtualDiskType::Unknown, maximumSize); }
	VirtualDiskCreateParameters(String^ path, VirtualDiskType type, __int64 maximumSize) { Construct(path, type, maximumSize); }

	//-----------------------------------------------------------------------
	// Properties

	// BlockSize
	//
	// Defines the block size to use with the virtual disk
	property int BlockSize
	{
		int get(void) { return m_blockSize; }
		void set(int value)
		{
			if(value < 0) throw gcnew ArgumentOutOfRangeException("value");
			m_blockSize = value;
		}
	}

	// MaximumSize
	//
	// Gets/sets the maximum size of the virtual disk
	property __int64 MaximumSize
	{
		__int64 get(void) { return m_maximumSize; }
		void set(__int64 value)
		{
			if(value < 0) throw gcnew ArgumentOutOfRangeException("value");
			m_maximumSize = value;
		}
	}

	// ParentDisk
	//
	// Accesses the contained ParentDiskParameters instance
	property VirtualDiskParentDiskParameters^ ParentDisk
	{
		VirtualDiskParentDiskParameters^ get(void) { return m_parentParams; }
	}

	// Path
	//
	// Gets/sets the path to the new virtual disk object
	property String^ Path
	{
		String^ get(void) { return m_path; }
		void set(String^ value) { m_path = value; }
	}

	// PreAllocateStorage
	//
	// Gets/Sets a flag indicating that the physical storage should be pre-allocated
	property bool PreAllocateStorage
	{
		bool get(void) { return m_preAllocate; }
		void set(bool value) { m_preAllocate = value; }
	}

	// ResiliencyGuid
	//
	// Gets/sets the resiliency GUID to apply to the new virtual disk
	property Guid ResiliencyGuid
	{
		Guid get(void) { return m_resiliencyGuid; }
		void set(Guid value) { m_resiliencyGuid = value; }
	}

	// SectorSize
	//
	// Defines the sector size to use with the virtual disk
	property int SectorSize
	{
		int get(void) { return m_sectorSize; }
		void set(int value)
		{
			if(value < 0) throw gcnew ArgumentOutOfRangeException("value");
			m_sectorSize = value;
		}
	}

	// SourceDisk
	//
	// Accesses the contained SourceDiskParameters instance
	property VirtualDiskSourceDiskParameters^ SourceDisk
	{
		VirtualDiskSourceDiskParameters^ get(void) { return m_sourceParams; }
	}

	// Type
	//
	// Gets/sets the virtual disk type of the parent disk object
	property VirtualDiskType Type
	{
		VirtualDiskType get(void) { return m_storageType; }
		void set(VirtualDiskType value) { m_storageType = value; }
	}

	// UniqueIdentifier
	//
	// Gets/sets the virtual disk unique identifier
	property Guid UniqueIdentifier
	{
		Guid get(void) { return m_uniqueIdentifier; }
		void set(Guid value) { m_uniqueIdentifier = value; }
	}

	// UseCachedIO
	//
	// Gets/sets a flag to open the new disk in cached IO mode rather than
	// no buffering/write through mode
	property bool UseCachedIO
	{
		bool get(void) { return m_cachedIO; }
		void set(bool value) { m_cachedIO = value; }
	}

internal:

	//-----------------------------------------------------------------------
	// Internal Member Functions

	// Releases strcuture allocated by ToCREATE_VIRTUAL_DISK_PARAMETERS()
	static void FreeCREATE_VIRTUAL_DISK_PARAMETERS(PCREATE_VIRTUAL_DISK_PARAMETERS pParams);

	// Gets the CREATE_VIRTUAL_DISK_FLAG bitmask
	CREATE_VIRTUAL_DISK_FLAG ToCREATE_VIRTUAL_DISK_FLAG(void);

	// Converts the state of this object into an unmanaged CREATE_VIRTUAL_DISK_PARAMETERS
	CREATE_VIRTUAL_DISK_VERSION ToCREATE_VIRTUAL_DISK_PARAMETERS(PCREATE_VIRTUAL_DISK_PARAMETERS* ppParams);

private:

	//-----------------------------------------------------------------------
	// Private Member Functions

	// Pseudo-constructor
	void Construct(String^ path, VirtualDiskType type, __int64 maximumSize);

	//-----------------------------------------------------------------------
	// Member Variables

	String^								m_path;
	VirtualDiskType				m_storageType;
	Guid								m_uniqueIdentifier;
	__int64								m_maximumSize;
	int									m_blockSize;
	int									m_sectorSize;
	Guid								m_resiliencyGuid;
	VirtualDiskParentDiskParameters^	m_parentParams;
	VirtualDiskSourceDiskParameters^	m_sourceParams;
	bool								m_preAllocate;
	bool								m_cachedIO;
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKCREATEPARAMETERS_H_
