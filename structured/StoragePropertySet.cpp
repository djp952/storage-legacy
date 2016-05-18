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
#include "StoragePropertySet.h"			// Include StoragePropertySet declarations

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// StoragePropertySet Constructor (internal)
//
// Arguments:
//
//	parent			- Reference to parent ComStorage instance
//	propStorage		- ComPropertyStorage for this object to use

StoragePropertySet::StoragePropertySet(ComStorage^ parent, ComPropertyStorage^ propStorage) : 
	m_parent(parent), m_propStorage(propStorage)
{
	if(m_parent == nullptr) throw gcnew ArgumentNullException();
	if(m_propStorage == nullptr) throw gcnew ArgumentNullException();

	// We can't get at the read-only flag directly for property sets via
	// Stat(), but we can assume it's the same as the parent container

	m_readOnly = StorageUtil::IsStorageReadOnly(m_parent);
	m_fmtid = StorageUtil::GetPropertySetID(m_propStorage);
}

//---------------------------------------------------------------------------
// StoragePropertySet::AddItem (private)
//
// Adds a NEW property to the property set.  An exception will be thrown if
// the property already exists, unlike default::set()
//
// Arguments:
//
//	name		- Name of the NEW property to be added
//	value		- The value to be assigned to the new property

void StoragePropertySet::AddItem(String^ name, Object^ value)
{
	CHECK_DISPOSED(m_disposed || m_propStorage->IsDisposed());

	if(name == nullptr) throw gcnew ArgumentNullException();
	if(value == nullptr) throw gcnew ArgumentNullException();

	// There is very little optimization we could do by implementing this
	// by hand, so just use the existing functionality we've already got

	if(Contains(name)) throw gcnew PropertyExistsException(name);
	else this[name] = value;
}

//---------------------------------------------------------------------------
// StoragePropertySet::Clear
//
// Removes all properties from the property set
//
// Arguments:
//
//	NONE

void StoragePropertySet::Clear(void)
{
	IEnumSTATPROPSTG*		pEnumStg;			// Storage enumerator
	STATPROPSTG				statstg;			// Enumerated information
	PROPSPEC				propspec;			// PROPSPEC structure
	ULONG					ulRead;				// Number of items read
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_disposed || m_propStorage->IsDisposed());

	// Attempt to grab the property set enumerator from the PropertySetStorage

	hResult = m_propStorage->Enum(&pEnumStg);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	// Just enumerate over all of the items in the property set and delete
	// them as we do.  According to MSDN, this is a legitimate way to do it

	try { 
	
		while(pEnumStg->Next(1, &statstg, &ulRead) == S_OK) {

			try {

				// Set up a PROPSPEC to define the property we want to delete,
				// and attempt to delete it from this property set ...

				propspec.ulKind = PRSPEC_PROPID;
				propspec.propid = statstg.propid;

				hResult = m_propStorage->DeleteMultiple(1, &propspec);
				if(FAILED(hResult)) throw gcnew StorageException(hResult, gcnew String(statstg.lpwstrName));
			}
			
			finally { if(statstg.lpwstrName) CoTaskMemFree(statstg.lpwstrName); }
		
		} // while
	} // try

	finally { pEnumStg->Release(); }			// Always release interface
}

//---------------------------------------------------------------------------
// StoragePropertySet::Contains
//
// Determines if a property with the specified name exists or not
// 
// Arguments:
//
//	name		- Name of the property to be tested

bool StoragePropertySet::Contains(String^ name)
{
	PROPSPEC			propspec;			// Property specification
	PinnedStringPtr		pinName;			// Pinned property name
	PROPVARIANT			value;				// Property value VARIANT
	HRESULT				hResult;			// Result from function call

	CHECK_DISPOSED(m_disposed || m_propStorage->IsDisposed());

	if(name == nullptr) throw gcnew ArgumentNullException();

	// Convert the property name into an unmanaged Unicode string and set
	// up the values in the PROPSPEC structure appropriately

	pinName = PtrToStringChars(name);
	propspec.ulKind = PRSPEC_LPWSTR;
	propspec.lpwstr = const_cast<LPWSTR>(pinName);

	// Attempt to access the property and get it's VARIANT value.  Note
	// that ReadMultiple() will return S_FALSE if the property is not found

	hResult = m_propStorage->ReadMultiple(1, &propspec, &value);
	if(FAILED(hResult)) return false;

	PropVariantClear(&value);				// We don't need the value
	return (hResult == S_OK);				// S_OK = property existed
}

//---------------------------------------------------------------------------
// StoragePropertySet::CopyTo
//
// Copies all properties in this set to an array
//
// Arguments:
//
//	target			- The target array object
//	index			- Starting index into the array

void StoragePropertySet::CopyTo(array<KeyValuePair<String^, Object^>>^ target, int index)
{
	CHECK_DISPOSED(m_disposed || m_propStorage->IsDisposed());
	GenerateList()->CopyTo(target, index);
}

//---------------------------------------------------------------------------
// StoragePropertySet::Count::get
//
// Retrieves the current number of properties in this property set

int StoragePropertySet::Count::get(void)
{
	IEnumSTATPROPSTG*		pEnumStg;			// Storage enumerator
	STATPROPSTG				statstg;			// Enumerated information
	ULONG					ulRead;				// Number of items read
	int						count = 0;			// Enumerated property count
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_disposed || m_propStorage->IsDisposed());

	// Attempt to grab the property set enumerator from the PropertySetStorage

	hResult = m_propStorage->Enum(&pEnumStg);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	// Just enumerate over all of the items in the collection once, being sure
	// to release the property name strings along the way ...

	try { 
	
		while(pEnumStg->Next(1, &statstg, &ulRead) == S_OK) {
			
			if(statstg.lpwstrName) CoTaskMemFree(statstg.lpwstrName); 
			count++; 
		}
	}

	finally { pEnumStg->Release(); }			// Always release interface

	return count;								// Return the enumerated count
}

//---------------------------------------------------------------------------
// StoragePropertySet::default::get
//
// Retrieves a property from the collection, or throws an exception if
// the specified property cannot be found
//
// Arguments:
//
//	name		- Name of the property to retrieve

Object^ StoragePropertySet::default::get(String^ name)
{
	PROPSPEC			propspec;			// Property specification
	PinnedStringPtr		pinName;			// Pinned property name
	PROPVARIANT			value;				// Property value VARIANT
	HRESULT				hResult;			// Result from function call

	CHECK_DISPOSED(m_disposed || m_propStorage->IsDisposed());

	if(name == nullptr) throw gcnew ArgumentNullException();

	// Convert the property name into an unmanaged Unicode string and set
	// up the values in the PROPSPEC structure appropriately

	pinName = PtrToStringChars(name);
	propspec.ulKind = PRSPEC_LPWSTR;
	propspec.lpwstr = const_cast<LPWSTR>(pinName);

	// Attempt to access the property and get it's VARIANT value.  Note
	// that ReadMultiple() will return S_FALSE if the property is not found

	hResult = m_propStorage->ReadMultiple(1, &propspec, &value);
	if(FAILED(hResult)) throw gcnew StorageException(hResult, name);

	try {

		// S_FALSE indicates that the property was not found in this set, 
		// otherwise ask the .NET runtime to convert the VARIANT for us

		if(hResult == S_FALSE) throw gcnew PropertyNotFoundException(name);
		else return Marshal::GetObjectForNativeVariant(IntPtr(&value));
	}
	
	finally { PropVariantClear(&value); }		// Always release VARIANT
}

//---------------------------------------------------------------------------
// StoragePropertySet::default::get (integer)
//
// Retrieves a property from the collection, or throws an exception if
// the specified property cannot be found
//
// Arguments:
//
//	name		- Name of the property to retrieve

Object^ StoragePropertySet::default::get(int index)
{
	CHECK_DISPOSED(m_disposed || m_propStorage->IsDisposed());
	return this[LookupIndex(index)];
}

//---------------------------------------------------------------------------
// StoragePropertySet::default::set
//
// Inserts or replaces a property value
//
// Arguments:
//
//	name		- Name of the property to be added/replaced
//	value		- Object to use as the new property value

void StoragePropertySet::default::set(String^ name, Object^ value)
{
	PROPVARIANT					varValue;		// New value as a PROPVARIANT
	PinnedStringPtr				pinName;		// Pinned property name string
	PROPSPEC					propspec;		// Property specification
	HRESULT						hResult;		// Result from function call

	CHECK_DISPOSED(m_disposed || m_propStorage->IsDisposed());

	if(name == nullptr) throw gcnew ArgumentNullException();
	if(value == nullptr) throw gcnew ArgumentNullException();
	if(m_readOnly) throw gcnew PropertySetReadOnlyException();

	// Make sure that the underlying implementation is going to be able to 
	// handle the type of object that has been provided here

	if(!IsValidPropertyDataType(value->GetType())) 
		throw gcnew InvalidPropertyDataTypeException(value->GetType());

	pinName = PtrToStringChars(name);				// Pin the string
	PropVariantInit(&varValue);						// Initialize the PROPVARIANT

	try {
		
		// Convert the new value into a VARIANT for the underyling storage
		// and set up the PROPSPEC information so we can add/change it

		Marshal::GetNativeVariantForObject(value, IntPtr(&varValue));
		
		propspec.ulKind = PRSPEC_LPWSTR;
		propspec.lpwstr = const_cast<LPWSTR>(pinName);

		// Attempt to insert/replace the VARIANT property value, and if
		// successful commit all property set changes to disk now

		hResult = m_propStorage->WriteMultiple(1, &propspec, &varValue,PROPERTYSET_BASEPROPID);
		if(FAILED(hResult)) throw gcnew StorageException(hResult, name);
		
		m_propStorage->Commit(STGC_DEFAULT);		// Commit changes
	}

	finally { PropVariantClear(&varValue); }		// Always release VARIANT
}

//---------------------------------------------------------------------------
// StoragePropertySet::default::set (integer)
//
// Inserts or replaces a property value
//
// Arguments:
//
//	index		- Index of the property to be added/replaced
//	value		- Object to use as the new property value

void StoragePropertySet::default::set(int index, Object^ value)
{
	CHECK_DISPOSED(m_disposed || m_propStorage->IsDisposed());

	if(value == nullptr) throw gcnew ArgumentNullException();
	this[LookupIndex(index)] = value;
}

//---------------------------------------------------------------------------
// StoragePropertySet::GenerateList (private)
//
// Generates a List<T> with the names and values of all the properties
//
// Arguments:
//
//	NONE

List<KeyValuePair<String^, Object^>>^ StoragePropertySet::GenerateList(void)
{
	IEnumSTATPROPSTG*			pEnumStg;			// Storage enumerator
	STATPROPSTG					statstg;			// Enumerated information
	PROPSPEC					propspec;			// PROPSPEC structure
	PROPVARIANT					value;				// Property value
	ULONG						ulRead;				// Number of items read
	HRESULT						hResult;			// Result from function call

	List<KeyValuePair<String^, Object^>>^	list;	// The generated list

	CHECK_DISPOSED(m_disposed || m_propStorage->IsDisposed());

	list = gcnew List<KeyValuePair<String^, Object^>>();	// Construct List<>

	// Attempt to grab the property set enumerator from the PropertySetStorage

	hResult = m_propStorage->Enum(&pEnumStg);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	// Just enumerate over all of the items in the property set and delete
	// them as we do.  According to MSDN, this is a legitimate way to do it

	try { 
	
		while(pEnumStg->Next(1, &statstg, &ulRead) == S_OK) {

			try {

				propspec.ulKind = PRSPEC_PROPID;			// Using a PROPID value
				propspec.propid = statstg.propid;			// Set the PROPID value

				// Attempt to read the current property value from the property set

				hResult = m_propStorage->ReadMultiple(1, &propspec, &value);
				if(FAILED(hResult)) throw gcnew StorageException(hResult, gcnew String(statstg.lpwstrName));

				try {

					// ReadMultiple() can return S_FALSE if the property didn't exist,
					// and even though that should never happen here, watch for it

					if(hResult == S_OK) list->Add(KeyValuePair<String^, Object^>(
						gcnew String(statstg.lpwstrName), Marshal::GetObjectForNativeVariant(IntPtr(&value))));
				}

				finally { PropVariantClear(&value); }	// Release the VARIANT
			}
			
			finally { if(statstg.lpwstrName) CoTaskMemFree(statstg.lpwstrName); }
		
		} // while
	} // try

	finally { pEnumStg->Release(); }			// Always release interface

	
	return list;
}

//---------------------------------------------------------------------------
// StoragePropertySet::GetEnumerator
//
// Retrieves an enumerator capable of reading all properties
//
// Arguments:
//
//	NONE

Generic::IEnumerator<KeyValuePair<String^, Object^>>^ StoragePropertySet::GetEnumerator(void)
{
	CHECK_DISPOSED(m_disposed || m_propStorage->IsDisposed());
	return GenerateList()->GetEnumerator();
}

//---------------------------------------------------------------------------
// StoragePropertySet::IsValidPropertyDataType (private)
//
// Determines if an object conforms to one of our pre-defined allowable
// property value data types or not
//
// Arguments:
//
//	pType		- Pointer to the object type in question

bool StoragePropertySet::IsValidPropertyDataType(Type^ type)
{
	if(type == Boolean::typeid) return true;
	else if(type == Byte::typeid) return true;
	else if(type == array<Byte>::typeid) return true;
	else if(type == Char::typeid) return true;
	else if(type == DateTime::typeid) return true;
	else if(type == Decimal::typeid) return true;
	else if(type == Double::typeid) return true;
	else if(type == Int16::typeid) return true;
	else if(type == Int32::typeid) return true;
	else if(type == Int64::typeid) return true;
	else if(type == SByte::typeid) return true;
	else if(type == Single::typeid) return true;
	else if(type == String::typeid) return true;
	else if(type == array<String^>::typeid) return true;
	else if(type == TimeSpan::typeid) return true;
	else if(type == UInt16::typeid) return true;
	else if(type == UInt32::typeid) return true;
	else if(type == UInt64::typeid) return true;

	return false;				// Nope.  Not something we can handle
}

//---------------------------------------------------------------------------
// StoragePropertySet::LookupIndex (private)
//
// Attempts to map an integer index value into a property name.  The index
// is always indicative of the order in which COM decides to enumerate
//
// Arguments:
//
//	index			- The index to be mapped into a string name

String^ StoragePropertySet::LookupIndex(int index)
{
	IEnumSTATPROPSTG*		pEnumStg;			// Storage enumerator
	STATPROPSTG				statstg;			// Enumerated information
	ULONG					ulRead;				// Number of items read
	HRESULT					hResult;			// Result from function call

	CHECK_DISPOSED(m_disposed || m_propStorage->IsDisposed());

	if(index < 0) throw gcnew ArgumentOutOfRangeException();

	// Attempt to grab the property set enumerator from the PropertySetStorage

	hResult = m_propStorage->Enum(&pEnumStg);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	try { 
	
		while(pEnumStg->Next(1, &statstg, &ulRead) == S_OK) {

			try {

				// If the index value has dropped to zero, this is the one we're 
				// looking for in the collection.  Otherwise decrement the index

				if(index == 0) { return gcnew String(statstg.lpwstrName); }
				else index--;
			}
			
			finally { if(statstg.lpwstrName) CoTaskMemFree(statstg.lpwstrName); }
		} 

		// If we run out of properties to enumerate, the index was beyond the
		// end of the property collection ...

		throw gcnew ArgumentOutOfRangeException();

	} // try

	finally { pEnumStg->Release(); }			// Always release interface
}

//---------------------------------------------------------------------------
// StoragePropertySet::Name::get
//
// Retrieves the current name of the property set

String^ StoragePropertySet::Name::get(void)
{
	CHECK_DISPOSED(m_disposed || m_propStorage->IsDisposed());
	CHECK_DISPOSED(m_parent->IsDisposed());

	return m_parent->PropertySetNameMapper->MapGuidToName(m_fmtid);
}

//---------------------------------------------------------------------------
// StoragePropertySet::Name::set
//
// Changes the name of the property set

void StoragePropertySet::Name::set(String^ value)
{
	CHECK_DISPOSED(m_disposed || m_propStorage->IsDisposed());
	CHECK_DISPOSED(m_parent->IsDisposed());

	if(value == nullptr) throw gcnew ArgumentNullException();
	if(m_readOnly) throw gcnew PropertySetReadOnlyException();
	
	m_parent->PropertySetNameMapper->RenameMapping(m_fmtid, value);
}

//---------------------------------------------------------------------------
// StoragePropertySet::Remove
//
// Removes a property from the property set
//
// Arguments:
//
//	name		- Name of the property to be removed

bool StoragePropertySet::Remove(String^ name)
{
	PROPSPEC			propspec;			// Property specification
	PinnedStringPtr		pinName;			// Pinned property name
	HRESULT				hResult;			// Result from function call

	CHECK_DISPOSED(m_disposed || m_propStorage->IsDisposed());

	if(name == nullptr) throw gcnew ArgumentNullException();
	if(m_readOnly) throw gcnew PropertySetReadOnlyException();

	// Convert the property name into an unmanaged Unicode string and set
	// up the values in the PROPSPEC structure appropriately

	pinName = PtrToStringChars(name);
	propspec.ulKind = PRSPEC_LPWSTR;
	propspec.lpwstr = const_cast<LPWSTR>(pinName);

	// Attempt to delete this property from the underlying storage, and
	// commit the changes to disk immediately if we succeed

	hResult = m_propStorage->DeleteMultiple(1, &propspec);
	if(SUCCEEDED(hResult)) m_propStorage->Commit(STGC_DEFAULT);

	return (SUCCEEDED(hResult));
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)
