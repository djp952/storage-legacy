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

#include "stdafx.h"						// Include project pre-compiled headers
#include "StorageNameMapper.h"			// Include StorageNameMapper declarations
#include "ComStorage.h"					// Include ComStorage declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_NAMESPACE(zuki)
BEGIN_NAMESPACE(storage)
BEGIN_NAMESPACE(structured)

//---------------------------------------------------------------------------
// Custom property set FMTID codes
//---------------------------------------------------------------------------

#include "initguid.h"					// Include INITGUID declarations

// FMTID_ContainerNameMapper {3D43F9C4-AB22-4b71-80E0-333EBF5EAA14}
// Used as the FMTID for the custom container name->GUID mapper properties
DEFINE_GUID(FMTID_ContainerNameMapper, 
0x3d43f9c4, 0xab22, 0x4b71, 0x80, 0xe0, 0x33, 0x3e, 0xbf, 0x5e, 0xaa, 0x14);

// FMTID_ObjectNameMapper {CAC7F8BA-50EB-4910-ACA7-27DA77718924}
// Used as the FMTID for the custom object name->GUID mapper properties
DEFINE_GUID(FMTID_ObjectNameMapper, 
0xcac7f8ba, 0x50eb, 0x4910, 0xac, 0xa7, 0x27, 0xda, 0x77, 0x71, 0x89, 0x24);

// FMTID_PropertySetNameMapper {10A46423-6C68-4536-9461-5F16A79D947A}
// Used as the FMTID for the custom property name->GUID mapper properies
DEFINE_GUID(FMTID_PropertySetNameMapper, 
0x10a46423, 0x6c68, 0x4536, 0x94, 0x61, 0x5f, 0x16, 0xa7, 0x9d, 0x94, 0x7a);

//---------------------------------------------------------------------------
// StorageNameMapper Implementation
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// StorageNameMapper Constructor
//
// Arguments:
//
//	fmtid		- FMTID identifier for this mapper instance
//	storage		- Referenced parent ComStorage interface

StorageNameMapper::StorageNameMapper(const FMTID& fmtid, ComStorage^ storage) 
	: m_fmtid(StorageUtil::UUIDToSysGuid(fmtid)), m_storage(storage)
{
	if(m_storage == nullptr) throw gcnew ArgumentNullException();
}

//---------------------------------------------------------------------------
// StorageNameMapper Finalizer

StorageNameMapper::!StorageNameMapper()
{
	if(m_pPropStorage) m_pPropStorage->Release();
	m_pPropStorage = NULL;
}

//---------------------------------------------------------------------------
// StorageNameMapper::AddMapping
//
// Adds a new mapping to the specialized property set of the storage object
//
// Arguments:
//
//	pStorage	- Pointer to the IStorage object to work with
//	name		- Name to assign to the new mapping
//	guid		- Guid to associate with the new mapping

void StorageNameMapper::AddMapping(String^ name, Guid guid)
{
	lock					cs(SyncRoot);		// Automatic critical section
	IPropertyStorage*		pPropStorage;		// IPropertyStorage interface
	PROPSPEC				propspec;			// Property specification
	PinnedStringPtr			pinName;			// Pinned string pointer
	PROPVARIANT				varValue;			// value as a PROPVARIANT
	GUID					uuid;				// The real UUID value
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());

	if(name == nullptr) throw gcnew ArgumentNullException();
	if(guid == Guid::Empty) throw gcnew ArgumentException();

	// Check to see if this name already exists in the property set

	if(ContainsName(name)) throw gcnew MappingExistsException(name);

	// Retrieve the appropriate IPropertyStorage for this template instance

	hResult = GetPropertyStorage(&pPropStorage);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	pinName = PtrToStringChars(name);			// Pin the string
	uuid = StorageUtil::SysGuidToUUID(guid);	// Convert the GUID
	PropVariantInit(&varValue);					// Initialize PROPVARIANT

	varValue.vt = VT_CLSID;						// Always sending in a VT_CLSID
	varValue.puuid = &uuid;						// Point to our unmanaged UUID

	// Set up the PROPSPEC structure .. we'll be using a name here

	propspec.ulKind = PRSPEC_LPWSTR;
	propspec.lpwstr = const_cast<LPWSTR>(pinName);

	hResult = pPropStorage->WriteMultiple(1, &propspec, reinterpret_cast<PROPVARIANT*>(&varValue), 
		NAMEMAPPER_BASEID);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	hResult = pPropStorage->Commit(STGC_DEFAULT);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	// NOTE: Do not call PropVariantClear() here, since it would try to 
	// release the pinned string (not a good thing to be doing)
}

//---------------------------------------------------------------------------
// StorageNameMapper::ContainsGuid
//
// Determines if a mapping with the specified GUID exists already
//
// Arguments:
//
//	guid			- GUID to be tested for existance

bool StorageNameMapper::ContainsGuid(Guid guid)
{
	lock				cs(SyncRoot);		// Automatic critical section
	String^				name;				// Located mapped name

	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());

	return MapGuidInternal(guid, name, NULL);	// Use MapGuidInternal
}

//---------------------------------------------------------------------------
// StorageNameMapper::ContainsName
//
// Determines if a mapping with the specified NAME exists already
//
// Arguments:
//
//	name			- NAME to be tested for existance

bool StorageNameMapper::ContainsName(String^ name)
{
	lock				cs(SyncRoot);		// Automatic critical section
	Guid				guid;				// Located mapped GUID

	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());

	return TryMapNameToGuid(name, guid);	// Use TryMapNameToGuid
}

//---------------------------------------------------------------------------
// StorageNameMapper::Count::get
//
// Returns the number of properties in this mapper property set

int StorageNameMapper::Count::get(void)
{
	lock					cs(SyncRoot);		// Automatic critical section
	IPropertyStorage*		pPropStorage;		// IPropertyStorage interface
	IEnumSTATPROPSTG*		pEnumStg;			// Storage enumerator
	STATPROPSTG				statstg;			// Enumerated information
	ULONG					ulRead;				// Number of items enumerated
	int						count = 0;			// Number of items read
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());

	hResult = GetPropertyStorage(&pPropStorage);
	if(FAILED(hResult)) return 0;

	// Attempt to grab the property set enumerator from the PropertySetStorage

	hResult = pPropStorage->Enum(&pEnumStg);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	// Enumerate all of the properties and keep track of how many we saw

	while(pEnumStg->Next(1, &statstg, &ulRead) == S_OK) {

		count++;
		if(statstg.lpwstrName) CoTaskMemFree(statstg.lpwstrName); 
	}

	return count;							// Return the number of properties
}

//---------------------------------------------------------------------------
// StorageNameMapper::GetPropertyStorage (private)
//
// Retrieves the IPropertyStorage interface for this storage and FMTID.  If
// it does not exist in the Storage, it is created
//
// Arguments:
//
//	ppPropStorage	- On success, contains the IPropertyStorage interface

HRESULT StorageNameMapper::GetPropertyStorage(IPropertyStorage** ppPropStorage)
{
	FMTID						fmtid;				// Prop set FMTID guid
	IComPropertySetStorage^		propSetStorage;		// IComPropertySetStorage
	bool						readOnly;			// IStorage read-only flag
	HRESULT						hResult;			// Result from function call

	*ppPropStorage = NULL;							// Initialize [out] to NULL

	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());

	if(m_pPropStorage) { *ppPropStorage = m_pPropStorage; return S_OK; }

	fmtid = StorageUtil::SysGuidToUUID(m_fmtid);
	propSetStorage = safe_cast<IComPropertySetStorage^>(m_storage);
	readOnly = StorageUtil::IsStorageReadOnly(m_storage);

	// First try to open an existing property set with the FMTID from the template

	hResult = propSetStorage->Open(fmtid, (readOnly ? STGM_READ : STGM_READWRITE) | 
		STGM_SHARE_EXCLUSIVE, ppPropStorage);

	// If the property storage didn't exist, attempt to create it instead.
	// (Any other error conditions are passed back to the caller)

	if((hResult == STG_E_FILENOTFOUND) && (!readOnly)) {
		
		hResult = propSetStorage->Create(fmtid, NULL, PROPSETFLAG_DEFAULT, 
			STGM_READWRITE | STGM_SHARE_EXCLUSIVE, ppPropStorage);
	}

	if(FAILED(hResult)) return hResult;			// Failed to create/access
	
	m_pPropStorage = *ppPropStorage;			// Cache off the pointer
	return S_OK;								// Success
}

//---------------------------------------------------------------------------
// StorageNameMapper::MapGuidInternal (private)
//
// Internal version of MapGuidToName.  Enumerates all of the properties in the
// set and if the GUID is found, it's corresponding name is returned
//
// Arguments:
//
//	guid				- GUID to be tested for existance
//	name				- On success, contains the mapped NAME
//	pPropid				- Optional pointer to receive the PROPID

bool StorageNameMapper::MapGuidInternal(Guid guid, String^% name, PROPID *pPropid)
{
	IPropertyStorage*		pPropStorage;		// IPropertyStorage interface
	IEnumSTATPROPSTG*		pEnumStg;			// Storage enumerator
	STATPROPSTG				statstg;			// Enumerated information
	ULONG					ulRead;				// Number of items read
	PROPSPEC				propspec;			// Property specification
	PROPVARIANT				varProperty;		// VARIANT property value
	HRESULT					hResult;			// Result from function call

	name = nullptr;								// Initialize [out] variable
	if(pPropid) *pPropid = PID_ILLEGAL;			// Initialize [out] variable

	hResult = GetPropertyStorage(&pPropStorage);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	// Attempt to grab the property set enumerator from the PropertySetStorage

	hResult = pPropStorage->Enum(&pEnumStg);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	try {

		// Enumerate all of the properties currently stuck in here one at a time
		// so we can examine each GUID until we find the one we're looking for

		while(pEnumStg->Next(1, &statstg, &ulRead) == S_OK) {

			try {

				propspec.propid = statstg.propid;		// Set the property ID
				propspec.ulKind = PRSPEC_PROPID;		// Using the ID code

				// Read information about the enumerated property and test to see if
				// it's name happens to match the one passed in by the caller

				hResult = pPropStorage->ReadMultiple(1, &propspec, &varProperty);
				if(FAILED(hResult)) throw gcnew StorageException(hResult);

				try { 

					// If the property type is VT_CLSID and it matches our GUID,
					// return the name associated with it back to the caller

					if((varProperty.vt == VT_CLSID) && (guid == StorageUtil::UUIDToSysGuid(*varProperty.puuid))) {

						if(pPropid) *pPropid = statstg.propid;		// Set PROPID as needed
						name = gcnew String(statstg.lpwstrName);	// Return the name
						return true;								// Mapping successful
					}
				}

				finally { PropVariantClear(&varProperty); }
			} 

			finally { if(statstg.lpwstrName) CoTaskMemFree(statstg.lpwstrName); }

		} // while(pEnumStg->Next
	} // try

	finally { pEnumStg->Release(); }			// Make sure this gets released

	return false;								// GUID could not be located
}

//---------------------------------------------------------------------------
// StorageNameMapper::MapGuidToName
//
// Maps a GUID into it's NAME
//
// Arguments:
//
//	guid		- GUID to be mapped into it's NAME

String^ StorageNameMapper::MapGuidToName(Guid guid)
{
	lock				cs(SyncRoot);		// Automatic critical section
	String^				name;				// Located mapped name string

	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());

	// Use the handy new TryMapGuidToName and just throw the old exception
	// if we were unable to find the specified GUID in the collection

	if(TryMapGuidToName(guid, name)) return name;
	else throw gcnew MappingNotFoundException(guid.ToString("D"));
}

//---------------------------------------------------------------------------
// StorageNameMapper::MapNameToGuid
//
// Maps a NAME into it's GUID
//
// Arguments:
//
//	name		- NAME to be mapped into it's GUID

Guid StorageNameMapper::MapNameToGuid(String^ name)
{
	lock				cs(SyncRoot);		// Automatic critical section
	Guid				guid;				// Located mapped GUID value

	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());

	// Use the handy new TryMapNameToGuid and just throw the old exception
	// if we were unable to find the specified GUID in the collection

	if(TryMapNameToGuid(name, guid)) return guid;
	else throw gcnew MappingNotFoundException(name);
}

//---------------------------------------------------------------------------
// StorageNameMapper::RemoveMapping
//
// Deletes a mapping from the IStorage property set based on it's name
//
// Arguments:
//
//	name			- Name of the mapping to be removed

void StorageNameMapper::RemoveMapping(String^ name)
{
	lock					cs(SyncRoot);	// Automatic critical section
	IPropertyStorage*		pPropStorage;	// IPropertyStorage interface
	PROPSPEC				propspec;		// Property specification
	PinnedStringPtr			pinName;		// Pinned proerty name string
	HRESULT					hResult;		// Result from function call

	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());

	if(name == nullptr) throw gcnew ArgumentNullException();

	hResult = GetPropertyStorage(&pPropStorage);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	// Convert the property name into an unmanaged Unicode string and set
	// up the values in the PROPSPEC structure appropriately

	pinName = PtrToStringChars(name);
	propspec.ulKind = PRSPEC_LPWSTR;
	propspec.lpwstr = const_cast<LPWSTR>(pinName);

	// Attempt to delete this property from the underlying storage

	hResult = pPropStorage->DeleteMultiple(1, &propspec);
	if(FAILED(hResult)) throw gcnew MappingNotFoundException(name);

	hResult = pPropStorage->Commit(STGC_DEFAULT);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);
}

//---------------------------------------------------------------------------
// StorageNameMapper::RemoveMapping
//
// Deletes a mapping from the IStorage property set based on it's GUID
//
// Arguments:
//
//	name			- Name of the mapping to be removed

void StorageNameMapper::RemoveMapping(Guid guid)
{
	lock					cs(SyncRoot);	// Automatic critical section
	IPropertyStorage*		pPropStorage;	// IPropertyStorage interface
	String^					name;			// GUID->NAME mapping
	PROPSPEC				propspec;		// Property specification
	PROPID					propid;			// Property ID code
	HRESULT					hResult;		// Result from function call

	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());

	hResult = GetPropertyStorage(&pPropStorage);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	// Use MapGuidInternal to map the GUID into it's property ID.  We don't
	// care about the name, but we have to get it back by contract here

	if(!MapGuidInternal(guid, name, &propid)) 
		throw gcnew MappingNotFoundException(guid.ToString("D"));

	// Use the PROPID value to remove this property from the collection

	propspec.ulKind = PRSPEC_PROPID;
	propspec.propid = propid;

	hResult = pPropStorage->DeleteMultiple(1, &propspec);
	if(FAILED(hResult)) throw gcnew MappingNotFoundException(guid.ToString("D"));

	hResult = pPropStorage->Commit(STGC_DEFAULT);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);
}

//---------------------------------------------------------------------------
// StorageNameMapper::RenameMapping
//
// Renames a mapping in this IStorage property set, if possible, by adding
// a new mapping with the same GUID and then deleting the original one
//
// Arguments:
//
//	guid			- GUID of the property to be renamed
//	newname			- The new name to change the mapping to

void StorageNameMapper::RenameMapping(Guid guid, String^ newname)
{
	lock						cs(SyncRoot);		// Automatic critical section
	IPropertyStorage*			pPropStorage;		// IPropertyStorage interface
	String^						name;				// Original property name
	PinnedStringPtr				pinNewName;			// Pinned version of newname
	PROPID						propid;				// Property ID code
	HRESULT						hResult;			// Result from function call

	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());

	if(newname == nullptr) throw gcnew ArgumentNullException();

	hResult = GetPropertyStorage(&pPropStorage);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	// Use MapGuidInternal to convert the NAME into it's GUID.  Throw if the GUID
	// could not be located in this name mapper instance

	if(!MapGuidInternal(guid, name, &propid))
		throw gcnew MappingNotFoundException(guid.ToString("D"));

	pinNewName = PtrToStringChars(newname);					// Pin the new name

	// If the name is actually changing, check to make sure it doesn't
	// already exist before allowing the operation to go through

	if((String::Compare(name, newname, true) != 0) && (ContainsName(newname)))
		throw gcnew MappingExistsException(newname);

	// Attempt to rename the property in-place, without removing and re-adding it

	LPOLESTR rgwsz[] = { const_cast<LPOLESTR>(pinNewName) };
	
	hResult = pPropStorage->WritePropertyNames(1, &propid, rgwsz);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	hResult = pPropStorage->Commit(STGC_DEFAULT);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);
}

//---------------------------------------------------------------------------
// StorageNameMapper::ToDictionary
//
// Generates a Dictionary<> of containing the entire contents of the name 
// mapper property set collection
//
// Arguments:
//
//	NONE

Dictionary<String^, Guid>^ StorageNameMapper::ToDictionary(void)
{
	lock					cs(SyncRoot);		// Automatic critical section
	IPropertyStorage*		pPropStorage;		// Pointer to IPropertyStorage
	IEnumSTATPROPSTG*		pEnumStg;			// Storage enumerator
	STATPROPSTG				statstg;			// Enumerated information
	ULONG					ulRead;				// Number of items read
	PROPSPEC				propspec;			// Property specification
	PROPVARIANT				varProperty;		// VARIANT property value
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());

	// Construct the local Dictionary<> collection that will hold the enumerated data

	Dictionary<String^, Guid>^ col = gcnew Dictionary<String^, Guid>(StringComparer::OrdinalIgnoreCase);

	hResult = GetPropertyStorage(&pPropStorage);	// Attempt to get prop storage
	if(FAILED(hResult)) return col;					// Return empty collection on error

	// Attempt to grab the property set enumerator from the PropertyStorage,
	// and immediately release that since we don't need it anymore

	hResult = pPropStorage->Enum(&pEnumStg);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	try {

		// Enumerate all of the properties currently stuck in here one at a time
		// to determine the information we're looking for ...

		while(pEnumStg->Next(1, &statstg, &ulRead) == S_OK) {

			try {

				propspec.propid = statstg.propid;		// Set the property ID
				propspec.ulKind = PRSPEC_PROPID;		// Using the ID code

				// Read information about the enumerated property and test to see if
				// it's name happens to match the one passed in by the caller

				hResult = pPropStorage->ReadMultiple(1, &propspec, &varProperty);
				if(FAILED(hResult)) throw gcnew StorageException(hResult);

				try { 

					if(varProperty.vt != VT_CLSID) continue;	// Should never happen

					// Insert the enumerated name and GUID into the local collection
					
					col->Add(gcnew String(statstg.lpwstrName), StorageUtil::UUIDToSysGuid(*varProperty.puuid));
				}

				finally { PropVariantClear(&varProperty); }

			}

			finally { if(statstg.lpwstrName) CoTaskMemFree(statstg.lpwstrName); }

		} // while(pEnumStg->Next
	} // try

	finally { pEnumStg->Release(); }			// Make sure this gets released

	return col;								// Return the generated list
}

//---------------------------------------------------------------------------
// StorageNameMapper::TryMapGuidToName
//
// Maps a GUID into it's NAME, but does not throw any exceptions
//
// Arguments:
//
//	guid		- GUID to be mapped into it's NAME
//	name		- On success, contains the mapped NAME

bool StorageNameMapper::TryMapGuidToName(Guid guid, String^% name)
{
	lock				cs(SyncRoot);		// Automatic critical section

	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());

	return MapGuidInternal(guid, name, NULL);	// Use MapGuidInternal
}

//---------------------------------------------------------------------------
// StorageNameMapper::TryMapNameToGuid
//
// Maps a name into it's GUID, but does not throw any exceptions
//
// Arguments:
//
//	name				- Name to be mapped back into a GUID
//	guid				- On success, contains the located GUID

bool StorageNameMapper::TryMapNameToGuid(String^ name, Guid% guid)
{
	lock					cs(SyncRoot);		// Automatic critical section
	IPropertyStorage*		pPropStorage;		// IPropertyStorage interface
	PinnedStringPtr			pinName;			// Pinned string pointer
	PROPSPEC				propspec;			// Property specification
	PROPVARIANT				varProperty;		// VARIANT property value
	HRESULT					hResult;			// Result from function call

	guid = Guid::Empty;							// Initialize [out] reference
	
	CHECK_DISPOSED(m_disposed || m_storage->IsDisposed());

	if(name == nullptr) throw gcnew ArgumentNullException();

	hResult = GetPropertyStorage(&pPropStorage);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	// Convert the property name into an unmanaged Unicode string and set
	// up the values in the PROPSPEC structure appropriately

	pinName = PtrToStringChars(name);
	propspec.ulKind = PRSPEC_LPWSTR;
	propspec.lpwstr = const_cast<LPWSTR>(pinName);

	hResult = pPropStorage->ReadMultiple(1, &propspec, &varProperty);
	if(FAILED(hResult)) return false;

	try { 
		
		// If the property isn't VT_CLSID (which it won't be if the property
		// didn't exist and ReadMultiple came back with S_FALSE), convert it
		// back into a GUID and return it to the caller

		if(varProperty.vt != VT_CLSID) return false;
		guid = StorageUtil::UUIDToSysGuid(*varProperty.puuid);
	}

	finally { PropVariantClear(&varProperty); }		// Release variant data

	return true;				// NAME was mapped into a GUID value
}

//---------------------------------------------------------------------------

END_NAMESPACE(structured)
END_NAMESPACE(storage)
END_NAMESPACE(zuki)

#pragma warning(pop)

