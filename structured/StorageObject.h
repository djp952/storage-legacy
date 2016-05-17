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

#ifndef __STORAGEOBJECT_H_
#define __STORAGEOBJECT_H_
#pragma once

#include "ComStorage.h"					// Include ComStorage declarations
#include "ComStream.h"					// Include ComStream declarationss
#include "StorageException.h"			// Include StorageException declarations
#include "StorageExceptions.h"			// Include exception declarations
#include "StorageObjectStream.h"		// Include StorageObjectStream declarations
#include "StorageObjectReader.h"		// Include StorageObjectReader decls
#include "StorageObjectWriter.h"		// Include StorageObjectWriter decls

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::IO;
using namespace System::Runtime::Serialization::Formatters::Binary;

BEGIN_NAMESPACE(zuki)
BEGIN_NAMESPACE(storage)
BEGIN_NAMESPACE(structured)

//---------------------------------------------------------------------------
// Forward Class Declarations
//
// Include the specified header files in the .CPP file for this class
//---------------------------------------------------------------------------

ref class StructuredStorage;				// <-- StructuredStorage.h
ref class StorageContainer;					// <-- StorageContainer.h

//---------------------------------------------------------------------------
// Class StorageObject
//
// StorageObject implements an object stream (IStream)
//---------------------------------------------------------------------------

STRUCTURED_STORAGE_PUBLIC ref class StorageObject sealed
{
public:

	//-----------------------------------------------------------------------
	// Methods

	// NOTE: CopyTo and MoveTo disappeared when the COM pointers were
	// refactored.  Now the application has to do those things on it's own

	StorageObjectReader^	GetReader(void);
	StorageObjectWriter^	GetWriter(void);

	//-----------------------------------------------------------------------
	// Properties

	property array<Byte>^ Data
	{
		array<Byte>^ get(void);
		void set(array<Byte>^ value);
	}

	property String^ Name
	{
		String^ get(void);
		void set(String^ value);
	}

	property bool ReadOnly { bool get(void) { return m_readOnly; } }

internal:

	// INTERNAL CONSTRUCTOR
	StorageObject(StructuredStorage^ root, ComStorage^ parent, ComStream^ stream);

	//-----------------------------------------------------------------------
	// Internal Properties

	// ObjectID
	//
	// Exposes the object identifier GUID for this storage object
	property Guid ObjectID
	{
		Guid get(void) { return m_objid; }
	}

private:

	//-----------------------------------------------------------------------
	// Member Variables

	StructuredStorage^			m_root;				// Root storage object
	ComStorage^					m_parent;			// Parent ComStorage instance
	ComStream^					m_stream;			// Contained stream instance
	bool						m_readOnly;			// Read-Only flag
	Guid						m_objid;			// Object ID GUID
};

//---------------------------------------------------------------------------

END_NAMESPACE(structured)
END_NAMESPACE(storage)
END_NAMESPACE(zuki)

#pragma warning(pop)

#endif	// __STORAGEOBJECT_H_
