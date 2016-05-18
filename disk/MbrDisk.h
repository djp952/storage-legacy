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

#ifndef __MBRDISK_H_
#define __MBRDISK_H_
#pragma once

#include "Disk.h"						// Include Disk declarations
#include "DiskApi.h"					// Include DiskApi declarations
#include "MbrPartition.h"				// Include MbrPartition declarations
#include "PartitionList.h"				// Include PartitionList declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings
#pragma warning(disable:4461)			// Finalizer without destructor

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class MbrDisk
//
// MBR Disk Management Class
//---------------------------------------------------------------------------

public ref class MbrDisk : public Disk
{
public:

	//-----------------------------------------------------------------------
	// Properties

	// Partitions
	//
	// Gets a reference to the contained Partition collection
	property PartitionList<MbrPartition^>^ Partitions
	{
		PartitionList<MbrPartition^>^ get(void) { return m_partitions; }
	}

	// Signature
	//
	// Gets/sets the disk signature
	property int Signature
	{
		int get(void) { return static_cast<int>(m_pLayoutMbr->Signature); }
		void set(int value);
	}

internal:

	// INTERNAL CONSTRUCTORS
	MbrDisk(HANDLE handle, PDISK_GEOMETRY_EX pGeometry, PDRIVE_LAYOUT_INFORMATION_EX pLayout);

private:

	// FINALIZER
	!MbrDisk();

	//-----------------------------------------------------------------------
	// Member Variables

	PDRIVE_LAYOUT_INFORMATION_MBR	m_pLayoutMbr;	// MBR specific layout
	PartitionList<MbrPartition^>^	m_partitions;	// Partition collection
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __GPTDISK_H_
