# Language Feature Specifications

## 1. Pointers
- **Declaration**: `alp <point> -> num` creates a pointer binding `<point>` to `num`.
- **Dereference/Assignment**: `num = <point>` reads the value at the pointer.
- **Usage Constraint**: Pointers activate only at explicit usage sites (e.g., `<point>`), enforcing lazy evaluation.

## 2. Raw Allocation
- **Syntax**: `alr num` allocates uninitialized memory for `num`.
- Provides zero-cost access to raw storage before explicit initialization.

## 3. Massive (Arrays)
- **Declaration**: `alm num<4> = @4, 3, 2, 1@` initializes a 4-element array.
- **Element Access**: `num<3> = 4` reads/writes the nth element (0-based indexing).
- `@...@` delimiters frame the array literal compactly.

## 4. Character/String Output
- **Functions**: `outc` for single characters; `outs` for strings.
- **Usage**: `outc <10>` prints ASCII 10; `outs <"something">` outputs the string literal.

## 5. Strings
- **Note**: Strings implement as specialized massive (arrays) under the hood.
- **Declaration**: `als str = "something"` allocates and initializes a string variable.
