#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h>

//Mapping
int buttonPin_START = 7;
int buttonPin_PLUS = 8;
int buttonPin_MOINS = 9;
int NeopixelPin= 10;
int Nombre_neopixels=22; //nombre de LEDs Neopixels connectées
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
char* temps_affiche[] = {"3 min","5 min","10 min","15 min","20 min","30 min","45 min","1h","1h 30","2h"};     //les différents temps affichés dans le menu, à faire correspondre avec les temps internes ci dessous
int temps_propose[] = {3,5,10,15,20,30,45,60,90,120};// les différents temps proposés dans le menu ( variable interne en secondes, n'est pas affichée)
char* message="SELECT TIME: +/-";  //affiché entre instances
//INFOS//

//MAJ Flavien 22/12/16
// Modification de la fonction "calcul_temps". Utilisation de deux tableaux: l'un pour affecter le temps (en seconde), l'autre pour l'affichage dans le menu.
// -> Pour ajouter un temps au menu, ajouter son nombre de seconde dans le tableau "temps_propose" et le texte équivalent dans "temps_affiche" à la même position.
// -> Affichage des temps dynamiquement directement dans le menu.
// Ajout d'une fonction "affichage_menu" pour afficher le menu dans son ensemble.
// Lors de l'appui sur "+", détection automatique (avec sizeof() ) de la taille du tableau "temps_propose" pour bloquer le compteur "duree_select"
// ->Bug: la taille du tableau est doublé, seule solution pour le moment, diviser par deux (et le -1 car le compteur commence à 0).
// Le temps affiché est un décompte et fini par zéro.
// Lors du retour au menu, le temps par défaut est celui sélectionné auparavant.
// Ajout du texte "Temps restant:" lors du décompte du temps.
// Bugfix: Les LED ne clignotent plus, j'ai mis le "eteindre_led()" en dehors de la boucle while.

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
  affichage_menu();
  // Affichage led neopixel
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
}

//Boucle principale
void loop() {

  //on lit l'etat des buttons
  buttonState_START = digitalRead(buttonPin_START);
  buttonState_PLUS = digitalRead(buttonPin_PLUS);
  buttonState_MOINS = digitalRead(buttonPin_MOINS);

  //traitement bouton +
  if (buttonState_PLUS == HIGH){
    lcd.setCursor(0, 0);
    if(duree_select < (sizeof(temps_propose))/2-1){ // un contrôle sur la valeur duree_select pour ne pas dépasser la valeur 5
       duree_select = duree_select + 1 ;
    }
    while(digitalRead(buttonPin_PLUS) == HIGH){
      delay(25);
    }
    calcul_temps(duree_select);
  }
  
  //traitement bouton -
  if (buttonState_MOINS == HIGH){
    if(duree_select > 0){ // un contrôle sur la valeur duree_select pour ne pas passer à la valeur -1
     duree_select = duree_select - 1 ;
    }
    while(digitalRead(buttonPin_MOINS) == HIGH){
      delay(25);
    }
    calcul_temps(duree_select);
  }
  
  //traitement bouton start
  if (buttonState_START == HIGH){
    temps_ecoule = (millis()-instant_start) / 1000; //calcul du temps écoulé depuis l'appui sur start //+1 pour commencer chrono à 1
    calcul_temps(duree_select);
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Temps restant:");
    digitalWrite(ledPin, HIGH);

    //garde la led allumée
    //set origine temps (on va ensuite la soustraire pour récupérer le temps écoulé)
    instant_start=millis(); 
    //mesure temps écoulé
    while(temps_ecoule != temps){
      //print the number of seconds since reset:
      if(temps-temps_ecoule == 9){ //Met un espace pour enlever le chiffre des unités
        lcd.setCursor(8, 1); 
        lcd.print(" ");
      }else if(temps-temps_ecoule == 99){ //Met un espace pour enlever le chiffre des unités
        lcd.setCursor(9, 1);
        lcd.print(" ");
      }
      lcd.setCursor(7, 1);// Revient à la ligne pour l'affichage du temps
      temps_ecoule = (millis()-instant_start) / 1000; //calcul du temps écoulé depuis l'appui sur start //+1 pour commencer chrono à 1
      lcd.print((temps-temps_ecoule));
      int nombre_led_a_allumer = (temps_ecoule* Nombre_neopixels)/temps;
      //lcd.print(nombre_led_a_allumer);
      affichage_led();
      //
       //Si appuie + et - arrête timer
       if (digitalRead(buttonPin_MOINS)==HIGH && digitalRead(buttonPin_PLUS)==HIGH){
          //Serial.print("RESET");
          break;
       }
    }
    lcd.setCursor(7, 1);
    lcd.print("0");
    delay(1000);
    eteindre_led();
    digitalWrite(ledPin, LOW);
    affichage_menu();
  }
  
  delay(25);
}

//Récupère le sélecteur "duree_select", attribue le temps en secondes en fonction du tableau "temps_propose"
//et réalise l'affichage en fonction du tableau "temps_affiche"
void calcul_temps(int select){
  temps = temps_propose[select];
  lcd.setCursor(10, 1);
  lcd.print("      ");
  lcd.setCursor(10, 1);
  lcd.print(temps_affiche[select]);
}

void affichage_menu(){
  lcd.clear();
  lcd.print("SELECT TIME: +/-");
  lcd.setCursor(0, 1);//change de ligne
  lcd.print("Or START:");
  lcd.setCursor(10, 1);
  lcd.print(temps_affiche[duree_select]);
}

void affichage_led(){
  //il faut savoir combien de temps s'est écoulé en proportion du temps sélectionné pour savoir combien de led on doit afficher.

int nombre_led_a_allumer = (temps_ecoule* Nombre_neopixels)/temps;

for (int i=0; i <= nombre_led_a_allumer; i++){ 
strip.setPixelColor(i, (0, 0, 50));
strip.show();
   }
  }
void eteindre_led(){
  for (int i=0; i <= Nombre_neopixels; i++){ 
  strip.setPixelColor(i, (0, 0, 0));
     }
  strip.show();
}





