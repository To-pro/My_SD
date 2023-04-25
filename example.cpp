#include <Arduino.h>

// #define SD_SPI
#include <My_SD.h>

double t = 0.0;
double x = sin(x);
const int data_length = 20;
double dt = 0.01;
double t_end = 10.0;
double rumTime=0.0;
unsigned long start_time;
unsigned long end_time;

void setup() {
  int count = round(t_end/dt)+1;

  Serial.begin(115200);

  // float data[data_length+2] = {0.0};
  // SD_save.data_type = 0;

  double data[data_length+2] = {0.0};
  SD_save.data_type = 1;

  SD_save.data_length = data_length+2;
  My_SD_init("data");
  My_SD_init_info_save();
  delay(1000);


  // 寫入資料到檔案
  for (int i = 0; i < count; i++)
  {
    t += dt;
    x = sin(t);
    
    data[0] = rumTime;
    data[1] = t;
    for (int k = 0; k < data_length; k++)
    {
      data[k+2]=x;
    }

    start_time=micros();//us
    write_data_to_file(data);
    // SD_write_count++;
    // if(SD_write_count>=9){
    //   SD_write_count=0;
    //   file.flush();
    // }
    end_time=micros();

    if(count == 500){//模擬錯誤的寫入
        for (int j = 0; j < 50; j++)
        {
          double test = (double)j;
          SD_save.file.write((byte*) &test, sizeof(float));
        }
        SD_save.file.flush();
    }
    // rumTime = static_cast<double>(end_time-start_time)/1000.0;
    rumTime = static_cast<float>(end_time-start_time)/1000.0;
  }

  // 關閉檔案
  SD_save.file.close();
  Serial.println("程式結束");
}

void loop(){

}