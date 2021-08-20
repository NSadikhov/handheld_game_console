#ifndef SNAKE_H
#define SNAKE_H

enum MoveDirections {up, right, down, left};

struct Position {
  byte x, y;

  Position() {

  }
  Position(byte _x, byte _y) {
    x = _x;
    y = _y;
  }
};

struct Snake_OBJ {
  volatile unsigned short maxSize = 20;
  Position* positions = new Position[maxSize];
  byte sizePerLen;
  // speed as milliseconds
  volatile unsigned short snakeSpeed;
  MoveDirections moveDirection = MoveDirections::up;
  // length of snake
  volatile short len = 1;
  volatile unsigned long lastTimeUpdated = 0;

  Snake_OBJ() {
  }

  Snake_OBJ(byte x, byte y, byte _sizePerLen = 1, unsigned short _snakeSpeed = 250) {
    positions[0] = Position(x, y);
    sizePerLen = _sizePerLen;
    snakeSpeed = _snakeSpeed;
  }

  void snakeSetPos(byte index, byte x, byte y) {
    positions[index] = Position(x, y);
  }

  void eatFood() {
    len++;
    snakeSpeed -= 5;
    if (len + 1 == maxSize) {
      maxSize += 20;
      Position* temp = positions;
      positions = new Position[maxSize];
      for (byte i = 0; i < len; ++i) {
        positions[i] = temp[i];
      }
      delete [] temp;
    }

    positions[len - 1] = Position(positions[len - 2].x, positions[len - 2].y);
  }
};

struct SnakeBoard {
  Position foodPos;
  byte foodSize;
  byte possibleFoodPosX[26] = {3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48, 51, 54, 57, 60, 63, 66, 69, 72, 75, 78};
  byte possibleFoodPosXSize = 26;
  byte possibleFoodPosY[12] = {11, 14, 17, 20, 23, 26, 29, 32, 35, 38, 41, 44};
  byte possibleFoodPosYSize = 12;

  volatile unsigned short score = 1;
  byte borderTopY;
  byte borderRightX;
  byte borderBottomY;
  byte borderLeftX;

  SnakeBoard() {
  }
  SnakeBoard(
    byte _foodSize,
    byte _borderTopY,
    byte _borderRightX,
    byte _borderBottomY,
    byte _borderLeftX
  ) {
    foodSize = _foodSize;
    borderTopY = _borderTopY;
    borderRightX = _borderRightX;
    borderBottomY = _borderBottomY;
    borderLeftX = _borderLeftX;
  }

  Position generateRandomFoodPos(Snake_OBJ& snake) {
    Position _pos(possibleFoodPosX[random(0, possibleFoodPosXSize)], possibleFoodPosY[random(0, possibleFoodPosYSize)]);
    for (auto i = 0; i < snake.len; i++) {
      if(snake.positions[i].x == _pos.x && snake.positions[i].y == _pos.y)
       _pos = generateRandomFoodPos(snake);
    }
    return _pos;
  }

};

#endif
