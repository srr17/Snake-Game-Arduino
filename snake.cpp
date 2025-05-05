#include <Adafruit_LiquidCrystal.h>
#include <Keypad.h>

Adafruit_LiquidCrystal lcd(0);
const int buzzerPin = 10;

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Snake structure
struct Point {
  int x;
  int y;
};

Point snake[32];     // Max length for 16x2 LCD
int length = 1;
int dx = 1, dy = 0;   // Initial direction
Point food = {7, 1};
bool gameOver = false;
int speed = 150; // Initial speed

void setup() {
  lcd.begin(16, 2);
  pinMode(buzzerPin, OUTPUT);
  lcd.clear();
  startPage();
}

void loop() {
  if (gameOver) {
    gameOverPage();
    return;
  }

  // Get user input for direction change
  char key = keypad.getKey();
  if (key == '2') { dx = 0; dy = -1; }
  else if (key == '8') { dx = 0; dy = 1; }
  else if (key == '4') { dx = -1; dy = 0; }
  else if (key == '6') { dx = 1; dy = 0; }

  // Move the snake body
  for (int i = length - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  // Move the snake head
  snake[0].x += dx;
  snake[0].y += dy;

  // Wrap the snake around screen edges
  if (snake[0].x < 0) snake[0].x = 15;
  if (snake[0].x > 15) snake[0].x = 0;
  if (snake[0].y < 0) snake[0].y = 1;
  if (snake[0].y > 1) snake[0].y = 0;

  // Check for collision with body (self-collision)
  for (int i = 1; i < length; i++) {
    if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
      gameOver = true; // End game if it collides with itself
      return;
    }
  }

  // Check if snake eats food
  if (snake[0].x == food.x && snake[0].y == food.y) {
    if (length < 32) length++; // Grow snake
    tone(buzzerPin, 1000, 150); // Beep on food eaten
    // Generate new food
    food.x = random(0, 16);
    food.y = random(0, 2);
  }

  // Clear screen and redraw everything
  lcd.clear();

  // Draw the snake
  for (int i = 0; i < length; i++) {
    lcd.setCursor(snake[i].x, snake[i].y);
    lcd.print("#");
  }

  // Draw the food
  lcd.setCursor(food.x, food.y);
  lcd.print("O");

  // Speed up the game as the score increases
  if (length % 5 == 0 && length > 1) {
    speed = max(50, speed - 10); // Increase speed but don't go below 50ms delay
  }

  // Delay for a faster game (adjusted)
  delay(speed);  // Lower delay for faster snake movement
}

// Start page with game title
void startPage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Snake Game");
  lcd.setCursor(0, 1);
  lcd.print("Press any key...");
  delay(2000); // Show title for 2 seconds
  lcd.clear();
}

// Game over page
void gameOverPage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over");
  delay(2000); // Show "Game Over" for 2 seconds
  resetGame();  // Reset game
}

// Reset the game after game over
void resetGame() {
  gameOver = false;
  length = 1;
  snake[0] = {0, 0}; // Reset snake position
  dx = 1;
  dy = 0;
  food.x = 7;
  food.y = 1;
  speed = 150; // Reset speed
  startPage(); // Restart the game
}