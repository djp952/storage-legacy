//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------

#ifndef __VIRTUALDISKMETADATACOLLECTION_H_
#define __VIRTUALDISKMETADATACOLLECTION_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::Collections::Generic;
using namespace System::ComponentModel;
using namespace System::Runtime::InteropServices;

BEGIN_ROOT_NAMESPACE(zuki::storage)

// FORWARD DECLARATIONS
//
ref class VirtualDiskSafeHandle;

//---------------------------------------------------------------------------
// Class VirtualDiskMetadataCollection
//
// Implements a colection of virtual disk metadata blobs
//---------------------------------------------------------------------------

public ref class VirtualDiskMetadataCollection : public IDictionary<Guid, array<Byte>^>
{
public:

	//-----------------------------------------------------------------------
	// Member Functions

	// Add
	//
	// Adds an item to the collection
	virtual void Add(KeyValuePair<Guid, array<Byte>^> item);

	// Add
	//
	// Adds an item with the provided key and value to the dictionary
	virtual void Add(Guid key, array<Byte>^ value);

	// Clear
	//
	// Removes all items from the dictionary
	virtual void Clear(void);

	// Contains
	//
	// Determines if the collection contains a specific item
	virtual bool Contains(KeyValuePair<Guid, array<Byte>^> item);

	// ContainsKey
	//
	// Determines whether the dictionary contains an element that has the specified key
	virtual bool ContainsKey(Guid key);

	// CopyTo
	//
	// Copies the elements of the collection into an array
	virtual void CopyTo(array<KeyValuePair<Guid, array<Byte>^>>^ destination, int index);

	// GetEnumerator
	//
	// Returns a generic IEnumerator<T> for the member collection
	virtual IEnumerator<KeyValuePair<Guid, array<Byte>^>>^ GetEnumerator(void);

	// Remove
	//
	// Removes an item from the collection
	virtual bool Remove(KeyValuePair<Guid, array<Byte>^> item);

	// Remove
	//
	// Removes an item with the specified key from the dictionary
	virtual bool Remove(Guid key);

	// TryGetValue
	//
	// Gets the value that is associated with the specified key
	virtual bool TryGetValue(Guid key, [OutAttribute] array<Byte>^% value);

	//-----------------------------------------------------------------------
	// Properties

	// default[Guid]
	//
	// Gets/sets the element at the specified index in the dictionary
	property array<Byte>^ default[Guid] 
	{
		virtual array<Byte>^ get(Guid key);
		virtual void set(Guid key, array<Byte>^ value);
	}

	// Count
	//
	// Gets the number of elements in the collection
	property int Count
	{
		virtual int get(void);
	}

	// IsReadOnly
	//
	// Gets a flag indicating if the dictionary is read-only
	property bool IsReadOnly
	{
		virtual bool get(void);
	}

	// Keys
	//
	// Gets an enumerable collection that contains the keys in the dictionary
	property ICollection<Guid>^ Keys
	{
		virtual ICollection<Guid>^ get(void);
	}

	// Values
	//
	// Gets an enumerable collection that contains the values in the dictionary
	property ICollection<array<Byte>^>^ Values
	{
		virtual ICollection<array<Byte>^>^ get(void);
	}

internal:

	// Instance Constructor
	//
	VirtualDiskMetadataCollection(VirtualDiskSafeHandle^ handle);

private:

	//-----------------------------------------------------------------------
	// Private Member Functions

	// GetEnumerator (IEnumerable)
	//
	// Returns a non-generic IEnumerator for the member collection
	virtual System::Collections::IEnumerator^ IEnumerable_GetEnumerator(void) sealed = System::Collections::IEnumerable::GetEnumerator;
		
	//-----------------------------------------------------------------------
	// Member Variables

	VirtualDiskSafeHandle^		m_handle;	// Virtual disk safe handle
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __VIRTUALDISKMETADATACOLLECTION_H_
