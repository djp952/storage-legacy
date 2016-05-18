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

#ifndef __GPTPARTITIONTYPE_H_
#define __GPTPARTITIONTYPE_H_
#pragma once

#include "DiskUtil.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class GptPartitionType
//
// Defines GPT partition EFI types
//---------------------------------------------------------------------------

public value class GptPartitionType sealed
{
public:

	// Constructor
	//
	// Necessary since Microsoft doesn't define all the possible type uuids
	GptPartitionType(Guid type) : m_type(type) {}

	//-----------------------------------------------------------------------
	// Overloaded Operators

	bool operator ==(GptPartitionType rhs) { return rhs.m_type == this->m_type; }
	bool operator !=(GptPartitionType rhs) { return rhs.m_type != this->m_type; }

	static operator Guid (GptPartitionType rhs) { return rhs.m_type; }

	//-----------------------------------------------------------------------
	// Fields

	static initonly GptPartitionType Unused						= GptPartitionType(PARTITION_ENTRY_UNUSED_GUID);
	static initonly GptPartitionType BasicData					= GptPartitionType(PARTITION_BASIC_DATA_GUID);
	static initonly GptPartitionType LogicalDiskManagerData		= GptPartitionType(PARTITION_LDM_DATA_GUID);
	static initonly GptPartitionType LogicalDiskManagerMetadata	= GptPartitionType(PARTITION_LDM_METADATA_GUID);
	static initonly GptPartitionType MicrosoftRecovery			= GptPartitionType(PARTITION_MSFT_RECOVERY_GUID);
	static initonly GptPartitionType MicrosoftReserved			= GptPartitionType(PARTITION_MSFT_RESERVED_GUID);
	static initonly GptPartitionType System						= GptPartitionType(PARTITION_SYSTEM_GUID);

internal:

	// INTERNAL CONSTRUCTORS
	GptPartitionType(const UUID& type) : m_type(DiskUtil::UUIDToSysGuid(type)) {}

	//-----------------------------------------------------------------------
	// Member Variables

	initonly	Guid				m_type;			// Type GUID
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __GPTPARTITIONTYPE_H_
