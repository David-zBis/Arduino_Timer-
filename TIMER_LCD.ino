#include <LiquidCrystal.h>
LiquidCrystal lcd(12,11,5,4,3,2);


int buttonPin_START = 7;
int buttonPin_PLUS = 8;
int buttonPin_MOINS = 9;
int ledPin = 13;
int buttonState_START = 0;
int buttonState_PLUS = 0;
int buttonState_MOINS = 0;
int duree_select = 0;
long temps = 0;
char* temps_affichage = "3 minutes";



void setup() {
  // put your setup code here, to run once:
lcd.begin(16,2);  
pinMode(ledPin, OUTPUT);
pinMode(buttonPin_START, INPUT);
pinMode(buttonPin_PLUS, INPUT);
pinMode(buttonPin_MOINS, INPUT);
lcd.print(temps_affichage);
}

void loop() {
  // put your main code here, to run repeatedly:

  //on lit l etat des buttons
buttonState_START = digitalRead(buttonPin_START);
buttonState_PLUS = digitalRead(buttonPin_PLUS);
buttonState_MOINS = digitalRead(buttonPin_MOINS);

//traitement bouton +
if (buttonState_PLUS == HIGH){
  if(duree_select < 9){ // un contrôle sur la valeur duree_select pour ne pas dépasser la valeur 5
     duree_select = duree_select + 1 ;
  }
 
  while(digitalRead(buttonPin_PLUS) == HIGH){
    delay(25);
  }
  calcul_temps();
  lcd.clear();
  lcd.print(temps_affichage);
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
  lcd.print(temps_affichage);
}


//affichage du temps choisi


//traitement bouton start
if (buttonState_START == HIGH){
  digitalWrite(ledPin, HIGH);
  delay (temps);
  digitalWrite(ledPin, LOW);
}
delay(25);

}

void calcul_temps(){
  switch (duree_select) { //conversion de duree_select en temps [ms]
       case 0:
         temps = 1000;
         temps_affichage = "3 minutes";
         break;
       case 1:
         temps = 2000;
         temps_affichage = "5 minutes";
         break;
       case 2:
         temps = 3000;
         temps_affichage = "10 minutes";
         break;
       case 3:
         temps = 3000;
         temps_affichage = "15 minutes";  
         break;
       case 4:
         temps = 3000;
         temps_affichage = "20 minutes";  
         break;
       case 5:
         temps = 4000;
         temps_affichage = "30 minutes";
         break;
       case 6:
         temps = 5000;
         temps_affichage = "45 minutes";
         break;
       case 7:
         temps = 60000;
         temps_affichage = "1 heure";
         break;
       case 8:
         temps = 60000;
         temps_affichage = "1 heure 30";
         break;
       case 9:
         temps = 60000;
         temps_affichage = "2 heures";
         break;
       default:
         break;
         lcd.print(temps_affichage);
         lcd.print("min");
  
     }
}
