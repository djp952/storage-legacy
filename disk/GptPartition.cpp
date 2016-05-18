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
#include "GptPartition.h"				// Include GptPartition decls

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// GptPartition Constructor
//
// Arguments:
//
//	NONE

GptPartition::GptPartition() : Partition()
{
	// Set the parent partition style to GPT
	m_pInfo->PartitionStyle = PARTITION_STYLE_GPT;

	// Set a couple reasonable defaults for a GPT partition
	m_pInfo->Gpt.PartitionId = DiskUtil::SysGuidToUUID(Guid::NewGuid());
	m_pInfo->Gpt.PartitionType = DiskUtil::SysGuidToUUID(GptPartitionType::BasicData);
}

//---------------------------------------------------------------------------
// GptPartition Constructor (internal)
//
// Arguments:
//
//	type			- Partition type
//	number			- Partition number
//	length			- Partition length
//	offset			- Partition starting offset
//	partitionId		- Partition identifier

GptPartition::GptPartition(GptPartitionType type, int number, __int64 length, __int64 offset, Guid partitionId) : Partition()
{
	// Set the parent partition style to GPT
	m_pInfo->PartitionStyle = PARTITION_STYLE_GPT;

	// Set a couple reasonable defaults for a GPT partition
	m_pInfo->Gpt.PartitionId = DiskUtil::SysGuidToUUID(Guid::NewGuid());
	m_pInfo->Gpt.PartitionType = DiskUtil::SysGuidToUUID(GptPartitionType::BasicData);

	// Apply constructor arguments
	Partition::PartitionNumber = number;
	Partition::StartingOffset = offset;
	Partition::PartitionLength = length;
	GptPartition::PartitionType = type;
	GptPartition::PartitionId = partitionId;
}

//---------------------------------------------------------------------------
// GptPartition Constructor (internal)
//
// Arguments:
//
//	pInfo		- Pointer to the PPARTITION_INFORMATION_EX structure

GptPartition::GptPartition(PPARTITION_INFORMATION_EX pInfo) : Partition(pInfo)
{
	if(!pInfo) throw gcnew ArgumentNullException("pInfo");
	if(pInfo->PartitionStyle != PARTITION_STYLE_GPT) 
		throw gcnew ArgumentException("Partition data is not GPT", "pInfo");
}

//---------------------------------------------------------------------------
// GptPartition::Name::get
//
// Gets the partition name

String^ GptPartition::Name::get(void)
{
	// This is a fixed-length string without a NULL terminator; handle it
	return Marshal::PtrToStringUni(IntPtr(m_pInfo->Gpt.Name), 36)->TrimEnd(gcnew array<wchar_t> { '\0' });
}

//---------------------------------------------------------------------------
// GptPartition::Name::set
//
// Sets the partition name

void GptPartition::Name::set(String^ value)
{
	PinnedStringPtr				pinName;			// Pinned string value

	// The partition name cannot be longer than 36 characters
	if(!String::IsNullOrEmpty(value) && (value->Length > 36))
		throw gcnew ArgumentOutOfRangeException("value", "GPT partition names are limited to 36 characters");

	pinName = PtrToStringChars(value);
	ZeroMemory(m_pInfo->Gpt.Name, sizeof(WCHAR) * 36);
	StringCchCopyN(m_pInfo->Gpt.Name, 36, pinName, value->Length);
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)
