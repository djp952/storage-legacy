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

#ifndef __VIRTUALDISKATTACHPARAMETERS_H_
#define __VIRTUALDISKATTACHPARAMETERS_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::Security::AccessControl;

BEGIN_ROOT_NAMESPACE(zuki::storage)

// FORWARD DECLARATIONS
//
enum class VirtualDiskAttachFlags;

//---------------------------------------------------------------------------
// Class VirtualDiskAttachParameters
//
// Virtual disk attach operation parameters
//---------------------------------------------------------------------------

public ref struct VirtualDiskAttachParameters sealed
{
	// Instance Constructors
	//
	VirtualDiskAttachParameters() {}
	VirtualDiskAttachParameters(VirtualDiskAttachFlags flags) : Flags(flags) {}

	//-----------------------------------------------------------------------
	// Fields

	// Flags
	//
	// Operation flags
	VirtualDiskAttachFlags Flags = VirtualDiskAttachFlags::None;

	// ProviderFlags
	//
	// Operation provider-specific flags
	unsigned int ProviderFlags = 0U;

	// SecurityDescriptor
	//
	// Security descriptor to apply to the attached virtual disk
	FileSecurity^ SecurityDescriptor = nullptr;
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKATTACHPARAMETERS_H_
