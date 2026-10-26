// ==============================================================================
//  cry_stl_compat.h
//  Small compatibility layer for building the CryEngine 1 sources with a modern
//  Microsoft STL (VS 2022 17.10+ / VS 2026).
//
//  * Visual C++ 7.1 (VS 2003), which this code base was written for, provided
//    std::hash_map / std::hash_set.  Those moved to stdext in VS 2005 and were
//    removed completely later on.  The engine uses them as simple associative
//    containers (the module StdAfx headers even do "#define hash_map map"), so
//    aliasing them to std::map / std::set keeps the original semantics.
//
//  This header is force-included in every translation unit (/FI) by
//  cmake/FarCryEngine.cmake.
// ==============================================================================
#pragma once

#ifdef __cplusplus

#include <map>
#include <set>
#include <cstring>
#include <cstdlib>

// The engine headers forward declare "struct _finddata_t" in ICryPak.h.  In the
// UCRT _finddata_t is a macro (for _finddata64i32_t / _finddata32_t), so the
// definition has to be visible before those headers are parsed.
#include <io.h>

namespace std
{
    template <class Key, class Value, class HashFunc = void>
    using hash_map = std::map<Key, Value>;

    template <class Key, class HashFunc = void>
    using hash_set = std::set<Key>;

    template <class Key, class Value, class HashFunc = void>
    using hash_multimap = std::multimap<Key, Value>;
}

#endif // __cplusplus
