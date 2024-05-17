#include "BH1750.h"

I2C_HandleTypeDef *i2c_bh1750; // Tham chiếu đối tượng I2C
BH1750_DATA BH1750;

void BH1750_Cmd_Write(uint8_t cmd)
{
	uint8_t data[1];
	data[0] = cmd;
	HAL_I2C_Master_Transmit(i2c_bh1750, BH1750_Addr_L, data, 1, BH1750.time);
	__HAL_I2C_CLEAR_FLAG(i2c_bh1750, I2C_FLAG_STOPF);
	HAL_Delay(100);
}

void BH1750_Init(I2C_HandleTypeDef *handle)
{
	i2c_bh1750 = handle;
	BH1750.time = 120; // Thời gian đợi cho I2C
	BH1750.mode = BH_CON_H_MODE; // Chế độ mặc định: 1 đo / 1 giây
	BH1750.status = GOOG; // Trạng thái mặc định: thành công
	BH1750_Cmd_Write(BH1750_ON);   // Bật nguồn
	BH1750_Cmd_Write(BH1750_RESET);  // Xóa bộ đếm
	BH1750_Cmd_Write(BH1750.mode);   // Thiết lập chế độ đo
	HAL_Delay(100);
}

uint16_t BH1750_Read(void)
{   
	uint8_t data_re[2] = {0, 0};
	HAL_I2C_Master_Receive(i2c_bh1750, BH1750_Addr_L, data_re, 2, BH1750.time * 2);
	HAL_Delay(100);
	BH1750.result_lx = data_re[0];
	BH1750.result_lx = (data_re[0] << 8) + data_re[1];  // Ghép hai byte dữ liệu nhận được
	BH1750.result_lx = BH1750.result_lx / 1.2; // Chuyển đổi kết quả thành đơn vị Lux
	HAL_Delay(100);
	return BH1750.result_lx;
}
