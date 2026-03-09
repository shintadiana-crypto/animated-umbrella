/*
 * VARIASI LED DENGAN ARDUINO - 10 MACAM VARIASI
 *
 * Daftar Variasi:
 * 1.  Blink Sederhana         - LED berkedip on/off
 * 2.  Fade In / Fade Out      - LED meredup dan menyala pelan (PWM)
 * 3.  Running LED             - LED menyala berurutan (sequential)
 * 4.  Alternating Blink       - LED ganjil/genap bergantian
 * 5.  Heartbeat               - Pola detak jantung
 * 6.  SOS Morse Code          - Sinyal SOS (... --- ...)
 * 7.  Knight Rider (KITT)     - Pola scanner maju-mundur
 * 8.  Random Flash            - LED menyala acak
 * 9.  Breathing Effect        - Efek napas (fade halus)
 * 10. Bounce Effect           - LED memantul dari tengah ke ujung
 *
 * Koneksi Hardware:
 *   LED 1 → Pin 2  (digital)
 *   LED 2 → Pin 3  (PWM)
 *   LED 3 → Pin 4  (digital)
 *   LED 4 → Pin 5  (PWM)
 *   LED 5 → Pin 6  (PWM)
 *   LED 6 → Pin 7  (digital)
 *   LED 7 → Pin 8  (digital)
 *   LED 8 → Pin 9  (PWM)
 *   Tombol ganti variasi → Pin 12 (INPUT_PULLUP)
 *
 * Cara pakai:
 *   Tekan tombol pada Pin 12 untuk berpindah ke variasi berikutnya.
 *   Variasi aktif ditampilkan pada Serial Monitor (9600 baud).
 */

#include <math.h>

// ── Konfigurasi Pin ────────────────────────────────────────────────────────────
const int NUM_LEDS             = 8;
const int ledPins[NUM_LEDS]    = {2, 3, 4, 5, 6, 7, 8, 9};
const int BUTTON_PIN           = 12;

// ── Jumlah Variasi ─────────────────────────────────────────────────────────────
const int NUM_VARIATIONS = 10;
int currentVariation     = 0;   // 0 – 9

// ── State Bersama ──────────────────────────────────────────────────────────────
int          fadeValue      = 0;
int          fadeDirection  = 5;
int          runningIndex   = 0;
int          knightDir      = 1;
int          knightPos      = 0;
int          bouncePos      = 0;
int          bounceDir      = 1;
bool         heartbeatState = false;
int          patternStep    = 0;  // langkah pola untuk variasi 5 (Heartbeat) dan 6 (SOS)
unsigned long prevMillis    = 0;

// ── Debounce Tombol ────────────────────────────────────────────────────────────
bool          lastButtonState = HIGH;
unsigned long debounceTime    = 0;
const unsigned long DEBOUNCE_DELAY = 50;

// ═══════════════════════════════════════════════════════════════════════════════
void setup() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  randomSeed(analogRead(A0));
  Serial.begin(9600);
  printVariationName();
}

// ═══════════════════════════════════════════════════════════════════════════════
void loop() {
  handleButton();

  switch (currentVariation) {
    case 0: variation1_simpleBlink();   break;
    case 1: variation2_fadeInOut();     break;
    case 2: variation3_runningLED();    break;
    case 3: variation4_alternating();   break;
    case 4: variation5_heartbeat();     break;
    case 5: variation6_sosMorse();      break;
    case 6: variation7_knightRider();   break;
    case 7: variation8_randomFlash();   break;
    case 8: variation9_breathing();     break;
    case 9: variation10_bounce();       break;
  }
}

// ────────────────────────────────────────────────────────────────────────────
// Ganti variasi saat tombol ditekan (dengan debounce)
// ────────────────────────────────────────────────────────────────────────────
void handleButton() {
  bool reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) {
    debounceTime = millis();
  }
  if ((millis() - debounceTime) > DEBOUNCE_DELAY) {
    if (reading == LOW && lastButtonState == HIGH) {
      allLedsOff();
      currentVariation = (currentVariation + 1) % NUM_VARIATIONS;
      resetState();
      printVariationName();
    }
  }
  lastButtonState = reading;
}

// ────────────────────────────────────────────────────────────────────────────
void resetState() {
  fadeValue      = 0;
  fadeDirection  = 5;
  runningIndex   = 0;
  knightDir      = 1;
  knightPos      = 0;
  bouncePos      = 0;
  bounceDir      = 1;
  heartbeatState = false;
  patternStep    = 0;
  prevMillis     = millis();
}

void allLedsOff() {
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

void printVariationName() {
  const char* names[NUM_VARIATIONS] = {
    "1.  Blink Sederhana",
    "2.  Fade In / Fade Out",
    "3.  Running LED",
    "4.  Alternating Blink",
    "5.  Heartbeat",
    "6.  SOS Morse Code",
    "7.  Knight Rider (KITT)",
    "8.  Random Flash",
    "9.  Breathing Effect",
    "10. Bounce Effect"
  };
  Serial.print("Variasi aktif: ");
  Serial.println(names[currentVariation]);
}

// ════════════════════════════════════════════════════════════════════════════
//  VARIASI 1 – Blink Sederhana
//  Semua LED menyala dan mati bergantian setiap 500 ms
// ════════════════════════════════════════════════════════════════════════════
void variation1_simpleBlink() {
  unsigned long now = millis();
  if (now - prevMillis >= 500) {
    prevMillis = now;
    bool state = digitalRead(ledPins[0]);
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(ledPins[i], !state);
    }
  }
}

// ════════════════════════════════════════════════════════════════════════════
//  VARIASI 2 – Fade In / Fade Out
//  Kecerahan LED naik dari 0 ke 255, lalu turun kembali (PWM)
// ════════════════════════════════════════════════════════════════════════════
void variation2_fadeInOut() {
  unsigned long now = millis();
  if (now - prevMillis >= 15) {
    prevMillis = now;
    fadeValue += fadeDirection;
    if (fadeValue >= 255) { fadeValue = 255; fadeDirection = -5; }
    if (fadeValue <= 0)   { fadeValue = 0;   fadeDirection =  5; }

    int pwmPins[] = {3, 5, 6, 9};
    for (int i = 0; i < 4; i++) {
      analogWrite(pwmPins[i], fadeValue);
    }
    // LED digital menyala saat setengah kecerahan ke atas
    int digPins[] = {2, 4, 7, 8};
    for (int i = 0; i < 4; i++) {
      digitalWrite(digPins[i], fadeValue > 127 ? HIGH : LOW);
    }
  }
}

// ════════════════════════════════════════════════════════════════════════════
//  VARIASI 3 – Running LED
//  Satu LED menyala, bergerak dari kiri ke kanan berulang
// ════════════════════════════════════════════════════════════════════════════
void variation3_runningLED() {
  unsigned long now = millis();
  if (now - prevMillis >= 120) {
    prevMillis = now;
    allLedsOff();
    digitalWrite(ledPins[runningIndex], HIGH);
    runningIndex = (runningIndex + 1) % NUM_LEDS;
  }
}

// ════════════════════════════════════════════════════════════════════════════
//  VARIASI 4 – Alternating Blink
//  LED dengan indeks genap dan ganjil bergantian menyala setiap 400 ms
// ════════════════════════════════════════════════════════════════════════════
void variation4_alternating() {
  unsigned long now = millis();
  if (now - prevMillis >= 400) {
    prevMillis = now;
    bool evenOn = (digitalRead(ledPins[0]) == LOW);
    for (int i = 0; i < NUM_LEDS; i++) {
      if (i % 2 == 0) digitalWrite(ledPins[i], evenOn ? HIGH : LOW);
      else            digitalWrite(ledPins[i], evenOn ? LOW  : HIGH);
    }
  }
}

// ════════════════════════════════════════════════════════════════════════════
//  VARIASI 5 – Heartbeat
//  Pola detak jantung: nyala-mati cepat dua kali, jeda panjang
// ════════════════════════════════════════════════════════════════════════════
void variation5_heartbeat() {
  // Pola interval: ON, OFF, ON, OFF (panjang)
  const unsigned long pattern[] = {100, 100, 100, 700};
  const int patternLen = 4;

  unsigned long now = millis();
  if (now - prevMillis >= pattern[patternStep]) {
    prevMillis     = now;
    heartbeatState = !heartbeatState;
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(ledPins[i], heartbeatState ? HIGH : LOW);
    }
    patternStep = (patternStep + 1) % patternLen;
  }
}

// ════════════════════════════════════════════════════════════════════════════
//  VARIASI 6 – SOS Morse Code
//  Pola:  ... --- ...
//  Titik = 200 ms ON, garis = 600 ms ON, jeda antar simbol = 200 ms OFF
//  Jeda antar huruf = 400 ms OFF, jeda akhir = 1200 ms OFF
// ════════════════════════════════════════════════════════════════════════════
void variation6_sosMorse() {
  // Setiap elemen: {durasi ON (ms), durasi OFF (ms)}
  const unsigned long sosPattern[][2] = {
    {200, 200}, {200, 200}, {200, 400},   // S  (tiga titik)
    {600, 200}, {600, 200}, {600, 400},   // O  (tiga garis)
    {200, 200}, {200, 200}, {200, 1200}   // S  (tiga titik + jeda akhir)
  };
  const int patternLen = 9;

  unsigned long now     = millis();
  int  elemIndex        = patternStep / 2;
  bool isOnPhase        = (patternStep % 2 == 0);
  unsigned long duration = isOnPhase ? sosPattern[elemIndex][0]
                                     : sosPattern[elemIndex][1];

  if (now - prevMillis >= duration) {
    prevMillis = now;
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(ledPins[i], isOnPhase ? HIGH : LOW);
    }
    patternStep++;
    if (patternStep >= patternLen * 2) patternStep = 0;
  }
}

// ════════════════════════════════════════════════════════════════════════════
//  VARIASI 7 – Knight Rider (KITT)
//  Satu LED menyala bergerak maju lalu mundur (seperti scanner)
// ════════════════════════════════════════════════════════════════════════════
void variation7_knightRider() {
  unsigned long now = millis();
  if (now - prevMillis >= 80) {
    prevMillis = now;
    allLedsOff();
    digitalWrite(ledPins[knightPos], HIGH);
    knightPos += knightDir;
    if (knightPos >= NUM_LEDS - 1) { knightPos = NUM_LEDS - 1; knightDir = -1; }
    if (knightPos <= 0)            { knightPos = 0;             knightDir =  1; }
  }
}

// ════════════════════════════════════════════════════════════════════════════
//  VARIASI 8 – Random Flash
//  Setiap 150 ms, satu LED yang dipilih secara acak akan menyala
// ════════════════════════════════════════════════════════════════════════════
void variation8_randomFlash() {
  unsigned long now = millis();
  if (now - prevMillis >= 150) {
    prevMillis = now;
    allLedsOff();
    int r = random(NUM_LEDS);
    digitalWrite(ledPins[r], HIGH);
  }
}

// ════════════════════════════════════════════════════════════════════════════
//  VARIASI 9 – Breathing Effect
//  Kecerahan mengikuti kurva sinus → efek seperti menghirup dan menghembuskan
//  napas. Diperbarui setiap 10 ms menggunakan fungsi exp(sin(t)).
// ════════════════════════════════════════════════════════════════════════════
void variation9_breathing() {
  unsigned long now = millis();
  if (now - prevMillis >= 10) {
    prevMillis = now;
    // Rumus: (e^sin(t) - e^(-1)) / (e - e^(-1)) * 255
    // 0.36787944 ≈ e^(-1) ; 108.0 ≈ 255 / (e - e^(-1)) ≈ 255 / 2.3504
    float t     = now / 2000.0 * PI;
    float val_f = (exp(sin(t)) - 0.36787944f) * 108.0f;
    int   val   = constrain((int)val_f, 0, 255);

    int pwmPins[] = {3, 5, 6, 9};
    for (int i = 0; i < 4; i++) {
      analogWrite(pwmPins[i], val);
    }
    int digPins[] = {2, 4, 7, 8};
    for (int i = 0; i < 4; i++) {
      digitalWrite(digPins[i], val > 128 ? HIGH : LOW);
    }
  }
}

// ════════════════════════════════════════════════════════════════════════════
//  VARIASI 10 – Bounce Effect
//  Dua LED menyala simetris dari tengah, melebar ke ujung, lalu kembali
// ════════════════════════════════════════════════════════════════════════════
void variation10_bounce() {
  unsigned long now = millis();
  if (now - prevMillis >= 100) {
    prevMillis = now;
    allLedsOff();

    int center = NUM_LEDS / 2;  // = 4
    int left   = center - bouncePos - 1;
    int right  = center + bouncePos;

    if (left  >= 0 && left  < NUM_LEDS) digitalWrite(ledPins[left],  HIGH);
    if (right >= 0 && right < NUM_LEDS) digitalWrite(ledPins[right], HIGH);

    bouncePos += bounceDir;
    if (bouncePos >= center - 1) { bouncePos = center - 1; bounceDir = -1; }
    if (bouncePos <= 0)          { bouncePos = 0;           bounceDir =  1; }
  }
}
