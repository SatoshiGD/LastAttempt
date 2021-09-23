#ifndef UTILS_HPP
#define UTILS_HPP 

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void* getFunctionPointer(const uintptr_t offset) {
    const auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
    return reinterpret_cast<void*>(base + offset);
}

template <class In_1, class In_2, class In_3>
auto createHookOf(In_1 pTarget, In_2 pDetour, In_3 ppOriginal) {
    return MH_CreateHook(reinterpret_cast<void*>(pTarget), reinterpret_cast<void*>(pDetour), reinterpret_cast<void**>(ppOriginal));
}

template<typename In>
void* union_cast(In in) {
  union {
    In in;
    void* out;
  } caster;
  caster.in = in;
  return caster.out;
}

#endif