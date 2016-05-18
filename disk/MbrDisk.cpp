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
#include "MbrDisk.h"					// Include MbrDisk declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//-----------------------------------------------------------------------------
// MbrDisk Constructor (internal)
//
// Arguments:
//
//	handle			- Handle to the open disk object
//	pGeometry		- DISK_GEOMETRY_EX information for the disk
//	pLayout			- DRIVE_LAYOUT_INFORMATION_EX data for the disk

MbrDisk::MbrDisk(HANDLE handle, PDISK_GEOMETRY_EX pGeometry, PDRIVE_LAYOUT_INFORMATION_EX pLayout) 
	: m_pLayoutMbr(NULL), Disk(handle, pGeometry)
{
	if(handle == INVALID_HANDLE_VALUE) throw gcnew ArgumentNullException("handle");
	if(!pGeometry) throw gcnew ArgumentNullException("pGeometry");
	if(!pLayout) throw gcnew ArgumentNullException("pLayout");

	// MBR LAYOUT
	m_pLayoutMbr = new DRIVE_LAYOUT_INFORMATION_MBR();
	if(!m_pLayoutMbr) throw gcnew OutOfMemoryException();
	memcpy(m_pLayoutMbr, &pLayout->Mbr, sizeof(DRIVE_LAYOUT_INFORMATION_GPT));

	// PARTITION COLLECTION
	m_partitions = gcnew PartitionList<MbrPartition^>(this);
}

//---------------------------------------------------------------------------
// MbrDisk Finalizer

MbrDisk::!MbrDisk()
{
	if(m_pLayoutMbr) delete m_pLayoutMbr;
	m_pLayoutMbr = NULL;
}

//---------------------------------------------------------------------------
// MbrDisk::Signature::set
//
// Sets the MBR disk signature

void MbrDisk::Signature::set(int value)
{
	m_pLayoutMbr->Signature = static_cast<DWORD>(value);
	DiskApi::SetDriveLayoutInformationMbr(this, m_pLayoutMbr);
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
