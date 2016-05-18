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

#include "stdafx.h"
#include "VirtualDiskStorageType.h"

#include "VirtualDiskUtil.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// VirtualDiskStorageType Constructor (internal)
//
// Arguments:
//
//	deviceid	- Virtual disk device identifier
//	vendorid	- Virtual disk vendor identifier

VirtualDiskStorageType::VirtualDiskStorageType(ULONG deviceid, const GUID& vendorid) : m_deviceid(deviceid), 
	m_vendorid(VirtualDiskUtil::UUIDToSysGuid(vendorid))
{
}

//---------------------------------------------------------------------------
// VirtualDiskStorageType Constructor (internal)
//
// Arguments:
//
//	type		- VIRTUAL_STORAGE_TYPE instance reference

VirtualDiskStorageType::VirtualDiskStorageType(const VIRTUAL_STORAGE_TYPE& type) : m_deviceid(type.DeviceId), 
	m_vendorid(VirtualDiskUtil::UUIDToSysGuid(type.VendorId))
{
}

//---------------------------------------------------------------------------
// VirtualDiskStorageType::operator == (static)

bool VirtualDiskStorageType::operator==(VirtualDiskStorageType lhs, VirtualDiskStorageType rhs)
{
	return ((lhs.m_deviceid == rhs.m_deviceid) && (lhs.m_vendorid == rhs.m_vendorid));
}

//---------------------------------------------------------------------------
// VirtualDiskStorageType::operator != (static)

bool VirtualDiskStorageType::operator!=(VirtualDiskStorageType lhs, VirtualDiskStorageType rhs)
{
	return ((lhs.m_deviceid != rhs.m_deviceid) || (lhs.m_vendorid != rhs.m_vendorid));
}

//---------------------------------------------------------------------------
// VirtualDiskStorageType::Equals
//
// Compares this VirtualDiskStorageType to another VirtualDiskStorageType
//
// Arguments:
//
//	rhs		- Right-hand VirtualDiskStorageType to compare against

bool VirtualDiskStorageType::Equals(VirtualDiskStorageType rhs)
{
	return (*this == rhs);
}

//---------------------------------------------------------------------------
// VirtualDiskStorageType::Equals
//
// Overrides Object::Equals()
//
// Arguments:
//
//	rhs		- Right-hand object instance to compare against

bool VirtualDiskStorageType::Equals(Object^ rhs)
{
	if(Object::ReferenceEquals(rhs, nullptr)) return false;

	// Convert the provided object into a VirtualDiskStorageType instance
	VirtualDiskStorageType^ rhsref = dynamic_cast<VirtualDiskStorageType^>(rhs);
	if(rhsref == nullptr) return false;

	return (*this == *rhsref);
}

//---------------------------------------------------------------------------
// VirtualDiskStorageType::GetHashCode
//
// Overrides Object::GetHashCode()
//
// Arguments:
//
//	NONE

int VirtualDiskStorageType::GetHashCode(void)
{
	return m_deviceid.GetHashCode() ^ m_vendorid.GetHashCode();
}

//---------------------------------------------------------------------------
// VirtualDiskStorageType::ToString
//
// Overrides Object::ToString()
//
// Arguments:
//
//	NONE

String^ VirtualDiskStorageType::ToString(void)
{
	switch(m_deviceid) {

		case VIRTUAL_STORAGE_TYPE_DEVICE_ISO:		return gcnew String("ISO");
		case VIRTUAL_STORAGE_TYPE_DEVICE_VHD:		return gcnew String("VHD");
		case VIRTUAL_STORAGE_TYPE_DEVICE_VHDX:		return gcnew String("VHDX");
		case VIRTUAL_STORAGE_TYPE_DEVICE_VHDSET:	return gcnew String("VHDSet");

		default: return gcnew String("Unknown");
	}
}

//-----------------------------------------------------------------------------
// VirtualDiskStorageType::ToVIRTUAL_STORAGE_TYPE (internal)
//
// Converts the value of this class into an unmanaged VIRTUAL_STORAGE_TYPE
//
// Arguments:
//
//	type		- Pointer to a VIRTUAL_STORAGE_TYPE structure

void VirtualDiskStorageType::ToVIRTUAL_STORAGE_TYPE(PVIRTUAL_STORAGE_TYPE type)
{
	if(type == __nullptr) throw gcnew ArgumentNullException("type");

	type->DeviceId = static_cast<ULONG>(m_deviceid);
	type->VendorId = VirtualDiskUtil::SysGuidToUUID(m_vendorid);
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
