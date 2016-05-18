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
#include "StorageContainerCollection.h"		// Include this class' declarations
#include "StructuredStorage.h"				// Include StructuredStorage declarations
#include "StorageContainer.h"				// Include StorageContainer declarations

#pragma warning(push, 4)					// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// StorageContainerCollection Constructor (internal)
//
// Arguments:
//
//	root			- Reference to the root Storage object
//	storage			- Reference to ComStorage to be contained

StorageContainerCollection::StorageContainerCollection(StructuredStorage^ root, 
	ComStorage^ storage) : m_root(root), m_storage(storage)
{
	if(m_root == nullptr) throw gcnew ArgumentNullException();	
	if(m_storage == nullptr) throw gcnew ArgumentNullException();

	m_readOnly = StorageUtil::IsStorageReadOnly(m_storage);
}

//---------------------------------------------------------------------------
// StorageContainerCollection::Add
//
// Creates a new sub container within this parent container
//
// Arguments:
//
//	name		- Name of the sub container to be created

StorageContainer^ StorageContainerCollection::Add(String^ name)
{
	Guid					contid;				// Container ID guid
	String^					contname;			// Container BASE64 name
	PinnedStringPtr			pinName;			// Pinned container name
	ComStorage^				subContainer;		// New subcontainer reference
	IStorage*				pSubContainer;		// New subcontainer IStorage
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_storage->IsDisposed());
	if(name == nullptr) throw gcnew ArgumentNullException();
	if(m_readOnly) throw gcnew ContainerReadOnlyException();

	// Make sure that the specified name doesn't already exist in this collection

	if(m_storage->ContainerNameMapper->ContainsName(name)) throw gcnew ContainerExistsException(name);

	contid = Guid::NewGuid();							// Generate a new container GUID
	contname = StorageUtil::SysGuidToBase64(contid);	// Convert into BASE64
	pinName = PtrToStringChars(contname);				// Pin it down in memory

	// Attempt to physically create the new container, and if successful
	// add it's name and GUID to the name mapper as well

	hResult = m_storage->CreateStorage(pinName, StorageUtil::GetStorageMode(m_storage), 
		0, 0, &pSubContainer);
	if(FAILED(hResult)) throw gcnew StorageException(hResult, name);

	// Create the IStorage wrapper and release the raw pointer.  If something
	// goes wrong from here, it will release itself automatically on finalization

	subContainer = gcnew ComStorage(pSubContainer);
	pSubContainer->Release();

	// Attempt to add the new pointer wrapper into the cache, and be sure to delete
	// the newly created sub container on exception since it will be orphaned

	try { m_root->ComStorageCache->Add(contid, subContainer); }
	catch(Exception^) { m_storage->DestroyElement(pinName); throw; }

	m_storage->ContainerNameMapper->AddMapping(name, contid);
	return gcnew StorageContainer(m_root, m_storage, subContainer);
}

//---------------------------------------------------------------------------
// StorageContainerCollection::Clear
//
// Deletes all sub containers that are underneath this parent container
//
// Arguments:
//
//	NONE

void StorageContainerCollection::Clear(void)
{
	PinnedStringPtr			pinContName;	// Pinned container name
	HRESULT					hResult;		// Result from function call

	CHECK_DISPOSED(m_storage->IsDisposed());
	if(m_readOnly) throw gcnew ContainerReadOnlyException();

	// The NameMapper has a handy ToList() function that essentially enumerates
	// all of the objects we placed into this container, so that should suffice.
	// Without it, we would still have to ask it to look up GUIDs which ends
	// up doing all kinds of enumerations anyway.  This is just better overall.

	for each(KeyValuePair<String^, Guid> item in m_storage->ContainerNameMapper->ToDictionary()) {

		// The physical IStorage is the GUID base64 encoded. The "name" from
		// the name mapper has no bearing on this operation whatsoever

		pinContName = PtrToStringChars(StorageUtil::SysGuidToBase64(item.Value));
		
		// First try to physically remove the container from storage

		hResult = m_storage->DestroyElement(pinContName);
		if(FAILED(hResult)) throw gcnew StorageException(hResult);

		// Remove the mapping from the container's property set mapper

		m_storage->ContainerNameMapper->RemoveMapping(item.Value);
	}
}

//---------------------------------------------------------------------------
// StorageContainerCollection::Contains
//
// Determines if a container with the specified name exists as a sub
// container within this parent container
//
// Arguments:
//
//	name		- Name of the sub container to search for

bool StorageContainerCollection::Contains(String^ name)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	if(name == nullptr) throw gcnew ArgumentNullException();

	return m_storage->ContainerNameMapper->ContainsName(name);
}

//---------------------------------------------------------------------------
// StorageContainerCollection::Contains (private)
//
// ICollection<T> implementation of Contains
//
// Arguments:
//
//	item		- Item to be tested for existance

bool StorageContainerCollection::Contains(StorageContainer^ item)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	if(item == nullptr) throw gcnew ArgumentNullException();

	return Contains(item->Name);
}

//---------------------------------------------------------------------------
// StorageContainerCollection::CopyTo
//
// Copies the contents of the collection into an array
//
// Arguments:
//
//	target		- The target array to place the objects into
//	index		- Offset into the array to start loading it

void StorageContainerCollection::CopyTo(array<StorageContainer^>^ target, int index)
{
	CHECK_DISPOSED(m_storage->IsDisposed());

	// I don't mind if this method is a little slow or a little wasteful, so
	// I'm just gonna cheat and leverage a List<> in conjunction with our
	// own enumerable interface to accomplish this task

	(gcnew List<StorageContainer^>(this))->CopyTo(target, index);
}

//---------------------------------------------------------------------------
// StorageContainerCollection::Count::get
//
// Returns the number of sub containers within this parent container
//
// Arguments:
//
//	NONE

int StorageContainerCollection::Count::get(void)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	return m_storage->ContainerNameMapper->Count;
}

//---------------------------------------------------------------------------
// StorageContainerCollection::default::get (string)
//
// Accesses a subcontainer by name.  If the collection is not read-only,
// the subcontainer will be created if it doesn't already exist
//
// Arguments:
//
//	name		- Sub container name to access/create

StorageContainer^ StorageContainerCollection::default::get(String^ name)
{
	Guid					contid;				// Container ID GUID

	CHECK_DISPOSED(m_storage->IsDisposed());
	if(name == nullptr) throw gcnew ArgumentNullException();

	// Attempt to retrieve the GUID for the specified name from the mapper.
	// If the name is not found and we're not read-only, attempt to add
	// a new container instead of just throwing an exception

	if(!m_storage->ContainerNameMapper->TryMapNameToGuid(name, contid)) {

		if(!m_readOnly) return Add(name);
		else throw gcnew ContainerNotFoundException(name);
	}

	return this[contid];			// <--- Use GUID version from here
}

//---------------------------------------------------------------------------
// StorageContainerCollection::default::get (integer)
//
// Retrieves a container from the collection, or throws an exception if
// the specified container cannot be found
//
// Arguments:
//
//	name		- Name of the property to retrieve

StorageContainer^ StorageContainerCollection::default::get(int index)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	return this[LookupIndex(index)];
}

//---------------------------------------------------------------------------
// StorageContainerCollection::default::get (internal)
//
// Accesses a container by it's GUID. For internal use only, as the behavior
// is different than the other indexers.  This will NOT automatically add
// a new StorageContainer into the collection -- it must exist
//
// Arguments:
//
//	contid		- GUID of the StorageContainer to be retrieved

StorageContainer^ StorageContainerCollection::default::get(Guid contid)
{
	String^					contname;			// Container BASE64 name
	PinnedStringPtr			pinName;			// Pinned container name
	ComStorage^				subContainer;		// Sub container reference
	IStorage*				pSubContainer;		// Sub container IStorage
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_storage->IsDisposed());

	contname = StorageUtil::SysGuidToBase64(contid);		// Convert into BASE64
	pinName = PtrToStringChars(contname);					// Pin it down in memory

	lock cacheLock(m_root->ComStorageCache->SyncRoot);		// <--- THREAD SAFETY

	// If an instance of this container already exists somewhere, we
	// can get a copy of that pointer and just wrap with a new class

	if(m_root->ComStorageCache->TryGetValue(contid, subContainer))
		return gcnew StorageContainer(m_root, m_storage, subContainer);

	// This container hasn't been cached, so we need to actually open it up.

	hResult = m_storage->OpenStorage(pinName, NULL, StorageUtil::GetStorageChildOpenMode(m_storage), 
		NULL, 0, &pSubContainer);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	// Wrap the new IStorage pointer up, and release our local reference
	// to it.  The ComStorage instance maintains it from here on

	subContainer = gcnew ComStorage(pSubContainer);
	pSubContainer->Release();

	// Insert the new pointer wrapper into cache (hence the lock), and
	// return a brand new StorageContainer back to the caller

	m_root->ComStorageCache->Add(contid, subContainer);
	return gcnew StorageContainer(m_root, m_storage, subContainer);
}

//---------------------------------------------------------------------------
// StorageContainerCollection::GetEnumerator
//
// Creates and returns a 'snapshot' enumerator of all the sub containers
// underneath this parent container
//
// Arguments:
//
//	NONE

Generic::IEnumerator<StorageContainer^>^ StorageContainerCollection::GetEnumerator(void)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	return gcnew StorageContainerEnumerator(m_root, m_storage);
}

//---------------------------------------------------------------------------
// StorageContainerCollection::LookupIndex (private)
//
// Attempts to map an integer index value into a container name.  The index
// is always indicative of the order in which COM decides to enumerate
//
// Arguments:
//
//	index			- The index to be mapped into a string name

String^ StorageContainerCollection::LookupIndex(int index)
{
	IEnumSTATSTG*			pEnumStg;			// Storage enumerator
	::STATSTG				statstg;			// Enumerated information
	ULONG					ulRead;				// Number of items read
	Guid					contid;				// Container ID guid
	String^					contname;			// Container name string
	HRESULT					hResult;			// Result from function call

	if(index < 0) throw gcnew ArgumentOutOfRangeException();

	// Attempt to grab the enumerator from the IStorage interface

	hResult = m_storage->EnumElements(0, NULL, 0, &pEnumStg);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	try { 
	
		while(pEnumStg->Next(1, &statstg, &ulRead) == S_OK) {

			try {

				if(statstg.type != STGTY_STORAGE) continue;		// <-- Not IStorage

				// Convert the BASE64 string back into a GUID, and if we
				// end up with Guid::Empty (error), skip this item

				contid = StorageUtil::Base64ToSysGuid(gcnew String(statstg.pwcsName));
				if(contid == Guid::Empty) continue;

				// Attempt to look up the name for this container, and if it
				// happens to not be one of ours, just skip over it

				if(!m_storage->ContainerNameMapper->TryMapGuidToName(contid, contname)) continue;
				if(index == 0) return contname;
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
// StorageContainerCollection::Remove
//
// Deletes the specified sub container
//
// Arguments:
//
//	name		- Name of the sub container to be deleted

bool StorageContainerCollection::Remove(String^ name)
{
	Guid					contid;			// Container ID guid
	String^					contname;		// Container BASE64 name
	PinnedStringPtr			pinName;		// Pinned container name
	HRESULT					hResult;		// Result from function call

	CHECK_DISPOSED(m_storage->IsDisposed());
	if(name == nullptr) throw gcnew ArgumentNullException();
	if(m_readOnly) throw gcnew ContainerReadOnlyException();

	// Attempt to retrieve the GUID for the specified name from the mapper.
	// Translate any error into a ContainerNotFoundException instead.

	if(!m_storage->ContainerNameMapper->TryMapNameToGuid(name, contid)) return false;

	contname = StorageUtil::SysGuidToBase64(contid);	// Convert into BASE64
	pinName = PtrToStringChars(contname);				// Pin it down in memory

	// Attempt to physically delete the container from this container,
	// and if successful remove it from cache and the name mapper

	hResult = m_storage->DestroyElement(pinName);
	if(FAILED(hResult)) throw gcnew StorageException(hResult, name);

	m_storage->ContainerNameMapper->RemoveMapping(name);		// Remove mapping
	return true;											// Success
}

//---------------------------------------------------------------------------
// StorageContainerCollection::Remove (private)
//
// ICollection<T> implementation of Remove
//
// Arguments:
//
//	item		- Item to be removed

bool StorageContainerCollection::Remove(StorageContainer^ item)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	if(item == nullptr) throw gcnew ArgumentNullException();

	return Remove(item->Name);
}

//---------------------------------------------------------------------------
// StorageContainerCollection::ToDictionary (internal)
//
// Converts the contents of the collection into a NAME->GUID dictionary.  This
// information is taken from the name mapper directly, no enumeration of storage
// is performed.  This method is for internal use only since the dictionary only
// represents a snapshot of the current collection state.
//
// Arguments:
//
//	NONE

Dictionary<String^, Guid>^ StorageContainerCollection::ToDictionary(void)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	return m_storage->ContainerNameMapper->ToDictionary();
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)
