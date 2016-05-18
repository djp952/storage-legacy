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

#ifndef __STORAGEPROPERTYSETCOLLECTION_H_
#define __STORAGEPROPERTYSETCOLLECTION_H_
#pragma once

#include "ComPropertyStorage.h"				// Include ComPropertyStorage decls
#include "ComStorage.h"						// Include ComStorage declarations
#include "StorageExceptions.h"				// Include exception declarations
#include "StoragePropertySetEnumerator.h"	// Include enumerator declarations

#pragma warning(push, 4)					// Enable maximum compiler warnings
#pragma warning(disable:4461)				// "finalizer without destructor"

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace msclr;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Forward Class Declarations
//
// Include the specified header files in the .CPP file for this class
//---------------------------------------------------------------------------

ref class StructuredStorage;				// <-- StructuredStorage.h
ref class StoragePropertySet;				// <-- StoragePropertySet.h

//---------------------------------------------------------------------------
// Class StoragePropertySetCollection
//
// StoragePropertySetCollection implements a collection of StoragePropertySets
// for a parent StorageContainer object.
//---------------------------------------------------------------------------

STRUCTURED_STORAGE_PUBLIC ref class StoragePropertySetCollection sealed : 
	public Generic::ICollection<StoragePropertySet^>
{
public:

	//-----------------------------------------------------------------------
	// IEnumerable<T> implementation

	virtual Generic::IEnumerator<StoragePropertySet^>^ GetEnumerator(void);

	//-----------------------------------------------------------------------
	// ICollection<T> implementation

	virtual StoragePropertySet^	Add(String^ name);
	virtual void				Clear(void);
	virtual bool				Contains(String^ name);
	virtual void				CopyTo(array<StoragePropertySet^>^ target, int index);
	virtual bool				Remove(String^ name);
	virtual bool				Remove(StoragePropertySet^) = Generic::ICollection<StoragePropertySet^>::Remove;

	virtual property int  Count { int get(void); }
	virtual property bool IsReadOnly { bool get(void) { return m_readOnly; } }

	//-----------------------------------------------------------------------
	// Properties

	property StoragePropertySet^ default[String^] { StoragePropertySet^ get(String^); }
	property StoragePropertySet^ default[int] { StoragePropertySet^ get(int index); }

internal:

	// INTERNAL CONSTRUCTOR
	StoragePropertySetCollection(StructuredStorage^ root, ComStorage^ storage);

	//-----------------------------------------------------------------------
	// Internal Member Functions
	
	// ToDictionary
	//
	// Creates a snapshot dictionary of NAME->GUIDs for this collection
	Dictionary<String^, Guid>^ ToDictionary(void);

	//-----------------------------------------------------------------------
	// Internal Properties

	// default[Guid]
	//
	// Accesses an object in the collection by it's GUID
	property StoragePropertySet^ default[Guid]
	{
		StoragePropertySet^ get(Guid propsetid);
	}

private:

	//-----------------------------------------------------------------------
	// Private Member Functions

	String^	LookupIndex(int index);

	// ICollection<T>::Add
	virtual void Add(StoragePropertySet^) sealed =
		Generic::ICollection<StoragePropertySet^>::Add { throw gcnew NotImplementedException(); }

	// ICollection<T>::Contains
	virtual bool Contains(StoragePropertySet^ item) sealed = 
		Generic::ICollection<StoragePropertySet^>::Contains;

	// IEnumerator::GetEnumerator
	virtual Collections::IEnumerator^ _GetEnumerator(void) sealed = 
		Collections::IEnumerable::GetEnumerator { return GetEnumerator(); }

	//-----------------------------------------------------------------------
	// Member Variables

	StructuredStorage^		m_root;				// Root Storage object
	ComStorage^				m_storage;			// Contained ComStorage instance
	bool					m_readOnly;			// Read-only collection?
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __STORAGEPROPERTYSETCOLLECTION_H_
