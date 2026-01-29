# Omni Language Support for VS Code

Provide rich language support for **OmniLang**, including syntax highlighting, code snippets, and IntelliSense.

## Features

### 1. Syntax Highlighting
*   Full colorization for keywords, control flow, functions, classes, and types.
*   Special support for **F-Strings** (`f"Value: {x}"`).
*   **Lambda expressions** (`x -> x + 1`) highlighting.

### 2. IntelliSense & Documentation
*   **Hover Tooltips**: Rich documentation for all standard library functions (`Math`, `String`, `File`, etc.).
*   **Signature Help**: Shows parameter hints when typing function calls (e.g., `Math.pow(base, exp)`).
*   **Go to Definition**: Ctrl+Click or F12 to jump to function/class definitions.

### 3. Code Snippets
Type part of a keyword and press `Tab` to expand:
*   `def` -> Function definition
*   `class` -> Class with constructor
*   `main` -> Main entry point
*   `if`, `for`, `while` -> Control flow structures
*   `import` -> Module import
*   `lambda` -> Lambda expression

### 4. Code Navigation
*   **Outline View**: See the structure of your file (Classes, Functions) in the sidebar.

## Standard Library Reference

### Math
*   `Math.sqrt(x)`, `Math.pow(b, e)`, `Math.random()`
*   `Math.sin(x)`, `Math.cos(x)`, `Math.abs(x)`

### String
*   `length()`, `substring(s, e)`, `split(delim)`
*   `replace(old, new)`, `format(fmt, ...args)`

### File & Path
*   `File.read(path)`, `File.write(path, content)`
*   `Path.join(parts)`, `Path.extension(path)`

## Installation

1.  Navigate to the extension directory:
    ```bash
    cd vscode-omni
    ```
2.  Package the extension:
    ```bash
    vsce package
    ```
3.  Install the `.vsix` file:
    ```bash
    code --install-extension vscode-omni-0.0.3.vsix
    ```
