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

#ifndef __VIRTUALDISKCREATEPARAMETERS2_H_
#define __VIRTUALDISKCREATEPARAMETERS2_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
//using namespace System::ComponentModel;
//using namespace System::IO;
//using namespace System::Runtime::InteropServices;
//using namespace System::Security::AccessControl;

BEGIN_ROOT_NAMESPACE(zuki::storage)

// FORWARD DECLARATIONS
//
value class VirtualDiskType;

//---------------------------------------------------------------------------
// Class VirtualDiskCreateParameters
//
// Contains virtual hard disk (VHD) creation parameters, providing control over 
// and information about, the newly created virtual disk
//---------------------------------------------------------------------------

public ref class VirtualDiskCreateParameters2 sealed
{
public:

	// Instance Constructors
	//
	VirtualDiskCreateParameters2();
	//VirtualDiskCreateParameters() { Construct(String::Empty, VirtualDiskType::Unknown, 0); }
	//VirtualDiskCreateParameters(String^ path) { Construct(path, VirtualDiskType::Unknown, 0); }
	//VirtualDiskCreateParameters(String^ path, VirtualDiskType type) { Construct(path, type, 0); }
	//VirtualDiskCreateParameters(String^ path, __int64 maximumSize) { Construct(path, VirtualDiskType::Unknown, maximumSize); }
	//VirtualDiskCreateParameters(String^ path, VirtualDiskType type, __int64 maximumSize) { Construct(path, type, maximumSize); }

	//-----------------------------------------------------------------------
	// Properties

	// BlockSize
	//
	// Defines the block size to use with the virtual disk
	property int BlockSize
	{
		int get(void);
		void set(int value);
	}

	// MaximumSize
	//
	// Gets/sets the maximum size of the virtual disk
	property __int64 MaximumSize
	{
		__int64 get(void);
		void set(__int64 value);
	}

	// ParentDiskPath
	//
	// Fully qualified path to a parent virtual disk object
	property String^ ParentDiskPath
	{
		String^ get(void);
		void set(String^ value);
	}

	// ParentDiskType
	//
	// Describes the parent virtual disk type specified by ParentDiskPath 
	property VirtualDiskType ParentDiskType
	{
		VirtualDiskType get(void);
		void set(VirtualDiskType value);
	}

	//// Path
	////
	//// Gets/sets the path to the new virtual disk object
	//property String^ Path
	//{
	//	String^ get(void) { return m_path; }
	//	void set(String^ value) { m_path = value; }
	//}

	//// PreAllocateStorage
	////
	//// Gets/Sets a flag indicating that the physical storage should be pre-allocated
	//property bool PreAllocateStorage
	//{
	//	bool get(void) { return m_preAllocate; }
	//	void set(bool value) { m_preAllocate = value; }
	//}

	// ResiliencyGuid
	//
	// Gets/sets the resiliency GUID to apply to the new virtual disk
	property Guid ResiliencyGuid
	{
		Guid get(void);
		void set(Guid value);
	}

	// SectorSize
	//
	// Defines the sector size to use with the virtual disk
	property int SectorSize
	{
		int get(void);
		void set(int value);
	}

	// SourceDiskPath
	//
	// Fully qualified path to pre-populate the new virtual disk object with block data from an existing disk
	property String^ SourceDiskPath
	{
		String^ get(void);
		void set(String^ value);
	}

	// SourceDiskType
	//
	// Describes the source virtual disk type specified by SourceDiskPath 
	property VirtualDiskType SourceDiskType
	{
		VirtualDiskType get(void);
		void set(VirtualDiskType value);
	}

	// UniqueIdentifier
	//
	// Gets/sets the virtual disk unique identifier
	property Guid UniqueIdentifier
	{
		Guid get(void);
		void set(Guid value);
	}

	//// UseCachedIO
	////
	//// Gets/sets a flag to open the new disk in cached IO mode rather than
	//// no buffering/write through mode
	//property bool UseCachedIO
	//{
	//	bool get(void) { return m_cachedIO; }
	//	void set(bool value) { m_cachedIO = value; }
	//}

private:

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

#endif	// __VIRTUALDISKCREATEPARAMETERS2_H_
