# IoT-trash-collector-boat

Dự án **thuyền thu gom rác ứng dụng IoT** nhằm thu gom rác nổi trên mặt nước, có khả năng điều khiển từ xa, giám sát thời gian thực và đảm bảo an toàn khi vận hành.


Sau khi dán xong, bấm **Commit changes...** (hoặc **Commit changes**) để lưu là hoàn tất.

## Tổng quan

Dự án xây dựng một **xe chữa cháy mini thông minh** trên sa bàn, có khả năng di chuyển, phát hiện dấu hiệu lửa trong phạm vi thử nghiệm và điều khiển bơm nước mô phỏng. Hệ thống được thiết kế theo kiến trúc IoT gồm ba phần:

- **Tầng thiết bị:** ESP32 điều khiển động cơ, đọc cảm biến và bơm nước.
- **Tầng mạng:** Kết nối Wi-Fi để gửi trạng thái và nhận lệnh điều khiển.
- **Tầng giám sát:** Giao diện web nội bộ để theo dõi và điều khiển xe.

Mục tiêu là minh họa cách phối hợp cảm biến, bộ điều khiển, cơ cấu chấp hành và kết nối IoT. Hệ thống mặc định ở trạng thái tắt, và mất kết nối Wi-Fi sẽ khiến xe và bơm dừng an toàn.

> ⚠️ Đây là mô hình giáo dục, không phải thiết bị chữa cháy chuyên dụng và không dùng để xử lý đám cháy thật.

---

## Poster dự án

<img width="543" height="796" alt="image" src="https://github.com/user-attachments/assets/ad9429b1-86b4-4262-a778-2360f92c6924" />

## Chức năng chính

- Điều khiển xe tiến, lùi, quay trái, quay phải và dừng
- Đọc cảm biến lửa và cảm biến khoảng cách, báo trạng thái khi phát hiện theo ngưỡng đã hiệu chỉnh
- Bật bơm theo lệnh có xác nhận, tự ngắt sau thời gian giới hạn
- Giám sát trạng thái và gửi lệnh qua giao diện web nội bộ
- Tự dừng động cơ và bơm khi mất kết nối (timeout)

---

## Công nghệ sử dụng

- **Vi điều khiển:** ESP32 DevKit (Wi-Fi/Bluetooth)
- **Lập trình:** Arduino IDE với Arduino framework cho ESP32
- **Cảm biến:** Module cảm biến lửa (ngõ ra số), HC-SR04 (siêu âm)
- **Truyền động:** 2 động cơ DC, driver cầu H (L298N hoặc tương đương), bơm DC mini đóng cắt qua MOSFET
- **Giao tiếp:** Wi-Fi, dữ liệu định dạng JSON
- **Giao diện:** Web Dashboard nội bộ

---

## Đóng góp

| Họ và tên | Vai trò |
|-----------|---------|
| Bùi Anh Tú | Tổng hợp yêu cầu, cơ sở lý thuyết, kiến trúc hệ thống, lựa chọn linh kiện |
| Lê Thế Duy | Thiết kế nối dây, lập trình ESP32/Arduino, tích hợp cảm biến, động cơ, bơm |
| Hoàng Xuân Hoàn | Lắp ráp cơ khí, kiểm thử an toàn, nhật ký thử nghiệm, báo cáo và slide |
