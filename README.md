# Handheld Game Console

This project is a handheld game console featuring a Snake game and a simple menu system. The console uses an LCD display and push buttons for user interaction, and it includes sound effects for various game events.

## Features

- **Menu System**: Navigate through different options such as Games and Settings.
- **Games**: Currently includes the Snake game.
- **Settings**: Adjust contrast and backlight settings.
- **Sound Effects**: Plays melodies for events like food eaten and game over.

## Components

- LCD5110 display
- Push buttons for navigation
- Buzzer for sound effects
- Arduino board

## Setup

1. **Hardware Connections**:
   - Connect the LCD5110 display to the Arduino pins 8, 9, 10, 12, and 11.
   - Connect the push buttons to pins 2, 3, 4, and 5.
   - Connect the buzzer to pin 6.
   - Connect the backlight control to pin 7.

2. **Software**:
   - Install the `LCD5110_Graph` library.
   - Upload the `Project.ino` file to the Arduino board.

## Usage

- **Navigation**:
  - Use the up and down buttons to navigate through the menu.
  - Use the left and right buttons to select options.

- **Playing Snake**:
  - Navigate to the Games menu and select Snake.
  - Use the buttons to control the snake's direction.
  - Eat food to grow the snake and increase your score.

## Demonstration

A demonstration of this project can be found in this link: [Project Demo](https://drive.google.com/file/d/1glHFU7Sodaz1yTzoB8x0fSjqpAu-F7QA/view?usp=sharing)
