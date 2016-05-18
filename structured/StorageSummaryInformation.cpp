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
#include "StorageSummaryInformation.h"	// Include StorageSummaryInformation decls
#include <initguid.h>					// Include DEFINE_GUID support

#pragma warning(push, 4)				// Enable maximum compiler warnings

BEGIN_ROOT_NAMESPACE(zuki.storage)

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------

// FMTID_SUMMARYINFORMATION {F29F85E0-4FF9-1068-AB91-08002B27B3D9}
//
// Replaces the real FMTID_SummaryInformation GUID so that this code can
// be compiled in /clr:pure mode without unresolved external errors
DEFINE_GUID(FMTID_SUMMARYINFORMATION,
0xf29f85e0L, 0x4ff9, 0x1068, 0xab, 0x91, 0x08, 0x00, 0x2b, 0x27, 0xb3, 0xd9);

//---------------------------------------------------------------------------
// StorageSummaryInformation Constructor (internal)
//
// Arguments:
//
//	storage		- Pointer to the root IComPropertySetStorage instance

StorageSummaryInformation::StorageSummaryInformation(IComPropertySetStorage^ storage) : 
	m_pPropertyStorage(NULL)
{
	IPropertyStorage*			pPropStorage;		// Local IPropertyStg interface
	HRESULT						hResult;			// Result from function call

	if(storage == nullptr) throw gcnew ArgumentNullException();

	m_readOnly = StorageUtil::IsStorageReadOnly(safe_cast<IComStorage^>(storage));

	// First try to open an existing property set with the FMTID from
	// the current IStorage object ...

	hResult = storage->Open(FMTID_SUMMARYINFORMATION, ((m_readOnly) ? 
		STGM_READ : STGM_READWRITE) | STGM_SHARE_EXCLUSIVE, &pPropStorage);

	// OK, that didn't work, so let's try to create it instead ...

	if((hResult == STG_E_FILENOTFOUND) && (!m_readOnly)) {
	
		hResult = storage->Create(FMTID_SUMMARYINFORMATION, NULL, PROPSETFLAG_DEFAULT, 
			STGM_READWRITE | STGM_SHARE_EXCLUSIVE, &pPropStorage);
	}

	// Failure to open or create the summary information set is fatal

	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	m_pPropertyStorage = pPropStorage;		// Copy into the __gc mvar
}

//---------------------------------------------------------------------------
// StorageSummaryInformation Finalizer

StorageSummaryInformation::!StorageSummaryInformation(void)
{
	// Release local COM interface pointers and reset them all to NULL

	if(m_pPropertyStorage) m_pPropertyStorage->Release();
	m_pPropertyStorage = NULL;
}

//---------------------------------------------------------------------------
// StorageSummaryInformation::GetFileTimeValue (private)
//
// Retrieves a FILETIME from the contained property set (if available)
//
// Arguments:
//
//	propid			- The special PROPID value for this item

DateTime StorageSummaryInformation::GetFileTimeValue(PROPID propid)
{
	HRESULT				hResult;			// Result from function call
	PROPSPEC			propspec;			// PROPSPEC structure
	PULARGE_INTEGER		puli;				// Pointer to a ULARGE_INTEGER
	PROPVARIANT			varValue;			// PROPVARIANT value

	CHECK_DISPOSED(m_disposed);

	if(!m_pPropertyStorage) return DateTime(0);	// <--- PROPSET NOT AVAILABLE

	propspec.ulKind = PRSPEC_PROPID;		// Using the PROPID
	propspec.propid = propid;				// Set up the PROPID

	// Attempt to read the value from the property set, and just return
	// a default value if it cannot be retrieved

	hResult = m_pPropertyStorage->ReadMultiple(1, &propspec, &varValue);
	if(FAILED(hResult)) return DateTime(0);

	// Convert the FILETIME into an unsigned 64 bit integer and then
	// convert it into a DateTime structure for the caller

	puli = reinterpret_cast<PULARGE_INTEGER>(&varValue.filetime);
	return DateTime::FromFileTimeUtc(puli->QuadPart);
}

//---------------------------------------------------------------------------
// StorageSummaryInformation::GetIntegerValue (private)
//
// Retrieves a 32bit integer from the contained property set (if available)
//
// Arguments:
//
//	propid			- The special PROPID value for this item

int StorageSummaryInformation::GetIntegerValue(PROPID propid)
{
	HRESULT				hResult;			// Result from function call
	PROPSPEC			propspec;			// PROPSPEC structure
	PROPVARIANT			varValue;			// PROPVARIANT value

	CHECK_DISPOSED(m_disposed);

	if(!m_pPropertyStorage) return 0;		// <--- PROPSET NOT AVAILABLE

	propspec.ulKind = PRSPEC_PROPID;		// Using the PROPID
	propspec.propid = propid;				// Set up the PROPID

	// Attempt to read the value from the property set, and just return
	// a default value if it cannot be retrieved

	hResult = m_pPropertyStorage->ReadMultiple(1, &propspec, &varValue);
	if(FAILED(hResult)) return 0;

	return varValue.lVal;					// Return the integer value
}

//---------------------------------------------------------------------------
// StorageSummaryInformation::GetStringValue (private)
//
// Retrieves a string from the contained property set (if available)
//
// Arguments:
//
//	propid			- The special PROPID value for this item

String^ StorageSummaryInformation::GetStringValue(PROPID propid)
{
	HRESULT				hResult;			// Result from function call
	PROPSPEC			propspec;			// PROPSPEC structure
	PROPVARIANT			varValue;			// PROPVARIANT value

	CHECK_DISPOSED(m_disposed);

	if(!m_pPropertyStorage) return String::Empty;	// <--- PROPSET NOT AVAILABLE

	propspec.ulKind = PRSPEC_PROPID;		// Using the PROPID
	propspec.propid = propid;				// Set up the PROPID

	// Attempt to read the value from the property set, and just return
	// a default value if it cannot be retrieved

	hResult = m_pPropertyStorage->ReadMultiple(1, &propspec, &varValue);
	if(FAILED(hResult)) return String::Empty;

	// Convert the ANSI property string into a managed String object,
	// and make sure to release the PROPVARIANT no matter what happens

	try { return gcnew String(varValue.pszVal); }
	finally { PropVariantClear(&varValue); }
}

//---------------------------------------------------------------------------
// StorageSummaryInformation::GetTimeSpanValue (private)
//
// Retrieves a FILETIME (timespan) from the contained property set (if available)
//
// Arguments:
//
//	propid			- The special PROPID value for this item

TimeSpan StorageSummaryInformation::GetTimeSpanValue(PROPID propid)
{
	HRESULT				hResult;			// Result from function call
	PROPSPEC			propspec;			// PROPSPEC structure
	PULARGE_INTEGER		puli;				// Pointer to a ULARGE_INTEGER
	PROPVARIANT			varValue;			// PROPVARIANT value

	CHECK_DISPOSED(m_disposed);

	if(!m_pPropertyStorage) return TimeSpan(0);	// <--- PROPSET NOT AVAILABLE

	propspec.ulKind = PRSPEC_PROPID;		// Using the PROPID
	propspec.propid = propid;				// Set up the PROPID

	// Attempt to read the value from the property set, and just return
	// a default value if it cannot be retrieved

	hResult = m_pPropertyStorage->ReadMultiple(1, &propspec, &varValue);
	if(FAILED(hResult)) return TimeSpan(0);

	// Convert the FILETIME into an unsigned 64 bit integer and then
	// convert it into a DateTime structure for the caller

	puli = reinterpret_cast<PULARGE_INTEGER>(&varValue.filetime);
	return TimeSpan(puli->QuadPart);
}

//---------------------------------------------------------------------------
// StorageSummaryInformation::InternalDispose (internal)
//
// Behaves as a pseudo-destructor for the class so we can implement a 
// finalizer without having to expose a .Dispose() method publically
//
// Arguments:
//
//	NONE

void StorageSummaryInformation::InternalDispose(void)
{
	if(m_disposed) return;		// Class has already been disposed of

	this->!StorageSummaryInformation();		// Invoke the finalizer
	GC::SuppressFinalize(this);				// Suppress finalization
	m_disposed = true;						// Object is now disposed
}

//---------------------------------------------------------------------------
// StorageSummaryInformation::SetFileTimeValue (private)
//
// Changes the value of a FILETIME in the property set
//
// Arguments:
//
//	propid			- PROPID of the property to be changed
//	value			- DateTime value to set the property to

void StorageSummaryInformation::SetFileTimeValue(PROPID propid, DateTime value)
{
	HRESULT				hResult;			// Result from function call
	PROPSPEC			propspec;			// PROPSPEC structure
	PULARGE_INTEGER		puli;				// Pointer to a ULARGE_INTEGER
	PROPVARIANT			varValue;			// PROPVARIANT value

	CHECK_DISPOSED(m_disposed);
	if(m_readOnly) throw gcnew InvalidOperationException("StorageSummaryInfo is read-only");

	if(!m_pPropertyStorage) return;			// <--- PROPERTY SET NOT AVAILABLE

	propspec.ulKind = PRSPEC_PROPID;		// Using the PROPID
	propspec.propid = propid;				// Set up the PROPID

	// Initialize the PROPVARIANT with the FILETIME information

	varValue.vt = VT_FILETIME;
	puli = reinterpret_cast<PULARGE_INTEGER>(&varValue.filetime);
	puli->QuadPart = value.ToFileTime();

	// Attempt to write the new value into the property set.  Unlike the
	// GetValue() method, we actually throw an exception if this fails

	hResult = m_pPropertyStorage->WriteMultiple(1, &propspec, &varValue, 0);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	m_pPropertyStorage->Commit(STGC_DEFAULT);		// Commit the changes to disk

	// NOTE: DO NOT CALL PropVariantClear
}

//---------------------------------------------------------------------------
// StorageSummaryInformation::SetIntegerValue (private)
//
// Changes the value of a 32bit integer in the property set
//
// Arguments:
//
//	propid			- PROPID of the property to be changed
//	value			- TimeSpan value to set the property to

void StorageSummaryInformation::SetIntegerValue(PROPID propid, int value)
{
	HRESULT				hResult;			// Result from function call
	PROPSPEC			propspec;			// PROPSPEC structure
	PROPVARIANT			varValue;			// PROPVARIANT value

	CHECK_DISPOSED(m_disposed);
	if(m_readOnly) throw gcnew InvalidOperationException("StorageSummaryInfo is read-only");

	if(!m_pPropertyStorage) return;			// <--- PROPERTY SET NOT AVAILABLE

	propspec.ulKind = PRSPEC_PROPID;		// Using the PROPID
	propspec.propid = propid;				// Set up the PROPID

	varValue.vt = VT_I4;					// Passing in a VT_I4
	varValue.lVal = value;					// Set the VT_I4 value

	// Attempt to write the new value into the property set.  Unlike the
	// GetValue() method, we actually throw an exception if this fails

	hResult = m_pPropertyStorage->WriteMultiple(1, &propspec, &varValue, 0);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);
	
	m_pPropertyStorage->Commit(STGC_DEFAULT);	// Commit the changes to disk

	// NOTE: DO NOT CALL PropVariantClear
}

//---------------------------------------------------------------------------
// StorageSummaryInformation::SetStringValue (private)
//
// Changes the value of a string in the property set
//
// Arguments:
//
//	propid			- PROPID of the property to be changed
//	value			- Value to set the property to

void StorageSummaryInformation::SetStringValue(PROPID propid, String^ value)
{
	HRESULT				hResult;			// Result from function call
	PROPSPEC			propspec;			// PROPSPEC structure
	PROPVARIANT			varValue;			// PROPVARIANT value
	IntPtr				ptValue;			// Unmanaged string buffer

	CHECK_DISPOSED(m_disposed);
	if(m_readOnly) throw gcnew InvalidOperationException("StorageSummaryInfo is read-only");

	if(!m_pPropertyStorage) return;			// <--- PROPERTY SET NOT AVAILABLE

	propspec.ulKind = PRSPEC_PROPID;		// Using the PROPID
	propspec.propid = propid;				// Set up the PROPID

	ptValue = Marshal::StringToHGlobalAnsi(value);		// Convert the string

	// Initialize the PROPVARIANT with the pointer to the ANSI string

	varValue.vt = VT_LPSTR;
	varValue.pszVal = reinterpret_cast<LPSTR>(ptValue.ToPointer());

	try {
	
		// Attempt to write the new value into the property set.  Unlike the
		// GetValue() method, we actually throw an exception if this fails

		hResult = m_pPropertyStorage->WriteMultiple(1, &propspec, &varValue, 0);
		if(FAILED(hResult)) throw gcnew StorageException(hResult);
	
		m_pPropertyStorage->Commit(STGC_DEFAULT);	// Commit the changes to disk
	}

	// NOTE: DO NOT CALL PropVariantClear!  It might try to release the
	// unmanaged string buffer that we allocated with Marshal::StringToHGlobal

	finally { Marshal::FreeHGlobal(ptValue); }
}

//---------------------------------------------------------------------------
// StorageSummaryInformation::SetTimeSpanValue (private)
//
// Changes the value of a FILETIME (timespan) in the property set
//
// Arguments:
//
//	propid			- PROPID of the property to be changed
//	value			- TimeSpan value to set the property to

void StorageSummaryInformation::SetTimeSpanValue(PROPID propid, TimeSpan value)
{
	HRESULT				hResult;			// Result from function call
	PROPSPEC			propspec;			// PROPSPEC structure
	PULARGE_INTEGER		puli;				// Pointer to a ULARGE_INTEGER
	PROPVARIANT			varValue;			// PROPVARIANT value

	CHECK_DISPOSED(m_disposed);
	if(m_readOnly) throw gcnew InvalidOperationException("StorageSummaryInfo is read-only");

	if(!m_pPropertyStorage) return;			// <--- PROPERTY SET NOT AVAILABLE

	propspec.ulKind = PRSPEC_PROPID;		// Using the PROPID
	propspec.propid = propid;				// Set up the PROPID

	// Initialize the PROPVARIANT with the FILETIME information

	varValue.vt = VT_FILETIME;
	puli = reinterpret_cast<PULARGE_INTEGER>(&varValue.filetime);
	puli->QuadPart = value.Ticks;

	// Attempt to write the new value into the property set.  Unlike the
	// GetValue() method, we actually throw an exception if this fails

	hResult = m_pPropertyStorage->WriteMultiple(1, &propspec, &varValue, 0);
	if(FAILED(hResult)) throw gcnew StorageException(hResult);

	m_pPropertyStorage->Commit(STGC_DEFAULT);	// Commit the changes to disk

	// NOTE: DO NOT CALL PropVariantClear
}

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki.storage)

#pragma warning(pop)
