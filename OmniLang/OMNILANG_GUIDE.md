# OmniLang User Guide

OmniLang is a modern, interpreted programming language designed for simplicity and power. This guide covers the language syntax, features, and the complete standard library.

## 1. Getting Started

To run an OmniLang program:
```bash
omni.exe <filename.omni>
```

## 2. Language Basics

### Comments
```omni
# This is a single-line comment
```

### Variables & Data Types
OmniLang is dynamically typed. You don't need to declare types explicitly.
```omni
x = 10              # Integer
pi = 3.14           # Double
name = "Omni"       # String
isActive = true     # Boolean
items = [1, 2, 3]   # Array (List)
person = {"name": "John", "age": 30} # Object (Map)
```

### Output & Input
```omni
print("Hello, world!")
name = input("Enter your name: ")
printf("Hello, %s! You are %d years old.\n", name, 25)
```

## 3. Control Flow

### If-Elif-Else
Note: Blocks are indented (Python-style).
```omni
if x > 10:
    print("Greater than 10")
elif x == 10:
    print("Equal to 10")
else:
    print("Less than 10")
```

### While Loop
```omni
i = 0
while i < 5:
    print(i)
    i = i + 1
```

### For Loop (Range)
```omni
# Loop from 0 to 4
for i in range(5):
    print(i)
```

## 4. Functions
Define functions using `def`.
```omni
def add(a, b):
    return a + b

result = add(5, 10)
```

## 5. Classes (Basic OOP)
```omni
class Person:
    def init(name):
        self.name = name
    
    def greet():
        print("Hello, I am " + self.name)

p = new Person("Alice")
p.greet()
```

## 6. Standard Library Reference

### Console I/O
| Function | Description | Usage |
|----------|-------------|-------|
| `print(...)` | Prints values followed by newline. | `print("Hello", 123)` |
| `printf(fmt, ...)` | formatted print (C-style). | `printf("Val: %d", 10)` |
| `input(prompt)` | Reads a line from stdin. | `name = input("Name: ")` |

### Math
| Function | Description | Usage |
|----------|-------------|-------|
| `Math.abs(x)` | Absolute value. | `Math.abs(-5)` |
| `Math.max(a,b)` | Maximum of two numbers. | `Math.max(5, 10)` |
| `Math.min(a,b)` | Minimum of two numbers. | `Math.min(5, 10)` |
| `Math.sqrt(x)` | Square root. | `Math.sqrt(16)` |
| `Math.pow(b,e)` | Power (base^exponent). | `Math.pow(2, 3)` |
| `Math.floor(x)` | Round down. | `Math.floor(3.9)` |
| `Math.ceil(x)` | Round up. | `Math.ceil(3.1)` |
| `Math.round(x)` | Round to nearest integer. | `Math.round(3.5)` |
| `Math.random()` | Random number (0.0 - 1.0). | `r = Math.random()` |
| `Math.sin(x)` | Sine (radians). | `Math.sin(0)` |
| `Math.cos(x)` | Cosine (radians). | `Math.cos(0)` |
| `Math.tan(x)` | Tangent (radians). | `Math.tan(0)` |
| `Math.log(x)` | Natural logarithm (ln). | `Math.log(Math.E)` |
| `Math.log10(x)` | Base-10 logarithm. | `Math.log10(100)` |
| `Math.exp(x)` | Exponential (e^x). | `Math.exp(1)` |
| `Math.PI` | Constant PI (3.14159...). | `Math.PI` |
| `Math.E` | Constant E (2.718...). | `Math.E` |

### String
Strings are objects but also have static helper functions.
| Function | Description | Usage |
|----------|-------------|-------|
| `len(str)` | Length of string. | `len("abc")` |
| `String.length(s)` | Length of string. | `String.length(s)` |
| `String.toUpperCase(s)` | Convert to uppercase. | `s = String.toUpperCase(s)` |
| `String.toLowerCase(s)` | Convert to lowercase. | `s = String.toLowerCase(s)` |
| `String.substring(s, start, end)` | Extract substring. | `sub = String.substring(s, 0, 3)` |
| `String.indexOf(s, sub)` | Find position of substring. | `pos = String.indexOf(s, "sub")` |
| `String.replace(s, old, new)` | Replace occurrences. | `s = String.replace(s, "a", "b")` |
| `String.trim(s)` | Remove whitespace. | `s = String.trim(s)` |
| `String.split(s, delimiter)` | Split string into array. | `parts = String.split(s, ",")` |
| `String.contains(s, sub)` | Check if contains substring. | `if String.contains(s, "abc"):` |
| `String.startsWith(s, prefix)` | Check prefix. | `if String.startsWith(s, "Pre"):` |
| `String.endsWith(s, suffix)` | Check suffix. | `if String.endsWith(s, ".txt"):` |
| `String.charAt(s, index)` | Get character at index. | `c = String.charAt(s, 0)` |
| `String.format(fmt, ...)` | Format string (returns string). | `msg = String.format("Hi %s", name)` |
| `String.isEmpty(s)` | Check if empty. | `if String.isEmpty(s):` |
| `String.equals(s1, s2)` | Check equality. | `if String.equals(a, b):` |
| `String.equalsIgnoreCase(s1, s2)`| Case-insensitive equality. | `if String.equalsIgnoreCase(a, b):` |

### List (Arrays)
Arrays are created with `[]` or `List.new()`.
| Function | Description | Usage |
|----------|-------------|-------|
| `List.new()` | Create empty list. | `l = List.new()` |
| `List.add(list, item)` | Add item to list (returns new list). | `l = List.add(l, item)` |
| `List.get(list, index)` | Get item at index. | `item = List.get(l, 0)` |
| `List.set(list, index, val)` | Set item at index. | `l = List.set(l, 0, val)` |
| `List.remove(list, index)` | Remove item at index. | `l = List.remove(l, 0)` |
| `List.size(list)` | Get list size. | `n = List.size(l)` |
| `List.isEmpty(list)` | Check if empty. | `if List.isEmpty(l):` |
| `List.contains(list, item)` | Check if item exists. | `if List.contains(l, 5):` |
| `List.indexOf(list, item)` | Find index of item. | `idx = List.indexOf(l, 5)` |
| `range(start, end, step)` | Create range array. | `nums = range(0, 10, 2)` |

### Map (Dictionaries)
Maps are created with `{}` or `Map.new()`.
| Function | Description | Usage |
|----------|-------------|-------|
| `Map.new()` | Create empty map. | `m = Map.new()` |
| `Map.put(map, key, val)` | Put key-value pair. | `m = Map.put(m, "key", "val")` |
| `Map.get(map, key)` | Get value by key. | `val = Map.get(m, "key")` |
| `Map.containsKey(map, key)` | Check if key exists. | `if Map.containsKey(m, "key"):` |
| `Map.keys(map)` | Get array of keys. | `keys = Map.keys(m)` |
| `Map.size(map)` | Get number of entries. | `n = Map.size(m)` |

### File I/O
| Function | Description | Usage |
|----------|-------------|-------|
| `File.read(path)` | Read entire file to string. | `content = File.read("file.txt")` |
| `File.write(path, content)` | Write string to file. | `File.write("file.txt", "data")` |
| `File.append(path, content)` | Append string to file. | `File.append("log.txt", "line\n")` |
| `File.exists(path)` | Check if file exists. | `if File.exists("file.txt"):` |

### Date & Time
| Function | Description | Usage |
|----------|-------------|-------|
| `Date.now()` | Current timestamp (int). | `ts = Date.now()` |
| `Date.format(ts, fmt)` | Format timestamp. | `s = Date.format(ts, "dd/MM/yyyy")` |
| `Date.parse(dateStr, fmt)` | Parse date string to timestamp. | `ts = Date.parse("01/01/2026")` |
| `Date.year(ts)` | Get year. | `y = Date.year(ts)` |
| `Date.month(ts)` | Get month (1-12). | `m = Date.month(ts)` |
| `Date.day(ts)` | Get day of month. | `d = Date.day(ts)` |
| `Date.before(ts1, ts2)` | Check if ts1 < ts2. | `if Date.before(t1, t2):` |
| `Date.after(ts1, ts2)` | Check if ts1 > ts2. | `if Date.after(t1, t2):` |

### Serialization (JSON)
| Function | Description | Usage |
|----------|-------------|-------|
| `Serializer.saveJSON(path, data)`| Save data to JSON file. | `Serializer.saveJSON("data.json", data)` |
| `Serializer.loadJSON(path)` | Load data from JSON file. | `data = Serializer.loadJSON("data.json")` |
| `Serializer.toJSON(data)` | Convert data to JSON string. | `jsonStr = Serializer.toJSON(obj)` |
| `Serializer.fromJSON(str)` | Parse JSON string to data. | `obj = Serializer.fromJSON(jsonStr)` |

### Regular Expressions
| Function | Description | Usage |
|----------|-------------|-------|
| `Regex.matches(str, pat)` | Check if full string matches. | `ok = Regex.matches(s, "^[0-9]+$")` |
| `Regex.search(str, pat)` | Check if pattern exists in str. | `ok = Regex.search(s, "error")` |
| `Regex.replace(str, pat, rep)`| Replace matches. | `s = Regex.replace(s, "\\s+", " ")` |
| `Regex.findAll(str, pat)` | Find all matches (array). | `list = Regex.findAll(s, "[0-9]+")` |

### CSV
| Function | Description | Usage |
|----------|-------------|-------|
| `CSV.readFile(path)` | Read CSV file to 2D array. | `rows = CSV.readFile("data.csv")` |
| `CSV.parse(content)` | Parse CSV string. | `rows = CSV.parse(csvStr)` |

### Utility
| Function | Description | Usage |
|----------|-------------|-------|
| `typeof(value)` | Get type name ("string", "int"...). | `t = typeof(x)` |
| `int(value)` | Convert to integer. | `x = int("123")` |
| `float(value)` | Convert to float/double. | `d = float("12.3")` |
| `str(value)` | Convert to string. | `s = str(123)` |
| `Integer.parseInt(s)` | Parse int string. | `i = Integer.parseInt("123")` |
| `Double.parseDouble(s)` | Parse double string. | `d = Double.parseDouble("12.3")` |
