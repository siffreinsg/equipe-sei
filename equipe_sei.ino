#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <FastLED.h>


// DEFINITION BOUTONS
#define EDIT_BUTTON A0
#define OK_BUTTON A1
#define PLUS_BUTTON A2
#define MINUS_BUTTON A3
#define PRESSED_THRESHOLD 800

// DEFINITION LCD
#define SDA_LCD 20
#define SCL_LCD 21
#define ADDRESS_LCD 0x3F
LiquidCrystal_I2C lcd(0x3F, 20, 4);

// DEFINITION EQUIPES
#define NUMBER_OF_TEAMS 13
struct Score {
  char id[5];
  int score = 0;
} scores[NUMBER_OF_TEAMS];

// DEFINITION STATUS
bool editing = false;
int editing_team = 0;
String command;

// DEFINITION LED
#define NUM_LEDS 20
#define LED_PIN 7
CRGB leds[NUM_LEDS];


void setup() {
  Serial.begin(9600);
  Serial.println("=== BEGIN ===");
  randomSeed(analogRead(14)); // Seed random number generator

  // TEAM SETUP
  strcpy(scores[0].id, "ORAN");
  strcpy(scores[1].id, "BORD");
  strcpy(scores[2].id, "JAUN");
  strcpy(scores[3].id, "BLEU");
  strcpy(scores[4].id, "B-Cl");
  strcpy(scores[5].id, "ROUG");
  strcpy(scores[6].id, "MARR");
  strcpy(scores[7].id, "ROSE");
  strcpy(scores[8].id, "V-Cl");
  strcpy(scores[9].id, "VIOL");
  strcpy(scores[10].id, "VERT");
  strcpy(scores[11].id, "FUSC");
  strcpy(scores[12].id, "VIEU");

  // BUTTON SETUP
  pinMode(EDIT_BUTTON, INPUT);
  pinMode(OK_BUTTON, INPUT);
  pinMode(PLUS_BUTTON, INPUT);
  pinMode(MINUS_BUTTON, INPUT);

  // LCD SETUP
  lcd.init();
  lcd.clear();
  lcd.backlight();

  print_scores();
  serial_print_scores();

  // LED SETUP
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

}

void loop() {
  command = "";
  if (Serial.available()) {
    command = Serial.readStringUntil('\n');
    Serial.print("You typed: " );
    Serial.println(command);
  }

  int edit_pressed = (command == "edit" /* || analogRead(EDIT_BUTTON) > PRESSED_THRESHOLD */);
  int ok_pressed = (command == "ok" /*  || analogRead(OK_BUTTON) > PRESSED_THRESHOLD */);
  int plus_pressed = (command == "plus" /*  || analogRead(PLUS_BUTTON) > PRESSED_THRESHOLD */);
  int minus_pressed = (command == "minus" /*  || analogRead(MINUS_BUTTON) > PRESSED_THRESHOLD */);

  if (command == "fake") fake_scores();
  if (command == "sort") sort_scores();

  if (command == "print") {
    print_scores();
    serial_print_scores();
  }


  if (editing == false) {
    if (ok_pressed) {
      sort_scores();
      print_scores();
      serial_print_scores();
    }

    if (edit_pressed) {
      editing = true;
      editing_team = 0;

      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("EDITION");
      lcd.setCursor(2, 1);
      lcd.print("Equipe     Score");
      lcd.setCursor(3 , 2);
      lcd.print(scores[editing_team].id);
      lcd.setCursor(14, 2);
      lcd.print(scores[editing_team].score);
    }
  } else {
    if (plus_pressed && scores[editing_team].score < 999) {
      scores[editing_team].score++;
      lcd.setCursor(14, 2);
      lcd.print(scores[editing_team].score);
      lcd.print("   ");
    }

    if (minus_pressed && scores[editing_team].score > 0) {
      scores[editing_team].score--;
      lcd.setCursor(14, 2);
      lcd.print(scores[editing_team].score);
      lcd.print("   ");
    }

    if (ok_pressed) {
      if (editing_team < NUMBER_OF_TEAMS) {
        editing_team++;
        lcd.setCursor(3 , 2);
        lcd.print(scores[editing_team].id);
        lcd.setCursor(14, 2);
        lcd.print(scores[editing_team].score);
        lcd.print("   ");
      } else {
        editing = false;
        sort_scores();
        print_scores();
      }
    }

    if (edit_pressed) {
      editing = false;
      sort_scores();
      print_scores();
    }
  }

  if (edit_pressed || ok_pressed || plus_pressed || minus_pressed) delay(1000);
}

int compare_two_scores(const void *a1, const void *b1) {
  Score *a = (Score *)a1;
  Score *b = (Score *)b1;

  if (a->score < b->score) return 1;
  if (a->score > b->score) return -1;
  return 0;
}

void sort_scores() {
  qsort(scores, NUMBER_OF_TEAMS, sizeof(Score), compare_two_scores);
}

void print_scores() {
  lcd.clear();

  for (int i = 0; i < 4; i++) {
    // LINE 0
    lcd.setCursor(i * 5, 0);
    lcd.print(scores[i].id);

    // LINE 1
    lcd.setCursor(i * 5, 1);
    lcd.print(scores[i].score);
  }

  for (int i = 0; i < 4; i++) {
    // LINE 2
    lcd.setCursor(i * 5, 2);
    lcd.print(scores[4 + i].id);

    // LINE 3
    lcd.setCursor(i * 5, 3);
    lcd.print(scores[4 + i].score);
  }
}

// DEBUG HELPERS
void fake_scores() {
  for (int i = 0; i < NUMBER_OF_TEAMS; i++) {
    scores[i].score = random(200);
  }
}

void serial_print_scores() {
  for (int i = 0; i < NUMBER_OF_TEAMS; i++) {
    Serial.print("Score for ");
    Serial.print(scores[i].id);
    Serial.print(" is: ");
    Serial.println(scores[i].score);
  }
}
