#ifndef SCRIPTLANGUAGE_GENERICALLOCATORPOLICIES_HPP
#define SCRIPTLANGUAGE_GENERICALLOCATORPOLICIES_HPP
//---------------------------------------------------------------------------
#include "GenericAllocatorPolicies.hpp"
#include <cassert>
#include <stdint.h>
//---------------------------------------------------------------------------
// Harriet Script Language
// Copyright (c) 2012, 2013 Alexander van Renen (alexandervanrenen@gmail.com)
// See the file LICENSE.txt for copying permission.
//---------------------------------------------------------------------------
namespace harriet {
//---------------------------------------------------------------------------
/// host class => a customizable allocator
template<class T, // class which should be allocated
         template <class Type> class Allocator = StdAllocatorPolicy> // how should the memory be managed
class GenericAllocator : public Allocator<T> {
public:

   static void* operator new(std::size_t size) throw(std::bad_alloc);

   static void operator delete(void* data, std::size_t size) throw();

protected:
   ~GenericAllocator() {} // protected so no one can delete the derived class by the base pointer
};
//---------------------------------------------------------------------------
/// new operator of host class, combining the policies
template<class T, template <class> class Allocator>
void* GenericAllocator<T, Allocator>::operator new(std::size_t size) throw(std::bad_alloc)
{
   return Allocator<T>::allocate(size);
}
//---------------------------------------------------------------------------
/// delete operator of host class, combining the policies
template<class T, template <class Type> class Allocator>
void GenericAllocator<T, Allocator>::operator delete(void* data, std::size_t size) throw()
{
   Allocator<T>::deallocate(data, size);
}
//---------------------------------------------------------------------------
} // end of namesapce scriptlanguage
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
