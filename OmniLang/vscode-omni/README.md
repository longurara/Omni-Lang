# Omni Language Support

This extension provides language support for the Omni Programming Language.

## Features

- **Syntax Highlighting**: Complete highlighting for keywords, types, strings, comments, and operators.
- **Bracket Matching**: Support for `()`, `[]`, `{}`.
- **Auto Indentation**: Python-style indentation handling (increases after lines ending with `:`).
- **Code Snippets**: (Coming soon)

## Installation

1. Open the `vscode-omni` folder in VS Code.
2. Press `F5` to launch the Extension Development Host window.
3. Open any `.omni` file to see the syntax highlighting in action.

## Building

To package the extension into a `.vsix` file:

```bash
npm install -g vsce
vsce package
```

## Supported Syntax

```omni
class Person:
    public String name
    public int age
    
    def __init__(String n, int a):
        self.name = n
        self.age = a
        
    def greet():
        print("Hello, " + self.name)

def main():
    p = new Person("John", 30)
    p.greet()
```
