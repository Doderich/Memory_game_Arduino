#include <Arduino.h>

#define RED_LED 2
#define BLUE_LED 3
#define YELLOW_LED 4
#define GREEN_LED 5

#define BTN_LED 6

#define RED_BTN 8
#define BLUE_BTN 9
#define YELLOW_BTN 10
#define GREEN_BTN 11

void setUpPins(); // Intiates all used Pins and turns every led off
void turnOffAllLed();
void turnOnAllLed();

enum BTN
{
  Red = RED_BTN,
  Blue = BLUE_BTN,
  Yellow = YELLOW_BTN,
  Green = GREEN_BTN
};

enum GameMode
{
  Ongoing,
  Error,
  NewGame,
  Victory
};

long currentTime = millis(); // Gets Current Runtime
int gameArr[20];             // This is the array wihch holds the current order of requested Button inputs
int currentIndex;            // current Index of the Count of inputs reuqested
int getRandom();             // gets a random Number betweeen 2 and 5

void populateGameArr();     // fills the gameArr with random numbers for the currentIndex
void showLed();             // led blink the order from gameArr
bool btnInput();            // The Player is requiered to put the led out back in the right order by pressing the correalted Btn
bool isBtnPressed(BTN btn); // Returns true only if the ONLY the right Btn is pressed in a timeframe of 3 seconds

void newGame();     // While Newgame is active all Led Blink and the game starts by pressing the red Btn
void errorMode();   // While ErrorMode is active the BTN LED blinks and newGameModeis initiated by pressing the red Btn
void victoryMode(); // While VictoryMode is active All Led blink in a patter resambleing a wave and newGameModeis initiated by pressing the red Btn
GameMode currentMode;

void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(A0)); // sets a almost random seed
  setUpPins();
  currentMode = NewGame;
  // put your setup code here, to run once:
}

void loop()
{
  if (currentMode == Ongoing)
  {
    populateGameArr();
    showLed();
    bool res = btnInput();
    if (!res)
    {
      currentIndex = 4;
      currentMode = Error;
    }
    if (currentIndex < 7)
    {
      currentIndex++;
    }
    else
    {
      currentMode = Victory;
    }
  }
  else if (currentMode == Error)
  {
    errorMode();
  }
  else if (currentMode == Victory)
  {
    victoryMode();
  }
  else
  {
    newGame();
  }

  Serial.println("");
  // put your main code here, to run repeatedly:
}

int getRandom()
{
  return (int)random(2, 6);
}

void setUpPins()
{
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BTN_LED, OUTPUT);

  pinMode(RED_BTN, INPUT_PULLUP);
  pinMode(BLUE_BTN, INPUT_PULLUP);
  pinMode(YELLOW_BTN, INPUT_PULLUP);
  pinMode(GREEN_BTN, INPUT_PULLUP);

  turnOffAllLed();
  pinMode(BTN_LED, LOW);
}
void turnOffAllLed()
{

  digitalWrite(RED_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BTN_LED, LOW);
}

void turnOnAllLed()
{

  digitalWrite(RED_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH);
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BTN_LED, HIGH);
}

void populateGameArr()
{
  for (int i = 0; i < currentIndex; i++)
  {
    gameArr[i] = getRandom();
    Serial.println("gameArray Content =  " + String(gameArr[i]) + " at " + String(i));
  }
}
void showLed()
{
  Serial.println("Start Phase 1");
  for (int i = 0; i < currentIndex; i++)
  {
    digitalWrite(gameArr[i], HIGH);
    delay(500);
    digitalWrite(gameArr[i], LOW);
    delay(100);
  }
}

bool btnInput()
{
  Serial.println("Start Phase 2");
  bool wrongPress = false;
  int i = 0;
  while (i < currentIndex && !wrongPress)
  {
    BTN btn = static_cast<BTN>(gameArr[i] + 6);
    if (isBtnPressed(btn))
    {
      Serial.println("Correct Btn Pressed");
    }
    else
    {
      Serial.println("Wrong Btn Pressed");
      wrongPress = true;
    }
    i++;
  }
  return !wrongPress;
}

bool isBtnPressed(BTN btn)
{
  bool isRightBtnPressend = false;
  bool isWrongBtnPressend = false;
  digitalWrite(BTN_LED, HIGH);
  Serial.println("Please press the Btn at " + String(btn));
  currentTime = millis();
  long neededTime = currentTime + 3000;
  while (currentTime < neededTime && !isRightBtnPressend && !isWrongBtnPressend)
  {
    currentTime = millis();
    for (int i = Red; i <= Green; i++)
    {
      if (!digitalRead(i))
      {
        if (btn == i)
        {
          isRightBtnPressend = true;
        }
        else
        {

          isWrongBtnPressend = true;
        }
      }
    }
  }
  digitalWrite(BTN_LED, LOW);
  if (neededTime > currentTime)
  {
    delay(500);
  }
  Serial.println("Right Buttn Pressed: " + String(isRightBtnPressend));
  Serial.println("Wrong Buttn Pressed: " + String(isWrongBtnPressend));
  return (isRightBtnPressend && !isWrongBtnPressend);
}

void errorMode()
{
  Serial.println("Start ERROR mode");
  while (currentMode == Error)
  {
    digitalWrite(BTN_LED, HIGH);
    delay(500);
    digitalWrite(BTN_LED, LOW);
    delay(500);
    if (!digitalRead((RED_BTN)))
    {
      currentMode = NewGame;
    }
  }
  delay(500);
  Serial.println("EXIT ERROR mode");
}

void newGame()
{
  Serial.println("Start NewGame mode");
  currentIndex = 4;
  while (currentMode == NewGame)
  {
    turnOffAllLed();
    delay(500);
    turnOnAllLed();
    digitalWrite(BTN_LED, LOW);
    delay(500);
    if (!digitalRead((RED_BTN)))
    {
      currentMode = Ongoing;
    }
  }
  turnOffAllLed();
  delay(500);

  Serial.println("EXIT NewGame mode");
}

void victoryMode()
{
  int delaytime = 200;
  while (currentMode == Victory)
  {
    digitalWrite(RED_LED, HIGH);
    delay(delaytime);
    digitalWrite(BLUE_LED, HIGH);
    delay(delaytime);
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    delay(delaytime);
    if (!digitalRead(RED_BTN))
    {
      currentMode = NewGame;
    }
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    delay(delaytime);
    digitalWrite(YELLOW_LED, LOW);
    delay(delaytime);
    if (!digitalRead(RED_BTN))
    {
      currentMode = NewGame;
    }
    digitalWrite(YELLOW_LED, HIGH);
    delay(delaytime);
    digitalWrite(GREEN_LED, LOW);
    delay(delaytime);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(BLUE_LED, HIGH);
    delay(delaytime);
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    delay(delaytime);
    digitalWrite(BLUE_LED, LOW);
    if (!digitalRead(RED_BTN))
    {
      currentMode = NewGame;
    }
  }
}