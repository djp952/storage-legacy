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

#ifndef __GPTPARTITIONLIST_H_
#define __GPTPARTITIONLIST_H_
#pragma once

#include "DiskUtil.h"					// Include DiskUtil declarations
#include "GptDisk.h"					// Include GptDisk declarations
#include "GptPartition.h"				// Include GptPartition declarations
#include "PartitionList.h"				// Include PartitionList declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::Collections::Generic;
using namespace System::ComponentModel;

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// Class GptPartitionList
//
// GPT partition management collection
//---------------------------------------------------------------------------

public ref class GptPartitionList : public PartitionList<GptPartition^>
{
public:

	//-----------------------------------------------------------------------
	// Member Functions

	// Add
	//
	// Automatically adds a new partition to the disk
	GptPartition^ Add(GptPartitionType type);
	GptPartition^ Add(GptPartitionType type, __int64 length);
	GptPartition^ Add(GptPartitionType type, __int64 length, Guid partitionId);

internal:

	// INTERNAL CONSTRUCTORS
	GptPartitionList(Disk^ disk);

private:
	
	//-----------------------------------------------------------------------
	// Private Member Functions

	// FindAlignedLargestFreeSpace
	//
	// Locates the largest available block of free space on the disk
	__int64 FindAlignedLargestFreeSpace([Out] __int64% length);

	// FindAlignedStartingOffsetForLength
	//
	// Locates an available starting offset for a specific partition length
	__int64 FindAlignedStartingOffsetForLength(__int64 length);

	// FindAvailablePartitionNumber
	//
	// Locates an available partition number
	int FindAvailablePartitionNumber(void);

	//-----------------------------------------------------------------------
	// Member Variables

	GptDisk^					m_disk;				// GPT disk instance
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)

#endif	// __GPTPARTITIONLIST_H_
