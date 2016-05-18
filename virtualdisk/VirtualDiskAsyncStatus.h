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

#ifndef __VIRTUALDISKASYNCSTATUS_H_
#define __VIRTUALDISKASYNCSTATUS_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::ComponentModel;
using namespace System::Threading;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class VirtualDiskAsyncStatus
//
// Provides status information about an asynchronous virtual disk operation
//---------------------------------------------------------------------------

public value class VirtualDiskAsyncStatus sealed
{
public:

	//-----------------------------------------------------------------------
	// Fields

	initonly int			Status;
	initonly __int64		CurrentValue;
	initonly __int64		CompletionValue;

	//-----------------------------------------------------------------------
	// Properties

	// PercentComplete
	//
	// Convenience property to expose the operation complete percentage
	property float PercentComplete
	{
		float get(void) 
		{ 
			if(CompletionValue <= 0) return 0.0f;
			else return (static_cast<float>(CurrentValue) / static_cast<float>(CompletionValue)) * 100.0f; 
		}
	}

internal:

	// CONSTRUCTOR
	VirtualDiskAsyncStatus(PVIRTUAL_DISK_PROGRESS progress) : 
		Status(progress->OperationStatus), CurrentValue(progress->CurrentValue), 
		CompletionValue(progress->CompletionValue) {}
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKASYNCSTATUS_H_
