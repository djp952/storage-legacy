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

#ifndef __VIRTUALDISKCREATEFLAGS_H_
#define __VIRTUALDISKCREATEFLAGS_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Enum VirtualDiskCreateFlags
//
// Provides flag that control the behavior of a create operation
//---------------------------------------------------------------------------

[FlagsAttribute]
public enum class VirtualDiskCreateFlags
{
    None								= CREATE_VIRTUAL_DISK_FLAG::CREATE_VIRTUAL_DISK_FLAG_NONE,
    FullPhysicalAllocation				= CREATE_VIRTUAL_DISK_FLAG::CREATE_VIRTUAL_DISK_FLAG_FULL_PHYSICAL_ALLOCATION,
    PreventWritesToSourceDisk			= CREATE_VIRTUAL_DISK_FLAG::CREATE_VIRTUAL_DISK_FLAG_PREVENT_WRITES_TO_SOURCE_DISK,
    DoNotCopyMetadataFromParent			= CREATE_VIRTUAL_DISK_FLAG::CREATE_VIRTUAL_DISK_FLAG_DO_NOT_COPY_METADATA_FROM_PARENT,
    CreateBackingStorage				= CREATE_VIRTUAL_DISK_FLAG::CREATE_VIRTUAL_DISK_FLAG_CREATE_BACKING_STORAGE,
    UseChangeTrackingSourceLimit		= CREATE_VIRTUAL_DISK_FLAG::CREATE_VIRTUAL_DISK_FLAG_USE_CHANGE_TRACKING_SOURCE_LIMIT,
    PreserveParentChangeTrackingState	= CREATE_VIRTUAL_DISK_FLAG::CREATE_VIRTUAL_DISK_FLAG_PRESERVE_PARENT_CHANGE_TRACKING_STATE,
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKCREATEFLAGS_H_
