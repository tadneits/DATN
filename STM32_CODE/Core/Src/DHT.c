#include "DHT.h"

#define lineDown()      HAL_GPIO_WritePin(sensor->DHT_Port, sensor->DHT_Pin, GPIO_PIN_RESET)
#define lineUp()        HAL_GPIO_WritePin(sensor->DHT_Port, sensor->DHT_Pin, GPIO_PIN_SET)
#define getLine()       (HAL_GPIO_ReadPin(sensor->DHT_Port, sensor->DHT_Pin) == GPIO_PIN_SET)
#define Delay(d)        HAL_Delay(d)

static void goToOutput(DHT_sensor *sensor) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Mặc định, tại dòng dữ liệu có mức cao
    lineUp();

    // Cấu hình cổng là đầu ra
    GPIO_InitStruct.Pin = sensor->DHT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;    // Chế độ đầu ra mở
    GPIO_InitStruct.Pull = sensor->pullUp;         // Kéo lên nguồn cấp

    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // Tốc độ cao cho cổng
    HAL_GPIO_Init(sensor->DHT_Port, &GPIO_InitStruct);
}

static void goToInput(DHT_sensor *sensor) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Cấu hình cổng là đầu vào
    GPIO_InitStruct.Pin = sensor->DHT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = sensor->pullUp;         // Kéo lên nguồn cấp
    HAL_GPIO_Init(sensor->DHT_Port, &GPIO_InitStruct);
}

DHT_data DHT_getData(DHT_sensor *sensor) {
    DHT_data data = {-128.0f, -128.0f};
    
    #if DHT_POLLING_CONTROL == 1
    /* Giới hạn tần số truy vấn cảm biến */
    // Xác định khoảng thời gian truy vấn dựa trên loại cảm biến
    uint16_t pollingInterval;
    if (sensor->type == DHT11) {
        pollingInterval = DHT_POLLING_INTERVAL_DHT11;
    } else {
        pollingInterval = DHT_POLLING_INTERVAL_DHT22;
    }

    // Nếu khoảng thời gian nhỏ, trả về giá trị cuối cùng nếu nó tồn tại
    if ((HAL_GetTick() - sensor->lastPollingTime < pollingInterval) && sensor->lastPollingTime != 0) {
        data.do_am = sensor->lastHum;
        data.nhiet_do = sensor->lastTemp;
        return data;
    }
    sensor->lastPollingTime = HAL_GetTick() + 1;
    #endif

    /* Yêu cầu dữ liệu từ cảm biến */
    // Chuyển chân dữ liệu sang chế độ đầu ra
    goToOutput(sensor);
    // Đặt mức thấp dòng dữ liệu trong 18 ms
    lineDown();
    Delay(18);
    // Nâng dòng dữ liệu, chuyển cổng sang chế độ đầu vào
    lineUp();
    goToInput(sensor);

    #ifdef DHT_IRQ_CONTROL
    // Tắt ngắt để không gây xung đột dữ liệu
    __disable_irq();
    #endif

    /* Đợi phản hồi từ cảm biến */
    uint16_t timeout = 0;
    // Đợi cạnh xuống
    while (getLine()) {
        timeout++;
        if (timeout > DHT_TIMEOUT) {
            #ifdef DHT_IRQ_CONTROL
            __enable_irq();
            #endif
            // Nếu cảm biến không phản hồi, thì không có cảm biến
            // Đặt giá trị cuối cùng thành giá trị mặc định để tránh giá trị giả
            sensor->lastHum = -128.0f;
            sensor->lastTemp = -128.0f;
            return data;
        }
    }
    timeout = 0;
    // Đợi cạnh lên
    while (!getLine()) {
        timeout++;
        if (timeout > DHT_TIMEOUT) {
            #ifdef DHT_IRQ_CONTROL
            __enable_irq();
            #endif
            // Nếu cảm biến không phản hồi, thì không có cảm biến
            // Đặt giá trị cuối cùng thành giá trị mặc định để tránh giá trị giả
            sensor->lastHum = -128.0f;
            sensor->lastTemp = -128.0f;
            return data;
        }
    }
    timeout = 0;
    // Đợi cạnh xuống
    while (getLine()) {
        timeout++;
        if (timeout > DHT_TIMEOUT) {
            #ifdef DHT_IRQ_CONTROL
            __enable_irq();
            #endif
            return data;
        }
    }
    
    /* Đọc phản hồi từ cảm biến */
    uint8_t rawData[5] = {0, 0, 0, 0, 0};
    for (uint8_t a = 0; a < 5; a++) {
        for (uint8_t b = 7; b != 255; b--) {
            uint16_t hT = 0, lT = 0;
            // Trong khi dòng dữ liệu ở mức thấp, tăng lT
            while (!getLine() && lT != 65535) lT++;
            // Trong khi dòng dữ liệu ở mức cao, tăng hT
            timeout = 0;
            while (getLine() && hT != 65535) hT++;
            // Nếu hT lớn hơn lT, thì bit là 1
            if (hT > lT) rawData[a] |= (1 << b);
        }
    }

    #ifdef DHT_IRQ_CONTROL
    // Bật ngắt sau khi nhận dữ liệu
    __enable_irq();
    #endif

    /* Kiểm tra tính toàn vẹn của dữ liệu */
    if ((uint8_t)(rawData[0] + rawData[1] + rawData[2] + rawData[3]) == rawData[4]) {
        // Nếu giá trị kiểm soát trùng khớp, chuyển đổi và trả về các giá trị nhận được
        if (sensor->type == DHT22) {
            data.do_am = (float)(((uint16_t)rawData[0] << 8) | rawData[1]) * 0.1f;
            // Kiểm tra xem nhiệt độ có phải là số âm hay không
            if (!(rawData[2] & (1 << 7))) {
                data.nhiet_do = (float)(((uint16_t)rawData[2] << 8) | rawData[3]) * 0.1f;
            } else {
                rawData[2] &= ~(1 << 7);
                data.nhiet_do = (float)(((uint16_t)rawData[2] << 8) | rawData[3]) * -0.1f;
            }
        }
        if (sensor->type == DHT11) {
            data.do_am = (float)rawData[0];
            data.nhiet_do = (float)rawData[2];
        }
    }

    #if DHT_POLLING_CONTROL == 1
		sensor->lastTemp = data.nhiet_do;
    sensor->lastHum = data.do_am;
    #endif

    return data;    
}
