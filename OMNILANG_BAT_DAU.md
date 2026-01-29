# Hướng Dẫn Sử Dụng OmniLang
*(Dành cho lập trình viên và người mới bắt đầu)*

Chào mừng bạn đến với **OmniLang**! Đây là ngôn ngữ lập trình được thiết kế để đơn giản, dễ đọc nhưng vẫn đầy đủ tính năng mạnh mẽ. Tài liệu này sẽ giúp bạn nắm bắt OmniLang từ con số 0.

## 1. Cài đặt & Chạy chương trình

OmniLang hoạt động rất đơn giản. Bạn chỉ cần file `omni.exe` và một file mã nguồn đuôi `.omni`.

**Cách chạy:**
Mở cmd hoặc PowerShell và gõ:
```bash
omni.exe ten_file_cua_ban.omni
```

**Mẹo:** Để có gợi ý code (Intellisense), hãy cài đặt Extension `vscode-omni` vào Visual Studio Code (xem hướng dẫn trong thư mục `vscode-omni`).

---

## 2. Những Khái Niệm Cơ Bản

### 2.1. Biến (Variables)
Trong OmniLang, bạn không cần khai báo kiểu dữ liệu (như `int`, `string`). Cứ gán là chạy.
```omni
x = 10              # Số nguyên
ten = "OmniLang"    # Chuỗi ký tự (Văn bản)
Diem = 9.5          # Số thực
KiemTra = true      # Logic (Đúng/Sai)
DanhSach = [1, 2]   # Mảng (List)
```

### 2.2. Bình luận (Comments)
Dùng dấu `#` để viết ghi chú (máy tính sẽ bỏ qua dòng này).
```omni
# Đây là dòng ghi chú
print("Xin chao") # Ghi chú ở cuối dòng cũng được
```

### 2.3. Nhập & Xuất dữ liệu
Giao tiếp với người dùng qua màn hình đen (Console).

*   `print(...)`: In ra màn hình.
*   `input(...)`: Hỏi người dùng nhập vào.

```omni
ten = input("Nhập tên của bạn: ")
print("Xin chào", ten)
```

---

## 3. Cấu trúc điều khiển (Logic)

### 3.1. Nếu... Thì... (If - Elif - Else)
Giống như văn nói. Lưu ý: **phải thụt đầu dòng** (dùng phím Tab) cho các lệnh bên trong.
```omni
diem = 8

if diem >= 9:
    print("Xuất sắc!")
elif diem >= 7:
    print("Khá tốt.")
else:
    print("Cần cố gắng hơn.")
```

### 3.2. Vòng lặp (Loops)
Khi muốn máy tính làm đi làm lại một việc.

**Lặp với `while` (Khi điều kiện còn đúng thì còn làm):**
```omni
i = 0
while i < 5:
    print("Lần thứ:", i)
    i = i + 1
```

**Lặp với `for` (Lặp theo số lần định trước):**
```omni
# Chạy i từ 0 đến 4 (5 lần)
for i in range(5):
    print("Số:", i)
```

---

## 4. Hàm (Functions) - Tạo công cụ riêng
Gom nhóm các lệnh hay dùng thành một "công cụ" để tái sử dụng.
```omni
# Định nghĩa hàm
def TinhTong(a, b):
    ketQua = a + b
    return ketQua

# Sử dụng hàm
x = TinhTong(5, 10)
print(x) # Kết quả: 15
```

---

## 5. Thư viện có sẵn (Standard Library)
OmniLang cung cấp sẵn rất nhiều công cụ mạnh mẽ. Bạn không cần cài thêm gì cả.

### 5.1. Toán Học (Math)
Dùng cho các tính toán số liệu.

| Hàm | Giải thích | Ví dụ |
|-----|------------|-------|
| `Math.abs(x)` | Trị tuyệt đối (làm dương số). | `Math.abs(-5)` -> `5` |
| `Math.sqrt(x)` | Căn bậc 2. | `Math.sqrt(9)` -> `3` |
| `Math.pow(a, b)` | Lũy thừa (a mũ b). | `Math.pow(2, 3)` -> `8` |
| `Math.round(x)` | Làm tròn số. | `Math.round(4.6)` -> `5` |
| `Math.floor(x)` | Làm tròn xuống. | `Math.floor(4.9)` -> `4` |
| `Math.ceil(x)` | Làm tròn lên. | `Math.ceil(4.1)` -> `5` |
| `Math.random()` | Sinh số ngẫu nhiên (từ 0 đến 1). | `r = Math.random()` |
| `Math.max(a, b)` | Tìm số lớn hơn. | `Math.max(1, 9)` -> `9` |
| `Math.min(a, b)` | Tìm số nhỏ hơn. | `Math.min(1, 9)` -> `1` |
| `Math.sin/cos/tan`| Lượng giác. | `Math.sin(0)` |

### 5.2. Xử lý Chuỗi (String)
Dùng để thao tác với văn bản.

| Hàm | Giải thích | Ví dụ |
|-----|------------|-------|
| `len(s)` | Đếm số ký tự. | `len("abc")` -> `3` |
| `String.toUpperCase(s)` | Chuyển thành CHỮ HOA. | `String.toUpperCase("a")` -> `"A"` |
| `String.toLowerCase(s)` | Chuyển thành chữ thường. | `String.toLowerCase("A")` -> `"a"` |
| `String.trim(s)` | Xóa khoảng trắng thừa 2 đầu. | `String.trim("  abc  ")` -> `"abc"` |
| `String.contains(s, sub)` | Kiểm tra có chứa từ không. | `String.contains("Hello", "ell")` -> `true` |
| `String.replace(s, cu, moi)`| Thay thế từ. | `String.replace("Hi a", "a", "b")` -> `"Hi b"` |
| `String.split(s, dau)` | Cắt chuỗi thành mảng. | `String.split("a,b,c", ",")` -> `["a","b","c"]` |
| `String.format(...)` | Định dạng chuỗi (giống C/Java). | `String.format("Chào %s", "Nam")` |
| `String.parseInt(s)` | Đổi chuỗi thành số nguyên. | `Integer.parseInt("123")` |
| `String.parseDouble(s)`| Đổi chuỗi thành số thực. | `Double.parseDouble("12.5")` |

### 5.3. Danh sách (List / Array)
Lưu trữ nhiều giá trị trong một biến.

| Hàm | Giải thích | Ví dụ |
|-----|------------|-------|
| `List.new()` | Tạo danh sách rỗng. | `ds = List.new()` hoặc `ds = []` |
| `List.add(ds, pt)` | Thêm phần tử vào cuối. | `ds = List.add(ds, "Cam")` |
| `List.get(ds, i)` | Lấy phần tử tại vị trí i (từ 0). | `qua = List.get(ds, 0)` |
| `List.set(ds, i, v)` | Sửa phần tử tại vị trí i. | `List.set(ds, 0, "Tao")` |
| `List.remove(ds, i)` | Xóa phần tử tại vị trí i. | `List.remove(ds, 0)` |
| `List.size(ds)` | Xem danh sách có bao nhiêu cái. | `n = List.size(ds)` |
| `List.contains(ds, v)`| Kiểm tra có chứa giá trị v không. | `coKhong = List.contains(ds, "Tao")` |

### 5.4. Đọc/Ghi File
Lưu dữ liệu ra ổ cứng.

| Hàm | Giải thích | Ví dụ |
|-----|------------|-------|
| `File.write(path, content)` | Ghi đè nội dung vào file. | `File.write("data.txt", "Hello")` |
| `File.append(path, content)`| Ghi nối tiếp vào cuối file. | `File.append("log.txt", "Lỗi...\n")` |
| `File.read(path)` | Đọc toàn bộ file. | `noiDung = File.read("data.txt")` |
| `File.exists(path)` | Kiểm tra file có tồn tại không. | `if File.exists("data.txt"):` |

### 5.5. Lưu trữ dữ liệu phức tạp (JSON)
Dùng để lưu danh sách khách hàng, đơn hàng, v.v.

| Hàm | Giải thích | Ví dụ |
|-----|------------|-------|
| `Serializer.saveJSON(path, data)` | Lưu biến (biến bất kỳ) ra file JSON. | `Serializer.saveJSON("khach.json", dsKhach)` |
| `Serializer.loadJSON(path)` | Đọc file JSON lên thành biến. | `dsKhach = Serializer.loadJSON("khach.json")` |

### 5.6. Ngày tháng (Date)
| Hàm | Giải thích | Ví dụ |
|-----|------------|-------|
| `Date.now()` | Lấy thời gian hiện tại (số). | `now = Date.now()` |
| `Date.format(t, fmt)` | Đổi thành chuỗi ngày tháng. | `Date.format(now, "dd/MM/yyyy")` |
| `Date.parse(s, fmt)` | Đổi chuỗi ngày tháng thành số. | `t = Date.parse("25/12/2026")` |

### 5.7. Biểu thức chính quy (Regex)
Dùng để kiểm tra định dạng (ví dụ: email, số điện thoại).

| Hàm | Giải thích | Ví dụ |
|-----|------------|-------|
| `Regex.matches(s, pat)` | Kiểm tra chuỗi khớp mẫu. | Khớp số: `Regex.matches("123", "^[0-9]+$")` |
| `Regex.search(s, pat)` | Tìm kiếm trong chuỗi. | |

---

## 6. Ví dụ thực tế

### Chương trình tính tổng 2 số
```omni
print("--- CHƯƠNG TRÌNH TÍNH TỔNG ---")
s1 = input("Nhập số thứ nhất: ")
s2 = input("Nhập số thứ hai: ")

# Chuyển chuỗi nhập vào thành số
n1 = Integer.parseInt(s1)
n2 = Integer.parseInt(s2)

tong = n1 + n2
printf("Tổng của %d và %d là: %d\n", n1, n2, tong)
```

Chúc bạn lập trình vui vẻ với OmniLang!
