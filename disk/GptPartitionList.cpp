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

#include "stdafx.h"						// Include project pre-compiled headers
#include "GptPartitionList.h"			// Include GptPartitionList decls

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// GptPartitionList Constructor (internal)
//
// Arguments:
//
//	disk		- Parent Disk object (must be GptDisk)

GptPartitionList::GptPartitionList(Disk^ disk) : PartitionList(disk)
{
	if(disk == nullptr) throw gcnew ArgumentNullException("disk");
	
	// Cast the Disk instance into GptDisk (will throw on invalid conversion)
	m_disk = safe_cast<GptDisk^>(disk);
}

//---------------------------------------------------------------------------
// GptPartitionList::Add
//
// Automatically adds a new partition that consumes the largest available
// chunk of free space on the disk (this is how tools like DISKPART behave)
//
// Arguments:
//
//	type		- Type of GPT partition to be added

GptPartition^ GptPartitionList::Add(GptPartitionType type)
{
	GptPartition^		partition;			// New partition
	__int64				offset;				// New partition offset
	__int64				length;				// New partition length

	// Locate the largest block of available free space on the disk
	offset = FindAlignedLargestFreeSpace(length);
	if(offset < 0) throw gcnew Exception("Insufficient free space exists for the partition");

	// Construct a new GptPartition and add it to the collection
	partition = gcnew GptPartition(type, FindAvailablePartitionNumber(), length, offset, Guid::NewGuid());
	Add(partition);

	return partition;			// Return reference for additional specialization
}

//---------------------------------------------------------------------------
// GptPartitionList::Add
//
// Automatically adds a new partition of the specified length into the first
// available contiguous block of empty disk space that can fit it.  Note that
// the length will be aligned DOWN to the current default alignment
//
// Arguments:
//
//	type			- Type of GPT partition to be added
//	length			- Requested partition length; may be aligned down

GptPartition^ GptPartitionList::Add(GptPartitionType type, __int64 length)
{
	return Add(type, length, Guid::NewGuid());
}

//---------------------------------------------------------------------------
// GptPartitionList::Add
//
// Automatically adds a new partition of the specified length into the first
// available contiguous block of empty disk space that can fit it.  Note that
// the length will be aligned DOWN to the current default alignment
//
// Arguments:
//
//	type			- Type of GPT partition to be added
//	length			- Requested partition length; may be aligned down
//	partitionId		- Partition identifier GUID

GptPartition^ GptPartitionList::Add(GptPartitionType type, __int64 length, Guid partitionId)
{
	GptPartition^		partition;		// New partition
	__int64				offset;			// Calculated offset for the partition

	if(length <= 0) throw gcnew ArgumentOutOfRangeException("length");

	// Align the requested length down to the nearest boundary
	length = DiskUtil::AlignDown(length, m_alignment);

	// Locate an aligned offset that will fit a partition of the requested length
	offset = FindAlignedStartingOffsetForLength(length);
	if(offset < 0) throw gcnew Exception("Insufficient free space exists for the partition");

	// Construct a new GptPartition and add it to the collection
	partition = gcnew GptPartition(type, FindAvailablePartitionNumber(), length, offset, partitionId);
	Add(partition);

	return partition;			// Return reference for additional specialization
}

//---------------------------------------------------------------------------
// GptPartitionList::FindAlignedLargestFreeSpace (private)
//
// Locates the largest available block of free space on the disk and returns
// an aligned offset and length for it
//
// Arguments:
//
//	length			- [OUT] receives the aligned length of the free space

__int64 GptPartitionList::FindAlignedLargestFreeSpace(__int64% length)
{
	List<KeyValuePair<__int64, __int64>>^	freeSpace;		// Free space list
	__int64									offset = -1;	// Offset to return

	// Generate a view of the collection sorted by starting offset
	array<Partition^>^ sorted = this->ToArray();
	Array::Sort(sorted, gcnew Comparison<Partition^>(Partition::CompareByStartingOffset));

	// Construct the collection to hold all the free space
	freeSpace = gcnew List<KeyValuePair<__int64, __int64>>();

	// Start with the extents of the empty disk
	__int64	start = DiskUtil::AlignUp(m_disk->StartingUsableOffset, m_alignment);
	__int64	end = DiskUtil::AlignDown(m_disk->StartingUsableOffset + m_disk->UsableLength, m_alignment);

	// Iterate over each partition to find the free space
	for(int index = 0; index < sorted->Length; index++) {

		// Free space detected -- add it to the collection
		if(sorted[index]->StartingOffset > start)
			freeSpace->Add(KeyValuePair<__int64, __int64>(start, sorted[index]->StartingOffset - start));

		// Move start to the end of this partition
		start = DiskUtil::AlignUp(sorted[index]->StartingOffset + sorted[index]->PartitionLength, m_alignment);
	}

	// If we didn't reach the end of the disk during enumeration, there is free space left
	if(start < end) freeSpace->Add(KeyValuePair<__int64, __int64>(start, end - start));

	// Iterate over the collection to find the largest space
	for each(KeyValuePair<__int64, __int64> pair in freeSpace)
		if(pair.Value > length) { offset = pair.Key; length = pair.Value; }
	
	// Align the largest length down and if it's still usable
	length = DiskUtil::AlignDown(length, m_alignment);
	return (length > 0) ? offset : -1;
}

//---------------------------------------------------------------------------
// GptPartitionList::FindAlignedStartingOffsetForLength (private)
//
// Scans the current partition table and locates a valid starting offset
// with which to place a partition of the specified length
//
// Arguments:
//
//	length			- Requested partition length

__int64 GptPartitionList::FindAlignedStartingOffsetForLength(__int64 length)
{
	// Generate a view of the collection sorted by starting offset
	array<Partition^>^ sorted = this->ToArray();
	Array::Sort(sorted, gcnew Comparison<Partition^>(Partition::CompareByStartingOffset));

	// Start with the extents of the empty disk
	__int64	start = DiskUtil::AlignUp(m_disk->StartingUsableOffset, m_alignment);
	__int64	end = DiskUtil::AlignDown(start + m_disk->UsableLength, m_alignment);

	// Iterate over all of the current partitions on the disk
	for(int index = 0; index < sorted->Length; index++) {

		// If there is free space that will fit the requested length, we're done
		if((sorted[index]->StartingOffset > start) && ((sorted[index]->StartingOffset - start) > length)) return start;

		// Not free space or length won't fit; move start to the end of this partition
		start = DiskUtil::AlignUp(sorted[index]->StartingOffset + sorted[index]->PartitionLength, m_alignment);
	}

	// Reached the end of current partitioned data without finding a free block,
	// check to see if there is room after the partitioned data for the length
	return ((end - start) >= length) ? start : -1;
}

//---------------------------------------------------------------------------
// GptPartitionList::FindAvailablePartitionNumber (private)
//
// Locates an available partition number
//
// Arguments:
//
//	NONE

int GptPartitionList::FindAvailablePartitionNumber(void)
{
	int					number = 0;					// Start at zero

	// Generate a view of the collection sorted by partition number
	array<Partition^>^ sorted = this->ToArray();
	Array::Sort(sorted, gcnew Comparison<Partition^>(Partition::CompareByPartitionNumber));

	// Iterate over all the existing partitions until a free number is located
	for(int index = 0; index < sorted->Length; index++) {

		if(sorted[index]->PartitionNumber <= number) number++;
		else break;
	}

	return number;
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
