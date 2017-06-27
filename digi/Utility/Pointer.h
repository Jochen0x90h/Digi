#ifndef digi_Utility_Pointer_h
#define digi_Utility_Pointer_h

#include <assert.h>
#include <typeinfo>

#include "Standard.h"


namespace digi {

/// @addtogroup Meta
/// @{


/*
#ifdef CHECK_NULL
	void ThrowNullError(); // todo: pass rtti-info
#endif
*/


template <typename Type>
class Pointer;

template <typename Type>
void swap(Pointer<Type>& a, Pointer<Type>& b);


/**
	smart pointer (also works for COM objects)
*/
template <typename Type>
class Pointer
{
	friend void swap<>(Pointer<Type>& a, Pointer<Type>& b);

public:

	/// constructor, initializes to null
	Pointer() 
		: ptr(NULL) {}

	/// constructor, initializes to null
	Pointer(NullType) 
		: ptr(NULL) {}

	///	constructs the pointer with a smart pointer of other type
	template <typename Type2>
	Pointer(const Pointer<Type2> &smartPtr) 
		: ptr(smartPtr.getPointer())
	{
		addReference(this->ptr);
	}                                                    			

	/// copy constructor
	Pointer(const Pointer<Type> &smartPtr) 
		: ptr(smartPtr.ptr)
	{
		addReference(this->ptr);
	}

	/// constructs the poiter with a c-style pointer of other type
	Pointer(Type *ptr) 
		: ptr(ptr)
	{
		addReference(this->ptr);
	}

	/// destructor calls release() on the object
	~Pointer()
	{
		release(this->ptr);
	}

	/// assigns null
	Pointer<Type> &operator =(NullType)
	{
		release(this->ptr);

		this->ptr = NULL;

		return *this;
	}
	
	/// assigns a smart pointer of other type to the pointer.
	template <typename Type2>
	Pointer<Type>& operator =(const Pointer<Type2>& smartPtr)
	{
		Type *ptr = smartPtr.getPointer();
		addReference(ptr);
		
		release(this->ptr);

		this->ptr = ptr;

		return *this;
	}

	/// assigns a smpart pointer to the pointer.
	Pointer<Type>& operator =(const Pointer<Type>& smartPtr)
	{
		Type *ptr = smartPtr.ptr;
		addReference(ptr);
		
		release(this->ptr);

		this->ptr = ptr;

		return *this;
	}

	/// assigns a c-style pointer to the pointer.
	Pointer<Type>& operator =(Type* ptr) 
	{
		addReference(ptr);
		
		release(this->ptr);

		this->ptr = ptr;

		return *this;
	}

// compare
		
	/// compares to null
	bool operator ==(NullType) const
	{
		return this->ptr == NULL;
	}

	/// compares to null
	bool operator !=(NullType) const
	{
		return this->ptr != NULL;
	}

	/// less operator
	bool operator <(const Pointer<Type>& other) const
	{
		return this->ptr < other.ptr;
	}

	/**
		dereferences the smartpointer just like a normal pointer. 
		Throws null pointer exception if CHECK_POINTERS is defined
	*/
	Type& operator *() const
	{
		#ifdef CHECK_NULL
			if (this->ptr == NULL)
				throwNullError();
		#endif

		return *this->ptr;
	}

	/// access operator. Throws null pointer exception if CHECK_POINTERS is defined
	Type *operator ->() const
	{
		#ifdef CHECK_NULL
			if (this->ptr == NULL)
				throwNullError();
		#endif

		return this->ptr;
	}
	/*
	operator bool() const
	{
		return this->ptr != NULL;
	}*/
/*
does not work on Xcode
	Type** operator &()
	{
		if (this->ptr != NULL)
		{
			this->ptr->release();
			this->ptr = NULL;
		}
		
		return &this->ptr;
	}
*/
	/// returns a c-style pointer to the object
	Type* getPointer() const
	{
		return this->ptr;
	}

	/// auto-convert to c-style pointer
	operator Type* () const
	{
		return this->ptr;
	}

protected:
	
	Type *ptr;
};

template <typename Type>
void swap(Pointer<Type> &a, Pointer<Type> &b)
{
	Type* ptr = a.ptr;
	a.ptr = b.ptr;
	b.ptr = ptr;
}



/// static cast, type check only at compile time or in debug mode
template <typename DstType, typename SrcType>
inline Pointer<DstType> staticCast(Pointer<SrcType> p)
{
	assert(dynamic_cast<DstType*>(p.getPointer()) != NULL && "staticCast failed");
	return Pointer<DstType>(static_cast<DstType*>(p.getPointer()));
}

template <typename DstType, typename SrcType>
inline Pointer<DstType> staticCast(SrcType* p)
{
	assert(dynamic_cast<DstType*>(p) != NULL && "staticCast failed");
	return Pointer<DstType>(static_cast<DstType*>(p));
}

/// dynamic cast, returns null if the types are incompatible
template <typename DstType, typename SrcType>
inline Pointer<DstType> dynamicCast(Pointer<SrcType> p)
{
	return dynamic_cast<DstType*>(p.getPointer());
}

template <typename DstType, typename SrcType>
inline Pointer<DstType> dynamicCast(SrcType* p)
{
	return dynamic_cast<DstType*>(p);
}

class BadCast : public std::bad_cast 
{
public:
	virtual ~BadCast() throw();
	virtual const char* what() const throw();
};

/// dynamic cast, throws std::bad_cast if the types are incompatible
template <typename DstType, typename SrcType>
inline Pointer<DstType> cast(Pointer<SrcType> p)
{
	DstType* casted = dynamic_cast<DstType*>(p.getPointer());
	if (casted == NULL)
		throw BadCast();
	return Pointer<DstType>(casted);
}

/// returns true if DstType is a SrcType
template <typename DstType, typename SrcType>
inline bool isa(Pointer<SrcType> p)
{
	return dynamic_cast<DstType*>(p.getPointer()) != NULL;
}


/// @}

} // namespace digi

#endif
