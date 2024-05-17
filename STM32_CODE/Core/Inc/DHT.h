#ifndef DHT_H_
#define DHT_H_

#include "main.h"

/* Cài đặt */
#define DHT_TIMEOUT 				10000	// Số lần lặp trước khi hàm trả về giá trị trống
#define DHT_POLLING_CONTROL			1		// Bật kiểm tra tần suất truy vấn cảm biến
#define DHT_POLLING_INTERVAL_DHT11	2000	// Khoảng thời gian truy vấn DHT11 (0.5 Hz theo tài liệu kỹ thuật). Bạn có thể đặt 1500 và vẫn hoạt động.
#define DHT_POLLING_INTERVAL_DHT22	1000	// Khoảng thời gian truy vấn DHT22 (1 Hz theo tài liệu kỹ thuật)
#define DHT_IRQ_CONTROL						// Tắt ngắt trong quá trình trao đổi dữ liệu với cảm biến
/* Cấu trúc dữ liệu trả về từ cảm biến */
typedef struct {
	float nhiet_do;
	float do_am;
} DHT_data;

/* Loại cảm biến */
typedef enum {
	DHT11,
	DHT22
} DHT_type;

/* Cấu trúc đối tượng cảm biến */
typedef struct {
	GPIO_TypeDef *DHT_Port;	// Cổng cảm biến (GPIOA, GPIOB, v.v.)
	uint16_t DHT_Pin;		// Số chân cảm biến (GPIO_PIN_0, GPIO_PIN_1, v.v.)
	DHT_type type;			// Loại cảm biến (DHT11 hoặc DHT22)
	uint8_t pullUp;			// Có cần kéo dòng dữ liệu lên nguồn cấp (GPIO_NOPULL - không, GPIO_PULLUP - có)

	// Kiểm soát tần suất truy vấn cảm biến. Đừng điền giá trị!
	#if DHT_POLLING_CONTROL == 1
	uint32_t lastPollingTime;// Thời gian truy vấn cuối cùng của cảm biến
	float lastTemp;			 // Giá trị nhiệt độ cuối cùng
	float lastHum;			 // Giá trị độ ẩm cuối cùng
	#endif
} DHT_sensor;

/* Hàm khởi tạo */

// DHT_sensor <tên cảm biến> = {GPIO?, GPIO_PIN_?, <Loại cảm biến>, (GPIO_NOPULL - Nếu đã có trở kéo lên) hoặc (GPIO_PULLUP - Nếu chưa có trở kéo lên)};

DHT_data DHT_getData(DHT_sensor *sensor); // Lấy dữ liệu từ cảm biến

#endif
