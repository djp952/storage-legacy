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

#ifndef __VIRTUALDISKOPENPARAMETERS_H_
#define __VIRTUALDISKOPENPARAMETERS_H_
#pragma once

#include "VirtualDiskStorageType.h"
#include "VirtualDiskUtil.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::ComponentModel;

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// Class VirtualDiskOpenParameters
//
// Contains virtual hard disk (VHD) open parameters
//---------------------------------------------------------------------------

public ref class VirtualDiskOpenParameters sealed
{
public:

	// Instance Constructors
	VirtualDiskOpenParameters() { Construct(String::Empty, VirtualDiskStorageType::Auto); }
	VirtualDiskOpenParameters(String^ path) { Construct(path, VirtualDiskStorageType::Auto); }
	VirtualDiskOpenParameters(String^ path, VirtualDiskStorageType type) { Construct(path, type); }

	//-----------------------------------------------------------------------
	// Properties

	// ManualParentChain
	//
	// Flag indicating that the differencing disk parent chain will be created manually
	property bool ManualParentChain
	{
		bool get(void) { return m_customDiffChain; }
		void set(bool value) { m_customDiffChain = value; }
	}

	// InformationOnly
	//
	// Flag indicating if the disk handle can only be used to get information
	property bool InformationOnly
	{
		bool get(void) { return m_getInfoOnly; }
		void set(bool value) { m_getInfoOnly = value; }
	}

	// NoParentDisks
	//
	// Open the disk without any differencing chain parents
	property bool NoParentDisks
	{
		bool get(void) { return m_noParents; }
		void set(bool value) { m_noParents = value; }
	}

	// Path
	//
	// Gets/sets the path to the new virtual disk object
	property String^ Path
	{
		String^ get(void) { return m_path; }
		void set(String^ value) { m_path = value; }
	}

	// ReadOnly
	//
	// Flag indicating if the backing store will be opened in read-only mode
	property bool ReadOnly
	{
		bool get(void) { return m_readOnly; }
		void set(bool value) { m_readOnly = value; }
	}

	// Type
	//
	// Gets/sets the virtual disk type of the parent disk object
	property VirtualDiskStorageType Type
	{
		VirtualDiskStorageType get(void) { return m_storageType; }
		void set(VirtualDiskStorageType value) { m_storageType = value; }
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

	// Releases strcuture allocated by ToOPEN_VIRTUAL_DISK_PARAMETERS()
	static void FreeOPEN_VIRTUAL_DISK_PARAMETERS(POPEN_VIRTUAL_DISK_PARAMETERS pParams);

	// Gets the OPEN_VIRTUAL_DISK_FLAG bitmask
	OPEN_VIRTUAL_DISK_FLAG ToOPEN_VIRTUAL_DISK_FLAG(void);

	// Converts the state of this object into an unmanaged OPEN_VIRTUAL_DISK_PARAMETERS
	OPEN_VIRTUAL_DISK_VERSION ToOPEN_VIRTUAL_DISK_PARAMETERS(POPEN_VIRTUAL_DISK_PARAMETERS* ppParams);

private:

	//-----------------------------------------------------------------------
	// Private Member Functions

	// Pseudo-constructor
	void Construct(String^ path, VirtualDiskStorageType type);

	//-----------------------------------------------------------------------
	// Member Variables

	String^						m_path;
	VirtualDiskStorageType		m_storageType;
	bool						m_getInfoOnly;
	bool						m_readOnly;
	bool						m_noParents;
	bool						m_cachedIO;
	bool						m_customDiffChain;
	Guid						m_resiliencyGuid;
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKOPENPARAMETERS_H_
