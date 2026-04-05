# Internal Design Document for Pokedex C++ Model

## Avoid linking with `libstdc++`

### DO NOT use exceptions nor RTTI

Do not use exceptions (`try`, `throw`, and `catch`) and RTTI (`typeid` and `dynamic_cast`).

The following compile options should be added (see `meson.build`):
- `-fno-exceptions`: disable exceptions support
- `-fno-rtti`: disable RTTI support

### DO NOT use `new` nor `delete`

1. Invoke constructors manually (known as "placement new")
2. Invoke destructors manually (simply `p_obj->~Type()`)
3. Be careful with alignment requirement. Use `aligned_alloc`

