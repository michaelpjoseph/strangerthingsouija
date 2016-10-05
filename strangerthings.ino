  // Mike Joseph - michaelpjoseph@gmail.com

// 'Stranger Things' inspired LED quiji board with sound
//    utilizing:  Adafruit neopixel library for control of 100 (2 50x strands) generic 2811 RGB LEDs
//                Adafruit soundboard library for control of Adafruit Sound FX 16 MB OGG / WAV decoder



#include <Adafruit_Soundboard.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h> // for serial control of soundboard
  
#define NUMBER_PIEXEL 100   // number of neopixels
#define LEDPIN    3   // signal control pin 

// --- define letter mapping
#define L_A 97
#define L_B 94
#define L_C 92
#define L_D 90
#define L_E 88
#define L_F 85
#define L_G 82
#define L_H 80
#define L_I 77
#define L_J 48
#define L_K 50
#define L_L 53
#define L_M 55
#define L_N 58
#define L_O 60
#define L_P 63
#define L_Q 66
#define L_R 69
#define L_S 39
#define L_T 36
#define L_U 32
#define L_V 29
#define L_W 27
#define L_X 24
#define L_Y 21
#define L_Z 19

// --- define phrases
#define totalphrases 16;
String PHRASES[] = {
  "right here",
  "run",
  "i know where you sleep",
  "i know what you dream",
  "be afraid",
  "undead",
  "i see you",
  "look behind you",
  "help me",
  "trapped",
  "can you hear it",
  "it got out",
  "hiding",
  "nevermore",
  "five three eight nine",
  "nightmare on kimberly"
};

// --- define sound arrays and serial audio control
#define SFX_TX 5
#define SFX_RX 6
#define SFX_RST 4

SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);
Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);

// --- define sound parameters
#define total_intermission_sounds 5;   // total # of intermission sounds for between phrases
#define total_letter_sounds 5;   // total # of letter sounds for playing during letter cycle

uint8_t current_letter_sound = 0;  // currently selected sounds for uniformity across functions
uint8_t current_intermission_sound = 0;

uint8_t LETTERSOUNDS[] = { 6, 5, 4, 3, 2 };   // indexes of sounds as they exist on 16MB Adafruit FX board as found with the listfiles() funct
uint8_t INTERMISSIONSOUNDS[] = { 1, 10, 9, 8, 7 };


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMBER_PIEXEL, LEDPIN, NEO_GRB + NEO_KHZ800);   // define LED strip object



void setup() {
  Serial.begin(115200);  // Console serial and Soundboard sfx serial need to be at different rates or lights wont work.  I don't know why.
  ss.begin(9600);

/*  DEBUB  -- test to see if SFX board present
 *  
 if (!sfx.reset()) {
    Serial.println("SFX board NOT found");
    while (1);
  }
  Serial.println("SFX board found");
  listfiles();
*/

  listfiles();   // list files and phrase arrays on startup to console
  listphrases();
  
  strip.begin();
}



void loop() {
  //testletters();  // DEBUG - lights letters from A-Z sequentially for mapping purposes
  Serial.println(" - - - - - - - LOOOOOOOP - - - - - - - - ");
  
  //randomize letter sounds, intermission sounds and current phrase
  current_letter_sound = rand() % total_letter_sounds;  
  current_intermission_sound = rand() % total_intermission_sounds;
  uint8_t current_phrase = rand() % totalphrases;
  Serial.println(PHRASES[current_phrase]);  // DEBUG - send current phrase to console

  //send current phrase to the LED board
  printphrase(current_phrase);
 
  //play intermission track and flash the board / decay random LEDs out
  sfx.playTrack(INTERMISSIONSOUNDS[current_intermission_sound]);
  lightningcrash();

  //hard delay between letter sequences
  delay(13000);
}




// --- DEBUG lists all phrases to console
void listphrases() {
  uint8_t tp = totalphrases;
  for (uint8_t i = 0; i < tp; i++) {
    Serial.print(i); Serial.print(" : "); Serial.println(PHRASES[i]);
  }
}


// --- DEBUG lists availabe audio files to console
void listfiles() {
  uint8_t files = sfx.listFiles();
    
  Serial.println("File Listing");
  Serial.println("========================");
  Serial.println();
  Serial.print("Found "); Serial.print(files); Serial.println(" Files");
  for (uint8_t f=0; f<files; f++) {
    Serial.print(f); 
    Serial.print("\tname: "); Serial.print(sfx.fileName(f));
    Serial.print("\tsize: "); Serial.println(sfx.fileSize(f));
  }
}



// DEBUG test letter mapping
void testletters() {
  for (char c='a'; c <= 'z'; c++) {
    uint8_t curr_light = find_light_letter(c);
    strip.setPixelColor(curr_light , 255, 255, 255);
    strip.show();
    delay(1500);
    strip.setPixelColor(curr_light , 0, 0, 0);
    strip.show();
  }
}


// --- returns the light # as defined by the constants at the head of this sketch
uint8_t find_light_letter(char letter) {
  switch (letter) {
    case 'a': return L_A;
    case 'b': return L_B;
    case 'c': return L_C;
    case 'd': return L_D;
    case 'e': return L_E;
    case 'f': return L_F;
    case 'g': return L_G;
    case 'h': return L_H;
    case 'i': return L_I;
    case 'j': return L_J;
    case 'k': return L_K;
    case 'l': return L_L;
    case 'm': return L_M;
    case 'n': return L_N;
    case 'o': return L_O;
    case 'p': return L_P;
    case 'q': return L_Q;
    case 'r': return L_R;
    case 's': return L_S;
    case 't': return L_T;
    case 'u': return L_U;
    case 'v': return L_V;
    case 'w': return L_W;
    case 'x': return L_X;
    case 'y': return L_Y;
    case 'z': return L_Z;
    default:  return 0;
  }
}


// sequentially iterate through all letters in the current phrase and light up the corrosponding light on the board
void printphrase(uint8_t phrasenum) {
  for (uint8_t b = 0; b < PHRASES[phrasenum].length(); b++) {
    char curr_letter = PHRASES[phrasenum].charAt(b);
    uint8_t curr_light = find_light_letter(curr_letter);
    Serial.print("let: "); Serial.print(curr_letter); Serial.print("\tled: "); Serial.print(curr_light); Serial.print("\tsnd:"); Serial.println(current_letter_sound);  // DEBUG - dump lots of info to console while running
    sfx.playTrack(LETTERSOUNDS[current_letter_sound]);
    if (curr_light == 0) {
      lightspace();  // flash all letters dim red and fade out on 'space' char
    } else {
      lightletter(curr_light);  // light correct light
    }
  }
}

// flash all letters dim red and fade out on 'space' char
void lightspace() {
  for (uint8_t i = 0; i <= strip.numPixels(); i++) {
    strip.setPixelColor(i    , 0, 66, 0);
  }
  strip.show();
  for (int i = 70; i >= 0; i-- ) {
    strip.setBrightness(i);
    strip.show();
    delay(36);  
  }
  strip.setBrightness(255);
}


// light up LED define by letter_num with a random RGB color
void lightletter(int letter_num) {
  uint8_t red = rand() % 255;
  uint8_t blu = rand() % 255;
  uint8_t gre = rand() % 255;
  strip.setPixelColor(letter_num, red, gre, blu);
  strip.setBrightness(255);
  strip.show();
  for (int i = 255; i >= 0; i-- ) {  // fade / dim current LED out slowly
    strip.setBrightness(i);
    strip.show();
    delay(2);
  }
  strip.setBrightness(255);  // reset strip brightness for next letter
  delay(2000);  // delay between letters - should probably be in printphrase()
}


//flash all lights several times then light them all up with random colors and decay out 
void lightningcrash() {
  // -- first - single flash - blue
  for (uint8_t i = 0; i <= strip.numPixels(); i++) {
    strip.setPixelColor(i    , 0, 0, 175);
  }
  strip.show();
  delay(15);
  strip.setBrightness(0);
  strip.show();
  strip.setBrightness(255);
  delay(250);
  // -- second - set of rapid flashes - white (currently at 50 RGB,   255 RGB is REALLY bright)
  for (int times = 0; times < 4; times++) {
    for (int i = 0; i <= strip.numPixels(); i++) {
      strip.setPixelColor(i    , 50, 50, 50);
    }
    strip.show();
    delay(50);
    strip.setBrightness(0);
    strip.show();
    strip.setBrightness(255);
    delay(50);
  }
  // -- third - single flash - bright white
  for (int i = 0; i <= strip.numPixels(); i++) {
    strip.setPixelColor(i    , 255, 255, 255);
  }
  strip.show();
  delay(200);
  // -- random light and decay sequence
  for (int i = 0; i <= strip.numPixels(); i++) {
    int red=rand() % 255;
    int gre=rand() % 255;
    int blu=rand() % 255;
    strip.setPixelColor(i, strip.Color(red, gre, blu));
  }
  strip.show(); 
  for (int i = 255; i >= 0; i-- ) {
    strip.setBrightness(i);
    strip.show();
    delay(25);  // decay speed
  }
  strip.setBrightness(255); // reset brightness
}

