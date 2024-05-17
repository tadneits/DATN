#ifndef _BH1750_H_
#define _BH1750_H_

#include "stm32f4xx_hal.h"

#define BH1750_Addr_H 0x5C // Địa chỉ I2C của BH1750 khi chân ADDR được nối đất (0x5C)
#define BH1750_Addr_L 0x46 // Địa chỉ I2C của BH1750 khi chân ADDR được nối VCC (0x46)

#define GOOG   						0x00 // Giá trị trả về thành công
#define FAIL   						0x01 // Giá trị trả về thất bại

#define BH1750_DOWN   		0x00 // Đưa BH1750 vào trạng thái chờ
#define BH1750_ON					0x01 // Đưa BH1750 vào trạng thái hoạt động
#define BH1750_RESET			0x07 // Làm mới BH1750

#define BH_CON_H_MODE			0x10 // Chế độ cao (High) 1 đo / 1 giây
#define BH_CON_H_MODE2		0x11 // Chế độ cao (High) 0.5 đo / 1 giây
#define BH_CON_L_MODE			0x13 // Chế độ thấp (Low) 4 đo / 1 giây
#define BH_ONE_H_MODE			0x20 // Chế độ đo 1 lần cao (High) chất lượng cao
#define BH_ONE_H_MODE2		0x21 // Chế độ đo 1 lần cao (High) chất lượng thấp
#define BH_ONE_L_MODE			0x23 // Chế độ đo 1 lần thấp (Low)

typedef struct
{
	uint8_t 	mode;       // Chế độ đo
	uint32_t	time;       // Thời gian đo
	uint16_t	data_re[2]; // Dữ liệu đọc được từ BH1750 (2 byte)
	uint16_t	result_lx;  // Kết quả đo được (đơn vị Lux)
	uint8_t		status;     // Trạng thái hoạt động
} BH1750_DATA;

void BH1750_Init(I2C_HandleTypeDef *handle);  			   // Khởi tạo BH1750
uint16_t BH1750_Read(void);          // Đọc giá trị ánh sáng từ BH1750

#endif
