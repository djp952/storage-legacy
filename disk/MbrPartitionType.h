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

#ifndef __MBRPARTITIONTYPE_H_
#define __MBRPARTITIONTYPE_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// Enum MbrPartitionType
//
// Defines partition type codes

public enum struct MbrPartitionType : unsigned char
{
	Unused				= PARTITION_ENTRY_UNUSED,
	FAT12				= PARTITION_FAT_12,
	Xenix1				= PARTITION_XENIX_1,
	Xenix2				= PARTITION_XENIX_2,
	FAT16				= PARTITION_FAT_16,
	Extended			= PARTITION_EXTENDED,
	Huge				= PARTITION_HUGE,
	IFS					= PARTITION_IFS,
	OS2BootManager		= PARTITION_OS2BOOTMGR,
	FAT32				= PARTITION_FAT32,
	XInt13				= PARTITION_XINT13,
	XInt13Extended		= PARTITION_XINT13_EXTENDED,
	PReP				= PARTITION_PREP,
	LDM					= PARTITION_LDM,
	Unix				= PARTITION_UNIX,
	Linux				= 0x83,					// <--- Not recognized by Windows
	Spaces				= PARTITION_SPACES,
	NTFT				= PARTITION_NTFT
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)

#endif	// __MBRPARTITIONTYPE_H_
