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
  servo.write(46);
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

void dramatic_dots(int dots=3, int millis=500) {
  delay(millis);
  for(int i = 0; i < dots; i++) {
	lcd.print("."); 
    delay(millis);
  }
}

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

void fillArray(unsigned long from[], unsigned long (&to)[3]) {
  size_t sz_f = sizeof(from);
  size_t sz_t = sizeof(to);
  if (sz_t != sz_f) return;
  
  for(size_t i = 0; i < sz_f; i++) {
    to[i] = from[i];
  }
  
}

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

int command_read;
const int start_read = 1022;
const int highscore_read = 512;
bool readStart() {
  command_read = analogRead(startPin);
  return (command_read >= start_read*(1.8 - TOLERANCE));
}

bool readHS() {
  command_read = analogRead(startPin);
  return (command_read >= highscore_read*(2-TOLERANCE));
}

/* END BUTTONS */

void setup()
{
  pinMode(startPin, INPUT);
  pinMode(A0, INPUT);
  pinMode(soundPin, OUTPUT);
  pinMode(DCPin, OUTPUT);
  
  for (int i=0; i < 5; i++) {
    pinMode(pins[i], OUTPUT);
  }
  
 /* pinMode(target1, OUTPUT);
  pinMode(target2, OUTPUT);
  pinMode(target3, OUTPUT);
  pinMode(target4, OUTPUT);
  pinMode(target5, OUTPUT);*/
  
  noTone(soundPin);
  
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  
  intro();
  
  lcd.print("Inititalizing");
  delay(500);
	dramatic_dots();
  if (!digitalRead(startPin)) {
    calibrate_photoresistors();
  }

  if (readStart()) DEBUG_MODE = true;
  setup_servo();
  
  stop_motor();
  
  lcd.setCursor(0,1);
  lcd.print("Ready!");
  delay(600);
  lcd.clear();
  
}

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


void loop()
{
  lcd.clear();
  bool gameStart = false;
  lcd.print("Press Start!");


  /* DEBUG MODE */

  if(DEBUG_MODE) {
    lcd.clear();
    lcd.print("DEBUG MODE");


    int lit_index = changeTarget(0);
    while(true) {
      if (readStart()) {
        move_servo(true); 
        delay(15);
        //start_motor();
      } else if (readHS()) {
        //lit_index = changeTarget(lit_index);
        //Serial.println(lit_index);
        //delay(500);
        move_servo(false); 
        delay(15);
        //stop_motor();
      } else {

      }



    }
  }

  /* END DEBUG MODE */
  
  
  //int command_read;
  while(!gameStart) {

   // command_read = analogRead(startPin);

    if( readStart() ) {
      //Start button pressed
      gameStart = true;
      Serial.println("START! ");
      //start_motor();
      //move_servo(true);
      //buzz();

    } else if ( readHS() ) {
      showHighScores();
      Serial.println("HIGHSCORES! ");
      //stop_motor();
      //move_servo(false);
      //buzz();
    }
  
    //int led_read = read_target();

    //total = total - readings[readIndex];
    //readings[readIndex] = map(read_target(), -500, 1023, 0, 100);

    //total = total + readings[readIndex];

    //readIndex += 1;

    //if (readIndex >= numReadings){
    //  readIndex = 0;
    //  //run_count++;

    //  if(!avg_ready[lit_index]) led_baselines[lit_index] = avg;
    //  avg_ready[lit_index] = true;

    //  if(run_count == 4) {
    //    avg_ready[lit_index] = false;
    //    run_count = 0;
    //  }
    //}

    //avg = total / numReadings;


   
    //bool state = avg_ready[lit_index] && ( avg > led_baselines[lit_index] + 3 );
    //if (state) {
    //  score_count++;
    //} else {
    //  Serial.print("MISS ");
    //}

    //if (score_count >= 5) {
    //  Serial.print("SCORE ");
    //  score_count = 0;
    //  change = true;
    //}

    //Serial.print(avg);
    //Serial.print(" ");
    //Serial.print(readIndex);
    //Serial.print(" ");
    //Serial.print(readings[readIndex-1]);
    //Serial.print(" ");
    //Serial.println(led_baselines[lit_index]);

    //delay(1);


   //bool val = digitalRead(startPin);
    //if(val) {
    // gameStart = true; 
    //}
  }
  
  Serial.println("Game has started!");
  game();
}

int changeTarget(int lit_index) {
  digitalWrite(pins[lit_index], LOW);
  
  int new_lit_index;
  do {
  	new_lit_index = random(0, 5);
  } while (lit_index == new_lit_index);
  
  digitalWrite(pins[new_lit_index], HIGH);
  return new_lit_index;
}

int checkHighScore(unsigned long score, unsigned long (&scores)[3], bool mode) {
  unsigned long shifted[3]; //New array 
  
  //Copy contents of scores[] tp shifted[]
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
      int j;
      for (j = i+1; j < 3; j++) {
        shifted[j] = scores[j-1];
      }
      shifted[i] = score;
      
      printArray(shifted);
      //fillArray(shifted, scores);
      for(int i = 0; i < 3; i++) {
        scores[i] = shifted[i];
      }
      
      return i+1;
    }
  }
  return 0;
}


void buzz(int n=0) {
  Serial.println("SCORE! ");
  tone(soundPin, 262 + (33*n), 500);
  delay(500);
  //digitalWrite(soundPin, HIGH);
  //delay(100);
  //digitalWrite(soundPin, LOW);
  //delay(500);
  noTone(soundPin);
}


void game() {
  lcd.clear();
  Serial.println("Running!");
  
  unsigned long start = millis();
  unsigned long current = 0;
  
  int hits = 0;
  
  int lit_index = changeTarget(0);
  
  start_motor();
  move_servo(true);


  int numReadings = 50;
  int readings[numReadings];
  int readIndex = 0;
  int total = 0;
  int avg = 0;

  bool avg_ready[5] = {0,0,0,0,0};
  int led_baselines[5] = {0,0,0,0,0};
  int led_tolerance[5] = {3,3,3,3,3};

  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }

  bool change = false;

  int run_count = 0;
  int score_count = 0;


  while(current <= MAX_TIME && hits < NUM_HITS) {
    
    //if (SERVO_NATURAL_FREQUENCY <= 1 && (1/run_count) == SERVO_NATURAL_FREQUENCY) {
    //  step_servo();
    //} else if (SERVO_NATURAL_FREQUENCY >= 1) {
    //  int n = ceil(1/SERVO_NATURAL_FREQUENCY);
    //  for(int i = 0; i < n; i++) {
    //    step_servo();
    //  }
    //}


    if (change) {

      change = false;
      
      lit_index = changeTarget(lit_index);

      for (int i = 0; i < numReadings; i++) {
        readings[i] = led_baselines[lit_index];
      }
     
      readIndex = 0;
      total = led_baselines[lit_index] * numReadings;
      avg = led_baselines[lit_index];

    }

    int led_read = read_target();

    total = total - readings[readIndex];
    readings[readIndex] = map(read_target(), -500, 1023, 0, 100);

    total = total + readings[readIndex];

    readIndex += 1;

    if (readIndex >= numReadings){
      readIndex = 0;
      //run_count++;

      if(!avg_ready[lit_index]) led_baselines[lit_index] = avg;
      avg_ready[lit_index] = true;

      if(run_count == 4) {
        avg_ready[lit_index] = false;
        run_count = 0;
      }
    }

    if ( readHS() ) led_baselines[lit_index] = avg;

    avg = total / numReadings;

    bool state = avg_ready[lit_index] && ( avg > led_baselines[lit_index] + led_tolerance[lit_index] );
    if (state) {
      score_count++;
    }

    if (score_count >= 5) {
      //Serial.print("SCORE ");
      score_count = 0;
      change = true;
      hits++;
      buzz(lit_index);
      Serial.print(" ");
      Serial.print(hits);
      Serial.print("/");
      Serial.println(NUM_HITS);
    }

    //if(read_target() > voltage_lows[lit_index] * TOLERANCE) {
    //  	hits++;
    //  	buzz();
    //  	Serial.print(" ");
    //  	Serial.print(hits);
    //  	Serial.print("/");
    //  	Serial.println(NUM_HITS);
  	//	//lit_index = changeTarget(lit_index);
    //	//Serial.println(lit_index);
    //}
    
    
    
    current = millis() - start; //in ms
    double rounded = current / 1000.0;

    lcd.setCursor(0,0);
    lcd.print(rounded, 3);
    
    lcd.setCursor(0, 1);
    
    
    lcd.print(hits);
    lcd.print(" / ");
    lcd.print(NUM_HITS);
    
    run_count++;
    

    /*DEBUG*/
    Serial.print(avg);
    Serial.print(" ");
    Serial.print(readIndex);
    Serial.print(" ");
    Serial.print(readings[readIndex-1]);
    Serial.print(" ");
    Serial.print(led_baselines[lit_index]);
    Serial.print(" ");
    Serial.print(score_count);
    Serial.println("");


    //Serial.println(read_target());
    /*END DEBUG*/
  }
  
  stop_motor();
  
  digitalWrite(pins[lit_index], LOW);
  
  int highscore = checkHighScore(current, high_scores, false);
  
  printArray(high_scores);
  
  Serial.print("HS: ");
  Serial.println(highscore);
  lcd.clear();
  lcd.print("Your score is:");

  lcd.setCursor(0,1);
  double score = current / 1000.0;
  lcd.print(score, 3);
  
  if (highscore > 0) {
  	lcd.setCursor(11, 1); 
    lcd.print("#");
    lcd.print(highscore);
    lcd.print(" HS");
  }
  
  move_servo(false);

  delay(2000);
}

int read_target() {
  int val = analogRead(voltage_read);
  return val;
}
 

