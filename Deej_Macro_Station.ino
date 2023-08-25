const int NUM_SLIDERS = 5;
const int analogInputs[NUM_SLIDERS] = {A4,A3,A2,A1,A0};

int analogSliderValues[NUM_SLIDERS];

#include <Keyboard.h>

const uint8_t inputs[] = { 5, 6, 7, 8, 9, 10};  // Columns
const uint8_t outputs[] = { 4, 3, 2 };   // Rows
#define inCount sizeof(inputs) / sizeof(inputs[0])
#define outCount sizeof(outputs) / sizeof(outputs[0])

const int keymap[outCount][inCount] = {
  // 5   6   7   8   9   10 (input pins)
  { KEY_F13, KEY_F16, 'c', 'd', 'e', 'f'},                     // 2 (output pins)
  {KEY_F14, KEY_F17, 'i', 'j', 'k', 'l'},                   // 3
  { KEY_F15, KEY_F18, 'p','q', 'r', 's'},                      // 4
};

// Press speed and delay configuration
int postOutputToLowDelayMicroseconds = 5;
int postOutputToHighDelayMicroseconds = 500;

int repeatsBeforeSecondPress = 90;  // 350;  // Number of repeats a switch encounters when a key is held down before the second press is lodged
int repeatPressDelay = 9;           // 15;           // Number of repeats a switch encounters between each press that is lodged

int currentKeyRepeatCount[outCount][inCount] = { 0 };
bool firstKeyPressFinished[outCount][inCount] = { false };

void setup() {
  for (int i = 0; i < inCount; i++) {
    pinMode(inputs[i], INPUT_PULLUP);
  }

  for (int i = 0; i < outCount; i++) {
    pinMode(outputs[i], OUTPUT);
    digitalWrite(outputs[i], HIGH);
  }

  Serial.begin(9600);
  Serial.println("Connected");
  Keyboard.begin();
  
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
  }
  
}

void loop() {
  for (int i = 0; i < outCount; i++) {
    digitalWrite(outputs[i], LOW);
    delayMicroseconds(postOutputToLowDelayMicroseconds);

    for (int j = 0; j < inCount; j++) {
      if (digitalRead(inputs[j]) == LOW) {
        // 3 cases
        // first press
        if (currentKeyRepeatCount[i][j] == 0) {
          Keyboard.write(keymap[i][j]);
        }
        // time for second press - spam mode
        else if (firstKeyPressFinished[i][j] && currentKeyRepeatCount[i][j] > repeatPressDelay) {
          Keyboard.write(keymap[i][j]);
          currentKeyRepeatCount[i][j] = 1;  // start repeat count again
        }
        // ready for spam mode
        else if (currentKeyRepeatCount[i][j] > repeatsBeforeSecondPress) {
          firstKeyPressFinished[i][j] = true;  // ready for spam
        }

        currentKeyRepeatCount[i][j]++;
      }
      // key recently released so reset
      else if (currentKeyRepeatCount[i][j] != 0) {
        currentKeyRepeatCount[i][j] = 0;
        firstKeyPressFinished[i][j] = false;
      }
    }
    
    digitalWrite(outputs[i], HIGH);
    delayMicroseconds(postOutputToHighDelayMicroseconds);
  }


updateSliderValues();
  sendSliderValues(); // Actually send data (all the time)
  // printSliderValues(); // For debug
  delay(10);
}

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
     analogSliderValues[i] = analogRead(analogInputs[i]);
  }
}

void sendSliderValues() {
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  
  Serial.println(builtString);
}

void printSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    String printedString = String("Slider #") + String(i + 1) + String(": ") + String(analogSliderValues[i]) + String(" mV");
    Serial.write(printedString.c_str());

    if (i < NUM_SLIDERS - 1) {
      Serial.write(" | ");
    } else {
      Serial.write("\n");
    }
  }
}


void printKey(int input, int output) {
  Serial.print("input col: ");
  Serial.print(input);
  Serial.print(", output row: ");
  Serial.print(output);
  Serial.print(", key: ");
  Serial.println(keymap[output][input]);
}