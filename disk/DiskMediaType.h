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

#ifndef __DISKMEDIATYPE_H_
#define __DISKMEDIATYPE_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Enum DiskMediaType
//
// Managed version of MEDIA_TYPE enumeration; defines the disk media type

public enum struct DiskMediaType
{
	Unknown         = 0x00,
	F5_1Pt2_512     = 0x01,
	F3_1Pt44_512    = 0x02,
	F3_2Pt88_512    = 0x03,
	F3_20Pt8_512    = 0x04,
	F3_720_512      = 0x05,
	F5_360_512      = 0x06,
	F5_320_512      = 0x07,
	F5_320_1024     = 0x08,
	F5_180_512      = 0x09,
	F5_160_512      = 0x0a,
	RemovableMedia  = 0x0b,
	FixedMedia      = 0x0c,
	F3_120M_512     = 0x0d,
	F3_640_512      = 0x0e,
	F5_640_512      = 0x0f,
	F5_720_512      = 0x10,
	F3_1Pt2_512     = 0x11,
	F3_1Pt23_1024   = 0x12,
	F5_1Pt23_1024   = 0x13,
	F3_128Mb_512    = 0x14,
	F3_230Mb_512    = 0x15,
	F8_256_128      = 0x16,
	F3_200Mb_512    = 0x17,
	F3_240M_512     = 0x18,
	F3_32M_512      = 0x19
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __DISKMEDIATYPE_H_
