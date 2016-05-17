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
#include "StorageObjectCollection.h"		// Include this class' declarations
#include "StructuredStorage.h"				// Include StructuredStorage declarations
#include "StorageObject.h"					// Include StorageObject declarations

#pragma warning(push, 4)					// Enable maximum compiler warnings

BEGIN_NAMESPACE(zuki)
BEGIN_NAMESPACE(storage)
BEGIN_NAMESPACE(structured)

//---------------------------------------------------------------------------
// StorageObjectCollection Constructor (internal)
//
// Arguments:
//
//	root			- Reference to the root Storage object
//	storage			- Reference to contained storage instance

StorageObjectCollection::StorageObjectCollection(StructuredStorage^ root, 
	ComStorage^ storage) : m_root(root), m_storage(storage)
{
	if(m_root == nullptr) throw gcnew ArgumentNullException();	
	if(m_storage == nullptr) throw gcnew ArgumentNullException();

	m_readOnly = StorageUtil::IsStorageReadOnly(m_storage);
}

//---------------------------------------------------------------------------
// StorageObjectCollection::Add
//
// Creates a new object stream within this parent container
//
// Arguments:
//
//	name		- Name of the object stream to be created

StorageObject^ StorageObjectCollection::Add(String^ name)
{
	Guid					objid;				// Object ID guid
	String^					objname;			// Object BASE64 name
	PinnedStringPtr			pinName;			// Pinned object name
	ComStream^				stream;				// New ComStream instance
	IStream*				pStream;			// New object IStream
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_storage->IsDisposed());
	if(name == nullptr) throw gcnew ArgumentNullException();
	if(m_readOnly) throw gcnew ContainerReadOnlyException();

	// Make sure that the specified name doesn't already exist in this collection

	if(m_storage->ObjectNameMapper->ContainsName(name)) throw gcnew ObjectExistsException(name);

	objid = Guid::NewGuid();							// Generate a new object GUID
	objname = StorageUtil::SysGuidToBase64(objid);		// Convert into BASE64
	pinName = PtrToStringChars(objname);				// Pin it down in memory

	// Attempt to physically create the new object stream, and if successful
	// add it's name and GUID to the name mapper as well

	hResult = m_storage->CreateStream(pinName, StorageUtil::GetStorageMode(m_storage), 
		0, 0, &pStream);
	if(FAILED(hResult)) throw gcnew StorageException(hResult, name);

	// Create the IStorage wrapper and release the raw pointer.  If something
	// goes wrong from here, it will release itself automatically on finalization

	stream = gcnew ComStream(pStream);
	pStream->Release();

	// Attempt to add the new pointer wrapper into the cache, and be sure to delete
	// the newly created sub container on exception since it will be orphaned

	try { m_root->ComStreamCache->Add(objid, stream); }
	catch(Exception^) { m_storage->DestroyElement(pinName); throw; }

	m_storage->ObjectNameMapper->AddMapping(name, objid);
	return gcnew StorageObject(m_root, m_storage, stream);
}

//---------------------------------------------------------------------------
// StorageObjectCollection::Clear
//
// Deletes all object streams that are underneath this parent container
//
// Arguments:
//
//	NONE

void StorageObjectCollection::Clear(void)
{
	PinnedStringPtr			pinObjName;		// Pinned object name
	HRESULT					hResult;		// Result from function call

	CHECK_DISPOSED(m_storage->IsDisposed());
	if(m_readOnly) throw gcnew ContainerReadOnlyException();

	// The NameMapper has a handy ToList() function that essentially enumerates
	// all of the objects we placed into this container, so that should suffice.
	// Without it, we would still have to ask it to look up GUIDs which ends
	// up doing all kinds of enumerations anyway.  This is just better overall.

	for each(KeyValuePair<String^, Guid> item in m_storage->ObjectNameMapper->ToDictionary()) {

		// The physical IStream is the GUID base64 encoded. The "name" from
		// the name mapper has no bearing on this operation whatsoever

		pinObjName = PtrToStringChars(StorageUtil::SysGuidToBase64(item.Value));
		
		// First try to physically remove the object stream from storage

		hResult = m_storage->DestroyElement(pinObjName);
		if(FAILED(hResult)) throw gcnew StorageException(hResult);

		// Remove the mapping from the container's property set mapper

		m_storage->ObjectNameMapper->RemoveMapping(item.Value);
	}
}

//---------------------------------------------------------------------------
// StorageObjectCollection::Contains
//
// Determines if an object stream with the specified name exists as an
// object stream within this parent container
//
// Arguments:
//
//	name		- Name of the object stream to search for

bool StorageObjectCollection::Contains(String^ name)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	if(name == nullptr) throw gcnew ArgumentNullException();

	return m_storage->ObjectNameMapper->ContainsName(name);
}

//---------------------------------------------------------------------------
// StorageObjectCollection::Contains (private)
//
// ICollection<T> implementation of Contains
//
// Arguments:
//
//	item		- Item to be tested for existance

bool StorageObjectCollection::Contains(StorageObject^ item)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	if(item == nullptr) throw gcnew ArgumentNullException();

	return Contains(item->Name);
}

//---------------------------------------------------------------------------
// StorageObjectCollection::CopyTo
//
// Copies the contents of the collection into an array
//
// Arguments:
//
//	target		- The target array to place the objects into
//	index		- Offset into the array to start loading it

void StorageObjectCollection::CopyTo(array<StorageObject^>^ target, int index)
{
	CHECK_DISPOSED(m_storage->IsDisposed());

	// I don't mind if this method is a little slow or a little wasteful, so
	// I'm just gonna cheat and leverage a List<> in conjunction with our
	// own enumerable interface to accomplish this task

	(gcnew List<StorageObject^>(this))->CopyTo(target, index);
}

//---------------------------------------------------------------------------
// StorageObjectCollection::Count::get
//
// Returns the number of object streams within this parent container
//
// Arguments:
//
//	NONE

int StorageObjectCollection::Count::get(void)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	return m_storage->ObjectNameMapper->Count;
}

//---------------------------------------------------------------------------
// StorageObjectCollection::default::get (string)
//
// Accesses an object name by name.  If the collection is not read-only,
// the object stream will be created if it doesn't already exist
//
// Arguments:
//
//	name		- Object stream name to access/create

StorageObject^ StorageObjectCollection::default::get(String^ name)
{
	Guid					objid;				// Object ID GUID

	CHECK_DISPOSED(m_storage->IsDisposed());
	if(name == nullptr) throw gcnew ArgumentNullException();

	// Attempt to retrieve the GUID for the specified name from the mapper.
	// If the name is not found and we're not read-only, attempt to add
	// a new container instead of just throwing an exception

	if(!m_storage->ObjectNameMapper->TryMapNameToGuid(name, objid)) {

		if(!m_readOnly) return Add(name);
		else throw gcnew ObjectNotFoundException(name);
	}

	return this[objid];			// <-- Use internal GUID version now
}

//---------------------------------------------------------------------------
// StorageObjectCollection::default::get (integer)
//
// Retrieves an object stream from the collection, or throws an exception if
// the specified object stream cannot be found
//
// Arguments:
//
//	name		- Name of the property to retrieve

StorageObject^ StorageObjectCollection::default::get(int index)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	return this[LookupIndex(index)];
}

//---------------------------------------------------------------------------
// StorageObjectCollection::default::get (internal)
//
// Accesses an object name by GUID. For internal use only, as the behavior
// is different than the other indexers.  This will NOT automatically add
// a new StorageObject into the collection -- it must exist
//
// Arguments:
//
//	objid		- GUID of the StorageObject to be retrieved

StorageObject^ StorageObjectCollection::default::get(Guid objid)
{
	String^					objname;			// Object BASE64 name
	PinnedStringPtr			pinName;			// Pinned object name
	ComStream^				stream;				// Object ComStream instance
	IStream*				pStream;			// Object IStream
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_storage->IsDisposed());

	objname = StorageUtil::SysGuidToBase64(objid);			// Convert into BASE64
	pinName = PtrToStringChars(objname);					// Pin it down in memory

	lock cacheLock(m_root->ComStreamCache->SyncRoot);		// <--- THREAD SAFETY

	// If an instance of this object already exists somewhere, we
	// can get a copy of that pointer and just wrap with a new class

	if(m_root->ComStreamCache->TryGetValue(objid, stream))
		return gcnew StorageObject(m_root, m_storage, stream);

	// This object hasn't been cached, so we need to actually open it up

	hResult = m_storage->OpenStream(pinName, NULL, 
		StorageUtil::GetStorageChildOpenMode(m_storage), 0, &pStream);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	// Wrap the new IStream pointer up, and release our local reference
	// to it.  The ComStream instance maintains it from here on

	stream = gcnew ComStream(pStream);
	pStream->Release();

	// Insert the new pointer wrapper into cache (hence the lock), and
	// return a brand new StorageContainer back to the caller

	m_root->ComStreamCache->Add(objid, stream);
	return gcnew StorageObject(m_root, m_storage, stream);
}

//---------------------------------------------------------------------------
// StorageObjectCollection::GetEnumerator
//
// Creates and returns a 'snapshot' enumerator of all the object streams
// underneath this parent container
//
// Arguments:
//
//	NONE

Generic::IEnumerator<StorageObject^>^ StorageObjectCollection::GetEnumerator(void)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	return gcnew StorageObjectEnumerator(m_root, m_storage);
}

//---------------------------------------------------------------------------
// StorageObjectCollection::LookupIndex (private)
//
// Attempts to map an integer index value into an object name.  The index
// is always indicative of the order in which COM decides to enumerate
//
// Arguments:
//
//	index			- The index to be mapped into a string name

String^ StorageObjectCollection::LookupIndex(int index)
{
	IEnumSTATSTG*			pEnumStg;			// Storage enumerator
	::STATSTG				statstg;			// Enumerated information
	ULONG					ulRead;				// Number of items read
	Guid					objid;				// Object ID guid
	String^					objname;			// Object name string
	HRESULT					hResult;			// Result from function call

	if(index < 0) throw gcnew ArgumentOutOfRangeException();

	// Attempt to grab the enumerator from the IStorage interface

	hResult = m_storage->EnumElements(0, NULL, 0, &pEnumStg);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	try { 
	
		while(pEnumStg->Next(1, &statstg, &ulRead) == S_OK) {

			try {

				if(statstg.type != STGTY_STREAM) continue;		// <-- Not IStream

				// Convert the BASE64 string back into a GUID, and if we
				// end up with Guid::Empty (error), skip this item

				objid = StorageUtil::Base64ToSysGuid(gcnew String(statstg.pwcsName));
				if(objid == Guid::Empty) continue;

				// Attempt to look up the name for this object, and if it
				// happens to not be one of ours, just skip over it

				if(!m_storage->ObjectNameMapper->TryMapGuidToName(objid, objname)) continue;
				if(index == 0) return objname;
				else index--;
			}
			
			finally { if(statstg.pwcsName) CoTaskMemFree(statstg.pwcsName); }
		} 

		// If we run out of items to enumerate, the index was beyond the end of
		// the collection ...

		throw gcnew ArgumentOutOfRangeException();

	} // try

	finally { pEnumStg->Release(); }			// Always release interface
}

//---------------------------------------------------------------------------
// StorageObjectCollection::Remove
//
// Deletes the specified object stream
//
// Arguments:
//
//	name		- Name of the object stream to be deleted

bool StorageObjectCollection::Remove(String^ name)
{
	Guid					objid;			// Object ID guid
	String^					objname;		// Object BASE64 name
	PinnedStringPtr			pinName;		// Pinned object name
	HRESULT					hResult;		// Result from function call

	CHECK_DISPOSED(m_storage->IsDisposed());
	if(name == nullptr) throw gcnew ArgumentNullException();
	if(m_readOnly) throw gcnew ContainerReadOnlyException();

	// Attempt to retrieve the GUID for the specified name from the mapper.

	if(!m_storage->ObjectNameMapper->TryMapNameToGuid(name, objid)) return false;

	objname = StorageUtil::SysGuidToBase64(objid);		// Convert into BASE64
	pinName = PtrToStringChars(objname);				// Pin it down in memory

	// Attempt to physically delete the object from this container,
	// and if successful remove it from cache and the name mapper

	hResult = m_storage->DestroyElement(pinName);
	if(FAILED(hResult)) throw gcnew StorageException(hResult, name);

	m_storage->ObjectNameMapper->RemoveMapping(name);		// Remove mapping
	return true;											// Success
}

//---------------------------------------------------------------------------
// StorageObjectCollection::Remove (private)
//
// ICollection<T> implementation of Remove
//
// Arguments:
//
//	item		- Item to be removed

bool StorageObjectCollection::Remove(StorageObject^ item)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	if(item == nullptr) throw gcnew ArgumentNullException();

	return Remove(item->Name);
}

//---------------------------------------------------------------------------
// StorageObjectCollection::ToDictionary (internal)
//
// Converts the contents of the collection into a NAME->GUID dictionary.  This
// information is taken from the name mapper directly, no enumeration of storage
// is performed.  This method is for internal use only since the dictionary only
// represents a snapshot of the current collection state.
//
// Arguments:
//
//	NONE

Dictionary<String^, Guid>^ StorageObjectCollection::ToDictionary(void)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	return m_storage->ObjectNameMapper->ToDictionary();
}

//---------------------------------------------------------------------------

END_NAMESPACE(structured)
END_NAMESPACE(storage)
END_NAMESPACE(zuki)

#pragma warning(pop)
