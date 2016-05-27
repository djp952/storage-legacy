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

#ifndef __VIRTUALDISKMERGEPARAMETERS_H_
#define __VIRTUALDISKMERGEPARAMETERS_H_
#pragma once

#include "VirtualDiskMergeFlags.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class VirtualDiskMergeParameters
//
// Virtual disk merge operation parameters
//---------------------------------------------------------------------------

public ref struct VirtualDiskMergeParameters sealed
{
	// Instance Constructors
	//
	VirtualDiskMergeParameters() {}
	VirtualDiskMergeParameters(unsigned int sourcedepth, unsigned int targetdepth) : SourceDepth(sourcedepth), TargetDepth(targetdepth) {}
	VirtualDiskMergeParameters(unsigned int sourcedepth, unsigned int targetdepth, VirtualDiskMergeFlags flags) : SourceDepth(sourcedepth), TargetDepth(targetdepth), Flags(flags) {}

	//-----------------------------------------------------------------------
	// Fields

	// Flags
	//
	// Operation flags
	VirtualDiskMergeFlags Flags = VirtualDiskMergeFlags::None;

	// SourceDepth
	//
	// Specifies the source depth within the differencing chain
	unsigned int SourceDepth = MERGE_VIRTUAL_DISK_DEFAULT_MERGE_DEPTH;		// Leaf disk

	// TargetDepth
	//
	// Specifies the target depth within the differencing chain
	unsigned int TargetDepth = MERGE_VIRTUAL_DISK_DEFAULT_MERGE_DEPTH + 1;	// Immediate parent disk
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKMERGEPARAMETERS_H_
