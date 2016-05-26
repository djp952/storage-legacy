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

#include "VirtualDiskOpenFlags.h"
#include "VirtualDiskType.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class VirtualDiskOpenParameters
//
// Virtual disk open operation parameters
//---------------------------------------------------------------------------

public ref struct VirtualDiskOpenParameters sealed
{
	// Instance Constructors
	//
	VirtualDiskOpenParameters() {}
	VirtualDiskOpenParameters(String^ path) : Path(path) {}
	VirtualDiskOpenParameters(String^ path, VirtualDiskOpenFlags flags) : Path(path), Flags(flags) {}

	//-----------------------------------------------------------------------
	// Fields

	// Flags
	//
	// Operation flags
	VirtualDiskOpenFlags Flags = VirtualDiskOpenFlags::None;

	// InformationOnly
	//
	// Flag indicating that the operation is only for getting vdisk information
	bool InformationOnly = false;

	// Path
	//
	// Path to the virtual disk to be opened
	String^ Path = String::Empty;

	// ReadOnlyBackingStore
	//
	// Flag indicating if the backing store is to opened as read-only
	bool ReadOnlyBackingStore = false;

	// ResiliencyGuid
	//
	// Resiliency GUID to specify when opening files
	Guid ResiliencyGuid = Guid::Empty;

	// SnapshotIdentifier
	//
	// Snapshot identifier GUID
	Guid SnapshotIdentifier = Guid::Empty;

	// Type
	//
	// The virtual disk type
	VirtualDiskType Type = VirtualDiskType::Unknown;
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKOPENPARAMETERS_H_
