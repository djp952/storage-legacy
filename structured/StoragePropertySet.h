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

#ifndef __STORAGEPROPERTYSET_H_
#define __STORAGEPROPERTYSET_H_
#pragma once

#include "ComPropertyStorage.h"				// Include ComPropertyStorage decls
#include "ComStorage.h"						// Include ComStorage declarations
#include "StorageException.h"				// Include StorageException declarations
#include "StorageExceptions.h"				// Include exception declarations

#pragma warning(push, 4)					// Enable maximum compiler warnings

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

BEGIN_NAMESPACE(zuki)
BEGIN_NAMESPACE(storage)
BEGIN_NAMESPACE(structured)

//---------------------------------------------------------------------------
// Class StoragePropertySet
//
// StoragePropertySet implements a property set (IPropertySet) that is part
// of the StoragePropertySetCollection that hangs off of every container.
//
// A collection in and of itself, a property set allows the user to store
// simple scalar values in the parent container.  Not all object types can
// be accomodated, only those that directly correspond to VARIANTs.
//---------------------------------------------------------------------------

STRUCTURED_STORAGE_PUBLIC ref class StoragePropertySet : public Generic::ICollection<KeyValuePair<String^, Object^>>
{
public:

	//-----------------------------------------------------------------------
	// IEnumerable<T> implementation

	virtual Generic::IEnumerator<KeyValuePair<String^, Object^>>^ GetEnumerator(void);

	//-----------------------------------------------------------------------
	// ICollection<T> implementation

	virtual void	Add(String^ name, Boolean value)		{ AddItem(name, value); }
	virtual void	Add(String^ name, Byte value)			{ AddItem(name, value); }
	virtual void	Add(String^ name, array<Byte>^ value)	{ AddItem(name, value); }
	virtual void	Add(String^ name, Char value)			{ AddItem(name, value); }
	virtual void	Add(String^ name, DateTime value)		{ AddItem(name, value); }
	virtual void	Add(String^ name, Decimal value)		{ AddItem(name, value); }
	virtual void	Add(String^ name, Double value)			{ AddItem(name, value); }
	virtual void	Add(String^ name, Int16 value)			{ AddItem(name, value); }
	virtual void	Add(String^ name, Int32 value)			{ AddItem(name, value); }
	virtual void	Add(String^ name, Int64 value)			{ AddItem(name, value); }
	virtual void	Add(String^ name, SByte value)			{ AddItem(name, value); }
	virtual void	Add(String^ name, Single value)			{ AddItem(name, value); }
	virtual void	Add(String^ name, String^ value)		{ AddItem(name, value); }
	virtual void	Add(String^ name, array<String^>^ value){ AddItem(name, value); }
	virtual void	Add(String^ name, TimeSpan value)		{ AddItem(name, value); }
	virtual void	Add(String^ name, UInt16 value)			{ AddItem(name, value); }
	virtual void	Add(String^ name, UInt32 value)			{ AddItem(name, value); }
	virtual void	Add(String^ name, UInt64 value)			{ AddItem(name, value); }

	virtual void	Clear(void);
	virtual bool	Contains(String^ name);
	virtual void	CopyTo(array<KeyValuePair<String^, Object^>>^ target, int index);
	virtual bool	Remove(String^ name);

	virtual property int  Count { int get(void); }
	virtual property bool IsReadOnly { bool get(void) { return m_readOnly; } }

	//-----------------------------------------------------------------------
	// Properties

	property Object^	default[String^]	{ Object^ get(String^); void set(String^, Object^); }
	property Object^	default[int]		{ Object^ get(int);		void set(int, Object^); }
	property String^	Name				{ String^ get(void);	void set(String^); }

	property bool		ReadOnly			{ bool get(void) { return m_readOnly; } }

internal:

	// INTERNAL CONSTRUCTOR
	StoragePropertySet(ComStorage^ parent, ComPropertyStorage^ propStorage);

	//-----------------------------------------------------------------------
	// Constants

	literal PROPID PROPERTYSET_BASEPROPID = 255;	// Base property ID code

	//-----------------------------------------------------------------------
	// Internal Properties

	// PropertySetID
	//
	// Exposes the GUID associated with this storage property set
	property Guid PropertySetID 
	{ 
		Guid get(void) { return m_fmtid; } 
	}

private:

	//-----------------------------------------------------------------------
	// Private Member Functions

	void		AddItem(String^, Object^);
	bool		IsValidPropertyDataType(Type^ type);
	String^		LookupIndex(int index);

	List<KeyValuePair<String^, Object^>>^ GenerateList(void);

	// ICollection<T>::Add
	virtual void Add(KeyValuePair<String^, Object^>) sealed =
		Generic::ICollection<KeyValuePair<String^, Object^>>::Add 
		{ throw gcnew NotImplementedException(); }

	// ICollection<T>::Contains
	virtual bool Contains(KeyValuePair<String^, Object^>) sealed = 
		Generic::ICollection<KeyValuePair<String^, Object^>>::Contains 
		{ throw gcnew NotImplementedException(); }

	// ICollection<T>::Remove
	virtual bool Remove(KeyValuePair<String^, Object^>) sealed =
		Generic::ICollection<KeyValuePair<String^, Object^>>::Remove 
		{ throw gcnew NotImplementedException(); }

	// IEnumerator::GetEnumerator
	virtual Collections::IEnumerator^ _GetEnumerator(void) sealed = 
		Collections::IEnumerable::GetEnumerator { return GetEnumerator(); }

	//-----------------------------------------------------------------------
	// Member Variables

	bool						m_disposed;			// Object disposal flag
	ComStorage^					m_parent;			// Parent ComStorage
	ComPropertyStorage^			m_propStorage;		// Contained ComPropertyStorage
	bool						m_readOnly;			// Read-Only flag
	Guid						m_fmtid;			// Property set FMTID
};

//---------------------------------------------------------------------------

END_NAMESPACE(structured)
END_NAMESPACE(storage)
END_NAMESPACE(zuki)

#pragma warning(pop)

#endif	// __STORAGEPROPERTYSET_H_
