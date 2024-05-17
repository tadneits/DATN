#include "LCD.h"
#include "stdio.h"

I2C_HandleTypeDef *i2c_lcd; // Biến I2C external để sử dụng trong thư viện.

#define Dia_chi_LCD 0x4E // Địa chỉ I2C của màn hình LCD.

void LCD_ghi_lenh(char lenh)
{
    char du_lieu_u, du_lieu_l;
    uint8_t du_lieu_t[4];

    // Chia lệnh thành các phần trên 4 bit.
    du_lieu_u = (lenh & 0xf0);
    du_lieu_l = ((lenh << 4) & 0xf0);

    // Tạo các giao tiếp I2C tương ứng cho mỗi phần.
    du_lieu_t[0] = du_lieu_u | 0x0C; // en=1, rs=0
    du_lieu_t[1] = du_lieu_u | 0x08; // en=0, rs=0
    du_lieu_t[2] = du_lieu_l | 0x0C; // en=1, rs=0
    du_lieu_t[3] = du_lieu_l | 0x08; // en=0, rs=0

    // Gửi dữ liệu qua giao tiếp I2C.
    HAL_I2C_Master_Transmit(i2c_lcd, Dia_chi_LCD, (uint8_t *)du_lieu_t, 4, 100);
}

void LCD_ghi_du_lieu(char du_lieu)
{
    char du_lieu_u, du_lieu_l;
    uint8_t du_lieu_t[4];

    // Chia dữ liệu thành các phần trên 4 bit.
    du_lieu_u = (du_lieu & 0xf0);
    du_lieu_l = ((du_lieu << 4) & 0xf0);

    // Tạo các giao tiếp I2C tương ứng cho mỗi phần.
    du_lieu_t[0] = du_lieu_u | 0x0D; // en=1, rs=1
    du_lieu_t[1] = du_lieu_u | 0x09; // en=0, rs=1
    du_lieu_t[2] = du_lieu_l | 0x0D; // en=1, rs=1
    du_lieu_t[3] = du_lieu_l | 0x09; // en=0, rs=1

    // Gửi dữ liệu qua giao tiếp I2C.
    HAL_I2C_Master_Transmit(i2c_lcd, Dia_chi_LCD, (uint8_t *)du_lieu_t, 4, 100);
}

void LCD_Init(I2C_HandleTypeDef *handle)
{
    // Khởi tạo màn hình LCD bằng cách gửi các lệnh tương ứng.
		i2c_lcd = handle;
    LCD_ghi_lenh(0x03);
    HAL_Delay(50);
    LCD_ghi_lenh(0x02);
    HAL_Delay(50);
    LCD_ghi_lenh(0x06);
    HAL_Delay(50);
    LCD_ghi_lenh(0x0C);
    HAL_Delay(50);
    LCD_ghi_lenh(0x28);
    HAL_Delay(50);
    LCD_ghi_lenh(0x80);
    HAL_Delay(50);
    LCD_ghi_lenh(0x01);
    HAL_Delay(50);
}

void LCD_ghi_chuoi(char *str)
{
    while (*str)
        LCD_ghi_du_lieu(*str++);
}

void LCD_xoa_man_hinh(void)
{
    // Gửi lệnh xóa màn hình.
    LCD_ghi_lenh(0x01);
}

void LCD_xy(int hang, int cot) {
    //int offset[] = {0x80, 0xC0, 0x94, 0xD4}; // LCD 2004
		int offset[] = {0x80, 0xC0, 0x90, 0xD0}; // LCD160X
    if (hang >= 0 && hang < 4 && cot >= 0 && cot < 20) {
        LCD_ghi_lenh(0x80 | (offset[hang] + cot));
    }
}

void LCD_ghi_so_thuc(float so_thuc, int so_chu_so_thap_phan)
{
    char str[16];
    // Sử dụng snprintf để định dạng số thực và ghi lên màn hình LCD.
    snprintf(str, sizeof(str), "%.*f", so_chu_so_thap_phan, so_thuc);
    LCD_ghi_chuoi(str);
}
