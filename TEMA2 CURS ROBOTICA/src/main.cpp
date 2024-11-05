#include <Arduino.h>

int ledRed = 6;
int ledGreen = 5;
int ledBlue = 4;
int buttonStart = 3;
int buttonDiff = 2;

const unsigned int ROUND_TIME = 30000; 
const unsigned int DISPLAY_TIME_EASY = 5000;
const unsigned int DISPLAY_TIME_MEDIUM = 4000;
const unsigned int DISPLAY_TIME_HARD = 2500;

const int WORD_COUNT = 50;
const char* WORDS[] = {
  "laptop", "cafea", "programare", "calatorie", "floare",
  "planeta", "chitara", "vulcan", "galaxie", "poezie",
  "fruct", "moneda", "astronomie", "muzeu", "telefon",
  "ceas", "librarie", "matematica", "meditatie", "lac",
  "curcubeu", "caramel", "stea", "joc", "film",
  "computer", "trompeta", "pian", "robot", "pictura",
  "vreme", "istorie", "copac", "cascada", "metafora",
  "bijuterie", "castel", "inghetata", "muzica", "navigare",
  "gratar", "parfum", "oceane", "broscuta", "oglinda",
  "masina", "vapor", "energie", "planificare", "balet"
};

size_t difficulty = 0;
bool gameRunning = false;
unsigned int score = 0;
unsigned long lastWordTime = 0;
unsigned int displayDelay;
unsigned long startGameTime = 0;
const char* currentWord = "";

unsigned long lastStartPress = 0;
unsigned long lastDiffPress = 0;
const unsigned int DEBOUNCE_DELAY = 50;

int countdown;
unsigned long countdownStart = 0;
bool countdownRunning = false;

void setRGB(int red, int green, int blue) {
  analogWrite(ledRed, red);
  analogWrite(ledGreen, green);
  analogWrite(ledBlue, blue);
}

void changeDifficulty() {
  if (!gameRunning && millis() - lastDiffPress > DEBOUNCE_DELAY) {
    difficulty = (difficulty % 3) + 1;
    switch (difficulty) {
      case 1:
        displayDelay = DISPLAY_TIME_EASY;
        Serial.println("Easy mode selected.");
        break;
      case 2:
        displayDelay = DISPLAY_TIME_MEDIUM;
        Serial.println("Medium mode selected.");
        break;
      case 3:
        displayDelay = DISPLAY_TIME_HARD;
        Serial.println("Hard mode selected.");
        break;
    }
    lastDiffPress = millis();
  }
}

void toggleGame() {
  if (millis() - lastStartPress > DEBOUNCE_DELAY && (difficulty != 0)) {
    gameRunning = !gameRunning;
    countdown = 3;
    if (gameRunning) {
      Serial.println("Game starting in 3...");
      countdownRunning = true;
      countdownStart = millis();
      score = 0;
      startGameTime = millis();
      lastWordTime = millis();
    } else {
      Serial.println("Game stopped.");
      setRGB(255, 255, 255);
      Serial.print("Final score: ");
      Serial.println(score);
    }
    lastStartPress = millis();
  }
}

void displayNewWord() {
  int randomIndex = random(WORD_COUNT);
  currentWord = WORDS[randomIndex];
  Serial.println('\n');
  Serial.println(currentWord);
  lastWordTime = millis();
}

int pozitie = 0;

void checkUserInput() {
  if (Serial.available() > 0) {
    char caracter = (char)Serial.read();
    int wordLength = strlen(currentWord);

    if (caracter == currentWord[pozitie])
    {
      if (pozitie < wordLength - 1)
          pozitie++;
      else
      {
        setRGB(0, 255, 0);
        score++;
        displayNewWord();
        pozitie = 0;
      }
    }
    else
    {
      setRGB(255, 0, 0);
      pozitie = 0;
    }

    // VARIANTA CU ENTER
    
    // String userInput = Serial.readStringUntil('\n');
    // userInput.trim();

    // if (userInput.equals(currentWord)) {
    //   setRGB(0, 255, 0);
    //   score++;
    //   displayNewWord();
    // } else {
    //   setRGB(255, 0, 0);
    // }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(buttonStart, INPUT);
  pinMode(buttonDiff, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(buttonStart), toggleGame, LOW);
  attachInterrupt(digitalPinToInterrupt(buttonDiff), changeDifficulty, LOW);

  setRGB(255, 255, 255);
  randomSeed(analogRead(0));
}

void loop() {
  if (countdownRunning) {
    if (millis() - countdownStart >= 1000) {
      countdown--;
      Serial.println(countdown);
      setRGB(0, 0, 0);
      delay(500);
      setRGB(255, 255, 255);
      countdownStart = millis();
    }

    if (countdown <= 0) {
      Serial.println("Game started!");
      setRGB(0, 255, 0);
      countdownRunning = false;
      displayNewWord();
      lastWordTime = millis();
    }
  } else {

    if (gameRunning && millis() - startGameTime >= ROUND_TIME) {
      Serial.println("Time is up! Game over.");
      gameRunning = false;
      setRGB(255, 255, 255);
      Serial.print("Final Score: ");
      Serial.println(score);
    }

    if (gameRunning && millis() - lastWordTime >= displayDelay) {
      displayNewWord();
    }

    if (gameRunning) {
      checkUserInput();
    }
  }
}
