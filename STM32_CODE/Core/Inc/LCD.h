#ifndef LCD_H
#define LCD_H

#include "stm32f4xx_hal.h"

// Hàm ghi lệnh lên màn hình LCD
void LCD_ghi_lenh(char lenh);

// Hàm ghi dữ liệu lên màn hình LCD
void LCD_ghi_du_lieu(char du_lieu);

// Hàm khởi tạo màn hình LCD
void LCD_Init(I2C_HandleTypeDef *handle);

// Hàm ghi một chuỗi ký tự lên màn hình LCD
void LCD_ghi_chuoi(char *str);

// Hàm xóa nội dung trên màn hình LCD
void LCD_xoa_man_hinh(void);

// Hàm di chuyển con trỏ đến vị trí hàng và cột cụ thể trên màn hình LCD
void LCD_xy(int hang, int cot);

// Hàm ghi một số thực lên màn hình LCD
void LCD_ghi_so_thuc(float so_thuc, int so_chu_so_thap_phan);

#endif
