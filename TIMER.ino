#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h>

//Mapping
int buttonPin_START = 7;
int buttonPin_PLUS = 8;
int buttonPin_MOINS = 9;
int NeopixelPin= 10;
int Nombre_neopixels=30; //nombre de LEDs Neopixels connectées
int ledPin = 13;
int buttonState_START = 0;
int buttonState_PLUS = 0;
int buttonState_MOINS = 0;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //Def connexion écran LCD
Adafruit_NeoPixel strip = Adafruit_NeoPixel(Nombre_neopixels, NeopixelPin, NEO_GRB + NEO_KHZ800); //Def ruban neopixel

//Variables
int duree_select = 0;         //compteur selection durée
unsigned long temps = 0;               //durée sélectionnée en s
unsigned long instant_start = 0;              //instant ou l'on a appuyé sur le bouton start en ms
long temps_ecoule=0; //temps ecoule en s depuis instant start
char* tps_affiche="3 min";     //durée min
char* message="SELECT TIME +/-";  //affiché entre instances

//INFOS//

//MAJ David 21/12/16
// intégration adafruit neopixel
// bugfix : jai changé long en unsigned long pour temps et timer, sinon ca buggait au dessus de 10s

//MAJ Yannick 16/12/16
//Temps affiché en minutes mais set en secondes pour tests
//Utiliser timer à la place de delay() cf: http://playground.arduino.cc/Code/AvoidDelay
//Pendant décompte si on appuie sur + et - cela reset



void setup() {
  // put your setup code here, to run once:
  // boutons
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin_START, INPUT);
  pinMode(buttonPin_PLUS, INPUT);
  pinMode(buttonPin_MOINS, INPUT);
  // écran lcd
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print(message);
  lcd.setCursor(0, 1);//change de ligne
  lcd.print("Or START : 3 min");
  // Affichage led neopixel
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

//Boucle principale
void loop() {
strip.setPixelColor(2, (255, 0, 255));
strip.show();
  //on lit l'etat des buttons
  buttonState_START = digitalRead(buttonPin_START);
  buttonState_PLUS = digitalRead(buttonPin_PLUS);
  buttonState_MOINS = digitalRead(buttonPin_MOINS);

  //traitement bouton +
  if (buttonState_PLUS == HIGH){
    if(duree_select < 5){ // un contrôle sur la valeur duree_select pour ne pas dépasser la valeur 5
       duree_select = duree_select + 1 ;
    }
    while(digitalRead(buttonPin_PLUS) == HIGH){
      delay(25);
    }
    calcul_temps();
    lcd.clear();
    lcd.print(tps_affiche);
  }
  
  //traitement bouton -
  if (buttonState_MOINS == HIGH){
    if(duree_select > 0){ // un contrôle sur la valeur duree_select pour ne pas passer à la valeur -1
     duree_select = duree_select - 1 ;
    }
    while(digitalRead(buttonPin_MOINS) == HIGH){
      delay(25);
    }
    calcul_temps();
    lcd.clear();
    lcd.print(tps_affiche);
  }
  
  //traitement bouton start
  if (buttonState_START == HIGH){
    calcul_temps();
    lcd.clear();
    
    digitalWrite(ledPin, HIGH);

    //garde la led allumée
    //set origine temps (on va ensuite la soustraire pour récupérer le temps écoulé)
    instant_start=millis(); 
    //mesure temps écoulé
    while(temps_ecoule != temps){  
      //print the number of seconds since reset:
      lcd.setCursor(7, 1);//change de ligne 
      temps_ecoule = (millis()-instant_start) / 1000 + 1; //calcul du temps écoulé depuis l'appui sur start //+1 pour commencer chrono à 1
      lcd.print(temps_ecoule); 
      affichage_led();
      //
       //Si appuie + et - arrête timer
       if (digitalRead(buttonPin_MOINS)==HIGH && digitalRead(buttonPin_PLUS)==HIGH){
          //Serial.print("RESET");
          break;
       }
    eteindre_led();   
    }
    
    digitalWrite(ledPin, LOW);
    lcd.clear();
    lcd.print(message);

    //Reset temps une fois qu'on a fini une iteration
    temps = 0;
  }
  
  delay(25);
}

//Récupère input duree et set temps du timer
void calcul_temps(){
  switch (duree_select) {
       case 0:
         temps = 3;
         tps_affiche="3 min";
         break;
       case 1:
         temps = 5;
         tps_affiche="5 min";
         break;
       case 2:
         temps = 10;
         tps_affiche="10 min";
         break;
       case 3:
         temps = 30;
         tps_affiche="30 min";
         break;
       case 4:
         temps = 45;
         tps_affiche="45 min";
         break;
       case 5:
         temps = 60;
         tps_affiche="60 min";
         break;
       default:
         break;
     }
}
void affichage_led(){
  //il faut savoir combien de temps s'est écoulé en proportion du temps sélectionné pour savoir combien de led on doit afficher.
int pourcentage_nombre_led_a_allulmer =  (temps_ecoule/temps) *100;
int nombre_led_a_allumer = pourcentage_nombre_led_a_allulmer * Nombre_neopixels;

for (int i=0; i <= nombre_led_a_allumer; i++){ 
strip.setPixelColor(i, (0, 0, 50));
strip.show();
   }
  }
void eteindre_led(){
  //il faut savoir combien de temps s'est écoulé en proportion du temps sélectionné pour savoir combien de led on doit afficher.
int pourcentage_nombre_led_a_allulmer =  (temps_ecoule/temps) *100;
int nombre_led_a_allumer = pourcentage_nombre_led_a_allulmer * Nombre_neopixels;

for (int i=0; i <= Nombre_neopixels; i++){ 
strip.setPixelColor(i, (0, 0, 0));
   }
strip.show();
  }





