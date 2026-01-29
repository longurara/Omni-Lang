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

    context.subscriptions.push(provider);
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
        createClass('Regex', 'Regular Expression functions'),
        createClass('CSV', 'CSV parsing functions'),

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
