// Copyright 2018 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_ROOTS_INL_H_
#define V8_ROOTS_INL_H_

#include "src/roots.h"

#include "src/handles.h"
#include "src/heap/heap-inl.h"

namespace v8 {
namespace internal {

V8_INLINE constexpr bool operator<(RootIndex lhs, RootIndex rhs) {
  typedef typename std::underlying_type<RootIndex>::type type;
  return static_cast<type>(lhs) < static_cast<type>(rhs);
}

V8_INLINE RootIndex operator++(RootIndex& index) {
  typedef typename std::underlying_type<RootIndex>::type type;
  index = static_cast<RootIndex>(static_cast<type>(index) + 1);
  return index;
}

template <typename T>
bool RootsTable::IsRootHandle(Handle<T> handle, RootIndex* index) const {
  Object** handle_location = bit_cast<Object**>(handle.address());
  return IsRootHandleLocation(handle_location, index);
}

ReadOnlyRoots::ReadOnlyRoots(Heap* heap)
    : roots_table_(heap->isolate()->roots_table()) {}

ReadOnlyRoots::ReadOnlyRoots(Isolate* isolate)
    : roots_table_(isolate->roots_table()) {}

// TODO(jkummerow): Drop std::remove_pointer after the migration to ObjectPtr.
#define ROOT_ACCESSOR(Type, name, CamelName)                             \
  Type ReadOnlyRoots::name() const {                                     \
    return std::remove_pointer<Type>::type::cast(                        \
        roots_table_[RootIndex::k##CamelName]);                          \
  }                                                                      \
  Handle<std::remove_pointer<Type>::type> ReadOnlyRoots::name##_handle() \
      const {                                                            \
    return Handle<std::remove_pointer<Type>::type>(                      \
        bit_cast<Address*>(&roots_table_[RootIndex::k##CamelName]));     \
  }

READ_ONLY_ROOT_LIST(ROOT_ACCESSOR)
#undef ROOT_ACCESSOR

Map* ReadOnlyRoots::MapForFixedTypedArray(ExternalArrayType array_type) {
  RootIndex root_index = RootsTable::RootIndexForFixedTypedArray(array_type);
  return Map::cast(roots_table_[root_index]);
}

Map* ReadOnlyRoots::MapForFixedTypedArray(ElementsKind elements_kind) {
  RootIndex root_index = RootsTable::RootIndexForFixedTypedArray(elements_kind);
  return Map::cast(roots_table_[root_index]);
}

FixedTypedArrayBase* ReadOnlyRoots::EmptyFixedTypedArrayForMap(const Map* map) {
  RootIndex root_index =
      RootsTable::RootIndexForEmptyFixedTypedArray(map->elements_kind());
  return FixedTypedArrayBase::cast(roots_table_[root_index]);
}

}  // namespace internal
}  // namespace v8

#endif  // V8_ROOTS_INL_H_
