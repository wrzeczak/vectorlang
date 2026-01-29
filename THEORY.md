Wrzeczak's Vector Language
--------------------------

I just wanted to write a small little programming language; the general idea here is vector manipulation, which could be cool for doing stuff like procedurally generating 3D models, or doing anything cool with list manipulation of numbers that can be translated to visual, audio, etc. data.

The data herein are vectors of integers (long long). Individual numbers are actually one-member vectors; this means multiple-dimensional vectors are easily accessible. They can be represented as normal numbers (e.g. 10), but vectors are notated with square braces e.g. [ 10, 11, 12 ].

A simple program might look like this:

--------------------------

```
vec1 = [ 10 .. 20 ]         // the ".." operator is like Haskell's Texas Ranges;
// vec1 == [ 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 ] -- by default, the increment is 1
vec2 = [ 21, 31 .. 120 ]    // here, we increment by ten; 120 is less than 121, so it's not in the list
// vec2 == [ 21, 31, 41, 51, 61, 71, 81, 91, 101, 111 ]
vec <- 121                  // this operator appends to the back of a list
// vec2 == [ 21, 31, 41, ... 111, 121 ]
vec3 = []                   // define an empty vector
vec3 <- vec1 + vec2         // add each element of vec1 to each element of vec2, and append to the back of the (empty) vec3
// vec3 == [ 31, 42, 53, 64, 75, ... ]
PRINT(vec3)                 // prints to stdout "vec3 == [ 31, 42 ... ]"
```

--------------------------

Here's a list of the operators:

| OPERATOR | NAME | DESCRIPTION |
|----------|------|-------------|
| `=`      | ASSIGNMENT     | Assign a value to a name; names cannot be re-assigned to unless you call `EMPTY()` on them. |
| `()`     | L/R-PAREN      | Used for grouping expressions. |
| `[]`     | L/R-SQUARE | Used for delineating vectors. |
| `==`     | EQUALITY       | Logical equal; `10 == 10`, `[10, 20, 30 ] == [ 10, 20, 30 ]`, etc. |
| `:==`    | LEFT-EQUAL     | Check if the heads of two vectors are equal; returns the length of the equal vector e.g. `([ 10, 20, 30 ] :== 10 ) == 1`. Returns `0` if not equal. |
| `==:`    | RIGHT-EQUAL    | As above, but with the rear; `([ 10, 20, 30 ] ==: [ 28, 29, 30 ]) == 1` |
| `:=:`    | CHEEK-EQUAL    | Checks if the asscheeks of a vector are equal; `([ 10, 11, ... 29, 30] :=: [ 10, 20, 30 ]) == 1` |
| `<-`     | APPEND         | Appends a vector to another vector; `([] <- 10) == [ 10 ]`, `([ 10, 20 ] <- [ 30, 40 ]) == [ 10, 20, 30, 40 ]` |
| `->`     | PREPEND        | Prepends a vector; `10 -> [ 20, 30, 40 ] == [ 10, 20, 30, 40 ]`; `[ 20, 30, 40 ] -> 10 = [ 20, 30, 40, 10 ]` |
| `+`      | ADD            | Adds the values of two vectors; `10 + 20 == 30`, `[ 10, 11 ] + [ 23, 24, 25 ] == [ 33, 35 ]` (excess values are stripped) |
| `-`      | SUBTRACT       | Subtracts the values of two vectors (as above) |

--------------------------

Here's a list of the built-in macros/functions:

### `PRINT(vector, front_visible <opt, -1 by default>, back_visible <opt, -1 by default>)`

`PRINT()` prints a vector's name, followed by a double-equal, and its values, followed by a newline. By default, it will print the whole list; if you pass the parameters, it will print only so many; using `vec == [ 10 .. 100 ]`:

`PRINT(vec) => "vec == [ 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 ] (newline)"`

`PRINT(vec, 2) => "vec == [ 10, 20, ... 90, 100 ] (newline)"`    
 `PRINT()` with one argument (greater than zero) will print that many of each cheek on the sides of some triple dots (ellipses, not double-dot ranges).

`PRINT(vec, 2, 4) => "vec == [ 10, 20, ... 70, 80, 90, 100 ]"`  
`PRINT()` with two arguments (greater than zero) will print the specified cheek that many times, but they can be non-equal.


`PRINT(vec, 2, 0) => "vec == [ 10, 20, ... ] (10 elements)"`
`PRINT(vec, 0, 2) => "vec == [ ... 90, 100 ] (10 elements)"`

Printing with 0 will omit a cheek and append a length marker.

`PRINT(vec, -1, 9) => "vec == [ 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 ]"`

Printing with either argument less than 0 will perform the default behavior (by default, PRINT(vec) is actually PRINT(vec, -1, -1))

### `EMPTY(vector)`

`EMPTY()` essentially sets a vector to `[]`; however, you can re-use a name after emptying it:
```
vec1 = [ 10, 20, 30 ]
...
vec1 = [ 20, 30, 40 ] // NOT ALLOWED!!
```
but...
```
vec1 = [ 10, 20, 30 ]
...
EMPTY(vec1)
vec1 = [ 20, 30, 40 ] // legal
```

### `MIN1(a, b)` and `MAX1(a, b)`

These take two vectors (if length > 1, or dim > 1, then a.0...0, b.000, are taken) and do min or max on them:
```
MIN1(10, 20) == 10
MAX1(10, 20) == 20
MIN1([ 10, 20, ...], 11) == 10
MAX1([[[ 91 ], 92 ], 93 ], 94 ) == 91
```

--------------------------

Some extra notes:

1) Spaces are not semantically important and are descarded before tokenization. I like to use a lot of space to keep things uncluttered, but it's purely stylistic. "vec<-10" is equivalent to "vec <- 10" is equivalent to "vec    <-                 10", etc.
2) For reasons of implementation details of the lexer (specifically, token_split_wv_file()), the name "__NEWLINE" is forbidden. I hope this doesn't break anyone's hearts.
3) For similar reasons, variables cannot begin with a capital letter (or numbers, but that's standard); this is because the builtin macros do. I don't think I've ever begun a *variable* name with a capital letter in my life, so I hope this is not so bad.

--------------------------

On Vectors:

Vectors are arbitrary-dimensioned lists of integers (long long). A list of integers is simple to implement, and a 2d, 3d, etc. are as well; but an arbitrary-dimensioned list is not trivial, because in C you can't really "create" those sorts of types on the fly. So, one option was code generation; we determine the dimension of the list, then generate some code that creates a struct, some functions, etc. This I deemed too complex for this project. We could just lie about the arbitrary dimension, and just create manually like 1-10d int list structs, but this is uninteresting and tedious. So, I need to create a way to represent arbitrary-dimension lists in, ideally, a single list. This is my solution.

First, for clarity; an n-dimensional vector is one which contains at least one member which requires n accessors to reach. In C-style syntax: 1d is `vec[x]`, 2d is `vec[x][y]`, etc. For now, I will only imagine perfect, geometric n-dimensional lists: a square list of "size" four looks like four vectors of four members: `[[(4)], [(4)], [(4)], [(4)]]`.

Now, imagine a simple 2d vector `[[ 10, 20, 30 ], [ 100, 200, 300 ], [ 1000, 2000, 3000 ]]`; this can be easily boiled-down to:
```
{ .size = 3, .dim = 2, .vals = [ 10, 20, 30, 100, 200, 300, 1000, 2000, 3000 ] }
```
Generalizing, we come to a structure that looks like:
```
{ .dim = 2, .sizes = [ 3, 3, 3 ], .vals = [ 10, 20, 30, 100, 200, 300, 1000, 2000, 3000 ] }
``` 
So, imagining a new vector that looks like this:
```
[[ 10, 20 ], [ 30, 100, 200, 300 ], [ 1000, 2000 ], 3000 ]
=> { .dim = 2, .sizes = [ 2, 4, 2, 1 ], .vals = ... }

[[[abc], [abc], [abc]], [[abc], [abc], [abc]], [[abc], [abc], [abc]]]
```
