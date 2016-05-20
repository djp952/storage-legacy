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

#ifndef __VIRTUALDISKOPENFLAGS_H_
#define __VIRTUALDISKOPENFLAGS_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Enum VirtualDiskOpenFlags
//
// Provides flag that control the behavior of an open operation
//---------------------------------------------------------------------------

[FlagsAttribute]
public enum class VirtualDiskOpenFlags
{
    None					= OPEN_VIRTUAL_DISK_FLAG::OPEN_VIRTUAL_DISK_FLAG_NONE,
    NoParents				= OPEN_VIRTUAL_DISK_FLAG::OPEN_VIRTUAL_DISK_FLAG_NO_PARENTS,
    CachedIO				= OPEN_VIRTUAL_DISK_FLAG::OPEN_VIRTUAL_DISK_FLAG_CACHED_IO,
    CustomDifferencingChain	= OPEN_VIRTUAL_DISK_FLAG::OPEN_VIRTUAL_DISK_FLAG_CUSTOM_DIFF_CHAIN,
    ParentCachedIO			= OPEN_VIRTUAL_DISK_FLAG::OPEN_VIRTUAL_DISK_FLAG_PARENT_CACHED_IO,
    VhdSetFileOnly			= OPEN_VIRTUAL_DISK_FLAG::OPEN_VIRTUAL_DISK_FLAG_VHDSET_FILE_ONLY,

	// The following constants are documented as reserved
	//
    //BlankFile				= OPEN_VIRTUAL_DISK_FLAG::OPEN_VIRTUAL_DISK_FLAG_BLANK_FILE,
    //BootDrive				= OPEN_VIRTUAL_DISK_FLAG::OPEN_VIRTUAL_DISK_FLAG_BOOT_DRIVE,
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKOPENFLAGS_H_
