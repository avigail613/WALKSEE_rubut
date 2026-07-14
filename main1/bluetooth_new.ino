#include "CONFIG.h"
#include "BluetoothA2DPSource.h"
#include "SD.h"
#include "SPI.h"
#include <WiFiClientSecure.h>

BluetoothA2DPSource a2dp;
File   audioFile;
bool   fileOpen     = false;
bool   isAnnouncing = false;
int    currentSong  = 0;

#define HEADPHONES_NAME "TWS"
#define MAX_SONGS 10

// קריאת נתוני שמע לאוזניות — נקראת אוטומטית
// תיקון חתימת הפונקציה עם המרחב השמות הנכון של הספרייה
// שים לב לתוספת a2dp:: לפני המילה Frame בשני המקומות!
// שינוי הטיפוס למבנה הבסיסי ביותר - uint8_t (בתים)
int32_t getSoundData(uint8_t *data, int32_t len) {
  if (!fileOpen) return 0;
  
  // הספרייה מצפה לקבל כמות בתים (len). כל פריים סאונד מורכב מ-4 בתים.
  // לכן נקרא ישירות מהקובץ לתוך מערך הנתונים (data)
  int32_t bytesRead = audioFile.read(data, len);
  
  if (bytesRead > 0) {
    return bytesRead / 4; // החזרת מספר הפריימים שנקראו בפועל
  } else {
    fileOpen = false;
    audioFile.close();
    if (musicPlaying) playNextSong();  // שיר הבא
    return 0;
  }
}

void bluetooth_setup() {
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS))
    Serial.println("[BT] שגיאה: SD לא נמצא");
  a2dp.set_volume(70);
  a2dp.start(HEADPHONES_NAME, getSoundData);
  Serial.println("[BT] אותחל");
}

// השמעת קובץ WAV מה-SD
void playAudio(const char* fileName) {
  if (audioFile) audioFile.close();
  char path[50];
  sprintf(path, "/%s", fileName);
  audioFile = SD.open(path);
  if (!audioFile) {
    Serial.print("[BT] קובץ לא נמצא: ");
    Serial.println(path);
    return;
  }
  audioFile.seek(44);  // דולג על WAV header
  fileOpen = true;
}

// הקראת מיקום נוכחי + זמן לסיום דרך OpenAI TTS
// שולח ל-OpenAI בקשה לטקסט, מקבל קול ומנגן
void announceLocation() {
  if (!hasValidLocation()) return;

  isAnnouncing = true;
  bool wasPlaying = musicPlaying;
  if (wasPlaying) { fileOpen = false; }  // עצור שיר זמנית

  // חישוב זמן משוער לסיום — לפי מרחק + מהירות הליכה (1 מ'/שנייה)
  double distLeft = calcDistance(currentLat, currentLon,
                                 targetLat, targetLon);
  int minutesLeft = (int)(distLeft / 60.0);  // 60 מטר לדקה = הליכה

  // בניית טקסט להקראה
  String text = "אתה נמצא כעת במיקום GPS: ";
  text += String(currentLat, 4) + " , " + String(currentLon, 4);
  text += ". נותרו בערך " + String(minutesLeft) + " דקות להגעה ליעד.";

  // שליחה ל-OpenAI TTS וקבלת קול
  WiFiClientSecure client;
  client.setInsecure();
  if (!client.connect("api.openai.com", 443)) {
    isAnnouncing = false;
    return;
  }

  // בניית בקשת TTS
  String body = "{\"model\":\"tts-1\",\"input\":\"" + text +
                "\",\"voice\":\"alloy\"}";

  client.println("POST /v1/audio/speech HTTP/1.1");
  client.println("Host: api.openai.com");
  client.println("Authorization: Bearer " + String(openAiKey));
  client.println("Content-Type: application/json");
  client.println("Content-Length: " + String(body.length()));
  client.println();
  client.print(body);

  // שמירת הקול ל-SD
  SD.remove("/location_tts.mp3");
  File ttsFile = SD.open("/location_tts.mp3", FILE_WRITE);

  unsigned long t = millis();
  while ((client.connected() || client.available()) && millis()-t < 10000) {
    while (client.available()) {
      uint8_t c = client.read();
      ttsFile.write(c);
    }
  }
  ttsFile.close();

  // ניגון הקול
  playAudio("location_tts.mp3");

  isAnnouncing = false;
  if (wasPlaying) playMusic();  // חזרה לשיר
}

// =שירים 

// לשים על כרטיס SD קבצים בשם:
// song1.wav, song2.wav, song3.wav ...
void playNextSong() {
  currentSong++;
  if (currentSong > MAX_SONGS) currentSong = 1;
  char name[20];
  sprintf(name, "song%d.wav", currentSong);
  playAudio(name);
}

void playMusic() {
  currentSong  = 1;
  musicPlaying = true;
  playNextSong();
}

void stopMusic() {
  musicPlaying = false;
  fileOpen     = false;
  if (audioFile) audioFile.close();
}

void bluetooth_update() {
  // ממתין לחיבור אוזניות
}
