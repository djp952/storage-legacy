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

#ifndef __VIRTUALDISKRESIZEFLAGS_H_
#define __VIRTUALDISKRESIZEFLAGS_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Enum VirtualDiskResizeFlags
//
// Provides flag that control the behavior of a resize operation
//---------------------------------------------------------------------------

[FlagsAttribute]
public enum class VirtualDiskResizeFlags
{
	None							= RESIZE_VIRTUAL_DISK_FLAG::RESIZE_VIRTUAL_DISK_FLAG_NONE,
	AllowUnsafeVirtualSize			= RESIZE_VIRTUAL_DISK_FLAG::RESIZE_VIRTUAL_DISK_FLAG_ALLOW_UNSAFE_VIRTUAL_SIZE,
	ResizeToSmallestSafeVirtualSize	= RESIZE_VIRTUAL_DISK_FLAG::RESIZE_VIRTUAL_DISK_FLAG_RESIZE_TO_SMALLEST_SAFE_VIRTUAL_SIZE,
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKRESIZEFLAGS_H_
