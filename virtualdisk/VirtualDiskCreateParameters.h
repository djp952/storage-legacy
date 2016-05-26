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

#include "VirtualDiskCreateFlags.h"
#include "VirtualDiskOpenFlags.h"
#include "VirtualDiskType.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::Security::AccessControl;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class VirtualDiskCreateParameters
//
// Virtual disk create operation parameters
//---------------------------------------------------------------------------

public ref struct VirtualDiskCreateParameters sealed
{
	// Instance Constructors
	//
	VirtualDiskCreateParameters() {}
	VirtualDiskCreateParameters(String^ path, VirtualDiskType type, unsigned __int64 maxsize) : Path(path), Type(type), MaximumSize(maxsize) {}
	VirtualDiskCreateParameters(String^ path, VirtualDiskType type, unsigned __int64 maxsize, VirtualDiskCreateFlags flags) : Path(path), Type(type), MaximumSize(maxsize), Flags(flags) {}

	//-----------------------------------------------------------------------
	// Fields

	// BackingStorageType
	//
	// Disk type to use when creating backing storage (todo: check this)
	VirtualDiskType BackingStorageType = VirtualDiskType::Unknown;

	// BlockSize
	//
	// Specifies the virtual disk block size in bytes
	unsigned int BlockSize = CREATE_VIRTUAL_DISK_PARAMETERS_DEFAULT_BLOCK_SIZE;

	// Flags
	//
	// Operation flags
	VirtualDiskCreateFlags Flags = VirtualDiskCreateFlags::None;

	// MaximumSize
	//
	// Specifies the maximum size of the virtual disk
	unsigned __int64 MaximumSize = 0ULL;

	// OpenFlags
	//
	// Flags indicating how the virtual disk should be opened
	VirtualDiskOpenFlags OpenFlags = VirtualDiskOpenFlags::None;

	// ParentDiskPath
	//
	// Path to the parent disk
	String^ ParentDiskPath = String::Empty;

	// ParentDiskType
	//
	// Type of disk referred to by ParentDiskPath
	VirtualDiskType ParentDiskType = VirtualDiskType::Unknown;

	// Path
	//
	// Path to the virtual disk to be createed
	String^ Path = String::Empty;

	// ProviderFlags
	//
	// Operation provider-specific flags
	unsigned int ProviderFlags = 0U;

	// ResiliencyGuid
	//
	// Resiliency GUID to specify when opening files
	Guid ResiliencyGuid = Guid::Empty;

	// SectorSize
	//
	// Specifies the virtual disk sector size in bytes
	unsigned int SectorSize = 512UL;

	// SecurityDescriptor
	//
	// Security descriptor to apply to the created virtual disk
	FileSecurity^ SecurityDescriptor = nullptr;

	// SourceDiskPath
	//
	// Path to a source disk from which to populate the new disk
	String^ SourceDiskPath = String::Empty;
	
	// SourceDiskType
	//
	// Type of disk referred to by SourceDiskPath
	VirtualDiskType SourceDiskType = VirtualDiskType::Unknown;

	// SourceLimitPath
	//
	// I quite honestly have no idea what this is for yet
	String^ SourceLimitPath = String::Empty;

	// Type
	//
	// The virtual disk type to create
	VirtualDiskType Type = VirtualDiskType::VHDX;

	// UniqueIdentifier
	//
	// The virtual disk unique identifier GUID
	Guid UniqueIdentifier = Guid::Empty;
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKCREATEPARAMETERS_H_
