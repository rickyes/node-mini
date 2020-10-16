// Copyright 2018 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_OBJECTS_STRING_TABLE_INL_H_
#define V8_OBJECTS_STRING_TABLE_INL_H_

#include "src/objects/string-table.h"

// Has to be the last include (doesn't have include guards):
#include "src/objects/object-macros.h"

namespace v8 {
namespace internal {

StringTableKey::StringTableKey(uint32_t hash_field, int length)
    : hash_field_(hash_field), length_(length) {}

void StringTableKey::set_hash_field(uint32_t hash_field) {
  hash_field_ = hash_field;
}

uint32_t StringTableKey::hash() const {
  return hash_field_ >> Name::kHashShift;
}

}  // namespace internal
}  // namespace v8

#include "src/objects/object-macros-undef.h"

#endif  // V8_OBJECTS_STRING_TABLE_INL_H_