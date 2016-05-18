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

#ifndef __GPTDISK_H_
#define __GPTDISK_H_
#pragma once

#include "Disk.h"						// Include Disk declarations
#include "DiskApi.h"					// Include DiskApi declarations
#include "DiskUtil.h"					// Include DiskUtil declarations
#include "GptPartition.h"				// Include GptPartition declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings
#pragma warning(disable:4461)			// Finalizer without destructor

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki.storage)

// FORWARD DECLARATIONS
ref class GptPartitionList;

//---------------------------------------------------------------------------
// Class GptDisk
//
// EFI/GPT Disk Management Class
//---------------------------------------------------------------------------

public ref class GptDisk : public Disk
{
public:

	//-----------------------------------------------------------------------
	// Properties

	// DiskId
	//
	// Gets/sets the disk identification guid
	property Guid DiskId
	{
		Guid get(void) { return DiskUtil::UUIDToSysGuid(m_pLayoutGpt->DiskId); }
		void set(Guid value);
	}

	// MaxPartitionCount
	//
	// Gets the maximum number of partitions allowed on the disk
	property int MaxPartitionCount
	{
		int get(void) { return static_cast<int>(m_pLayoutGpt->MaxPartitionCount); }
	}

	// Partitions
	//
	// Gets a reference to the contained Partition collection
	property GptPartitionList^ Partitions
	{
		GptPartitionList^ get(void) { return m_partitions; }
	}

	// StartingUsableOffset
	//
	// Gets the starting byte offset of the first usable block
	property __int64 StartingUsableOffset
	{
		__int64 get(void) { return static_cast<__int64>(m_pLayoutGpt->StartingUsableOffset.QuadPart); }
	}

	// UsableLength
	//
	// Gets the size of the usable blocks on the disk, in bytes
	property __int64 UsableLength
	{
		__int64 get(void) { return static_cast<__int64>(m_pLayoutGpt->UsableLength.QuadPart); }
	}

internal:

	// INTERNAL CONSTRUCTORS
	GptDisk(HANDLE handle, PDISK_GEOMETRY_EX pGeometry, PDRIVE_LAYOUT_INFORMATION_EX pLayout);

private:

	// FINALIZER
	!GptDisk();

	//-----------------------------------------------------------------------
	// Member Variables

	PDRIVE_LAYOUT_INFORMATION_GPT	m_pLayoutGpt;		// GPT specific layout
	GptPartitionList^				m_partitions;		// Partition collection
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)

#endif	// __GPTDISK_H_
