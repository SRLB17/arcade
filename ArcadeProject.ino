/* Program Name: Zeanah Strike (Arcade Game Project)
 * Program Description: Code for an arduino-based arcade game
 * in EF158 Spring 2025. The game has targets that the user must
 * hit with a laser, the goal being to hit the required number
 * of targets in the fastest time possible.
 *
 * Created 30 Jan. 2025 
 * by Spencer R. Byrd <sbyrd29@vols.utk.edu>
 * last modified 23 April 2025
 *
 * Credits: 
 * - David A. Mellis, Tom Igoe for arduino data smoothing (https://www.arduino.cc/en/Tutorial/BuiltInExamples/Smoothing)
 */

#include <LiquidCrystal.h>
#include <math.h>
#include <Servo.h>


/* SETTINGS */
bool DEBUG_MODE = false; //Triggers the debug mode

const unsigned long MAX_TIME = 120000; // 2min * 60s * 1000 ms, amount of time before the game will auto-end
const int NUM_HITS = 20; //Number of times user needs to hit the targets
const int TOLERANCE = 1.2; //Global base tolerance for various statements
/* END SETTINGS */

/* SERVO CONSTANTS */
const int SERVO_RANGE[2] = {180, 0}; //(start, end) values for servo
const int SERVO_MIN = 500, SERVO_MAX = 2500; //Min and max pulse width for arduino
const int servoPin = 9;
/* END SERVO CONSTANTS */

/* VARIOUS PINS */
const int startPin = A5; //Both buttons
const int soundPin = 10; //Piezo buzzer
const int DCPin = 6;     //DC motor
/* END VARIOUS PINS */

/* TARGET GLOBALS */
int voltage_read = A0; //Target sensor input
int voltage_lows[5];   //Baselines for voltage
int pins[5] = {A4, 7, 8, A1, 13}; //Array of target pins
/* END TARGET GLOBALS */

//High score array
unsigned long high_scores[3] = {0,0,0};

/* LCD */
const int rs = 12, e = 11, db4 = 5, db5 = 4, db6 = 3, db7 = 2;
LiquidCrystal lcd(rs, e, db4, db5, db6, db7);
/* END LCD */


/* SERVO STUFFZ */

Servo servo;
int pos; // 0 := end value, 1 := start value

//Takes nothing, sets up the servo
void setup_servo() {
  servo.attach(servoPin, SERVO_MIN, SERVO_MAX);
  servo.write(SERVO_RANGE[1]);
}

/**
 * Takes two booleans, a start and a toggle flag:
 * start | toggle
 *  0       0    -> moves servo to end position
 *  1    |  0    -> moves servo to start position
 *  0    |  1    -> toggles position
 *  1    |  1    -> toggles position
 */
void move_servo(bool start=true, bool toggle=false) {

  if (toggle) {
    //Toggle flag is set, switch position
    if(pos) {
      //Position is at start, move to end
      servo.write(SERVO_RANGE[0]);
      pos = 0;
    } else {
      //Position is at end, move to start
      servo.write(SERVO_RANGE[1]);
      pos = 1;
    }
  } else if (start) {
    //Start flag is set, 
    servo.write(SERVO_RANGE[1]);
    pos = 1;
  } else {
    servo.write(SERVO_RANGE[0]);
    pos = 0;
  }
  delay(1);
}


/* END SERVO CODE */

/* DC MOTOR */

void start_motor() {
  digitalWrite(DCPin, HIGH);
}

void stop_motor() {
  digitalWrite(DCPin, LOW); 
}

/* END DC MOTOR */

//Takes an amount of dots, and a delay, then outputs the amount of
//dots with that delay of milliseconds in between to the lcd
void dramatic_dots(int dots=3, int millis=500) {
  delay(millis);
  for(int i = 0; i < dots; i++) {
    lcd.print("."); 
    delay(millis);
  }
}

//Used for debugging, takes an unsigned long array, then prints it to Serial
void printArray(unsigned long arr[]) {
  Serial.print("{");
  int i;
  for(i = 0; i < sizeof(arr); i++) {
    Serial.print(arr[i]);
    Serial.print(", ");
  }
  Serial.print(arr[i]);
  Serial.println("}");
}

//Takes two arrays, then fills the second with the contents of the first
void fillArray(unsigned long from[], unsigned long (&to)[3]) {
  size_t sz_f = sizeof(from);
  size_t sz_t = sizeof(to);
  if (sz_t != sz_f) return;

  for(size_t i = 0; i < sz_f; i++) {
    to[i] = from[i];
  }

}

//LCD intro sequence
void intro() {
  lcd.print("Welcome to");
  dramatic_dots(3, 600);
  lcd.clear();
  lcd.print("Zeanah");
  lcd.setCursor(6, 1);
  lcd.print("Strike!");
  delay(1000);
  lcd.clear();
}

//Gets the voltage read from each photocell, then prints it to Serial
void calibrate_photoresistors() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(pins[i], HIGH);
    voltage_lows[i] = analogRead(voltage_read);
    delay(2000);
    digitalWrite(pins[i], LOW);
  }

  Serial.print("Voltages: {");
  for (int i = 0; i < 4; i++) {

    Serial.print(voltage_lows[i]);
    Serial.print(", ");
  }
  Serial.print(voltage_lows[4]);
  Serial.println("}");
}

/* BUTTONS */

//Voltage read from the start pin
int command_read;
//Read values for the start button and the HS button
const int start_read = 1022;
const int highscore_read = 512;

//Takes nothing, returns true if the start button is being pressed
bool readStart() {
  command_read = analogRead(startPin);
  return (command_read >= start_read*(1.8 - TOLERANCE));
}

//Takes nothing, returns true if the HS button is being pressed
bool readHS() {
  command_read = analogRead(startPin);
  //return (command_read >= highscore_read * (2.0-TOLERANCE));
  return (command_read >= 400 && !readStart());
}

/* END BUTTONS */

//Setup function for Arduino
void setup()
{
  //Setup the pinmode for various pins
  pinMode(startPin, INPUT);
  pinMode(A0, INPUT);
  pinMode(soundPin, OUTPUT);
  pinMode(DCPin, OUTPUT);

  //Set the pinmode for each photocell target
  for (int i=0; i < 5; i++) {
    pinMode(pins[i], OUTPUT);
  }

  //Make sure the speaker isn't buzzing
  noTone(soundPin);

  //Start the Serial
  Serial.begin(9600);

  //Start the lcd
  lcd.begin(16, 2);

  //Play the LCD intro sequence
  intro();

  //Calibration sequence, if the start button is pressed, skip the photocell calibration
  //and enter DEBUG mode
  lcd.print("Inititalizing");
  delay(500);
  dramatic_dots();
  if (!digitalRead(startPin)) {
    calibrate_photoresistors();
  }
  if (readStart()) DEBUG_MODE = true;

  //Run the servo setup
  setup_servo();

  //Make sure DC motor isn't running
  stop_motor();

  //Tell the user that setup is complete, then enter loop()
  lcd.setCursor(0,1);
  lcd.print("Ready!");
  delay(600);
  lcd.clear();

}

//Takes nothing, displays each highscore to the LCD
void showHighScores() {
  for (int i = 0; i < 3; i++) {
    lcd.clear();
    lcd.print("#");
    lcd.print(i+1);
    lcd.print(" High Score:");
    lcd.setCursor(0,1);

    double score = high_scores[i] / 1000.0;

    lcd.print(score, 3);

    delay(1000);
  }

  lcd.clear();
  lcd.print("Press Start!");
}


//Main Arduino loop
void loop()
{
  //Print the main homescreen text to LCD
  lcd.clear();
  bool gameStart = false;
  lcd.print("Press Start!");


  /* DEBUG MODE */

  /** 
   * The code below will only run if the DEBUG_MODE flag is set.
   * If set, the code execution will break off and will enter the 
   * while loop below, where the programmer can plug in debug functions
   * for each button. 
   *
   */
  if(DEBUG_MODE) {
    lcd.clear();
    lcd.print("DEBUG MODE");

    //Light up a target if we need to change them
    int lit_index = changeTarget(0);
    
    while(true) {


      if (readStart()) {
        /* Start button pressed */

        move_servo(true); 
        //delay(15);
        start_motor();
        Serial.print(analogRead(startPin));
        Serial.println(" Start!");

      } else if (readHS()) {
        /* HS button pressed */
        Serial.print(analogRead(startPin));

        Serial.println(" HS");
        //lit_index = changeTarget(lit_index);
        //Serial.println(lit_index);
        //delay(500);
        move_servo(false); 
        //delay(15);
        stop_motor();

      } else {
        /* Neither button pressed */

        Serial.println(analogRead(startPin));
      }
      /* Run every time */
      

    }
  }

  /* END DEBUG MODE */


  //Check if the game has been requested to start
  while(!gameStart) {

    if( readStart() ) {
      /* Start button pressed */
      //Start the game!
      gameStart = true;
      Serial.print(analogRead(startPin));
      Serial.println("START! ");

    } else if ( readHS() ) {
      /* HS button pressed */
      //Show the user the current high scores, then return to the home screen
      showHighScores();
      Serial.println("HIGHSCORES!");
    } else {
      
      Serial.println(analogRead(startPin));
    }
  }

  //Start button has been pressed, so run the game loop
  Serial.println("Game has started!");
  game();
}

/* Takes an index [0,4] of a target, then
 * turns that target off, and turns a random target
 * [0,4], that is NOT the same as the old, on, then
 * returns the index of the new target
 */
int changeTarget(int lit_index) {
  //Turn old target off
  digitalWrite(pins[lit_index], LOW);

  //Randomly pick a new target that is not equal to the old
  int new_lit_index;
  do {
    new_lit_index = random(0, 5);
  } while (lit_index == new_lit_index);

  //Turn the new target on, then return the index
  digitalWrite(pins[new_lit_index], HIGH);
  return new_lit_index;
}

/* Takes a score, the high scores array, and a mode flag
 * Much of the code below is superfluous, but in case
 * other game modes are added, there are different ways of
 * checking scores against other scores (i.e. low score wins vs. high score).
 * If mode is true, high-score wins
 * If mode is false, low score wins
 *
 * returns the players "place" (1st, 2nd, 3rd, etc.) or 0 if it wasn't a highscore
 */
int checkHighScore(unsigned long score, unsigned long (&scores)[3], bool mode) {
  unsigned long shifted[3]; //New array 

  //Copy contents of scores[] to shifted[]
  for (int i = 0; i < 3; i++) {
    shifted[i] = scores[i];
  }

  //Loop through each high score and check if score is a winner
  for(int i = 0; i < 3; i++) {
    //Score will be added in the place after
    if (score == scores[i]) continue;
    //if mode==false, lower score is counted, if mode==true, greater
    bool state = (mode && (score > scores[i])) || (!mode && (score < scores[i] || scores[i] == 0UL));
    Serial.println(state);
    if ( state ) {
      /* Score was counted */
      //Shift the array and add the new score
      int j;
      for (j = i+1; j < 3; j++) {
        shifted[j] = scores[j-1];
      }
      shifted[i] = score;

      printArray(shifted);
      for(int i = 0; i < 3; i++) {
        scores[i] = shifted[i];
      }

      //Return the place of the high score
      return i+1;
    }
  }
  return 0;
}


//Takes an nth note, then plays a short sound on the buzzer
void buzz(int n=0) {
  Serial.println("SCORE! ");
  //262Hz ≈ C, Δnote ≈ 33Hz 
  tone(soundPin, 262 + (33*n), 500);
  delay(500);
  noTone(soundPin);
}

//Takes nothing, runs the main game
void game() {
  lcd.clear();
  Serial.println("Running!");

  //Start time and current time in ms
  unsigned long start = millis();
  unsigned long current = 0;

  //Number of targets the user has hit
  int hits = 0;

  //The index of the currently lit target
  int lit_index = changeTarget(0);

  //Start the DC motor and the servo
  start_motor();
  move_servo(true);


  //Vars for photocell data smoothing
  //Credit to David A. Mellis for the basics of data smoothing
  //https://www.arduino.cc/en/Tutorial/BuiltInExamples/Smoothing
  int numReadings = 50;
  int readings[numReadings];
  int readIndex = 0;
  int total = 0;
  int avg = 0;

  //Whether or not each target has collected enough data for a steady average
  bool avg_ready[5] = {0,0,0,0,0};
  //Each target's baseline reading
  int led_baselines[5] = {0,0,0,0,0};
  //Tolerances for each photocell
  int led_tolerance[5] = {3,3,3,3,3};

  //Populate the readings array with 0's
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }

  //Whether or not to change the target
  bool change = false;

  //Counts the number of runs, and the number of possible scores respectively
  int run_count = 0;
  int score_count = 0;

  //Main game loop, run while not all the targets have been hit and we're still under the time limit
  while(current <= MAX_TIME && hits < NUM_HITS) {

    //Check if the target needs to be changed (user has hit it)
    if (change) {

      //Reset the change var
      change = false;

      //Change the target
      lit_index = changeTarget(lit_index);

      //Populate the averages array with the new target's baselines
      for (int i = 0; i < numReadings; i++) {
        readings[i] = led_baselines[lit_index];
      }

      //Reset the smoothing vars
      readIndex = 0;
      total = led_baselines[lit_index] * numReadings;
      avg = led_baselines[lit_index];

      //Move the servo
      move_servo(false, true);
    }

    //Get data from the sensor
    int led_read = read_target();

    //Remove the old reading
    total = total - readings[readIndex];

    //Map the new read to a number [0,100]
    readings[readIndex] = map(read_target(), -500, 1023, 0, 100);

    //Add the new reading to a running total
    total = total + readings[readIndex];

    readIndex += 1;

    //A `numReadings` amount of values has been read
    if (readIndex >= numReadings){
      //Reset the index
      readIndex = 0;

      //The first time `numReadings` has been read, the average is steady, so set the baseline
      if(!avg_ready[lit_index]) led_baselines[lit_index] = avg;

      //Baseline is set
      avg_ready[lit_index] = true;

      //If the run count is 4, reset the baseline
      if(run_count == 4) {
        avg_ready[lit_index] = false;
        run_count = 0;
      }
    }

    //If a target is acting up, pressing the HS button will automatically "hit" it for the user and reset the baseline
    if ( readHS() ) {
      avg_ready[lit_index] = false;
      change = true;
    }

    //Get the current average
    avg = total / numReadings;

    //State is true if the average is greater than the baseline plus a tolerance
    bool state = avg_ready[lit_index] && ( avg > led_baselines[lit_index] + led_tolerance[lit_index] );

    //Update the score count
    if (state) {
      score_count++;
    }

    //If the score count is sufficiently great, we have a score!
    if (score_count >= 5) {
      //Reset the score count
      score_count = 0;

      //Set the change flag to true
      change = true;

      //Update the current hits, and buzz the buzzer
      hits++;
      buzz(lit_index);

      //Log some messages to the Serial moinitor
      Serial.print(" ");
      Serial.print(hits);
      Serial.print("/");
      Serial.println(NUM_HITS);
    }

    //Get the current time and round it
    current = millis() - start; //in ms
    double rounded = current / 1000.0;

    //Print the current time
    lcd.setCursor(0,0);
    lcd.print(rounded, 3);

    //Print the number of hits compared to the total
    lcd.setCursor(0, 1);
    lcd.print(hits);
    lcd.print(" / ");
    lcd.print(NUM_HITS);

    //Update the run count
    run_count++;

    /*DEBUG*/
    //Debugging messages while the game is running
    // [avg] [readIndex] [current read] [baseline] [score count]
    ////Serial.print(avg);
    ////Serial.print(" ");
    ////Serial.print(readIndex);
    ////Serial.print(" ");
    ////Serial.print(readings[readIndex-1]);
    ////Serial.print(" ");
    ////Serial.print(led_baselines[lit_index]);
    ////Serial.print(" ");
    ////Serial.print(score_count);
    ////Serial.println("");
    /*END DEBUG*/
  }

  //Stop the DC motor
  stop_motor();

  //Turn the last target off
  digitalWrite(pins[lit_index], LOW);

  //Check if the score was a high score
  int highscore = checkHighScore(current, high_scores, false);

  //Print out the high scores to the Serial monitor
  printArray(high_scores);
  Serial.print("HS: ");
  Serial.println(highscore);

  //Tell the user their score
  lcd.clear();
  lcd.print("Your score is:");

  //Score itself
  lcd.setCursor(0,1);
  double score = current / 1000.0;
  lcd.print(score, 3);

  //If the user had a high score, an indeicator will be in bottom right (ex: #1 HS)
  if (highscore > 0) {
    lcd.setCursor(11, 1); 
    lcd.print("#");
    lcd.print(highscore);
    lcd.print(" HS");
  }

  //Move servo to end
  move_servo(false);

  //Wait a couple seconds for everything to wrap up
  delay(2000);
}

//Takes nothing, reads a value from the read pin
int read_target() {
  int val = analogRead(voltage_read);
  return val;
}
