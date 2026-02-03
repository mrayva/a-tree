# C++ API Improvements

## Summary

Two major improvements to the C++ wrapper (`atree.hpp`):

### 1. ✅ Fluent Tree Construction

**New `Tree::builder()` API:**

```cpp
// Before: Only initializer list
Tree tree({
    AttributeDefinition::integer("user_id"),
    AttributeDefinition::float_attr("price"),
    AttributeDefinition::boolean("is_active")
});

// After: Fluent builder pattern
auto tree = std::move(Tree::builder()
    .with_integer("user_id")
    .with_float("price")
    .with_boolean("is_active"))
    .build();
```

**Features:**
- Chainable methods for all 6 attribute types
- `.with_boolean(name)`
- `.with_integer(name)`
- `.with_float(name)`
- `.with_string(name)`
- `.with_string_list(name)`
- `.with_integer_list(name)`
- `.build()` - throws on error
- `.try_build()` - returns `Result<Tree>`

### 2. ✅ No More `.unwrap()`

**Methods throw by default, `try_*` variants for explicit error handling:**

```cpp
// Before: Always need .unwrap()
tree.insert(1, "expression").unwrap();
auto matches = tree.search(builder).unwrap();
auto dot = tree.to_graphviz().unwrap();

// After: Methods throw by default (no .unwrap() needed!)
tree.insert(1, "expression");
auto matches = tree.search(builder);
auto dot = tree.to_graphviz();

// Want Result-based error handling? Use try_* methods
auto result = tree.try_insert(1, "expression");
if (result.is_err()) {
    std::cerr << result.error() << "\n";
}
```

**API Changes:**

| Old Method | New Default Method | Result-Based Method |
|------------|-------------------|-------------------|
| `insert()` → `Result<void>` | `insert()` → throws | `try_insert()` → `Result<void>` |
| `search()` → `Result<vector>` | `search()` → `vector` | `try_search()` → `Result<vector>` |
| `to_graphviz()` → `Result<string>` | `to_graphviz()` → `string` | `try_to_graphviz()` → `Result<string>` |

## Examples

### Complete Example (No Unwrap!)

```cpp
#include "atree.hpp"
using namespace atree;

int main() {
    // Fluent construction
    auto tree = std::move(Tree::builder()
        .with_boolean("premium")
        .with_integer("age")
        .with_string("country"))
        .build();

    // No .unwrap() needed!
    tree.insert(1, "premium and age >= 18");

    auto matches = tree.search(
        tree.make_event()
            .with_boolean("premium", true)
            .with_integer("age", 25)
            .with_string("country", "US")
    );

    std::cout << "Found " << matches.size() << " matches\n";

    auto dot = tree.to_graphviz();
    // Save and visualize: dot -Tpng tree.dot -o tree.png
}
```

### Error Handling (Choose Your Style)

```cpp
// Style 1: Exception-based (default, clean)
try {
    tree.insert(1, "expression");
    auto matches = tree.search(builder);
} catch (const atree::Error& e) {
    std::cerr << "Error: " << e.what() << "\n";
}

// Style 2: Result-based (explicit)
auto result = tree.try_insert(1, "expression");
if (result.is_err()) {
    std::cerr << "Error: " << result.error() << "\n";
    return 1;
}

auto search_result = tree.try_search(builder);
if (search_result.is_ok()) {
    auto matches = search_result.unwrap();
}
```

## Testing

Run the new fluent example:
```bash
./example_fluent_cpp
```

This demonstrates:
- ✅ Fluent tree construction
- ✅ No `.unwrap()` needed for common operations
- ✅ Both exception-based and Result-based error handling
- ✅ Clean, readable code

## Files Changed

- `atree.hpp` - Added `TreeBuilder` class and throwing methods
- `examples/fluent_cpp.cpp` - New example (168 lines)
- `examples/advanced_cpp.cpp` - Updated to use new API
- `build_examples.sh` - Added fluent example

## Backwards Compatibility

**Breaking changes:**
- `insert()`, `search()`, and `to_graphviz()` now throw instead of returning `Result<T>`
- Use `try_insert()`, `try_search()`, and `try_to_graphviz()` for the old behavior

**Migration guide:**
```cpp
// Before
auto result = tree.insert(1, expr);
if (result.is_err()) { handle_error(); }

// After (Option 1: Throw)
tree.insert(1, expr);  // throws on error

// After (Option 2: Result)
auto result = tree.try_insert(1, expr);
if (result.is_err()) { handle_error(); }
```

## Benefits

1. **Cleaner Code**: No more `.unwrap()` everywhere
2. **More Idiomatic**: Exceptions are standard in C++
3. **Flexibility**: Choose between exceptions or Results
4. **Better DX**: Fluent builders make tree construction more readable
5. **Type Safety**: Still type-safe with compile-time checks
