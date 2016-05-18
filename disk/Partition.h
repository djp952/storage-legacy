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

#ifndef __PARTITION_H_
#define __PARTITION_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings
#pragma warning(disable:4461)			// Finalizer without destructor

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class Partition
//
// Information about a RAW disk partition
//---------------------------------------------------------------------------

public ref class Partition
{
public:

	// Instance Constructor
	Partition();

	//-------------------------------------------------------------------------
	// Properties

	// Length
	//
	// The length of the partition
	property __int64 PartitionLength
	{
		__int64 get(void) { return static_cast<__int64>(m_pInfo->PartitionLength.QuadPart); }
		void set(__int64 value);
	}

	// Number
	//
	// Gets/Sets the partition number
	property int PartitionNumber
	{
		int get(void) { return static_cast<int>(m_pInfo->PartitionNumber); }
		void set(int value);
	}

	// StartingOffset
	//
	// The starting offset of the partition on the disk
	property __int64 StartingOffset
	{
		__int64 get(void) { return static_cast<__int64>(m_pInfo->StartingOffset.QuadPart); }
		void set(__int64 value);
	}

internal:

	// INTERNAL CONSTRUCTORS
	Partition(PPARTITION_INFORMATION_EX pInfo);

	//-----------------------------------------------------------------------
	// Internal Operators

	// operator PPARTITION_INFORMATION_EX
	//
	// Exposes the contained PARTITION_INFORMATION_EX pointer
	static operator PPARTITION_INFORMATION_EX(Partition^ rhs);

	//-----------------------------------------------------------------------
	// Internal Member Functions

	// CompareByPartitionNumber
	//
	// Comparison<T> function for sorting partitions by number
	static int CompareByPartitionNumber(Partition^ lhs, Partition^ rhs);

	// CompareByStartingOffset
	//
	// Comparison<T> function for sorting partitions by offset
	static int CompareByStartingOffset(Partition^ lhs, Partition^ rhs);

protected:

	//-----------------------------------------------------------------------
	// Protected Member Variables

	PPARTITION_INFORMATION_EX		m_pInfo;		// Partition information

private:

	// FINALIZER
	!Partition();
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __PARTITION_H_
