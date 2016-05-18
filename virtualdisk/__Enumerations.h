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

#ifndef __ENUMERATIONS_H_
#define __ENUMERATIONS_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Enum AttachFlags
//
// Virtual Disk Attach Request Flags

public enum struct AttachFlags
{
	None						= ATTACH_VIRTUAL_DISK_FLAG_NONE,
	ReadOnly					= ATTACH_VIRTUAL_DISK_FLAG_READ_ONLY,
	NoDriveLetter				= ATTACH_VIRTUAL_DISK_FLAG_NO_DRIVE_LETTER,
	PermanentLifetime			= ATTACH_VIRTUAL_DISK_FLAG_PERMANENT_LIFETIME,
	NoLocalHost					= ATTACH_VIRTUAL_DISK_FLAG_NO_LOCAL_HOST
};

//---------------------------------------------------------------------------
// Enum DependentDiskFlags
//
// Contains virtual hard disk (VHD) dependency information flags

public enum struct DependentDiskFlags
{
	None						= DEPENDENT_DISK_FLAG_NONE,
	MultipleBackingFiles		= DEPENDENT_DISK_FLAG_MULT_BACKING_FILES,
	FullyAllocated				= DEPENDENT_DISK_FLAG_FULLY_ALLOCATED,
	ReadOnly					= DEPENDENT_DISK_FLAG_READ_ONLY,
	Remote						= DEPENDENT_DISK_FLAG_REMOTE,
	SystemVolume				= DEPENDENT_DISK_FLAG_SYSTEM_VOLUME,
	SystemVolumeParent			= DEPENDENT_DISK_FLAG_SYSTEM_VOLUME_PARENT,
	Removable					= DEPENDENT_DISK_FLAG_REMOVABLE,
	NoDriveLetter				= DEPENDENT_DISK_FLAG_NO_DRIVE_LETTER,
	Parent						= DEPENDENT_DISK_FLAG_PARENT,
	NoHostDisk					= DEPENDENT_DISK_FLAG_NO_HOST_DISK,
	PermanentLifetime			= DEPENDENT_DISK_FLAG_PERMANENT_LIFETIME
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __ENUMERATIONS_H_
