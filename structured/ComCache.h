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

#ifndef __COMCACHE_H_
#define __COMCACHE_H_
#pragma once

#include "IComPointer.h"					// Include IComPointer declarations
#include "StorageExceptions.h"				// Include exception declarations
#include "StorageUtil.h"					// Include StorageUtil declarations

#pragma warning(push, 4)					// Enable maximum compiler warnings

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace	msclr;

BEGIN_NAMESPACE(zuki)
BEGIN_NAMESPACE(storage)
BEGIN_NAMESPACE(structured)

//---------------------------------------------------------------------------
// Class ComCache (internal)
//
// ComCache implements a GUID->OBJECT collection. This collection is used to
// keep the exclusively opened storage and stream pointers alive and shared
// among all managed object instances.  By using a WeakReference against the
// pointer wrappers, we can safely wait for them to die without interfering
// with their actual lifetime.  Note that the pointer classes don't get
// disposed of unless the cache is being disposed of, which happens when
// the root storage is closed/disposed.
//---------------------------------------------------------------------------

generic<class T>
where T : IComPointer
ref class ComCache sealed : public Generic::IDictionary<Guid, T>
{
public:

	//-----------------------------------------------------------------------
	// Constructor

	ComCache();

	//-----------------------------------------------------------------------
	// Member Functions

	// Add (IDictionary<Guid, T>)
	//
	// Adds an element with the provided key and value to the IDictionary
	virtual void Add(Guid key, T value);

	// Clear (IDictionary<Guid, T>)
	//
	// Removes all items from the collection
	virtual void Clear(void);

	// ContainsKey (IDictionary<Guid, T>)
	//
	// Determines whether the IDictionary contains an element with the specified key
	virtual bool ContainsKey(Guid key);

	// Remove (IDictionary<Guid, T>)
	//
	// Removes the element with the specified key from the IDictionary.
	virtual bool Remove(Guid key);

	// TryGetValue (IDictionary<Guid, T>)
	//
	// Gets the value associated with the specified key.
	virtual bool TryGetValue(Guid key, T% value);

	//-----------------------------------------------------------------------
	// Properties

	// default (IDictionary<Guid, T>)
	//
	// Gets or sets the element with the specified key
	virtual property T default [Guid]
	{
		T get(Guid key);
		void set(Guid key, T value);
	}

	// Count (IDictionary<Guid, T>)
	//
	// Gets the number of items stored in the collection
	virtual property int Count
	{
		int get(void);
	}

	// IsReadOnly (IDictionary<Guid, T>)
	//
	// Determines if the collection is read-only or not
	virtual property bool IsReadOnly
	{
		bool get(void) { return false; }
	}

	// SyncRoot
	//
	// Gets an object instance that should be used for external locking
	property Object^ SyncRoot
	{
		Object^ get(void) { return this; }
	}

private:

	// DESTRUCTOR
	~ComCache();

	//-----------------------------------------------------------------------
	// Private Member Functions

	// Add (ICollection<KeyValuePair<Guid, T>>)
	virtual void Add(KeyValuePair<Guid, T> value) sealed = 
		Generic::ICollection<KeyValuePair<Guid, T>>::Add { return Add(value.Key, value.Value); }

	// Contains (ICollection<KeyValuePair<Guid, T>>)
	virtual bool Contains(KeyValuePair<Guid, T> value) sealed = 
		Generic::ICollection<KeyValuePair<Guid, T>>::Contains { return ContainsKey(value.Key); }

	// CopyTo (ICollection<KeyValuePair<Guid, T>>)
	virtual void CopyTo(array<KeyValuePair<Guid, T>>^, int) sealed = 
		Generic::ICollection<KeyValuePair<Guid, T>>::CopyTo { throw gcnew NotImplementedException(); }

	// GetEnumerator (IEnumerable)
	virtual Collections::IEnumerator^ _GetEnumerator(void) sealed = 
		Collections::IEnumerable::GetEnumerator { throw gcnew NotImplementedException(); }

	// GetEnumerator (IDictionary<Guid, T>)
	//
	// Returns an enumerator that iterates through the collection
	virtual Generic::IEnumerator<KeyValuePair<Guid, T>>^ GetEnumerator(void) sealed =
		Generic::IEnumerable<KeyValuePair<Guid, T>>::GetEnumerator { throw gcnew NotImplementedException(); }

	// KillWeakReference
	//
	// Properly kills off a WeakReference instance
	void KillWeakReference(WeakReference^ reference);

	// Remove (ICollection<KeyValuePair<Guid, T>>)
	virtual bool Remove(KeyValuePair<Guid, T> value) sealed = 
		Generic::ICollection<KeyValuePair<Guid, T>>::Remove { return Remove(value.Key); }

	//-----------------------------------------------------------------------
	// Private Properties

	// Keys (IDictionary<Guid, T>)
	virtual property Generic::ICollection<Guid>^ Keys
	{
		Generic::ICollection<Guid>^ get(void) sealed = 
			Generic::IDictionary<Guid, T>::Keys::get { throw gcnew NotImplementedException(); }
	}

	// Values (IDictionary<Guid, T>)
	virtual property Generic::ICollection<T>^ Values
	{
		Generic::ICollection<T>^ get(void) sealed = 
			Generic::IDictionary<Guid, T>::Values::get { throw gcnew NotImplementedException(); }
	}

	//-----------------------------------------------------------------------
	// Member Variables

	bool								m_disposed;		// Object disposal flag
	Dictionary<Guid, WeakReference^>^	m_cache;		// Cache dictionary
};

//---------------------------------------------------------------------------

END_NAMESPACE(structured)
END_NAMESPACE(storage)
END_NAMESPACE(zuki)

#pragma warning(pop)

#endif	// __COMCACHE_H_
