const vscode = require('vscode');

/**
 * @param {vscode.ExtensionContext} context
 */
function activate(context) {
    console.log('OmniLang extension is now active!');

    const provider = vscode.languages.registerCompletionItemProvider('omni', {
        provideCompletionItems(document, position) {
            const linePrefix = document.lineAt(position).text.substr(0, position.character);

            // 1. Method completion (after dot)
            if (linePrefix.endsWith('.')) {
                // Check what's before the dot
                // Math.
                if (linePrefix.trim().endsWith('Math.')) {
                    return getMathMethods();
                }
                // String.
                if (linePrefix.trim().endsWith('String.')) {
                    return getStringMethods();
                }
                // File.
                if (linePrefix.trim().endsWith('File.')) {
                    return getFileMethods();
                }
                // List.
                if (linePrefix.trim().endsWith('List.')) {
                    return getListMethods();
                }
                // Map.
                if (linePrefix.trim().endsWith('Map.')) {
                    return getMapMethods();
                }
                // Date.
                if (linePrefix.trim().endsWith('Date.')) {
                    return getDateMethods();
                }
                // Serializer.
                if (linePrefix.trim().endsWith('Serializer.')) {
                    return getSerializerMethods();
                }
                // Regex.
                if (linePrefix.trim().endsWith('Regex.')) {
                    return getRegexMethods();
                }
                // CSV.
                if (linePrefix.trim().endsWith('CSV.')) {
                    return getCSVMethods();
                }

                return undefined;
            }

            // 2. Top-level completion (Keywords + Classes)
            return getTopLevelItems();
        }
    }, '.'); // Trigger on dot

    // Hover Provider for documentation tooltips
    const hoverProvider = vscode.languages.registerHoverProvider('omni', {
        provideHover(document, position) {
            const wordRange = document.getWordRangeAtPosition(position);
            if (!wordRange) return undefined;

            const word = document.getText(wordRange);
            const lineText = document.lineAt(position).text;

            // Check for module.method pattern (e.g., Math.sqrt)
            const dotIndex = lineText.lastIndexOf('.', position.character - 1);
            if (dotIndex !== -1) {
                const beforeDot = lineText.substring(0, dotIndex).trim().split(/\s+/).pop();
                const fullName = beforeDot + '.' + word;
                const doc = getHoverDoc(fullName);
                if (doc) {
                    return new vscode.Hover(new vscode.MarkdownString(doc));
                }
            }

            // Check standalone functions and keywords
            const doc = getHoverDoc(word);
            if (doc) {
                return new vscode.Hover(new vscode.MarkdownString(doc));
            }

            return undefined;
        }
    });

    // 3. Signature Help Provider (Parameter hints)
    const signatureProvider = vscode.languages.registerSignatureHelpProvider('omni', {
        provideSignatureHelp(document, position) {
            const linePrefix = document.lineAt(position).text.substr(0, position.character);
            const openParen = linePrefix.lastIndexOf('(');
            if (openParen === -1) return undefined;

            // Find function name before the parenthesis
            const beforeParen = linePrefix.substring(0, openParen).trim();
            const funcName = beforeParen.split(/[\s\.]+/).pop();
            const fullPrefix = beforeParen.split(/\s+/).pop(); // Handle obj.method

            let doc = getHoverDoc(fullPrefix) || getHoverDoc(funcName);

            if (doc) {
                // Extract signature from doc string (e.g., "**print(...args)**")
                const match = doc.match(/\*\*([a-zA-Z0-9_.]+)(\(.*\))\*\*/);
                if (match) {
                    const signatureLabel = match[1] + match[2];
                    const signature = new vscode.SignatureInformation(signatureLabel, new vscode.MarkdownString(doc));
                    return {
                        signatures: [signature],
                        activeSignature: 0,
                        activeParameter: 0
                    };
                }
            }
            return undefined;
        }
    }, '(', ',');

    // 4. Document Symbol Provider (Outline view)
    const symbolProvider = vscode.languages.registerDocumentSymbolProvider('omni', {
        provideDocumentSymbols(document) {
            const symbols = [];

            for (let i = 0; i < document.lineCount; i++) {
                const line = document.lineAt(i);

                // Match class definition
                const classMatch = line.text.match(/^class\s+([a-zA-Z_]\w*)/);
                if (classMatch) {
                    symbols.push(new vscode.DocumentSymbol(
                        classMatch[1],
                        'Class',
                        vscode.SymbolKind.Class,
                        line.range, line.range
                    ));
                    continue;
                }

                // Match function definition
                const funcMatch = line.text.match(/^def\s+([a-zA-Z_]\w*)/);
                if (funcMatch) {
                    symbols.push(new vscode.DocumentSymbol(
                        funcMatch[1],
                        'Function',
                        vscode.SymbolKind.Function,
                        line.range, line.range
                    ));
                    continue;
                }
            }
            return symbols;
        }
    });

    // 5. Definition Provider (Go to Definition)
    const definitionProvider = vscode.languages.registerDefinitionProvider('omni', {
        async provideDefinition(document, position) {
            const wordRange = document.getWordRangeAtPosition(position);
            if (!wordRange) return undefined;
            const word = document.getText(wordRange);

            // Search in current workspace
            const files = await vscode.workspace.findFiles('**/*.omni');
            const locations = [];

            for (const file of files) {
                const doc = await vscode.workspace.openTextDocument(file);
                const text = doc.getText();

                // Regex for definition: def name or class name
                const regex = new RegExp(`(class|def)\\s+${word}\\b`, 'g');
                let match;
                while ((match = regex.exec(text)) !== null) {
                    const pos = doc.positionAt(match.index);
                    locations.push(new vscode.Location(file, pos));
                }
            }
            return locations;
        }
    });

    context.subscriptions.push(provider);
    context.subscriptions.push(hoverProvider);
    context.subscriptions.push(signatureProvider);
    context.subscriptions.push(symbolProvider);
    context.subscriptions.push(definitionProvider);
}

function deactivate() { }

module.exports = {
    activate,
    deactivate
};

// --- Helper Functions ---

function createMethod(label, detail, doc) {
    const item = new vscode.CompletionItem(label, vscode.CompletionItemKind.Method);
    item.detail = detail;
    item.documentation = new vscode.MarkdownString(doc);
    return item;
}

function createKeyword(label, doc) {
    const item = new vscode.CompletionItem(label, vscode.CompletionItemKind.Keyword);
    item.documentation = new vscode.MarkdownString(doc);
    return item;
}

function createClass(label, doc) {
    const item = new vscode.CompletionItem(label, vscode.CompletionItemKind.Class);
    item.documentation = new vscode.MarkdownString(doc);
    item.commitCharacters = ['.']; // Allow typing '.' to auto-accept class name
    return item;
}

function getTopLevelItems() {
    return [
        // Keywords
        createKeyword('def', 'Define a function'),
        createKeyword('class', 'Define a class'),
        createKeyword('if', 'Conditional statement'),
        createKeyword('elif', 'Else-if statement'),
        createKeyword('else', 'Else statement'),
        createKeyword('while', 'While loop'),
        createKeyword('for', 'For loop'),
        createKeyword('return', 'Return value'),
        createKeyword('break', 'Break loop'),
        createKeyword('continue', 'Continue loop'),
        createKeyword('try', 'Try block'),
        createKeyword('catch', 'Catch block'),
        createKeyword('finally', 'Finally block'),
        createKeyword('throw', 'Throw exception'),
        createKeyword('import', 'Import module'),
        createKeyword('var', 'Variable declaration (optional)'),
        createKeyword('new', 'Instantiate class'),
        createKeyword('this', 'Current instance'),
        createKeyword('self', 'Current instance (alias)'),

        // Built-in Classes
        createClass('Math', 'Mathematical functions and constants'),
        createClass('String', 'String utility functions'),
        createClass('File', 'File I/O operations'),
        createClass('List', 'List (Array) utility functions'),
        createClass('Map', 'Map (Dictionary) utility functions'),
        createClass('Date', 'Date and Time functions'),
        createClass('Serializer', 'JSON and Binary serialization'),
        createClass('Regex', 'Regular expression functions'),
        createClass('CSV', 'CSV parsing and reading functions'),
        createClass('Path', 'File path utilities'),
        createClass('System', 'System utilities (exit, env, time)'),
    ];
}

function getBuiltinFunctions() {
    return [
        // Built-in Functions
        createMethod('print', 'print(...args)', 'Print to console'),
        createMethod('printf', 'printf(fmt, ...args)', 'Formatted print'),
        createMethod('input', 'input(prompt)', 'Read input from console'),
        createMethod('typeof', 'typeof(value)', 'Get value type'),
        createMethod('len', 'len(obj)', 'Get length/size'),
        createMethod('range', 'range(start, end, step)', 'Create a range of numbers'),
    ];
}

function getMathMethods() {
    return [
        createMethod('abs', 'abs(x)', 'Absolute value'),
        createMethod('max', 'max(a, b)', 'Maximum value'),
        createMethod('min', 'min(a, b)', 'Minimum value'),
        createMethod('sqrt', 'sqrt(x)', 'Square root'),
        createMethod('pow', 'pow(base, exp)', 'Power'),
        createMethod('floor', 'floor(x)', 'Round down'),
        createMethod('ceil', 'ceil(x)', 'Round up'),
        createMethod('round', 'round(x)', 'Round to nearest integer'),
        createMethod('random', 'random()', 'Random number 0.0-1.0'),
        createMethod('sin', 'sin(x)', 'Sine function'),
        createMethod('cos', 'cos(x)', 'Cosine function'),
        createMethod('tan', 'tan(x)', 'Tangent function'),
        createMethod('log', 'log(x)', 'Natural logarithm'),
        createMethod('log10', 'log10(x)', 'Base-10 logarithm'),
        createMethod('exp', 'exp(x)', 'Exponential e^x'),
        createMethod('PI', 'Constant PI', '3.14159...'),
        createMethod('E', 'Constant E', '2.718...'),
    ];
}

function getStringMethods() {
    return [
        createMethod('length', 'length(s)', 'Get string length'),
        createMethod('toUpperCase', 'toUpperCase(s)', 'Convert to uppercase'),
        createMethod('toLowerCase', 'toLowerCase(s)', 'Convert to lowercase'),
        createMethod('substring', 'substring(s, start, [end])', 'Get substring'),
        createMethod('indexOf', 'indexOf(s, sub)', 'Find substring index'),
        createMethod('contains', 'contains(s, sub)', 'Check if contains substring'),
        createMethod('startsWith', 'startsWith(s, prefix)', 'Check prefix'),
        createMethod('endsWith', 'endsWith(s, suffix)', 'Check suffix'),
        createMethod('replace', 'replace(s, old, new)', 'Replace all occurrences'),
        createMethod('trim', 'trim(s)', 'Remove whitespace'),
        createMethod('split', 'split(s, delim)', 'Split string'),
        createMethod('charAt', 'charAt(s, index)', 'Get character at index'),
        createMethod('format', 'format(fmt, ...args)', 'Format string'),
        createMethod('isEmpty', 'isEmpty(s)', 'Check if empty'),
        createMethod('equals', 'equals(s1, s2)', 'Check equality'),
        createMethod('equalsIgnoreCase', 'equalsIgnoreCase(s1, s2)', 'Check case-insensitive equality'),
    ];
}

function getFileMethods() {
    return [
        createMethod('read', 'read(path)', 'Read file content'),
        createMethod('write', 'write(path, content)', 'Write content to file'),
        createMethod('append', 'append(path, content)', 'Append content to file'),
        createMethod('exists', 'exists(path)', 'Check if file exists'),
    ];
}

function getListMethods() {
    return [
        createMethod('new', 'new()', 'Create new list'),
        createMethod('add', 'add(list, item)', 'Add item to list'),
        createMethod('get', 'get(list, index)', 'Get item at index'),
        createMethod('set', 'set(list, index, val)', 'Set item at index'),
        createMethod('remove', 'remove(list, index)', 'Remove item at index'),
        createMethod('size', 'size(list)', 'Get list size'),
        createMethod('isEmpty', 'isEmpty(list)', 'Check if list is empty'),
        createMethod('contains', 'contains(list, item)', 'Check if item exists'),
        createMethod('indexOf', 'indexOf(list, item)', 'Find item index'),
    ];
}

function getMapMethods() {
    return [
        createMethod('new', 'new()', 'Create new map'),
        createMethod('put', 'put(map, key, val)', 'Put key-value'),
        createMethod('get', 'get(map, key)', 'Get value'),
        createMethod('containsKey', 'containsKey(map, key)', 'Check key existence'),
        createMethod('keys', 'keys(map)', 'Get all keys'),
        createMethod('size', 'size(map)', 'Get map size'),
    ];
}

function getDateMethods() {
    return [
        createMethod('now', 'now()', 'Current timestamp'),
        createMethod('format', 'format(ts, [fmt])', 'Format timestamp'),
        createMethod('parse', 'parse(str, [fmt])', 'Parse date string'),
        createMethod('year', 'year(ts)', 'Get year'),
        createMethod('month', 'month(ts)', 'Get month'),
        createMethod('day', 'day(ts)', 'Get day'),
        createMethod('before', 'before(ts1, ts2)', 'Check order'),
        createMethod('after', 'after(ts1, ts2)', 'Check order'),
    ];
}

function getSerializerMethods() {
    return [
        createMethod('saveJSON', 'saveJSON(path, data)', 'Save to JSON file'),
        createMethod('loadJSON', 'loadJSON(path)', 'Load from JSON file'),
        createMethod('toJSON', 'toJSON(data)', 'Convert to JSON string'),
        createMethod('fromJSON', 'fromJSON(str)', 'Parse JSON string'),
    ];
}

function getRegexMethods() {
    return [
        createMethod('matches', 'matches(str, pat)', 'Full match check'),
        createMethod('search', 'search(str, pat)', 'Partial match check'),
        createMethod('find', 'find(str, pat)', 'Find match position'),
        createMethod('findAll', 'findAll(str, pat)', 'Find all matches'),
        createMethod('replace', 'replace(str, pat, rep)', 'Replace matches'),
        createMethod('split', 'split(str, pat)', 'Split by regex'),
        createMethod('groups', 'groups(str, pat)', 'Extract capture groups'),
    ];
}

function getCSVMethods() {
    return [
        createMethod('readFile', 'readFile(path)', 'Read CSV file'),
        createMethod('parse', 'parse(content)', 'Parse CSV string'),
    ];
}

// Hover documentation for all functions
function getHoverDoc(name) {
    const docs = {
        // --- Keywords ---
        'def': `**def**
Define a new function.

\`\`\`omni
def add(a, b):
    return a + b
\`\`\``,
        'class': `**class**
Define a new class or interface.

\`\`\`omni
class Person:
    def __init__(self, name):
        self.name = name
\`\`\``,
        'if': `**if statement**
Execute code block if condition is true.

\`\`\`omni
if x > 0:
    print("Positive")
elif x < 0:
    print("Negative")
else:
    print("Zero")
\`\`\``,
        'while': `**while loop**
Repeat code block while condition is true.

\`\`\`omni
while x > 0:
    print(x)
    x = x - 1
\`\`\``,
        'for': `**for loop**
Iterate over a sequence or range.

\`\`\`omni
for i in range(10):
    print(i)
\`\`\``,
        'return': `**return**
Exit function and return a value.`,

        'print': `**print(...args)**
Print values to standard output with newline.

**Parameters:**
* \`...args\`: Values to print (converted to string).

**Example:**
\`\`\`omni
print("Hello", "World", 123)
\`\`\``,

        'input': `**input(prompt)**
Read a line of text from standard input.

**Parameters:**
* \`prompt\` (String): Text to display before waiting for input.

**Returns:**
* \`String\`: The user input.`,

        // --- Math Module ---
        'Math.sqrt': `**Math.sqrt(x)**
Calculates the square root of a number.

**Parameters:**
* \`x\` (Number): Must be non-negative.

**Returns:**
* \`Float\`: Square root of x.

**Example:**
\`\`\`omni
Math.sqrt(16) // 4.0
\`\`\``,

        'Math.pow': `**Math.pow(base, exp)**
Calculates base raised to the power of exponent.

**Parameters:**
* \`base\` (Number): The base number.
* \`exp\` (Number): The exponent.

**Returns:**
* \`Float\`: Result of base^exp.`,

        'Math.random': `**Math.random()**
Returns a pseudo-random number between 0.0 (inclusive) and 1.0 (exclusive).

**Returns:**
* \`Float\`: Random value.`,

        // --- String Module ---
        'String.length': `**String.length(s)**
Returns the length of the string.

**Parameters:**
* \`s\` (String): The input string.

**Returns:**
* \`Integer\`: Number of characters.`,

        'String.substring': `**String.substring(s, start, [end])**
Extracts a section of the string.

**Parameters:**
* \`s\` (String): Input string.
* \`start\` (Integer): Start index (inclusive).
* \`end\` (Integer): End index (exclusive). Optional.

**Returns:**
* \`String\`: Extracted substring.`,

        'String.split': `**String.split(s, delimiter)**
Splits string by delimiter.

**Parameters:**
* \`s\` (String): Input string.
* \`delimiter\` (String): Separator pattern.

**Returns:**
* \`List<String>\`: List of substrings.`,

        // --- File Module ---
        'File.read': `**File.read(path)**
Reads entire file content as string.

**Parameters:**
* \`path\` (String): Path to the file.

**Returns:**
* \`String\`: File content.
* Throws error if file not found.`,

        'File.write': `**File.write(path, content)**
Writes string to file (overwrites existing).

**Parameters:**
* \`path\` (String): Path to the file.
* \`content\` (String): Text to write.`,

        // --- CSV Module ---
        'CSV.parse': `**CSV.parse(content)**
Parses a CSV string into a 2D list.

**Parameters:**
* \`content\` (String): Raw CSV text.

**Returns:**
* \`List<List<String>>\`: Rows and columns.

**Example:**
\`\`\`omni
rows = CSV.parse("name,age\\nAlice,30")
print(rows[1][0]) // "Alice"
\`\`\``,

        // --- Regex Module ---
        'Regex.matches': `**Regex.matches(str, pattern)**
Checks if string matches pattern exactly.

**Parameters:**
* \`str\` (String): Input string.
* \`pattern\` (String): Regex pattern.

**Returns:**
* \`Boolean\`: True if full match.`,

        // --- Path Module ---
        'Path.join': `**Path.join(...parts)**
Joins path segments ensuring correct separators.

**Parameters:**
* \`...parts\` (String): Path segments.

**Returns:**
* \`String\`: Joined path.`,
    };

    return docs[name] || null;
}
