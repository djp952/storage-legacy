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

#ifndef __STORAGECONTAINERCOLLECTION_H_
#define __STORAGECONTAINERCOLLECTION_H_
#pragma once

#include "ComStorage.h"						// Include ComStorage declarations
#include "StorageContainerEnumerator.h"		// Include enumerator declarations
#include "StorageExceptions.h"				// Include exception declarations

#pragma warning(push, 4)					// Enable maximum compiler warnings
#pragma warning(disable:4461)				// "finalizer without destructor"

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Forward Class Declarations
//
// Include the specified header files in the .CPP file for this class
//---------------------------------------------------------------------------

ref class StructuredStorage;				// <-- StructuredStorage.h
ref class StorageContainer;					// <-- StorageContainer.h

//---------------------------------------------------------------------------
// Class StorageContainerCollection
//
// StorageContainerCollection implements a collection of StorageContainers
// for a parent StorageContainer object.
//---------------------------------------------------------------------------

STRUCTURED_STORAGE_PUBLIC ref class StorageContainerCollection sealed : 
	public Generic::ICollection<StorageContainer^>
{
public:

	//-----------------------------------------------------------------------
	// IEnumerable<T> implementation

	virtual Generic::IEnumerator<StorageContainer^>^ GetEnumerator(void);

	//-----------------------------------------------------------------------
	// ICollection<T> implementation

	virtual StorageContainer^	Add(String^ name);
	virtual void				Clear(void);
	virtual bool				Contains(String^ name);
	virtual void				CopyTo(array<StorageContainer^>^ target, int index);
	virtual bool				Remove(String^ name);
	virtual bool				Remove(StorageContainer^) = Generic::ICollection<StorageContainer^>::Remove;

	virtual property int  Count { int get(void); }
	virtual property bool IsReadOnly { bool get(void) { return m_readOnly; } }

	//-----------------------------------------------------------------------
	// Properties

	property StorageContainer^ default[String^] { StorageContainer^ get(String^ name); }
	property StorageContainer^ default[int] { StorageContainer^ get(int index); }

internal:

	// INTERNAL CONSTRUCTOR
	StorageContainerCollection(StructuredStorage^ root, ComStorage^ storage);

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
	property StorageContainer^ default[Guid]
	{
		StorageContainer^ get(Guid contid);
	}

private:

	//-----------------------------------------------------------------------
	// Private Member Functions

	String^	LookupIndex(int index);

	// ICollection<T>::Add
	virtual void Add(StorageContainer^) sealed =
		Generic::ICollection<StorageContainer^>::Add { throw gcnew NotImplementedException(); }

	// ICollection<T>::Contains
	virtual bool Contains(StorageContainer^ item) sealed = 
		Generic::ICollection<StorageContainer^>::Contains;

	// IEnumerator::GetEnumerator
	virtual Collections::IEnumerator^ _GetEnumerator(void) sealed = 
		Collections::IEnumerable::GetEnumerator { return GetEnumerator(); }

	//-----------------------------------------------------------------------
	// Member Variables

	StructuredStorage^		m_root;				// Root Storage object
	ComStorage^				m_storage;			// Contained storage instance
	bool					m_readOnly;			// Read-only collection?
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __STORAGECONTAINERCOLLECTION_H_
