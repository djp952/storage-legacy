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

#ifndef __VIRTUALDISKATTACHFLAGS_H_
#define __VIRTUALDISKATTACHFLAGS_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Enum VirtualDiskAttachFlags
//
// Provides flag that control the behavior of an attach operation
//---------------------------------------------------------------------------

[FlagsAttribute]
public enum class VirtualDiskAttachFlags
{
    None					= ATTACH_VIRTUAL_DISK_FLAG::ATTACH_VIRTUAL_DISK_FLAG_NONE,
    ReadOnly				= ATTACH_VIRTUAL_DISK_FLAG::ATTACH_VIRTUAL_DISK_FLAG_READ_ONLY,
    NoDriveLetter			= ATTACH_VIRTUAL_DISK_FLAG::ATTACH_VIRTUAL_DISK_FLAG_NO_DRIVE_LETTER,
    PermanentLifetime		= ATTACH_VIRTUAL_DISK_FLAG::ATTACH_VIRTUAL_DISK_FLAG_PERMANENT_LIFETIME,
    NoSecurityDescriptor	= ATTACH_VIRTUAL_DISK_FLAG::ATTACH_VIRTUAL_DISK_FLAG_NO_SECURITY_DESCRIPTOR,

	// The following constants are documented as reserved
	//
    // NoLocalHost				= ATTACH_VIRTUAL_DISK_FLAG::ATTACH_VIRTUAL_DISK_FLAG_NO_LOCAL_HOST,
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKATTACHFLAGS_H_
