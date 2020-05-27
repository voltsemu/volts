# Issues

## 1. Windows does not ship with VSRT142.dll by default
This is solved by statically linking it into the final exe.

## 2. The MSVC C++ STL fails to build when exceptions are disabled
This is solved by defining `-D_HAS_EXCEPTIONS=0` when building for windows.

## 3. alloca is deprecated on MSVC
This is solved by using a macro to alias both `_malloca` and `alloca` to `ALLOCA`