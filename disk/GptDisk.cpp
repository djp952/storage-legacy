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
#include "GptDisk.h"					// Include GptDisk declarations
#include "GptPartitionList.h"			// Include GptPartitionList declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki.storage)

//-----------------------------------------------------------------------------
// GptDisk Constructor (internal)
//
// Arguments:
//
//	handle			- Handle to the open disk object
//	pGeometry		- DISK_GEOMETRY_EX information for the disk
//	pLayout			- DRIVE_LAYOUT_INFORMATION_EX for the disk

GptDisk::GptDisk(HANDLE handle, PDISK_GEOMETRY_EX pGeometry, PDRIVE_LAYOUT_INFORMATION_EX pLayout) 
	: m_pLayoutGpt(NULL), Disk(handle, pGeometry)
{
	if(handle == INVALID_HANDLE_VALUE) throw gcnew ArgumentNullException("handle");
	if(!pGeometry) throw gcnew ArgumentNullException("pGeometry");
	if(!pLayout) throw gcnew ArgumentNullException("pLayout");

	// GPT LAYOUT
	m_pLayoutGpt = new DRIVE_LAYOUT_INFORMATION_GPT();
	if(!m_pLayoutGpt) throw gcnew OutOfMemoryException();
	memcpy(m_pLayoutGpt, &pLayout->Gpt, sizeof(DRIVE_LAYOUT_INFORMATION_GPT));

	// PARTITION COLLECTION
	m_partitions = gcnew GptPartitionList(this);
}

//---------------------------------------------------------------------------
// GptDisk Finalizer

GptDisk::!GptDisk()
{
	if(m_pLayoutGpt) delete m_pLayoutGpt;
	m_pLayoutGpt = NULL;
}

//---------------------------------------------------------------------------
// GptDisk::DiskID::set
//
// Sets the GPT disk identifier

void GptDisk::DiskId::set(Guid value)
{
	m_pLayoutGpt->DiskId = DiskUtil::SysGuidToUUID(value);
	DiskApi::SetDriveLayoutInformationGpt(this, m_pLayoutGpt);
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)
