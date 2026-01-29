# OmniLang Project (2026)

[English](#english) | [Русский](#russian) | [Tiếng Việt](#vietnamese)

---

<a name="english"></a>
## 🇺🇸 English

This is the official repository of **OmniLang** - A modern, simple, and powerful programming language.

### 📚 Documentation

To get started, please check the following documents:

1.  **[OMNILANG_BAT_DAU.md](./OMNILANG_BAT_DAU.md)** (Vietnamese beginner guide)
2.  **[OMNILANG_START_RU.md](./OMNILANG_START_RU.md)** (Russian beginner guide)
    *   Installation guide, basic syntax, and easy-to-understand examples.
3.  **[OmniLang/OMNILANG_GUIDE.md](./OmniLang/OMNILANG_GUIDE.md)**
    *   Full reference documentation (English).
    *   Detailed lookup for functions and advanced features.

### 📂 Directory Structure

*   `OmniLang/`: Main source code of the compiler/interpreter written in C++.
    *   `src/`: C++ source code (Parser, Lexer, StdLib...).
    *   `vscode-omni/`: VS Code extension for OmniLang support (Intellisense).
    *   `examples/`: Sample code examples.
*   `java_port_test/`: Real-world tests and demo applications.
    *   Experiments porting code from Java to OmniLang.
    *   Contains `Main.omni` (Feast Order Management System).

### 🚀 Key Features

*   **Simple Syntax**: Easy to read and write (Python-like).
*   **Rich Standard Library**: Built-in support for Math, Strings, Files, JSON, Regex, CSV...
*   **Powerful**: Supports Object-Oriented Programming (Class), Functions, and data structures (List, Map).
*   **Tooling**: Dedicated VS Code Extension for code completion.

### 🛠 VS Code Extension Installation

1.  Go to `OmniLang/vscode-omni` directory.
2.  Run `vsce package` to create a `.vsix` file (or use the existing one).
3.  Install the `.vsix` file into VS Code for intelligent code suggestions.

*Project developed by Longurara (2026).*

---

<a name="russian"></a>
## 🇷🇺 Русский

Это официальный репозиторий **OmniLang** - современного, простого и мощного языка программирования.

### 📚 Документация

Для начала ознакомьтесь со следующими документами:

1.  **[OMNILANG_START_RU.md](./OMNILANG_START_RU.md)** 👈 **(Руководство для начинающих)**
    *   Руководство по установке, базовый синтаксис и простые примеры.
2.  **[OmniLang/OMNILANG_GUIDE.md](./OmniLang/OMNILANG_GUIDE.md)**
    *   Полная справочная документация (на английском).
    *   Подробное описание функций и продвинутых возможностей.

### 📂 Структура каталогов

*   `OmniLang/`: Основной исходный код компилятора/интерпретатора на C++.
    *   `src/`: Исходный код C++ (Parser, Lexer, StdLib...).
    *   `vscode-omni/`: Расширение для VS Code (Intellisense).
    *   `examples/`: Примеры кода.
*   `java_port_test/`: Реальные тесты и демо-приложения.
    *   Эксперименты по портированию кода с Java на OmniLang.
    *   Содержит `Main.omni` (Система управления заказами банкетов).

### 🚀 Ключевые особенности

*   **Простой синтаксис**: Легко читать и писать (похож на Python).
*   **Богатая стандартная библиотека**: Встроенная поддержка математики, строк, файлов, JSON, Regex, CSV...
*   **Мощность**: Поддержка ООП (классы), функций и структур данных (списки, карты).
*   **Инструменты**: Собственное расширение для VS Code с автодополнением кода.

### 🛠 Установка расширения VS Code

1.  Перейдите в папку `OmniLang/vscode-omni`.
2.  Запустите `vsce package`, чтобы создать файл `.vsix` (или используйте существующий).
3.  Установите файл `.vsix` в VS Code для умных подсказок кода.

*Проект разработан Longurara (2026).*

---

<a name="vietnamese"></a>
## 🇻🇳 Tiếng Việt

Đây là kho mã nguồn chính thức của **OmniLang** - Ngôn ngữ lập trình hiện đại, đơn giản và mạnh mẽ.

### 📚 Tài Liệu Hướng Dẫn

Để bắt đầu, bạn hãy xem các tài liệu sau:

1.  **[OMNILANG_BAT_DAU.md](./OMNILANG_BAT_DAU.md)** 👈 **(Đọc cái này trước)**
    *   Tài liệu tiếng Việt dành cho người mới.
    *   Hướng dẫn cài đặt, cú pháp cơ bản và ví dụ dễ hiểu.

2.  **[OmniLang/OMNILANG_GUIDE.md](./OmniLang/OMNILANG_GUIDE.md)**
    *   Tài liệu tham khảo đầy đủ (tiếng Anh).
    *   Tra cứu chi tiết các hàm và tính năng nâng cao.

### 📂 Cấu Trúc Thư Mục

*   `OmniLang/`: Mã nguồn chính của bộ biên dịch (Compiler/Interpreter) viết bằng C++.
    *   `src/`: Source code C++ (Parser, Lexer, StdLib...).
    *   `vscode-omni/`: Extension hỗ trợ code OmniLang trên VS Code (Intellisense).
    *   `examples/`: Các ví dụ mẫu.
*   `java_port_test/`: Các bài test thực tế và ứng dụng demo.
    *   Thử nghiệm chuyển đổi từ Java sang OmniLang.
    *   Chứa `Main.omni` (Chương trình Quản lý Đặt tiệc).

### 🚀 Tính Năng Nổi Bật

*   **Cú pháp đơn giản**: Dễ đọc, dễ viết (giống Python).
*   **Thư viện chuẩn phong phú**: Hỗ trợ sẵn Toán, Chuỗi, File, JSON, Regex, CSV...
*   **Mạnh mẽ**: Hỗ trợ lập trình hướng đối tượng (Class), hàm (Function), và các cấu trúc dữ liệu mạng (List, Map).
*   **Công cụ hỗ trợ**: Có Extension VS Code riêng để gợi ý code.

### 🛠 Cách Cài Đặt Extension VS Code

1.  Vào thư mục `OmniLang/vscode-omni`.
2.  Chạy lệnh `vsce package` để tạo file `.vsix` (hoặc dùng file có sẵn).
3.  Cài đặt file `.vsix` vào VS Code để có tính năng gợi ý code thông minh.

*Dự án được phát triển bởi Longurara (2026).*
