# 📝 Hướng dẫn Ôn tập Phỏng vấn (Bare-Metal STM32)

Tài liệu này lưu trữ các câu hỏi phỏng vấn thực tế và checklist tự ôn tập cho dự án bare-metal này. Hãy đảm bảo bạn có thể tự trả lời trôi chảy trước khi bước vào buổi phỏng vấn tuyển dụng.

---

## 💡 Câu hỏi phỏng vấn cốt lõi & Cách trả lời

### 1. Tại sao phải bật RCC clock trước khi cấu hình bất kỳ ngoại vi nào?
* **Trả lời**: Trên dòng STM32, STMicroelectronics thiết kế ngắt clock của tất cả ngoại vi mặc định sau khi reset để tối ưu hóa năng lượng tiêu thụ. Nếu không bật clock cho ngoại vi tương ứng trong thanh ghi RCC (ví dụ: `Rcc_AHB1ENR` cho GPIOA/GPIOB, `Rcc_APB2ENR` cho USART1/SPI1), việc ghi đè lên các thanh ghi cấu hình của ngoại vi đó sẽ hoàn toàn vô hiệu (thanh ghi không nhận giá trị).

### 2. Từ khóa `volatile` dùng làm gì trong các định nghĩa thanh ghi?
* **Trả lời**: Từ khóa `volatile` báo cho trình biên dịch (compiler) biết rằng giá trị của thanh ghi này có thể thay đổi bất kỳ lúc nào bởi phần cứng ngoài tầm kiểm soát của chương trình C. Compiler sẽ không tối ưu hóa các lệnh đọc/ghi thanh ghi này (không lưu giá trị vào thanh ghi CPU tạm thời mà bắt buộc phải đọc trực tiếp từ địa chỉ RAM/Register vật lý mỗi khi được gọi).
* **Ví dụ**: Nếu không có `volatile` trong vòng lặp chờ cờ trạng thái như `while (!(SPI1_SR & SPI_SR_TXE));`, compiler có thể tối ưu hóa và biến nó thành vòng lặp vô hạn vì nó nghĩ giá trị `SPI1_SR` không đổi.

### 3. Tại sao I2C dùng chân Open-Drain, còn SPI thì sử dụng Push-Pull?
* **I2C (Open-Drain)**: I2C là giao tiếp bus đa điểm (nhiều master, nhiều slave cùng dùng chung đường dây). Cấu hình Open-Drain giúp các chip chỉ có thể chủ động kéo đường dây xuống mức 0 (GND). Khi không giao tiếp, đường dây tự động kéo lên mức 1 thông qua điện trở kéo lên (pull-up resistors) bên ngoài. Điều này tránh hiện tượng ngắn mạch (chập điện) nếu một chip kéo lên 1 và chip khác kéo xuống 0 cùng một lúc.
* **SPI (Push-Pull)**: SPI sử dụng các đường truyền chuyên biệt point-to-point (một chân phát nối trực tiếp một chân nhận, ví dụ MOSI ──▶ MOSI). Không có sự chia sẻ dây chung của nhiều chân phát cùng lúc (trừ đường MISO nhưng được điều khiển bằng chân CS), do đó sử dụng Push-Pull giúp tốc độ sườn lên/xuống cực nhanh, truyền được ở tần số rất cao (lên đến hàng chục MHz).

### 4. Giải thích trình tự đọc thanh ghi I2C cho LM75. Tại sao phải dùng Repeated START?
* **Trình tự**:
  1. Tạo điều kiện **START**.
  2. Gửi địa chỉ Slave + bit ghi (0) để thiết lập kết nối ghi.
  3. Chờ cờ **ADDR** (chứng tỏ slave đã phản hồi ACK). Xóa cờ ADDR bằng cách đọc thanh ghi `SR1` rồi đến `SR2`.
  4. Gửi địa chỉ thanh ghi cần đọc (0x00 - thanh ghi nhiệt độ). Chờ dữ liệu truyền đi hoàn tất.
  5. Phát tín hiệu **Repeated START** (không được dùng STOP rồi START lại vì trên bus I2C đa master, việc dùng STOP sẽ giải phóng bus, thiết bị khác có thể nhảy vào chiếm quyền trước khi ta kịp đọc).
  6. Gửi địa chỉ Slave + bit đọc (1). Chờ cờ ADDR và xóa cờ.
  7. Bật bit ACK để nhận byte thứ nhất (MSB), lưu vào biến.
  8. Tắt bit ACK (gửi NACK) và tạo tín hiệu **STOP** trước khi đọc byte cuối cùng (LSB) theo đúng khuyến nghị của ST để báo hiệu kết thúc truyền.
  9. Đọc byte thứ hai (LSB) từ thanh ghi dữ liệu.

### 5. Tại sao BMP280 phải đọc "hệ số hiệu chỉnh" (Calibration Coefficients)?
* **Trả lời**: BMP280 là cảm biến đo áp suất MEMS cực kỳ nhạy cảm với sai lệch do môi trường và sản xuất. Số liệu từ ADC là số liệu thô (raw ADC values). Trong quá trình sản xuất tại nhà máy, mỗi chip BMP280 đều được đo đạc và lưu sẵn 12 tham số hiệu chỉnh riêng biệt vào ROM (địa chỉ 0x88 đến 0x9F). Ta bắt buộc phải đọc 24 byte dữ liệu hiệu chỉnh này, sau đó áp dụng vào công thức hiệu chỉnh chuẩn của Bosch (sử dụng toán học số nguyên 32-bit/64-bit) để tính ra nhiệt độ (°C) và áp suất (Pa) thực tế chính xác.

---

## 📝 Checklist Ôn Tập Tự Đánh Giá
- [ ] Tôi tự giải thích được cách tính giá trị thanh ghi `BRR` cho USART1 chạy baudrate 9600.
- [ ] Tôi giải thích được mục đích chân `CS` (Chip Select) của SPI và tại sao phải kéo xuống 0 lúc giao tiếp.
- [ ] Tôi biết cách chuyển đổi giá trị nhiệt độ thô 11-bit của LM75 về giá trị thực bằng phép dịch bit (`raw >> 7`) và nhân với độ nhạy (`0.5°C`).
