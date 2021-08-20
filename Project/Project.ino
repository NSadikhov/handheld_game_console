#include <LCD5110_Graph.h>

#include "Snake.h"
#include "Melody.h"

#define ACTIVATED LOW

#define LCD_WIDTH 84
#define LCD_HEIGHT 48
#define DEFAULT_CONTR 70
#define BTN_DEBOUNCE_DELAY 20

LCD5110 lcd(8, 9, 10, 12, 11);

extern uint8_t SmallFont[];

extern uint8_t hex_logo[];

extern const unsigned int short_melody[];
extern const unsigned int short_noteDurations[];

extern const unsigned int losing_melody[];
extern const unsigned int losing_noteDurations[];

void turnBackLight(bool = true);

volatile byte page = 0;

enum MenuOptions {Games, Settings, MenuCount};
volatile byte menuOption = MenuOptions::Games;

enum SettingsOptions {Contrast, Light, Reset, SettingsCount};
volatile byte settingsOption = SettingsOptions::Contrast;

enum GameOptions {Snake, PingPong, GameCount};
volatile byte gameOption = GameOptions::Snake;

volatile byte contrast = 70;
volatile byte light = LOW;

// pushbutton pins
const byte buttonDownPin = 2;
const byte buttonLeftPin = 3;
const byte buttonUpPin = 4;
const byte buttonRightPin = 5;

const byte buzzerPin = 6;
const byte backLightPin = 7;

struct Button {
  byte pin;
  volatile byte buttonState;
  volatile byte lastButtonState;

  volatile unsigned long lastDebounceTime = 0;
  unsigned long debounceDelay;

  Button(byte _pin, unsigned long _debounceDelay = BTN_DEBOUNCE_DELAY) {
    pin = _pin;
    debounceDelay = _debounceDelay;
  }
};

Button btn_Down(buttonDownPin);
Button btn_Left(buttonLeftPin);
Button btn_Up(buttonUpPin);
Button btn_Right(buttonRightPin);

Snake_OBJ snake;
SnakeBoard snakeBoard;

volatile bool isGameInitialized = false;

void playSongAndAnimation(int _buzzerPin, const unsigned int* _melody, const unsigned int* _noteDurations) {
  unsigned long lastPauseTime = 0;
  int i = 0;
  for (byte x = LCD_WIDTH - 1; x >= 6; --x) {
    lcd.clrScr();
    lcd.drawBitmap(x, LCD_HEIGHT / 2 - 10, hex_logo, 24, 24);
    lcd.update();
    while (_melody[i] != END)
    {
      int noteDuration = 1200 / _noteDurations[i];
      tone(_buzzerPin, _melody[i], noteDuration);
      if (millis() < (lastPauseTime + noteDuration)) break;
      noTone(_buzzerPin);
      i++;
      lastPauseTime = millis();
    }
    delay(10);
  }

  lcd.print("ENJOY :)", LCD_WIDTH / 2 - 10, LCD_HEIGHT / 2 - (SmallFont[1] / 2));
  lcd.update();

}

void turnBackLight(byte state = HIGH) {
  digitalWrite(backLightPin, state);
}

void printPageOptions() {
  switch (page) {
    case 0:
      lcd.clrScr();
      lcd.print("MENU", CENTER, 0);
      lcd.drawLine(0, 10, LCD_WIDTH - 1, 10);
      if (menuOption == MenuOptions::Games) {
        lcd.print("> Games", 0, 15);
        lcd.print("  Settings", 0, 25);
      } else {
        lcd.print("  Games", 0, 15);
        lcd.print("> Settings", 0, 25);
      }
      break;
    case 1:
      lcd.clrScr();
      lcd.drawLine(0, 10, LCD_WIDTH - 1, 10);
      if (menuOption == MenuOptions::Games) {
        lcd.print("GAMES", CENTER, 0);
        if (gameOption == GameOptions::Snake) {
          lcd.print("> Snake", 0, 15);
          lcd.print("  Ping Pong", 0, 25);
        } else {
          lcd.print("  Snake", 0, 15);
          lcd.print("> Ping Pong", 0, 25);
        }
      }
      else {
        lcd.print("SETTINGS", CENTER, 0);
        if (settingsOption == SettingsOptions::Contrast) {
          lcd.print("> Contr.: " + String(contrast), 0, 15);
          lcd.print("  Light: " + (light == LOW ? String("OFF") : String("ON")), 0, 25);
          lcd.print("  Reset", 0, 35);
        } else if (settingsOption == SettingsOptions::Light) {
          lcd.print("  Contr.: " + String(contrast), 0, 15);
          lcd.print("> Light: " + (light == LOW ? String("OFF") : String("ON")), 0, 25);
          lcd.print("  Reset", 0, 35);
        } else {
          lcd.print("  Contr.: " + String(contrast), 0, 15);
          lcd.print("  Light: " + (light == LOW ? String("OFF") : String("ON")), 0, 25);
          lcd.print("> Reset", 0, 35);
        }
      }
      break;
    case 2:
      if (menuOption == MenuOptions::Games) {
        startGame();
      }
      else {
        if (settingsOption == SettingsOptions::Contrast) {
          lcd.print("> Contr.:<" + String(contrast) + String("> "), 0, 15);
        } else if (settingsOption == SettingsOptions::Light) {
          lcd.print("> Light:<" + (light == LOW ? String("OFF") : String("ON")) + String("> "), 0, 25);
        } else {
          contrast = DEFAULT_CONTR;
          light = LOW;
          lcd.setContrast(contrast);
          turnBackLight(light);
          lcd.print("  Contr.: " + String(contrast) + ' ', 0, 15);
          lcd.print("  Light: " + (light == LOW ? String("OFF") : String("ON")) + ' ', 0, 25);
          page = 1;
        }
      }
      break;
  }
  lcd.update();
}

void drawInitialPage() {
  lcd.print("MENU", CENTER, 0);
  lcd.drawLine(0, 10, LCD_WIDTH - 1, 10);
  printPageOptions();
  lcd.update();
}

void startGame() {
  lcd.clrScr();

  for (byte y = 0; y < LCD_HEIGHT; y++) {
    for (byte x = 0; x < LCD_WIDTH; x++) {
      lcd.setPixel(x, y);
    }
    lcd.update();
  }

  lcd.clrScr();

  switch (gameOption)
  {
    case GameOptions::Snake: initSnakeBoard();
      break;
    case GameOptions::PingPong:
      lcd.print("<<< SOON >>>", CENTER, LCD_HEIGHT / 2 - (SmallFont[1] / 2));
      lcd.update();
      break;
    default:
      break;
  }

  lcd.update();
}

void drawSnakeFood () {
  snakeBoard.foodPos = snakeBoard.generateRandomFoodPos(snake);
  lcd.drawRect(snakeBoard.foodPos.x, snakeBoard.foodPos.y, snakeBoard.foodPos.x + snakeBoard.foodSize - 1, snakeBoard.foodPos.y + snakeBoard.foodSize - 1);
}

void initSnakeBoard() {
  snake = Snake_OBJ((LCD_WIDTH) / 2, LCD_HEIGHT - 10, 3, 250);
  snakeBoard = SnakeBoard(snake.sizePerLen, 10, LCD_WIDTH - 1 - 2, LCD_HEIGHT - 1, 2);

  lcd.print("SCORE: " + String(snakeBoard.score), CENTER, 0);

  // drawing borders
  lcd.drawRect(snakeBoard.borderLeftX, snakeBoard.borderTopY, snakeBoard.borderRightX, snakeBoard.borderBottomY);

  // drawing snake
  for (byte x = snake.positions[0].x; x < snake.positions[0].x + snake.sizePerLen; x++) {
    for (byte y = snake.positions[0].y; y < snake.positions[0].y + snake.sizePerLen; y++) {
      lcd.setPixel(x, y);
    }
  }

  drawSnakeFood();
  isGameInitialized = true;
}

void snakeLost() {
  for (byte x = snakeBoard.borderLeftX + 1; x < snakeBoard.borderRightX; x++) {
    for (byte y = snakeBoard.borderTopY + 1; y < snakeBoard.borderBottomY; y++) {
      lcd.clrPixel(x, y);
    }
  }

  lcd.print("< YOU LOST >", CENTER, (snakeBoard.borderBottomY + snakeBoard.borderTopY + 1) / 2 - (SmallFont[1] / 2));
  lcd.update();

  unsigned long lastPauseTime = 0;
  int i = 0;
  while (losing_melody[i] != END)
  {
    int noteDuration = 1200 / losing_noteDurations[i];
    tone(buzzerPin, losing_melody[i], noteDuration);
    while (millis() < (lastPauseTime + noteDuration));
    noTone(buzzerPin);
    i++;
    lastPauseTime = millis();
  }

  isGameInitialized = false;
  page = 1;
  printPageOptions();
}

void moveSnake() {
  if (millis() > (snake.lastTimeUpdated + snake.snakeSpeed + BTN_DEBOUNCE_DELAY)) {
    switch (snake.moveDirection) {
      case MoveDirections::up:
        if (snake.positions[0].y - snake.sizePerLen < snakeBoard.borderTopY) {
          snakeLost();
        }
        else {
          for (auto i = snake.len - 1; i >= 0; --i) {

            for (byte x = snake.positions[i].x; x < snake.positions[i].x + snake.sizePerLen; x++) {
              for (byte y = snake.positions[i].y; y < snake.positions[i].y + snake.sizePerLen; y++) {
                if (i == 0) {
                  if (snake.len == 1) {
                    lcd.clrPixel(x, y);
                  }
                  lcd.setPixel(x, y - snake.sizePerLen);
                } else if (i == snake.len - 1) {
                  lcd.clrPixel(x, y);
                }
              }
            }
            if (i == 0) {
              snake.snakeSetPos(i, snake.positions[i].x, snake.positions[i].y - snake.sizePerLen);
            }
            else {
              if (snake.positions[i - 1].x == snake.positions[0].x && snake.positions[i - 1].y == snake.positions[0].y - snake.sizePerLen)
                return snakeLost();
              snake.snakeSetPos(i, snake.positions[i - 1].x, snake.positions[i - 1].y);
            }
          }
        }
        break;
      case MoveDirections::right:
        if (snake.positions[0].x + snake.sizePerLen >= snakeBoard.borderRightX) {
          snakeLost();
        }
        else {
          for (auto i = snake.len - 1; i >= 0; --i) {

            for (byte x = snake.positions[i].x; x < snake.positions[i].x + snake.sizePerLen; x++) {
              for (byte y = snake.positions[i].y; y < snake.positions[i].y + snake.sizePerLen; y++) {
                if (i == 0) {
                  if (snake.len == 1) {
                    lcd.clrPixel(x, y);
                  }
                  lcd.setPixel(x + snake.sizePerLen, y);
                } else if (i == snake.len - 1) {
                  lcd.clrPixel(x, y);
                }
              }
            }
            if (i == 0) {
              snake.snakeSetPos(i, snake.positions[i].x + snake.sizePerLen, snake.positions[i].y);
            }
            else {
              if (snake.positions[i - 1].x == snake.positions[0].x + snake.sizePerLen && snake.positions[i - 1].y == snake.positions[0].y)
                return snakeLost();
              snake.snakeSetPos(i, snake.positions[i - 1].x, snake.positions[i - 1].y);
            }
          }
        }
        break;
      case MoveDirections::down:
        if (snake.positions[0].y + snake.sizePerLen >= snakeBoard.borderBottomY) {
          snakeLost();
        }
        else {
          for (auto i = snake.len - 1; i >= 0; --i) {

            for (byte x = snake.positions[i].x; x < snake.positions[i].x + snake.sizePerLen; x++) {
              for (byte y = snake.positions[i].y; y < snake.positions[i].y + snake.sizePerLen; y++) {
                if (i == 0) {
                  if (snake.len == 1) {
                    lcd.clrPixel(x, y);
                  }
                  lcd.setPixel(x, y + snake.sizePerLen);
                } else if (i == snake.len - 1) {
                  lcd.clrPixel(x, y);
                }
              }
            }
            if (i == 0) {
              snake.snakeSetPos(i, snake.positions[i].x, snake.positions[i].y + snake.sizePerLen);
            }
            else {
              if (snake.positions[i - 1].x == snake.positions[0].x && snake.positions[i - 1].y == snake.positions[0].y + snake.sizePerLen)
                return snakeLost();
              snake.snakeSetPos(i, snake.positions[i - 1].x, snake.positions[i - 1].y);
            }
          }
        }

        break;
      case MoveDirections::left:
        if (snake.positions[0].x - snake.sizePerLen < snakeBoard.borderLeftX) {
          snakeLost();
        }
        else {
          for (auto i = snake.len - 1; i >= 0; --i) {

            for (byte x = snake.positions[i].x; x < snake.positions[i].x + snake.sizePerLen; x++) {
              for (byte y = snake.positions[i].y; y < snake.positions[i].y + snake.sizePerLen; y++) {
                if (i == 0) {
                  if (snake.len == 1) {
                    lcd.clrPixel(x, y);
                  }
                  lcd.setPixel(x - snake.sizePerLen, y);
                } else if (i == snake.len - 1) {
                  lcd.clrPixel(x, y);
                }
              }
            }
            if (i == 0) {
              snake.snakeSetPos(i, snake.positions[i].x - snake.sizePerLen, snake.positions[i].y);
            }
            else {
              if (snake.positions[i - 1].x == snake.positions[0].x - snake.sizePerLen && snake.positions[i - 1].y == snake.positions[0].y)
                return snakeLost();
              snake.snakeSetPos(i, snake.positions[i - 1].x, snake.positions[i - 1].y);
            }
          }
        }
        break;
      default:
        break;
    }
    if (snake.positions[0].x == snakeBoard.foodPos.x && snake.positions[0].y == snakeBoard.foodPos.y) {
      snakeBoard.score++;
      snake.eatFood();
      playFoodEatenSong(buzzerPin);
      lcd.print("SCORE: " + String(snakeBoard.score), CENTER, 0);
      drawSnakeFood();
    }
    lcd.update();
    // reseting time
    snake.lastTimeUpdated = millis();
  }
}


void buttonDownPressed() {
  if (page == 0) {
    if (menuOption < MenuOptions::MenuCount - 1) {
      menuOption++;
      printPageOptions();
    }
  } else if (page == 1) {
    if (menuOption == MenuOptions::Games) {
      if (gameOption < GameOptions::GameCount - 1) {
        gameOption++;
        printPageOptions();
      }
    } else {
      if (settingsOption < SettingsOptions::SettingsCount - 1) {
        settingsOption++;
        printPageOptions();
      }
    }
  } else {
    if (menuOption == MenuOptions::Games) {
      if (isGameInitialized && gameOption == GameOptions::Snake) {
        if (!(snake.len > 1 && snake.moveDirection == MoveDirections::up))
          snake.moveDirection = MoveDirections::down;
      }
    } else if (menuOption == MenuOptions::Settings) {
      if (settingsOption == SettingsOptions::Contrast) {
        if (contrast > 55) {
          contrast--;
          lcd.setContrast(contrast);
        }
      } else if (settingsOption == SettingsOptions::Light) {
        light = !light;
        turnBackLight(light);
      }
      printPageOptions();
    }
  }
}

void buttonUpPressed() {
  if (page == 0) {
    if (menuOption > 0) {
      menuOption--;
      printPageOptions();
    }
  } else if (page == 1) {
    if (menuOption == MenuOptions::Games) {
      if (gameOption > 0) {
        gameOption--;
        printPageOptions();
      }
    } else {
      if (settingsOption > 0) {
        settingsOption--;
        printPageOptions();
      }
    }
  } else {
    if (menuOption == MenuOptions::Games) {
      if (isGameInitialized && gameOption == GameOptions::Snake) {
        if (!(snake.len > 1 && snake.moveDirection == MoveDirections::down))
          snake.moveDirection = MoveDirections::up;
      }
    } else if (menuOption == MenuOptions::Settings) {
      if (settingsOption == SettingsOptions::Contrast) {
        if (contrast < 100) {
          contrast++;
          lcd.setContrast(contrast);
        }
      } else if (settingsOption == SettingsOptions::Light) {
        light = !light;
        turnBackLight(light);
      }
      printPageOptions();
    }
  }
}


void buttonLeftPressed() {
  if (isGameInitialized) {
    if (page == 2 && gameOption == GameOptions::Snake) {
      if (!(snake.len > 1 && snake.moveDirection == MoveDirections::right))
        snake.moveDirection = MoveDirections::left;
    }
  } else {
    if (page > 0) {
      page--;
      printPageOptions();
    }
  }
}

void buttonRightPressed() {
  if (isGameInitialized) {
    if (page == 2 && gameOption == GameOptions::Snake) {
      if (!(snake.len > 1 && snake.moveDirection == MoveDirections::left))
        snake.moveDirection = MoveDirections::right;
    }
  } else {
    if (page < 2) {
      page++;
      printPageOptions();
    }
  }
}


void checkButtonState(Button &button) {
  byte currentButtonState = digitalRead(button.pin);

  if (currentButtonState != button.lastButtonState) {
    // reseting time
    button.lastDebounceTime = millis();
  }

  if ((millis() - button.lastDebounceTime) > button.debounceDelay) {
    if (currentButtonState != button.buttonState) {
      button.buttonState = currentButtonState;
      if (currentButtonState == ACTIVATED) {
        switch (button.pin)
        {
          case buttonDownPin: buttonDownPressed();
            break;
          case buttonLeftPin: buttonLeftPressed();
            break;
          case buttonUpPin: buttonUpPressed();
            break;
          case buttonRightPin: buttonRightPressed();
            break;
        }
      }
    }
  }

  button.lastButtonState = currentButtonState;
}

void setup()
{
  // to avoid usage of resisters, internal pull up resistence is used
  pinMode(btn_Down.pin, INPUT_PULLUP);
  pinMode(btn_Left.pin, INPUT_PULLUP);
  pinMode(btn_Up.pin, INPUT_PULLUP);
  pinMode(btn_Right.pin, INPUT_PULLUP);

  pinMode(buzzerPin, OUTPUT);

  pinMode(backLightPin, OUTPUT);

  lcd.InitLCD();
  lcd.setFont(SmallFont);
  randomSeed(analogRead(7));
  lcd.clrScr();

  playSongAndAnimation(buzzerPin, short_melody, short_noteDurations);
  delay(2000);

  drawInitialPage();
}

void loop()
{
  checkButtonState(btn_Down);
  checkButtonState(btn_Left);
  checkButtonState(btn_Up);
  checkButtonState(btn_Right);

  if (isGameInitialized && page == 2 && gameOption == GameOptions::Snake) {
    moveSnake();
  }
}
