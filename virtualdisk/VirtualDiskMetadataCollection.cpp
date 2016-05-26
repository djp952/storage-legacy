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
#include "VirtualDiskMetadataCollection.h"

#include "GuidUtil.h"
#include "VirtualDiskSafeHandle.h"

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection Constructor (internal)
//
// Arguments:
//
//	handle		- Referenced VirtualDiskSafeHandle instance

VirtualDiskMetadataCollection::VirtualDiskMetadataCollection(VirtualDiskSafeHandle^ handle) : m_handle(handle)
{
	if(Object::ReferenceEquals(handle, nullptr)) throw gcnew ArgumentNullException("handle");
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::default[Guid]::get
//
// Gets the element at the specified index in the dictionary

array<Byte>^ VirtualDiskMetadataCollection::default::get(Guid key)
{
	ULONG cb = 0;

	// Unwrap the safe handle and convert the System::Guid into a GUID
	VirtualDiskSafeHandle::Reference handle(m_handle);
	GUID guid = GuidUtil::SysGuidToUUID(key);				

	// Get the size of the metadata blob associated with this GUID
	DWORD result = GetVirtualDiskMetadata(handle, &guid, &cb, __nullptr);
	if(result == ERROR_SUCCESS) return gcnew array<Byte>(0);
	if(result != ERROR_MORE_DATA) throw gcnew Win32Exception(result);

	// Allocate and pin the managed array to be returned to the caller
	array<Byte>^ blob = gcnew array<Byte>(cb);
	pin_ptr<uint8_t> pinblob = &blob[0];

	// Call the method again, this time loading the blob data into the array
	result = GetVirtualDiskMetadata(handle, &guid, &cb, pinblob);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception();

	return blob;
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::default[Guid]::set
//
// Sets the element at the specified index in the dictionary

void VirtualDiskMetadataCollection::default::set(Guid key, array<Byte>^ value)
{
	if(Object::ReferenceEquals(value, nullptr)) throw gcnew ArgumentNullException("value");

	GUID guid = GuidUtil::SysGuidToUUID(key);
	pin_ptr<uint8_t> pinblob = (value->Length > 0) ? &value[0] : __nullptr;

	DWORD result = SetVirtualDiskMetadata(VirtualDiskSafeHandle::Reference(m_handle), &guid, value->Length, pinblob);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);
}
		
//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::Add
//
// Adds an item to the collection
//
// Arguments:
//
//	item		- Item to be added to the collection

void VirtualDiskMetadataCollection::Add(KeyValuePair<Guid, array<Byte>^> item)
{
	Add(item.Key, item.Value);
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::Add
//
// Adds an item with the provided key and value to the dictionary
//
// Arguments:
//
//	key			- Key to be added to the collection
//	value		- Value associated with the key

void VirtualDiskMetadataCollection::Add(Guid key, array<Byte>^ value)
{
	if(Object::ReferenceEquals(value, nullptr)) throw gcnew ArgumentNullException("value");

	GUID guid = GuidUtil::SysGuidToUUID(key);
	pin_ptr<uint8_t> pinvalue = (value->Length > 0) ? &(value[0]) : __nullptr;

	DWORD result = SetVirtualDiskMetadata(VirtualDiskSafeHandle::Reference(m_handle), &guid, value->Length, pinvalue);
	if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::Clear
//
// Removes all items from the dictionary
//
// Arguments:
//
//	NONE

void VirtualDiskMetadataCollection::Clear(void)
{
	VirtualDiskSafeHandle::Reference	handle(m_handle);	// Unwrap safe handle
	ULONG								count = 0;			// Number of items
	GUID*								guids;				// Array of GUIDs

	// Determine the number of GUIDs there are to be cleared from the metadata
	DWORD result = EnumerateVirtualDiskMetadata(handle, &count, __nullptr);
	if(result == ERROR_SUCCESS) return;
	else if(result != ERROR_MORE_DATA) throw gcnew Win32Exception(result);

	// Allocate an array of GUIDs on the unmanaged heap to store the data
	try { guids = new GUID[count]; }
	catch(Exception^) { throw gcnew OutOfMemoryException(); }

	try {

		// Now get all of the GUIDs associated with virtual disk metadata
		result = EnumerateVirtualDiskMetadata(handle, &count, guids);
		if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);

		// Remove all of the metadata GUIDs from the virtual disk
		for(ULONG index = 0; index < count; index++) {

			result = DeleteVirtualDiskMetadata(handle, &guids[index]);
			if(result != ERROR_SUCCESS) throw gcnew Win32Exception(result);
		}
	}

	finally { delete[] guids; }			// Always release the allocated memory
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::Contains
//
// Determines if the collection contains a specific item
//
// Arguments:
//
//	item		- Item to look for in the collection

bool VirtualDiskMetadataCollection::Contains(KeyValuePair<Guid, array<Byte>^> item)
{
	return ContainsKey(item.Key);
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::ContainsKey
//
// Determines whether the dictionary contains an element that has the specified key
//
// Arguments:
//
//	key			- Key to look for in the collection

bool VirtualDiskMetadataCollection::ContainsKey(Guid key)
{
	GUID guid = GuidUtil::SysGuidToUUID(key);
	ULONG cb = 0;

	DWORD result = GetVirtualDiskMetadata(VirtualDiskSafeHandle::Reference(m_handle), &guid, &cb, __nullptr);
	return ((result == ERROR_SUCCESS) || (result == ERROR_MORE_DATA));
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::CopyTo
//
// Copies the elements of the collection into an array
//
// Arguments:
//
//	destination		- Destination array instance
//	index			- Starting index into the array to begin copying

void VirtualDiskMetadataCollection::CopyTo(array<KeyValuePair<Guid, array<Byte>^>>^ destination, int index)
{
	// todo
	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::Count::get
//
// Gets the number of elements in the collection

int VirtualDiskMetadataCollection::Count::get(void)
{
	ULONG count = 0;

	DWORD result = EnumerateVirtualDiskMetadata(VirtualDiskSafeHandle::Reference(m_handle), &count, __nullptr);
	if((result == ERROR_SUCCESS) || (result == ERROR_MORE_DATA)) return static_cast<int>(count);
	else throw gcnew Win32Exception(result);
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::GetEnumerator
//
// Returns a generic IEnumerator<T> for the member collection
//
// Arguments:
//
//	NONE

IEnumerator<KeyValuePair<Guid, array<Byte>^>>^ VirtualDiskMetadataCollection::GetEnumerator(void)
{
	// todo
	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::IEnumerable_GetEnumerator
//
// Returns a non-generic IEnumerator for the member collection
//
// Arguments:
//
//	NONE

System::Collections::IEnumerator^ VirtualDiskMetadataCollection::IEnumerable_GetEnumerator(void)
{
	return GetEnumerator();
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::IsReadOnly::get
//
// Gets a flag indicating if the dictionary is read-only

bool VirtualDiskMetadataCollection::IsReadOnly::get(void)
{
	return false;
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::Keys::get
//
// Gets an enumerable collection that contains the keys in the dictionary

ICollection<Guid>^ VirtualDiskMetadataCollection::Keys::get(void)
{
	// todo
	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::Remove
//
// Removes an item from the collection
//
// Arguments:
//
//	item	- Item to be removed from the collection

bool VirtualDiskMetadataCollection::Remove(KeyValuePair<Guid, array<Byte>^> item)
{
	return Remove(item.Key);
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::Remove
//
// Removes an item with the specified key from the dictionary
//
// Arguments:
//
//	key		- Key of the item to be removed from the collection

bool VirtualDiskMetadataCollection::Remove(Guid key)
{
	GUID guid = GuidUtil::SysGuidToUUID(key);

	DWORD result = DeleteVirtualDiskMetadata(VirtualDiskSafeHandle::Reference(m_handle), &guid);
	return (result == ERROR_SUCCESS);
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::TryGetValue
//
// Gets the value that is associated with the specified key
//
// Arguments:
//
//	key		- Key of the item to look for in the collection
//	value	- On success, contains the located value instance

bool VirtualDiskMetadataCollection::TryGetValue(Guid key, [OutAttribute] array<Byte>^% value)
{
	try { value = default[key]; }
	catch(Exception^) { return false; }

	return true;
}

//---------------------------------------------------------------------------
// VirtualDiskMetadataCollection::Values::get
//
// Gets an enumerable collection that contains the values in the dictionary

ICollection<array<Byte>^>^ VirtualDiskMetadataCollection::Values::get(void)
{
	// todo
	throw gcnew NotImplementedException();
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
