#ifndef MY_SD_H
#define MY_SD_H
/*
 * If using MMC. 
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32
 *    D2       12
 *    D3       13
 *    CMD      15
 *    VSS      GND
 *    VDD      3.3V
 *    CLK      14
 *    VSS      GND
 *    D0       2  (add 1K pull up after flashing)
 *    D1       4
 */


#ifdef SD_SPI
#include <SPI.h>
#include <SD.h>
#else
#include <FS.h>
#include <SD_MMC.h>
#endif
// #define SD_WRITE_COUNT_FLAG//是否要每幾次才 flush 要寫入的檔案(不然每次都flush)

struct{
  char CONFIRM_BYTE1 = 0xAA;
  char CONFIRM_BYTE2 = 0x55;
  int data_length = 10;
  int SD_write_count_gain = 10;//每幾次flush要寫入的檔案
  int SD_write_count = 0;
  int data_type = 1;//1:double  0:float
  int fileNumber = 1;
  String fileName;
  File file;
  int SD_SPI_CS = 10;
}SD_save;

#include <FS.h>
#ifdef SD_SPI
int getNextFileNumber( ) {
  int fileNumber = 1; // 起始檔案編號
  String fileName_test = SD_save.fileName + String(fileNumber, DEC) + ".bin";
  //使用 c_str() 函數取得字串的 c-style 字元陣列，以符合 exists() 函數的引數型態。
  while (SD.exists(fileName_test.c_str())) {
    fileNumber++; // 檢查下一個檔案編號
    fileName_test = SD_save.fileName + String(fileNumber, DEC) + ".bin";
  }
//   SD_MMC.mkdir("/data" + String(fileNumber, DEC) + ".bin");
  return fileNumber;
}
void My_SD_init(String fileName_set){
  SD_save.fileName = fileName_set;
  // 初始化 SD 卡
  if (!SD.begin(SD_save.SD_SPI_CS)) {
    Serial.println("SD 卡初始化失敗");
    return;
  }
  SD_save.fileName = fileName_set;
  // 取得下一個檔案編號
  SD_save.fileNumber = getNextFileNumber();
  
  // 新檔案名稱
  String filename_all = fileName_set + String(SD_save.fileNumber, DEC) + ".bin";
  
  // 開啟新檔案
  SD_save.file = SD.open(filename_all.c_str(), FILE_WRITE);
  if (!SD_save.file) {
    Serial.println("開啟檔案失敗");
    return;
  }
}

void My_SD_init_info_save(){
// 新檔案名稱
  String filename_all = SD_save.fileName + String(SD_save.fileNumber, DEC) + ".txt";
  
  // 開啟新檔案
  File file_info_save = SD.open(filename_all.c_str(), FILE_WRITE);
  file_info_save.println((int)SD_save.CONFIRM_BYTE1);
  file_info_save.println((int)SD_save.CONFIRM_BYTE2);
  file_info_save.println(SD_save.data_length);
  if (SD_save.data_type==1){
    file_info_save.println("double");
  }else{
    file_info_save.println("float");
  }
  file_info_save.flush();
  file_info_save.close();
}
#else
int getNextFileNumber( ) {
  int fileNumber = 1; // 起始檔案編號
  while (SD_MMC.exists("/" + SD_save.fileName + String(fileNumber, DEC) + ".bin")) {
    fileNumber++; // 檢查下一個檔案編號
  }
//   SD_MMC.mkdir("/data" + String(fileNumber, DEC) + ".bin");
  return fileNumber;
}
void My_SD_init(String fileName_set){
  SD_save.fileName = fileName_set;
  // 初始化 SD 卡
  if (!SD_MMC.begin()) {
    Serial.println("SD 卡初始化失敗");
    return;
  }
  SD_save.fileName = fileName_set;
  // 取得下一個檔案編號
  SD_save.fileNumber = getNextFileNumber();
  
  // 新檔案名稱
  String filename_all = "/"+ fileName_set + String(SD_save.fileNumber, DEC) + ".bin";
  
  // 開啟新檔案
  SD_save.file = SD_MMC.open(filename_all, FILE_WRITE);
  if (!SD_save.file) {
    Serial.println("開啟檔案失敗");
    return;
  }
}

void My_SD_init_info_save(){
// 新檔案名稱
  String filename_all = "/"+ SD_save.fileName + String(SD_save.fileNumber, DEC) + ".txt";
  
  // 開啟新檔案
  File file_info_save = SD_MMC.open(filename_all, FILE_WRITE);
  file_info_save.println((int)SD_save.CONFIRM_BYTE1);
  file_info_save.println((int)SD_save.CONFIRM_BYTE2);
  file_info_save.println(SD_save.data_length);
  if (SD_save.data_type==1){
    file_info_save.println("double");
  }else{
    file_info_save.println("float");
  }
  file_info_save.flush();
  file_info_save.close();
}
#endif

template<typename T>
bool write_data_to_file(T* data)
{
  // 確認輸入的檔案物件是否有效
  if (! SD_save.file)
  {
      return false;
  }
  SD_save.file.write((byte*) &SD_save.CONFIRM_BYTE1, sizeof(SD_save.CONFIRM_BYTE1));
  SD_save.file.write((byte*) &SD_save.CONFIRM_BYTE1, sizeof(SD_save.CONFIRM_BYTE1));
  for (int j = 0; j < SD_save.data_length; j++)
  {
     SD_save.file.write((byte*) &data[j], sizeof(data[0]));
  }
  SD_save.file.write((byte*) &SD_save.CONFIRM_BYTE2, sizeof(SD_save.CONFIRM_BYTE2));
  SD_save.file.write((byte*) &SD_save.CONFIRM_BYTE2, sizeof(SD_save.CONFIRM_BYTE2));
  
  #ifdef SD_WRITE_COUNT_FLAG
  SD_save.SD_write_count++;
  if(SD_save.SD_write_count>=SD_write_count_gain){
    SD_save.SD_write_count=0;
     SD_save.file.flush();
  }
  #else
   SD_save.file.flush();
  #endif
  return true;
}

#endif