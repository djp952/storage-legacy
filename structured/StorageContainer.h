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

#ifndef __STORAGECONTAINER_H_
#define __STORAGECONTAINER_H_
#pragma once

#include "ComStorage.h"						// Include ComStorage declarations
#include "StorageContainerCollection.h"		// Include StorageContainerCollection decls
#include "StorageException.h"				// Include StorageException declarations
#include "StorageExceptions.h"				// Include exception declarations
#include "StorageObjectCollection.h"		// Include StorageObjectCollection decls
#include "StoragePropertySetCollection.h"	// Include StoragePropertySetCollection

#pragma warning(push, 4)					// Enable maximum compiler warnings

using namespace System;

BEGIN_ROOT_NAMESPACE(zuki::storage)

//---------------------------------------------------------------------------
// Forward Class Declarations
//
// Include the specified header files in the .CPP file for this class
//---------------------------------------------------------------------------

ref class StructuredStorage;				// <-- StructuredStorage.h

//---------------------------------------------------------------------------
// Class StorageContainer
//
// StorageContainer implements a container object (IStorage).  Every container
// can contain objects (Streams), property sets, and sub container objects.
//---------------------------------------------------------------------------

STRUCTURED_STORAGE_PUBLIC ref class StorageContainer
{
public:

	//-----------------------------------------------------------------------
	// Properties

	property String^ Name
	{
		String^ get(void);
		void set(String^ value);
	}

	property bool ReadOnly { bool get(void) { return m_readOnly; } }

	property StorageContainerCollection^ Containers { StorageContainerCollection^ get(void); }
	property StorageObjectCollection^ Objects { StorageObjectCollection^ get(void); }
	property StoragePropertySetCollection^ PropertySets { StoragePropertySetCollection^ get(void); }

internal:

	// INTERNAL CONSTRUCTOR
	StorageContainer(StructuredStorage^ root, ComStorage^ parent, ComStorage^ storage);

	//-----------------------------------------------------------------------
	// Internal Properties

	// ContainerID
	//
	// Exposes the object identifier GUID for this storage container
	property Guid ContainerID
	{
		Guid get(void) { return m_contid; }
	}

	// IsRoot
	//
	// Exposes if this container happens to be the root container
	property bool IsRoot
	{
		bool get(void) { return m_contid == Guid::Empty; }
	}

private:

	//-----------------------------------------------------------------------
	// Private Constants

	literal String^ CONTAINER_NAME_ROOT = gcnew String("StorageRoot");

	//-----------------------------------------------------------------------
	// Member Variables

	StructuredStorage^				m_root;				// Root storage object
	ComStorage^						m_parent;			// Parent storage instance
	ComStorage^						m_storage;			// This storage instance
	bool							m_readOnly;			// Read-Only flag
	Guid							m_contid;			// Container ID GUID
	StorageContainerCollection^		m_containers;		// Containers collection
	StorageObjectCollection^		m_objects;			// Objects collection
	StoragePropertySetCollection^	m_propsets;			// PropSets collection
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::storage)

#pragma warning(pop)

#endif	// __STORAGECONTAINER_H_
