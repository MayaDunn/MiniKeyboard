// Maya Dunn 2018

#include <Keyboard.h>

int layer = 0;

int pinArr[7] = {2, 3, 4, 5, 15, 14, 16};
int numCols = 4;
int numRows = 3;
int mode = 1;
bool shiftToggle = false;

char email[] = "youremailhere";

/// 0 = status
/// 2 = toggle mac vs pc mode
/// 3 = punctuation layer
/// 4 = shift
/// 5 = email
/// 6 = tab
/// 7 = down
/// 8 = delete

/// 10 = return

const int maxOutKeys = 5; // how many keypresses we can get before we stop counting
char keys[maxOutKeys];
int keyLoc = 0;

bool MAC = true;
bool pressed = true;

int keyMatrix[3][3][3][3] = {
  {
    // letters
    {
      {101, 116, 3  },      ///  [E    T    PUNCT]
      {97 , 111, 104},      ///  [A    O    H]
      {110, 105, 115}       ///  [N    I    S]
    }, {
      {114, 100, 3  },      ///  [ R    D   PUNCT]
      {108, 117, 119},      ///  [ L    U   W]
      {109, 99 , 103}       ///  [ M    C   G]
    }, {
      {102, 121, 3  },      ///  [ F    Y   PUNCT]
      {112, 118, 107},      ///  [ P    V    K]
      {98,  106, 120}       ///  [ B    J    X]
    }
  }, {
    // commands
    {
      {49, 50, 51},       ///  [ 1    2    3]
      {113, 119, 101},     ///  [q     w    e]
      {97, 115, 100}       ///  [a     s    d]
    }, {
      {-99, -120, -118},     ///  [ copy    cut    paste]
      {-110, -116, -119},     ///  [ n      t       close]
      {-91, 7, -114}         ///  [ back    down arrow       refresh]
    }, {
      {5   , 0   , 2  },     ///  [email    status    MAC MODE]
      {-122, -97 , -90},     ///  [ undo    all    redo]
      {-102, -115, -113}     ///  [ find    save    quit]  
    }
  }, {
   // punctuation
   {
      {122, 113, 4 },    ///  [Z    Q    SHIFT]
      {32 , 6,   10},    ///  [space    tab    return]
      {46,  8,   48}     ///  [.    delete   0]
    }, { // idk what to do with this layer
      
      {0, 0, 0},      ///  [        ] 
      {0, 0, 0},      ///  [        ]
      {0, 0, 0}       ///  [        ]
    }, { // these ones won't go back to letters until you press a mode button
      {49, 50, 51},      ///  [ 1    2   3]
      {52, 53, 54},      ///  [ 4    5   6]
      {55, 56 , 57}      ///  [ 7    8   9]
    }
  }
};

void setup() {
  // put your setup code here, to run once:
  for (int setPin = 0; setPin < numRows+numCols; setPin++) {
    pinMode(pinArr[setPin], INPUT);
  }
  
  Keyboard.begin();
}

void switchMode() {
  if (mode == 1) {
    mode = 0;
  } else {
    mode = 1;
  }
}

void doCol(int col) {
    // turn on column
    pinMode(pinArr[col], OUTPUT);
    // ground the pin - this will make the switch a short to ground which will read as LOW when there's a pullup resistor to 5V
    // no resistor needed on the circuit s no current anyway
    digitalWrite(pinArr[col], LOW);
    int toggleswitch = 0;
    for (int curRow = numCols; (curRow < numRows+numCols) && (keyLoc <= maxOutKeys); curRow++) {
      pinMode(pinArr[curRow], INPUT_PULLUP);
      if (digitalRead(pinArr[curRow]) == LOW) {
        if (col == 0) {
          layer = curRow-numCols;
          if (mode == 2) {
            mode = 0;
          }
          Keyboard.releaseAll();
          toggleswitch++;
        } else {
          pressed = true;
          keys[keyLoc] = keyMatrix[mode][layer][curRow-numCols][col-1];
          if (keys[keyLoc] == 3) {
            // go to punctuation
            keyLoc--;
            mode = 2;
            delay(200);
          }
          if (keys[keyLoc] == 4) {
            // shift when back to letters
            keyLoc--;
            mode = 0;
            shiftToggle = true;
            delay(200);
          }
          keyLoc++;
        }
      } else if (layer == 0 && col > 0 && mode) {
          Keyboard.release(keyMatrix[mode][layer][curRow-numCols][col-1]);
      }
      // disable again
      pinMode(pinArr[curRow], INPUT);
    }
    // disable again
    pinMode(pinArr[col], INPUT);
    if (toggleswitch == 3 && pressed) { // switch to letters but only if we've had a keystroke in the middle.
      switchMode();
      layer = 0;
      pressed = false;
    } 
}

void loop() {
    keyLoc = 0;
    
    // Cycle through each column
    for (int curCol = 0; (curCol < numCols) && (keyLoc <= maxOutKeys); curCol++) {
      doCol(curCol);
    } 
    
  for (int i = 0; i < keyLoc; i++) {
      if(keys[i] == 0) {
        if (MAC) {
          Keyboard.print("Commands are for Mac");
        } else {
          Keyboard.print("Controls are for Windows");
        }
      } else if(keys[i] == 5) { 
        Keyboard.print(email);
      } else if(keys[i] == 6) { 
        Keyboard.press(KEY_TAB);
      } else if(keys[i] == 7) { 
        Keyboard.press(KEY_DOWN_ARROW);
      } else if(keys[i] == 7) { 
        Keyboard.press(KEY_DELETE);
      } else if (keys[i] == 2) {
          MAC = !MAC;
      } else if (keys[i] < 0 && MAC) {
        Keyboard.press(KEY_LEFT_GUI); // command it
        Keyboard.press(keys[i]*-1);
        Keyboard.release(KEY_LEFT_GUI);
      } else if (keys[i] < 0) {
        Keyboard.press(KEY_LEFT_CTRL); // ctrl it
        Keyboard.press(keys[i]*-1);
        Keyboard.release(KEY_LEFT_CTRL);
      } else {
        if (shiftToggle) {
          Keyboard.press(133); // press the shift key
        }
        Keyboard.press(keys[i]);
      }
  }

  // don't release keys if in layer 0, mode 1
  if (keyLoc && !(layer==0 && mode==1)) {
    Keyboard.releaseAll();
    delay(200);
    if (mode == 2 && layer != 2) {
      // return from punctuation to letters
      // after a key has been pressed unless acting as numpad
      mode = 0;
    }
    if (mode == 0) {
      layer = 0;
    }
    shiftToggle = 0;
  }
}

