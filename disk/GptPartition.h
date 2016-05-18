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

#ifndef __GPTPARTITION_H_
#define __GPTPARTITION_H_
#pragma once

#include "DiskUtil.h"
#include "GptPartitionAttributes.h"
#include "GptPartitionType.h"
#include "Partition.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::Runtime::InteropServices;

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// Class GptPartition
//
// Specializes Partition for GPT-specific information
//---------------------------------------------------------------------------

public ref class GptPartition sealed : public Partition
{
public:

	// Instance Constructors
	GptPartition();

	//-------------------------------------------------------------------------
	// Properties

	// Attributes
	//
	// Gets/Sets the EFI attribute flags
	property GptPartitionAttributes Attributes
	{
		GptPartitionAttributes get(void) { return static_cast<GptPartitionAttributes>(m_pInfo->Gpt.Attributes); }
		void set(GptPartitionAttributes value) { m_pInfo->Gpt.Attributes = static_cast<DWORD64>(value); }
	}

	// Name
	//
	// Gets/Sets the partition name (36 character maximum)
	property String^ Name
	{
		String^ get(void);
		void set(String^ value);
	}

	// PartitionId
	//
	// Gets/Sets the partition ID GUID
	property Guid PartitionId
	{
		Guid get(void) { return DiskUtil::UUIDToSysGuid(m_pInfo->Gpt.PartitionId); }
		void set(Guid value) { m_pInfo->Gpt.PartitionId = DiskUtil::SysGuidToUUID(value); }
	}

	// PartitionType
	//
	// Gets/Sets the partition type GUID
	property GptPartitionType PartitionType
	{
		GptPartitionType get(void) { return GptPartitionType(m_pInfo->Gpt.PartitionType); }
		void set(GptPartitionType value) { m_pInfo->Gpt.PartitionType = DiskUtil::SysGuidToUUID(value); }
	}

internal:

	// INTERNAL CONSTRUCTORS
	GptPartition(PPARTITION_INFORMATION_EX pInfo);
	GptPartition(GptPartitionType type, int number, __int64 length, __int64 offset, Guid partitionId);
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)

#endif	// __GPTPARTITION_H_
