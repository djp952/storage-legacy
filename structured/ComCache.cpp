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

#include "stdafx.h"							// Include project pre-compiled headers
#include "ComCache.h"						// Include ComCache declarations

#pragma warning(push, 4)					// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// StorageCache Constructor (internal)
//
// Arguments:
//
//	NONE

generic<class T>
ComCache<T>::ComCache()
{
	m_cache = gcnew Dictionary<Guid, WeakReference^>();
}

//---------------------------------------------------------------------------
// ComCache Destructor

generic<class T>
ComCache<T>::~ComCache()
{
	Clear();						// Remove all cached weak references
	m_disposed = true;				// Object is now disposed of
}

//---------------------------------------------------------------------------
// ComCache::Add
//
// Inserts a new item into the collection.  Will throw an exception if
// the item already exists in the collection
//
// Arguments:
//
//	key			- GUID representing the object's key
//	value		- Object to be inserted into the collection

generic<class T>
void ComCache<T>::Add(Guid key, T value)
{
	lock					cs(SyncRoot);	// Automatic critical section
	WeakReference^			reference;		// Weak reference to item

	CHECK_DISPOSED(m_disposed);

	if(value == T()) throw gcnew ArgumentNullException();

	// See if an item with the same GUID already exists in the collection.
	// If it does, and the object is not dead yet, throw an ArgumentException

	if(m_cache->TryGetValue(key, reference)) {

		if(reference->IsAlive) throw gcnew ArgumentException();
		else reference->Target = nullptr;
	}

	// Insert or replace the item in the collection with a new weak reference

	m_cache[key] = gcnew WeakReference(value, false);
	GC::KeepAlive(value);
}

//---------------------------------------------------------------------------
// ComCache::Clear
//
// Removes all items from the collection
//
// Arguments:
//
//	NONE

generic<class T>
void ComCache<T>::Clear(void)
{
	lock					cs(SyncRoot);	// Automatic crtiical section
	
	CHECK_DISPOSED(m_disposed);

	// Walk the collection and kill all the WeakReferences before we clear
	// out the contents of the underlying collection ...

	for each(KeyValuePair<Guid, WeakReference^> item in m_cache) KillWeakReference(item.Value);
	m_cache->Clear();
}

//---------------------------------------------------------------------------
// ComCache::ContainsKey
//
// Determines if the provided object exists in the collection
//
// Arguments:
//
//	key			- Key to be searched for in the collection

generic<class T>
bool ComCache<T>::ContainsKey(Guid key)
{
	lock					cs(SyncRoot);	// Automatic critical section
	WeakReference^			reference;		// WeakReference instance

	CHECK_DISPOSED(m_disposed);

	// If there isn't an item, dead or alive, in the collection we're done.
	// Otherwise, if the item is dead, pretend it didn't exist at all since
	// an attempt to Add() it would not fail ...

	if(!m_cache->TryGetValue(key, reference)) return false;
	else return reference->IsAlive;
}

//---------------------------------------------------------------------------
// ComCache::Count::get
//
// Returns the number of items current stored in the cache

generic<class T>
int ComCache<T>::Count::get(void)
{
	lock					cs(SyncRoot);	// Automatic critical section
	int						count = 0;		// Number of alive items

	CHECK_DISPOSED(m_disposed);

	// Don't include dead objects in the count to be returned to the caller

	for each(WeakReference^ reference in m_cache->Values) { if(reference->IsAlive) count++; }
	return count;
}

//---------------------------------------------------------------------------
// ComCache::default::get
//
// Retrieves an item from the collection, or NULL if not found/not alive
//
// Arguments:
//
//	key			- GUID key of the item to be retrieved

generic<class T>
T ComCache<T>::default::get(Guid key)
{
	lock					cs(SyncRoot);	// Automatic critical section
	WeakReference^			reference;		// Located object reference

	CHECK_DISPOSED(m_disposed);

	// If the item doesn't exist, or is dead throw a standard exception. Now
	// that this implements TryGetValue() this is an appropriate behavior

	if((!m_cache->TryGetValue(key, reference)) || (!reference->IsAlive)) throw gcnew KeyNotFoundException();
	else return safe_cast<T>(reference->Target);
}

//---------------------------------------------------------------------------
// ComCache::default::set
//
// Inserts or replaces an item in the cache.
//
// Arguments:
//
//	key				- GUID of the item being inserted/replaced
//	value			- Object instance to be set/replaced

generic<class T>
void ComCache<T>::default::set(Guid key, T value)
{
	lock					cs(SyncRoot);	// Automatic critical section
	WeakReference^			reference;		// Located object reference

	CHECK_DISPOSED(m_disposed);

	if(value == T()) throw gcnew ArgumentNullException();

	// If the item already exists in the collection, nuke it before re-adding it.
	// Otherwise, just add it into the collection as a new WeakReference

	if(m_cache->TryGetValue(key, reference)) KillWeakReference(reference);
	m_cache[key] = gcnew WeakReference(value);

	GC::KeepAlive(value);			// Keep it alive as long as possible
}

//---------------------------------------------------------------------------
// ComCache::KillWeakReference (private)
//
// Properly kills off a WeakReference instance
//
// Arguments:
//
//	reference	- WeakReference to be nuked

generic<class T>
void ComCache<T>::KillWeakReference(WeakReference^ reference)
{
	T					instance;		// Strong object instance

	// If the reference is NULL or the target isn't alive anymore,
	// there really isn't anything interesting for us to do here

	if((reference == nullptr) || (!reference->IsAlive)) return;

	instance = safe_cast<T>(reference->Target);		// Get strong ref
	if(!instance->IsDisposed()) delete instance;	// Dispose it
	instance = T();									// Release strong ref

	reference->Target = nullptr;		// Release weak reference as well
}

//---------------------------------------------------------------------------
// ComCache::Remove
//
// Removes an item from the collection
//
// Arguments:
//
//	key			- GUID of the object to be removed

generic<class T>
bool ComCache<T>::Remove(Guid key)
{
	lock					cs(SyncRoot);	// Automatic critical section
	WeakReference^			reference;		// Located object reference

	CHECK_DISPOSED(m_disposed);

	// If the reference exists, kill it off before removing it from the cache

	if(m_cache->TryGetValue(key, reference)) KillWeakReference(reference);
	return m_cache->Remove(key);
}

//---------------------------------------------------------------------------
// ComCache::TryGetValue
//
// Attempts to retrieve an object from the collection, but does not throw
// a KeyNotFoundException if it's not found, unlike default::get
//
// Arguments:
//
//	key			- Key to be located in the collection
//	value		- On success, will be set to the collection value

generic<class T>
bool ComCache<T>::TryGetValue(Guid key, T% value)
{
	lock					cs(SyncRoot);	// Automatic critical section
	WeakReference^			reference;		// Located object reference

	value = T();							// Initialize [out] reference
	CHECK_DISPOSED(m_disposed);				// Do this after [out] init

	// If the item doesn't exist in the collection, or it's been finalized
	// just return false back to the caller ... 

	if(!m_cache->TryGetValue(key, reference)) return false;
	if(!reference->IsAlive) return false;

	value = safe_cast<T>(reference->Target);	// Cast into [out] reference
	return (value != nullptr);					// Return final object status
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
