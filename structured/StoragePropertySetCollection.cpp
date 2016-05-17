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
#include "StoragePropertySetCollection.h"	// Include this class' declarations
#include "StructuredStorage.h"				// Include StructuredStorage declarations
#include "StoragePropertySet.h"				// Include StoragePropertySet declarations

#pragma warning(push, 4)					// Enable maximum compiler warnings

BEGIN_NAMESPACE(zuki)
BEGIN_NAMESPACE(storage)
BEGIN_NAMESPACE(structured)

//---------------------------------------------------------------------------
// StoragePropertySetCollection Constructor (internal)
//
// Arguments:
//
//	root			- Reference to the root Storage object
//	storage			- ComStorage instance that contains this prop set

StoragePropertySetCollection::StoragePropertySetCollection(StructuredStorage^ root, 
	ComStorage^ storage) : m_root(root), m_storage(storage)
{
	if(m_root == nullptr) throw gcnew ArgumentNullException();	
	if(m_storage == nullptr) throw gcnew ArgumentNullException();

	m_readOnly = StorageUtil::IsStorageReadOnly(m_storage);
}

//---------------------------------------------------------------------------
// StoragePropertySetCollection::Add
//
// Creates a new property set within this parent container
//
// Arguments:
//
//	name		- Name of the property set to be created

StoragePropertySet^ StoragePropertySetCollection::Add(String^ name)
{
	Guid					propsetid;			// Property set ID guid
	IPropertyStorage*		pPropStorage;		// Property set interface
	ComPropertyStorage^		propStorage;		// The new property set object
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_storage->IsDisposed());
	if(name == nullptr) throw gcnew ArgumentNullException();
	if(m_readOnly) throw gcnew ContainerReadOnlyException();

	// Make sure that the specified name doesn't already exist in this collection

	if(m_storage->PropertySetNameMapper->ContainsName(name)) throw gcnew PropertySetExistsException(name);

	propsetid = Guid::NewGuid();				// Generate a new GUID (FMTID)

	// Attempt to physically create the new property set, and if successful add
	// it's name and GUID to the name mapper as well

	hResult = m_storage->CreatePropertySet(StorageUtil::SysGuidToUUID(propsetid), 
		NULL, PROPSETFLAG_DEFAULT, STGM_CREATE | StorageUtil::GetStorageMode(m_storage), &pPropStorage);
	if(FAILED(hResult)) throw gcnew StorageException(hResult, name);

	// Create the IPropertyStorage wrapper and release the raw pointer.  If something
	// goes wrong from here, it will release itself automatically on finalization

	propStorage = gcnew ComPropertyStorage(pPropStorage);
	pPropStorage->Release();

	// Attempt to add the new pointer wrapper into the cache, and be sure to delete
	// the newly created property set on exception since it will be orphaned

	try { m_root->ComPropStorageCache->Add(propsetid, propStorage); }
	catch(Exception^) { m_storage->DeletePropertySet(StorageUtil::SysGuidToUUID(propsetid)); throw; }

	m_storage->PropertySetNameMapper->AddMapping(name, propsetid);
	return gcnew StoragePropertySet(m_storage, propStorage);
}

//---------------------------------------------------------------------------
// StoragePropertySetCollection::Clear
//
// Deletes all property sets that are underneath this parent container
//
// Arguments:
//
//	NONE

void StoragePropertySetCollection::Clear(void)
{
	HRESULT					hResult;		// Result from function call

	CHECK_DISPOSED(m_storage->IsDisposed());
	if(m_readOnly) throw gcnew ContainerReadOnlyException();

	// The NameMapper has a handy ToList() function that essentially enumerates
	// all of the objects we placed into this container, so that should suffice.

	for each(KeyValuePair<String^, Guid> item in m_storage->PropertySetNameMapper->ToDictionary()) {

		// First try to physically remove the property set from the collection

		hResult = m_storage->DeletePropertySet(StorageUtil::SysGuidToUUID(item.Value));
		if(FAILED(hResult)) throw gcnew StorageException(hResult);

		// Remove the mapping from the container's property set mapper

		m_storage->PropertySetNameMapper->RemoveMapping(item.Value);
	}
}

//---------------------------------------------------------------------------
// StoragePropertySetCollection::Contains
//
// Determines if an object stream with the specified name exists as an
// object stream within this parent container
//
// Arguments:
//
//	name		- Name of the object stream to search for

bool StoragePropertySetCollection::Contains(String^ name)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	if(name == nullptr) throw gcnew ArgumentNullException();

	return m_storage->PropertySetNameMapper->ContainsName(name);
}

//---------------------------------------------------------------------------
// StoragePropertySetCollection::Contains (private)
//
// ICollection<T> implementation of Contains
//
// Arguments:
//
//	item		- Item to be tested for existance

bool StoragePropertySetCollection::Contains(StoragePropertySet^ item)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	if(item == nullptr) throw gcnew ArgumentNullException();

	return Contains(item->Name);
}

//---------------------------------------------------------------------------
// StoragePropertySetCollection::CopyTo
//
// Copies the contents of the collection into an array
//
// Arguments:
//
//	target		- The target array to place the objects into
//	index		- Offset into the array to start loading it

void StoragePropertySetCollection::CopyTo(array<StoragePropertySet^>^ target, int index)
{
	CHECK_DISPOSED(m_storage->IsDisposed());

	// I don't mind if this method is a little slow or a little wasteful, so
	// I'm just gonna cheat and leverage a List<> in conjunction with our
	// own enumerable interface to accomplish this task

	(gcnew List<StoragePropertySet^>(this))->CopyTo(target, index);
}

//---------------------------------------------------------------------------
// StoragePropertySetCollection::Count::get
//
// Returns the number of object streams within this parent container
//
// Arguments:
//
//	NONE

int StoragePropertySetCollection::Count::get(void)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	return m_storage->PropertySetNameMapper->Count;
}

//---------------------------------------------------------------------------
// StoragePropertySetCollection::default::get (string)
//
// Accesses an object name by name.  If the collection is not read-only,
// the object stream will be created if it doesn't already exist
//
// Arguments:
//
//	name		- Object stream name to access/create

StoragePropertySet^ StoragePropertySetCollection::default::get(String^ name)
{
	Guid					propsetid;			// Property set ID GUID

	CHECK_DISPOSED(m_storage->IsDisposed());
	if(name == nullptr) throw gcnew ArgumentNullException();

	// Attempt to retrieve the GUID for the specified name from the mapper.
	// If the name is not found and we're not read-only, attempt to add
	// a new property set instead of just throwing an exception

	if(!m_storage->PropertySetNameMapper->TryMapNameToGuid(name, propsetid)) {

		if(!m_readOnly) return Add(name);
		else throw gcnew PropertySetNotFoundException(name);
	}

	return this[propsetid];				// <--- Use GUID version from here
}

//---------------------------------------------------------------------------
// StoragePropertySetCollection::default::get (integer)
//
// Retrieves a property set from the collection, or throws an exception if
// the specified property cannot be found
//
// Arguments:
//
//	name		- Name of the property to retrieve

StoragePropertySet^ StoragePropertySetCollection::default::get(int index)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	return this[LookupIndex(index)];
}

//---------------------------------------------------------------------------
// StorageContainerCollection::default::get (internal)
//
// Accesses a property set by it's GUID. For internal use only, as the behavior
// is different than the other indexers.  This will NOT automatically add
// a new StoragePropertySet into the collection -- it must exist
//
// Arguments:
//
//	propsetid		- GUID of the StoragePropertySet to be retrieved

StoragePropertySet^ StoragePropertySetCollection::default::get(Guid propsetid)
{
	ComPropertyStorage^		propStorage;		// ComPropertyStorage wrapper
	IPropertyStorage*		pPropStorage;		// Raw IPropertyStorage pointer
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_storage->IsDisposed());

	lock cacheLock(m_root->ComPropStorageCache->SyncRoot);	// <--- THREAD SAFETY

	// If an instance of this property set already exists somewhere, we
	// can get a copy of that pointer and just wrap with a new class

	if(m_root->ComPropStorageCache->TryGetValue(propsetid, propStorage))
		return gcnew StoragePropertySet(m_storage, propStorage);

	// This property set hasn't been cached, so we need to actually open it up

	hResult = m_storage->OpenPropertySet(StorageUtil::SysGuidToUUID(propsetid), 
		StorageUtil::GetStorageMode(m_storage), &pPropStorage);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	// Wrap the new IPropertyStorage pointer up, and release our local 
	// reference to it.  The ComPropertyStorage maintains it from here on

	propStorage = gcnew ComPropertyStorage(pPropStorage);
	pPropStorage->Release();

	// Insert the new pointer wrapper into cache (hence the lock), and
	// return a brand new StoragePropertySet back to the caller

	m_root->ComPropStorageCache->Add(propsetid, propStorage);
	return gcnew StoragePropertySet(m_storage, propStorage);
}

//---------------------------------------------------------------------------
// StoragePropertySetCollection::GetEnumerator
//
// Creates and returns a 'snapshot' enumerator of all the object streams
// underneath this parent container
//
// Arguments:
//
//	NONE

Generic::IEnumerator<StoragePropertySet^>^ StoragePropertySetCollection::GetEnumerator(void)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	return gcnew StoragePropertySetEnumerator(m_root, m_storage);
}

//---------------------------------------------------------------------------
// StoragePropertySetCollection::LookupIndex (private)
//
// Attempts to map an integer index value into a property set name.  The index
// is always indicative of the order in which COM decides to enumerate
//
// Arguments:
//
//	index			- The index to be mapped into a string name

String^ StoragePropertySetCollection::LookupIndex(int index)
{
	IEnumSTATPROPSETSTG*	pEnumStg;			// Storage enumerator
	STATPROPSETSTG			statstg;			// Enumerated information
	ULONG					ulRead;				// Number of items read
	Guid					propsetid;			// Property set ID guid
	String^					propsetname;		// Property set name
	HRESULT					hResult;			// Result from function call

	if(index < 0) throw gcnew ArgumentOutOfRangeException();

	// Attempt to grab the property set enumerator from the PropertySetStorage

	hResult = m_storage->EnumPropertySets(&pEnumStg);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	try { 
	
		while(pEnumStg->Next(1, &statstg, &ulRead) == S_OK) {

			propsetid = StorageUtil::UUIDToSysGuid(statstg.fmtid);	// Convert

			// Attempt to look up the name for this property set, and if it
			// happens to not be one of ours, just skip over it

			if(!m_storage->PropertySetNameMapper->TryMapGuidToName(propsetid, propsetname)) continue;
			if(index == 0) return propsetname;
			else index--;
		} 

		// If we run out of properties to enumerate, the index was beyond the
		// end of the property collection ...

		throw gcnew ArgumentOutOfRangeException();

	} // try

	finally { pEnumStg->Release(); }			// Always release interface
}

//---------------------------------------------------------------------------
// StoragePropertySetCollection::Remove
//
// Deletes the specified object stream
//
// Arguments:
//
//	name		- Name of the object stream to be deleted

bool StoragePropertySetCollection::Remove(String^ name)
{
	Guid					propsetid;			// Property set ID guid
	HRESULT					hResult;		// Result from function call

	CHECK_DISPOSED(m_storage->IsDisposed());
	if(name == nullptr) throw gcnew ArgumentNullException();
	if(m_readOnly) throw gcnew ContainerReadOnlyException();

	// Attempt to retrieve the GUID for the specified name from the mapper.
	// Just return false if any kind of exceptions happen in there

	if(!m_storage->PropertySetNameMapper->TryMapNameToGuid(name, propsetid)) return false;

	// Attempt to physically delete the property set and if successful
	// remove it from cache and the name mapper

	hResult = m_storage->DeletePropertySet(StorageUtil::SysGuidToUUID(propsetid));
	if(FAILED(hResult)) throw gcnew StorageException(hResult, name);

	m_storage->PropertySetNameMapper->RemoveMapping(name);		// Remove mapping
	return true;											// Success
}

//---------------------------------------------------------------------------
// StoragePropertySetCollection::Remove (private)
//
// ICollection<T> implementation of Remove
//
// Arguments:
//
//	item		- Item to be removed

bool StoragePropertySetCollection::Remove(StoragePropertySet^ item)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	if(item == nullptr) throw gcnew ArgumentNullException();

	return Remove(item->Name);		// Just use the name version
}

//---------------------------------------------------------------------------
// StoragePropertySetCollection::ToDictionary (internal)
//
// Converts the contents of the collection into a NAME->GUID dictionary.  This
// information is taken from the name mapper directly, no enumeration of storage
// is performed.  This method is for internal use only since the dictionary only
// represents a snapshot of the current collection state.
//
// Arguments:
//
//	NONE

Dictionary<String^, Guid>^ StoragePropertySetCollection::ToDictionary(void)
{
	CHECK_DISPOSED(m_storage->IsDisposed());
	return m_storage->PropertySetNameMapper->ToDictionary();
}

//---------------------------------------------------------------------------

END_NAMESPACE(structured)
END_NAMESPACE(storage)
END_NAMESPACE(zuki)

#pragma warning(pop)
