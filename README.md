# Dự án OmniLang

Đây là kho mã nguồn chính thức của **OmniLang** - Ngôn ngữ lập trình hiện đại, đơn giản và mạnh mẽ.

## 📚 Tài Liệu Hướng Dẫn

Để bắt đầu, bạn hãy xem các tài liệu sau:

1.  **[OMNILANG_BAT_DAU.md](./OMNILANG_BAT_DAU.md)** 👈 **(Đọc cái này trước)**
    *   Tài liệu tiếng Việt dành cho người mới.
    *   Hướng dẫn cài đặt, cú pháp cơ bản và ví dụ dễ hiểu.

2.  **[OmniLang/OMNILANG_GUIDE.md](./OmniLang/OMNILANG_GUIDE.md)**
    *   Tài liệu tham khảo đầy đủ (tiếng Anh).
    *   Tra cứu chi tiết các hàm và tính năng nâng cao.

## 📂 Cấu Trúc Thư Mục

*   `OmniLang/`: Mã nguồn chính của bộ biên dịch (Compiler/Interpreter) viết bằng C++.
    *   `src/`: Source code C++ (Parser, Lexer, StdLib...).
    *   `vscode-omni/`: Extension hỗ trợ code OmniLang trên VS Code (Intellisense).
    *   `examples/`: Các ví dụ mẫu.
*   `java_port_test/`: Các bài test thực tế và ứng dụng demo.
    *   Thử nghiệm chuyển đổi từ Java sang OmniLang.
    *   Chứa `Main.omni` (Chương trình Quản lý Đặt tiệc).

## 🚀 Tính Năng Nổi Bật

*   **Cú pháp đơn giản**: Dễ đọc, dễ viết (giống Python).
*   **Thư viện chuẩn phong phú**: Hỗ trợ sẵn Toán, Chuỗi, File, JSON, Regex, CSV...
*   **Mạnh mẽ**: Hỗ trợ lập trình hướng đối tượng (Class), hàm (Function), và các cấu trúc dữ liệu mạng (List, Map).
*   **Công cụ hỗ trợ**: Có Extension VS Code riêng để gợi ý code.

## 🛠 Cách Cài Đặt Extension VS Code

1.  Vào thư mục `OmniLang/vscode-omni`.
2.  Chạy lệnh `vsce package` để tạo file `.vsix` (hoặc dùng file có sẵn).
3.  Cài đặt file `.vsix` vào VS Code để có tính năng gợi ý code thông minh.

---
*Dự án được phát triển bởi Longurara (2026).*
