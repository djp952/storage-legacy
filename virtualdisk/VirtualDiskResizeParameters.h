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

#ifndef __VIRTUALDISKRESIZEPARAMETERS_H_
#define __VIRTUALDISKRESIZEPARAMETERS_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

// FORWARD DECLARATIONS
//
enum class VirtualDiskResizeFlags;

//---------------------------------------------------------------------------
// Class VirtualDiskResizeParameters
//
// Virtual disk resize operation parameters
//---------------------------------------------------------------------------

public ref struct VirtualDiskResizeParameters sealed
{
	// Instance Constructors
	//
	VirtualDiskResizeParameters() {}
	VirtualDiskResizeParameters(unsigned __int64 newsize) : NewSize(newsize) {}
	VirtualDiskResizeParameters(unsigned __int64 newsize, VirtualDiskResizeFlags flags) : NewSize(newsize), Flags(flags) {}

	//-----------------------------------------------------------------------
	// Fields

	// Flags
	//
	// Operation flags
	VirtualDiskResizeFlags Flags = VirtualDiskResizeFlags::None;

	// NewSize
	//
	// New size to resize the virtual disk to
	unsigned __int64 NewSize = 0ULL;
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKRESIZEPARAMETERS_H_
