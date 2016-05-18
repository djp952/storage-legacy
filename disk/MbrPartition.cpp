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
#include "MbrPartition.h"				// Include MbrPartition decls

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// MbrPartition Constructor
//
// Arguments:
//
//	NONE

MbrPartition::MbrPartition() : Partition()
{
	// Set the parent partition style to MBR
	m_pInfo->PartitionStyle = PARTITION_STYLE_MBR;
}

//---------------------------------------------------------------------------
// MbrPartition Constructor (internal)
//
// Arguments:
//
//	pInfo	- Pointer to the PPARTITION_INFORMATION_EX structure

MbrPartition::MbrPartition(PPARTITION_INFORMATION_EX pInfo) : Partition(pInfo)
{
	if(!pInfo) throw gcnew ArgumentNullException("pInfo");
	if(pInfo->PartitionStyle != PARTITION_STYLE_MBR) 
		throw gcnew ArgumentException("Partition data is not MBR", "pInfo");
}

//---------------------------------------------------------------------------
// MbrPartition::HiddenSectors::set
//
// Sets the number of hidden sectors to allocate with the partition table

void MbrPartition::HiddenSectors::set(int value)
{
	if(value < 0) throw gcnew ArgumentOutOfRangeException("value");
	m_pInfo->Mbr.HiddenSectors = static_cast<DWORD>(value);
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
