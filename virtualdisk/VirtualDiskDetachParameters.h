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

#ifndef __VIRTUALDISKDETACHPARAMETERS_H_
#define __VIRTUALDISKDETACHPARAMETERS_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

// FORWARD DECLARATIONS
//
enum class VirtualDiskDetachFlags;

//---------------------------------------------------------------------------
// Class VirtualDiskDetachParameters
//
// Virtual disk detach operation parameters
//---------------------------------------------------------------------------

public ref struct VirtualDiskDetachParameters sealed
{
	// Instance Constructors
	//
	VirtualDiskDetachParameters() {}
	VirtualDiskDetachParameters(VirtualDiskDetachFlags flags) : Flags(flags) {}

	//-----------------------------------------------------------------------
	// Fields

	// Flags
	//
	// Operation flags
	VirtualDiskDetachFlags Flags = VirtualDiskDetachFlags::None;

	// ProviderFlags
	//
	// Operation provider-specific flags
	unsigned int ProviderFlags = 0U;
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKDETACHPARAMETERS_H_
