// Cấu hình Blynk
#define BLYNK_TEMPLATE_ID "TMPL6ZSt0Ei44"
#define BLYNK_TEMPLATE_NAME "Hệ thống giám sát môi trường trong nhà kính"
#define BLYNK_AUTH_TOKEN "Z4ajWTa_wC_UWV8FXDUv0MacpVF6z3Q0"
#define BLYNK_PRINT Serial

#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Cấu hình OLED Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Cấu hình UART
#define RX 0
#define TX 2
SoftwareSerial uart(RX, TX);

// Cấu hình WiFi
char SSID[] = "DATN";
char PASS[] = "123456789";

// Biến
int mode, cuong_do_sang, trang_thai_thiet_bi, tb1, tb2, tb3, tb4;
float nhiet_do, do_am, do_am_dat;

// Blynk Timer
BlynkTimer timer;

// Hàm gửi dữ liệu đến Blynk
void myTimerEvent() {
  Blynk.virtualWrite(V0, nhiet_do);
  Blynk.virtualWrite(V1, do_am);
  Blynk.virtualWrite(V2, do_am_dat);
  Blynk.virtualWrite(V3, cuong_do_sang);
  if (mode == 2) {
    Blynk.virtualWrite(V4, tb1);
    Blynk.virtualWrite(V5, tb2);
    Blynk.virtualWrite(V6, tb3);
    Blynk.virtualWrite(V7, tb4);
  }
  else {
    Blynk.virtualWrite(V4, trang_thai_thiet_bi & 1);
    Blynk.virtualWrite(V5, (trang_thai_thiet_bi >> 1) & 1);
    Blynk.virtualWrite(V6, (trang_thai_thiet_bi >> 2) & 1);
    Blynk.virtualWrite(V7, (trang_thai_thiet_bi >> 3) & 1);
  }
}

// Hàm nhận và phân tích dữ liệu từ UART
void Nhan_UART() {
  if (uart.available() > 0) {
    String input = uart.readStringUntil('\n');
    sscanf(input.c_str(), "%d %f %f %f %d %d", &mode, &nhiet_do, &do_am, &do_am_dat, &cuong_do_sang, &trang_thai_thiet_bi);

    // Đầu ra Debugging
    // Serial.print("mode: "); Serial.print(mode);
    // Serial.print(", nhiet_do: "); Serial.print(nhiet_do);
    // Serial.print(", do_am: "); Serial.print(do_am);
    // Serial.print(", do_am_dat: "); Serial.print(do_am_dat);
    // Serial.print(", cuong_do_sang: "); Serial.print(cuong_do_sang);
    // Serial.print(", trang_thai_thiet_bi: "); Serial.println(trang_thai_thiet_bi);
  }
}

void Truyen_UART() {
  uart.printf("%d %d %d %d\n", tb1, tb2, tb3, tb4);
}

// Hàm hiển thị văn bản trên OLED
void displayText(const char* text) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(text);
  display.display();
}

void setup() {
  // Khởi tạo Serial và UART
  Serial.begin(9600);
  uart.begin(115200);

  // Khởi tạo OLED Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    for (;;);
  }

  // Khởi tạo Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, SSID, PASS);

  // Thiết lập Timer cho Blynk
  timer.setInterval(1000L, myTimerEvent);
}

BLYNK_WRITE(V4)
{
  if (mode == 2) {
    tb1 = param.asInt();
  }
}

BLYNK_WRITE(V5)
{
  if (mode == 2) {
    tb2 = param.asInt();
  }
}

BLYNK_WRITE(V6)
{
  if (mode == 2) {
    tb3 = param.asInt();
  }
}

BLYNK_WRITE(V7)
{
  if (mode == 2) {
    tb4 = param.asInt();
  }
}

void loop() {
  // Nhận Dữ liệu từ UART
  Nhan_UART();

  // Hiển thị chế độ trên OLED
  if (mode == 0)
    displayText("AUTOMATIC MODE");
  else if (mode == 1)
    displayText("MANUAL    MODE");
  else {
    displayText("REMOTE    MODE");
    Truyen_UART();
  }

  // Chạy Blynk và Timer
  Blynk.run();
  timer.run();
}
