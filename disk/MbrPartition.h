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

#ifndef __MBRPARTITION_H_
#define __MBRPARTITION_H_
#pragma once

#include "MbrPartitionType.h"
#include "Partition.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::Runtime::InteropServices;

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// Class MbrPartition
//
// Specializes Partition for MBR-specific information
//---------------------------------------------------------------------------

public ref class MbrPartition sealed : public Partition
{
public:

	// Instance Constructor
	MbrPartition();

	//-------------------------------------------------------------------------
	// Properties

	// BootIndicator
	//
	// Indicates if this is the bootable partition
	property bool BootIndicator
	{
		bool get(void) { return (m_pInfo->Mbr.BootIndicator) ? true : false; }
		void set(bool value) { m_pInfo->Mbr.BootIndicator = (value) ? TRUE : FALSE; }
	}

	// HiddenSectors
	//
	// Gets/Sets the number of hidden sectors allocated
	property int HiddenSectors
	{
		int get(void) { return static_cast<int>(m_pInfo->Mbr.HiddenSectors); }
		void set(int value);
	}

	// PartitionType
	//
	// Gets/Sets the MBR partition type flag
	property MbrPartitionType PartitionType
	{
		MbrPartitionType get(void) { return static_cast<MbrPartitionType>(m_pInfo->Mbr.PartitionType); }
		void set(MbrPartitionType value) { m_pInfo->Mbr.PartitionType = static_cast<BYTE>(value); }
	}

	// RecognizedPartition
	//
	// Indicates if the partition type is recognized by Windows
	property bool RecognizedPartition
	{
		bool get(void) { return (m_pInfo->Mbr.RecognizedPartition) ? true : false; }
	}

internal:

	// INTERNAL CONSTRUCTORS
	MbrPartition(PPARTITION_INFORMATION_EX pInfo);
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)

#endif	// __MBRPARTITION_H_
