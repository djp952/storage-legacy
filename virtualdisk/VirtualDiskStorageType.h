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

	// operator== (static)
	//
	static bool operator==(VirtualDiskStorageType lhs, VirtualDiskStorageType rhs);

	// operator!= (static)
	//
	static bool operator!=(VirtualDiskStorageType lhs, VirtualDiskStorageType rhs);
	
	//-----------------------------------------------------------------------
	// Member Functions

	// Equals
	//
	// Overrides Object::Equals()
	virtual bool Equals(Object^ rhs) override;

	// Equals
	//
	// Compares this CompilationDatabaseLoadErrorCode to another CompilationDatabaseLoadErrorCode
	bool Equals(VirtualDiskStorageType rhs);

	// GetHashCode
	//
	// Overrides Object::GetHashCode()
	virtual int GetHashCode(void) override;

	// ToString
	//
	// Overrides Object::ToString()
	virtual String^ ToString(void) override;

	//-----------------------------------------------------------------------
	// Fields

	static initonly VirtualDiskStorageType Unknown	= VirtualDiskStorageType(VIRTUAL_STORAGE_TYPE_DEVICE_UNKNOWN, VIRTUAL_STORAGE_TYPE_VENDOR_UNKNOWN);
	static initonly VirtualDiskStorageType ISO		= VirtualDiskStorageType(VIRTUAL_STORAGE_TYPE_DEVICE_ISO, VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT);
	static initonly VirtualDiskStorageType VHD		= VirtualDiskStorageType(VIRTUAL_STORAGE_TYPE_DEVICE_VHD, VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT);
	static initonly VirtualDiskStorageType VHDX		= VirtualDiskStorageType(VIRTUAL_STORAGE_TYPE_DEVICE_VHDX, VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT);
	static initonly VirtualDiskStorageType VHDSet	= VirtualDiskStorageType(VIRTUAL_STORAGE_TYPE_DEVICE_VHDSET, VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT);

internal:

	// Instance Constructors
	//
	VirtualDiskStorageType(const VIRTUAL_STORAGE_TYPE& type);
	VirtualDiskStorageType(ULONG deviceid, const GUID& vendorid);

	//-----------------------------------------------------------------------
	// Internal Member Functions

	// Converts the contained value into a VIRTUAL_STORAGE_TYPE
	//
	void ToVIRTUAL_STORAGE_TYPE(PVIRTUAL_STORAGE_TYPE type);

private:

	//-----------------------------------------------------------------------
	// Member Variables
	
	unsigned int		m_deviceid;			// Storage type device id
	Guid				m_vendorid;			// Storage type vendor id
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKSTORAGETYPE_H_
