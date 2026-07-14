#include "CONFIG.h"
#include <ESP_I2S.h>
#include <WiFiClientSecure.h>
#include "FS.h"
#include "SD.h"

// אובייקט מיקרופון I2S
I2SClass i2s;

// ערוץ SPI נפרד לכרטיס ה-SD של המיקרופון
SPIClass sdSPI(VSPI);

// אתחול מיקרופון וכרטיס SD
void voice_setup() {
  // אתחול I2S למיקרופון INMP441
  i2s.setPins(I2S_SCK, I2S_WS, -1, I2S_SD);
  if (!i2s.begin(I2S_MODE_STD, 16000,
                 I2S_DATA_BIT_WIDTH_16BIT,
                 I2S_SLOT_MODE_MONO,
                 I2S_STD_SLOT_LEFT)) {
    Serial.println("[Voice] שגיאה: מיקרופון לא אותחל");
    return;
  }

  // אתחול SD
  sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS, sdSPI)) {
    Serial.println("[Voice] שגיאה: SD לא נמצא");
    return;
  }
  Serial.println("[Voice] אותחל");
}

// כתיבת כותרת WAV לקובץ
void writeWavHeader(File& file, uint32_t dataSize) {
  uint32_t sampleRate    = 16000;
  uint16_t channels      = 1;
  uint16_t bitsPerSample = 16;
  uint32_t byteRate      = sampleRate * channels * bitsPerSample / 8;
  uint16_t blockAlign    = channels * bitsPerSample / 8;
  uint32_t chunkSize     = 36 + dataSize;

  file.write((const uint8_t*)"RIFF", 4);
  file.write((uint8_t*)&chunkSize, 4);
  file.write((const uint8_t*)"WAVE", 4);
  file.write((const uint8_t*)"fmt ", 4);

  uint32_t subChunk1 = 16;
  uint16_t format    = 1;
  file.write((uint8_t*)&subChunk1, 4);
  file.write((uint8_t*)&format, 2);
  file.write((uint8_t*)&channels, 2);
  file.write((uint8_t*)&sampleRate, 4);
  file.write((uint8_t*)&byteRate, 4);
  file.write((uint8_t*)&blockAlign, 2);
  file.write((uint8_t*)&bitsPerSample, 2);

  file.write((const uint8_t*)"data", 4);
  file.write((uint8_t*)&dataSize, 4);
}

// הקלטת 5 שניות ושמירה כ-RAW
bool recordAudio() {
  SD.remove("/record.raw");
  File file = SD.open("/record.raw", FILE_WRITE);
  if (!file) return false;

  Serial.println("[Voice] מקליט 5 שניות...");
  char buffer[512];
  unsigned long start = millis();

  while (millis() - start < 5000) {
    int n = i2s.readBytes(buffer, sizeof(buffer));
    if (n > 0) file.write((uint8_t*)buffer, n);
  }

  file.close();
  Serial.println("[Voice] הקלטה הסתיימה");
  return true;
}

// המרת RAW ל-WAV
bool convertToWav() {
  SD.remove("/record.wav");
  File raw = SD.open("/record.raw");
  if (!raw) return false;

  File wav = SD.open("/record.wav", FILE_WRITE);
  if (!wav) return false;

  uint32_t dataSize = raw.size();
  writeWavHeader(wav, dataSize);

  uint8_t buffer[512];
  while (raw.available()) {
    int n = raw.read(buffer, sizeof(buffer));
    if (n > 0) wav.write(buffer, n);
  }

  raw.close();
  wav.close();
  return true;
}

// שליחת הקלטה ל-OpenAI ובקשת זיהוי יעד
// מחזיר את הכתובת שהמשתמש אמר
String recognizeDestination() {
  if (!recordAudio())    return "";
  if (!convertToWav())   return "";

  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(15);

  if (!client.connect("api.openai.com", 443)) return "";

  File file = SD.open("/record.wav");
  if (!file) return "";

  String boundary = "----ESP32Boundary";
  String modelPart =
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"model\"\r\n\r\n"
    "gpt-4o-transcribe\r\n";
  String langPart =
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"language\"\r\n\r\n"
    "he\r\n";
  String fileHeader =
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"file\"; filename=\"audio.wav\"\r\n"
    "Content-Type: audio/wav\r\n\r\n";
  String endBound = "\r\n--" + boundary + "--\r\n";

  int contentLen = modelPart.length() + langPart.length() +
                   fileHeader.length() + file.size() + endBound.length();

  client.println("POST /v1/audio/transcriptions HTTP/1.1");
  client.println("Host: api.openai.com");
  client.println("Authorization: Bearer " + String(openAiKey));
  client.println("Content-Type: multipart/form-data; boundary=" + boundary);
  client.println("Content-Length: " + String(contentLen));
  client.println();
  client.print(modelPart);
  client.print(langPart);
  client.print(fileHeader);

  uint8_t buf[512];
  while (file.available()) {
    int n = file.read(buf, sizeof(buf));
    if (n > 0) client.write(buf, n);
  }
  client.print(endBound);
  file.close();

  // קריאת תשובה
  String response = "";
  unsigned long start = millis();
  while ((client.connected() || client.available()) && millis() - start < 15000) {
    while (client.available()) response += client.readString();
  }

  // ניקוי קבצים זמניים
  SD.remove("/record.raw");
  SD.remove("/record.wav");

  return response;   // מחזיר JSON עם הטקסט
}
