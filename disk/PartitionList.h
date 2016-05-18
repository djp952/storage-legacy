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

#ifndef __PARTITIONLIST_H_
#define __PARTITIONLIST_H_
#pragma once

#include "Disk.h"						// Include Disk declarations
#include "DiskApi.h"					// Include DiskApi declarations
#include "Partition.h"					// Include Partition declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::Collections::Generic;
using namespace System::ComponentModel;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class PartitionList
//
// Partition management collection
//---------------------------------------------------------------------------

generic <class T>
where T : Partition
public ref class PartitionList : public List<T>
{
public:

	//-----------------------------------------------------------------------
	// Member Functions

	// Persist
	//
	// Persists the contents of the collection to the disk
	void Persist(void);

	// Refresh
	//
	// Refreshes the contents of the collection
	void Refresh(void);

	//-----------------------------------------------------------------------
	// Properties

	// Alignment
	//
	// Gets/sets the partition alignment value for the disk
	property __int64 DefaultAlignment
	{
		__int64 get(void) { return m_alignment; }
		void set(__int64 value) { m_alignment = value; }
	}

internal:

	// INTERNAL CONSTRUCTORS
	PartitionList(Disk^ disk);

protected:
	
	//-----------------------------------------------------------------------
	// Protected Member Variables

	__int64				m_alignment;		// Partition alignment

private:

	//-----------------------------------------------------------------------
	// Private Member Functions

	// CreatePartitionClass
	//
	// Workaround for C++/CLI generic limitation
	static T CreatePartitionClass(PPARTITION_INFORMATION_EX pInfo);

	// GetDefaultAlignment
	//
	// Gets the default partition alignment for the parent disk
	static __int64 GetDefaultAlignment(__int64 length);

	//-----------------------------------------------------------------------
	// Member Variables

	initonly Disk^			m_disk;			// Parent disk object
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __PARTITIONLIST_H_
