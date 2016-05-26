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

#ifndef __VIRTUALDISKMETADATAENUMERATOR_H_
#define __VIRTUALDISKMETADATAENUMERATOR_H_
#pragma once

#pragma warning(push, 4)				// Enable maximum compiler warnings

using namespace System;
using namespace System::Collections::Generic;
using namespace System::ComponentModel;

BEGIN_ROOT_NAMESPACE(zuki::tools::llvm::clang)

// FORWARD DECLARATIONS
//
ref class VirtualDiskSafeHandle;

//---------------------------------------------------------------------------
// Class VirtualDiskMetadataEnumerator (internal)
//
// Implements an enumerator for VirtualDiskMetadataCollection
//---------------------------------------------------------------------------

ref class VirtualDiskMetadataEnumerator : public IEnumerator<KeyValuePair<Guid, array<Byte>^>>
{
public:

	// Instance Constructor
	//
	VirtualDiskMetadataEnumerator(VirtualDiskSafeHandle^ handle, IList<Guid>^ keys);

	//-----------------------------------------------------------------------
	// Member Functions

	// MoveNext
	//
	// Advances the enumerator to the next element of the collection
	virtual bool MoveNext(void);

	// Reset
	//
	// Sets the enumerator to its initial position 
	virtual void Reset(void);

	//-----------------------------------------------------------------------
	// Properties

	// Current
	//
	// Gets the current element in the collection
	property KeyValuePair<Guid, array<Byte>^> Current
	{
		virtual KeyValuePair<Guid, array<Byte>^> get(void);
	}

private:

	// Destructor
	//
	~VirtualDiskMetadataEnumerator();

	//-----------------------------------------------------------------------
	// Private Properties

	// IEnumerator_Current (IEnumerator)
	//
	// Gets the current element in the collection as an untyped Object^
	property Object^ IEnumerator_Current
	{
		virtual Object^ get(void) sealed = System::Collections::IEnumerator::Current::get;
	}

	//-----------------------------------------------------------------------
	// Member Variables

	VirtualDiskSafeHandle^	m_handle;		// Virtual disk safe handle instance
	IList<Guid>^			m_keys;			// Collection of metadata keys
	bool					m_disposed;		// Object disposal flag
	int						m_index = -1;	// Current index into collection
};

//---------------------------------------------------------------------------

END_ROOT_NAMESPACE(zuki::tools::llvm::clang)

#pragma warning(pop)

#endif	// __VIRTUALDISKMETADATAENUMERATOR_H_
