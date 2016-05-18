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
#include "GptPartition.h"				// Include GptPartition declarations
#include "MbrPartition.h"				// Include MbrPartition declarations
#include "PartitionList.h"				// Include PartitionList declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// PartitionList Constructor (internal)
//
// Arguments:
//
//	handle		- Parent Disk object's DiskSafeHandle

generic <class T>
PartitionList<T>::PartitionList(Disk^ disk) : m_disk(disk)
{
	if(disk == nullptr) throw gcnew ArgumentNullException("disk");

	// Get the default alignment value for the disk
	m_alignment = GetDefaultAlignment(m_disk->Length);

	Refresh();				// Load the collection
}

//---------------------------------------------------------------------------
// PartitionList::ConstructPartitionClass (private, static)
//
// Creates the generic partition class; this overcomes a limitation of C++/CLI
// wherein "gcnew T" cannot be used.  If newer C++/CLI implementations
// resolve this issue, this function should be removed
//
// Arguments:
//
//	NONE

generic <class T>
T PartitionList<T>::CreatePartitionClass(PPARTITION_INFORMATION_EX pInfo)
{
	if(T::typeid == GptPartition::typeid) return safe_cast<T>(gcnew GptPartition(pInfo));
	else if(T::typeid == MbrPartition::typeid) return safe_cast<T>(gcnew MbrPartition(pInfo));
	else if(T::typeid == Partition::typeid) return safe_cast<T>(gcnew Partition(pInfo));
	else throw gcnew Exception("Unexpected generic Partition class specified");
}

//---------------------------------------------------------------------------
// PartitionList::GetDefaultAlignment (static, private)
//
// Gets the default partition alignment value for the parent disk
//
// Arguments:
//
//	length			- Length of the parent disk

generic <class T>
__int64 PartitionList<T>::GetDefaultAlignment(__int64 length)
{
	// Just go with the Windows default here, disks less than 4GB get 64K alignment
	// and everything else gets 1MiB alignment.  This data is in the Virtual Disk
	// Service registry settings, but it seemed kinda silly to look it up when the
	// rules are this trivial ....

	if(length <= Int32::MaxValue) return 65536;
	else return 1048576;
}

//---------------------------------------------------------------------------
// PartitionList::Persist
//
// Persists the current state of the collection to the underlying disk
//
// Arguments:
//
//	NONE

generic <class T>
void PartitionList<T>::Persist(void)
{
	DWORD								cbNewLayout;		// Buffer size
	PDRIVE_LAYOUT_INFORMATION_EX		pNewLayout;			// New layout
	PDRIVE_LAYOUT_INFORMATION_EX		pOldLayout;			// Original layout

	// Convert the collection into an array to work with it more easily
	array<T>^ partitions = List<T>::ToArray();

	// Calculate the size of the buffer required to hold all the data and allocate
	cbNewLayout = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + (sizeof(PARTITION_INFORMATION_EX) * partitions->Length);
	pNewLayout = reinterpret_cast<PDRIVE_LAYOUT_INFORMATION_EX>(new BYTE[cbNewLayout]);
	if(!pNewLayout) throw gcnew OutOfMemoryException();

	try {

		// Retrieve the current DRIVE_LAYOUT_INFORMATION for the disk
		DiskApi::GetDriveLayoutInformationEx(m_disk, &pOldLayout);

		try {

			// Start with a copy of the original DRIVE_LAYOUT_INFORMATION_EX
			memcpy(pNewLayout, pOldLayout, sizeof(DRIVE_LAYOUT_INFORMATION_EX));
			pNewLayout->PartitionCount = partitions->Length;

			// Copy the data for each partition into the new DRIVE_LAYOUT_INFORMATION
			for(int index = 0; index < partitions->Length; index++) {

				memcpy(&pNewLayout->PartitionEntry[index], static_cast<PPARTITION_INFORMATION_EX>(partitions[index]),
					sizeof(PARTITION_INFORMATION_EX));
			}

			// Apply the new drive layout and synchronize with the system
			DiskApi::SetDriveLayoutInformationEx(m_disk, cbNewLayout, pNewLayout);
			DiskApi::TryUpdateProperties(m_disk);
		}

		finally { pOldLayout = DiskApi::FreeDriveLayoutInformationEx(pOldLayout); }
	}

	finally { delete[] reinterpret_cast<BYTE*>(pNewLayout); }
}

//---------------------------------------------------------------------------
// PartitionList::Refresh
//
// Clears and reloads the collection contents from the underlying disk
//
// Arguments:
//
//	NONE

generic <class T>
void PartitionList<T>::Refresh(void)
{
	PDRIVE_LAYOUT_INFORMATION_EX	pLayout;				// Disk layout information

	// Get the most current layout information for the disk
	DiskApi::GetDriveLayoutInformationEx(m_disk, &pLayout);

	try { 

		List<T>::Clear();					// Remove all the existing partition data

		// Repopulate the underlying collection with the new partition data
		for(int index = 0; index < static_cast<int>(pLayout->PartitionCount); index++)
			List<T>::Add(CreatePartitionClass(&pLayout->PartitionEntry[index]));
		}

	// Always release the unmanaged DRIVE_LAYOUT_INFORMATION_EX data
	finally { pLayout = DiskApi::FreeDriveLayoutInformationEx(pLayout); }
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)
