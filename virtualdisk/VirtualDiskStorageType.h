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

#ifndef __VIRTUALDISKSTORAGETYPE_H_
#define __VIRTUALDISKSTORAGETYPE_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Class VirtualDiskStorageType
//
// Defines the type of a virtual storage device
//---------------------------------------------------------------------------

public value class VirtualDiskStorageType sealed
{
public:

	//-----------------------------------------------------------------------
	// Overloaded Operators

	bool operator ==(VirtualDiskStorageType rhs) { return rhs.m_deviceId == this->m_deviceId; }
	bool operator !=(VirtualDiskStorageType rhs) { return rhs.m_deviceId != this->m_deviceId; }

	//-----------------------------------------------------------------------
	// Fields

	static initonly VirtualDiskStorageType Auto		= VirtualDiskStorageType(VIRTUAL_STORAGE_TYPE_DEVICE_UNKNOWN);
	static initonly VirtualDiskStorageType ISO		= VirtualDiskStorageType(VIRTUAL_STORAGE_TYPE_DEVICE_ISO);
	static initonly VirtualDiskStorageType VHD		= VirtualDiskStorageType(VIRTUAL_STORAGE_TYPE_DEVICE_VHD);
	static initonly VirtualDiskStorageType VHDX		= VirtualDiskStorageType(VIRTUAL_STORAGE_TYPE_DEVICE_VHDX);

internal:

	//-----------------------------------------------------------------------
	// Internal Member Functions

	// Converts from an unmanaged VIRTUAL_STORAGE_TYPE structure
	static VirtualDiskStorageType FromVIRTUAL_STORAGE_TYPE(PVIRTUAL_STORAGE_TYPE ptype);

	// Converts into an unmanaged VIRTUAL_STORAGE_TYPE structure
	void ToVIRTUAL_STORAGE_TYPE(PVIRTUAL_STORAGE_TYPE ptype);

private:

	// PRIVATE CONSTRUCTOR
	VirtualDiskStorageType(int deviceId) : m_deviceId(deviceId) {}

	//-----------------------------------------------------------------------
	// Member Variables
	
	initonly int		m_deviceId;
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKSTORAGETYPE_H_
