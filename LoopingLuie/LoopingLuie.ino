#include <MAX7219_8x8_matrix.h>


//*****************************************************************************************************
//Hier können werte angepasst werden:

//Pins zu dem Schieberegister
const int din = 2;
const int cs = 3;
const int clk = 4;


//Wert ab dem Ein analoges Eingangssignal erkannt wird
//500 sollte nicht verändert werden außer die Taster werden nicht erkannt.
//Dann kann dieser Wert verringert werden
const int groesser = 500;

//Analoger Pin für Taster Spieler 1
const int pin1 = 0;

//Analoger Pin für Taster Spieler 2
const int pin2 = 1;

//Analoger Pin für Taster Spieler 3
const int pin3 = 2;

//Analoger Pin für Taster Spieler 4
const int pin4 = 3;

//Analoger Pin für den ersten Spielmodus
const int spielwahl1 = 5;

//Analoger Pin für den zweiten Spielmodus
const int spielwahl2 = 4;

//Anzahl der Random Leds in der Anfangssequenz
const int anzahl = 15;

//Delay am Anfang des Spieles. 14 bedeutet, dass eine Led 140ms an ist und danach die nächste an geht
const int anfangsDelay = 14;

//Hier hinter nichts verändern
//*****************************************************************************************************



//MAX7219_8x8_matrix(DIN, CS, CLK);
MAX7219_8x8_matrix leds(din,cs,clk);
//BoolArray ist true wenn ein Spieler gedrückt hat
bool p[4];
//True ist im Uhrzeigersinn
bool richtung;
//IntArray wieviele Leben die 4 Spieler noch haben
int leben[4];
//Delay für die Geschwindigkeit des Spiels 
//dela * 10ms ist das Delay zwischen jedem Schritt
int dela;
//Spielmodus value entspricht der Schalterstellung
//0: Kein Richtungswechsel, kein Random Sprung
//1: Richtungswechsel, kein Random Sprung
//2: Richtungswechsel, Random Sprünge
int spielModus = 0;
//int anzahl der Spieler.
//wird verwendet um das Delay herunterzusetzen
//Delay wird um einen verringert, wenn durchlaeufe >= anzahlSpieler
int anzahlSpieler;
int durchlaeufe;
//ints für die Matrix der Leds
int i;
int j;
//Random Ints zwischen 0 >= x < 8
//Für das Random aufblinken von Leds am anfang vor dem Spiel
int ra1[anzahl];
int ra2[anzahl];

void setup() {
  durchlaeufe = 0;
  anzahlSpieler = 0;
  richtung = true;
  
  for(int g = 0; g<4; g++){
    p[g] = false;
    leben[g] = 0;
  }
  delay(10);
  i = random(0,8);
  delay(10);
  j = random(0,8);
  dela = anfangsDelay;// Anfangsdelay von 140ms
  leds.clear();
  leds.setBrightness(11);
  Serial.begin(9600);
  spielerFinden();
  
  delay(1);
  i = random(0,8);
  j = 0;
  for(int f = 0; f<anzahl; f++){//ToDo: testen ob die forschleife entfernt werden kann oder vor das spielerFinden kommen muss
    delay(1);
    ra1[f] = random(0,8);
    delay(1);
    ra2[f] = random(0, 8);
  }
}
void show(){
  leds.set_pixel(ra1[0], ra2[0], LOW);//letzten Pixel auf low setzen
  delay(50);//Delay zwischen wechseln

  for(int f = 0; f<anzahl-1; f++){//array um einen nach vorne verschieben
    ra1[f] = ra1[f+1];
    ra2[f] = ra2[f+1];
  }
  ra1[anzahl - 1] = random(0,8);//neue random ints finden
  ra2[anzahl - 1] = random(0,8);
  leds.set_pixel(ra1[anzahl - 1], ra2[anzahl - 1], HIGH);//Random led setzen
}
void spielerFinden(){
  
  while(leben[0] == 0 &&
    leben[1] == 0 &&
    leben[2] == 0 &&
    leben[3] == 0){// so lange die Show machen bis ein Spieler sich angemeldet hat
      eingaengeTesten();
    if(analogRead(pin1) >= groesser)
      leben[0] = 3;
    if(analogRead(pin2) >= groesser)
      leben[1] = 3;
    if(analogRead(pin3) >= groesser)
      leben[2] = 3;
    if(analogRead(pin4) >= groesser)
      leben[3] = 3;
      //delay(20);
      show();//Die Random leds setzen
    }
  leds.clear();//Clearen da noch random leds leuchten
  for(int g = 0; g < 64; g++){//Alle 64 leds im Kreis durchgehen 
    eingaengeTesten();
    if(analogRead(pin1) >= groesser)
      leben[0] = 3;
    if(analogRead(pin2) >= groesser)
      leben[1] = 3;
    if(analogRead(pin3) >= groesser)
      leben[2] = 3;
    if(analogRead(pin4) >= groesser)
      leben[3] = 3;
    lebenSetzen();
    
    j--;
    if(j == -1){
      j = 7;
      i++;
    }
    if(i == 8)
      i = 0;
    if((j == 1 || j == 3 || j == 5)&&
      (i == 0 || i == 2 || i == 4 || i == 6))
      continue;
        
    leds.set_pixel(i,j, HIGH);
    delay(50);
  }
  leds.clear();//
  
  lebenSetzen();
  int spieler = 0;
  for(int g = 0; g<4; g++){
    if(leben[g] > 0)
      spieler++;
  }
  if(spieler == 1)//wenn sich nur ein Spieler angemeldet hat nicht starten sondern das Setup neu ausführen
    setup();
}
///Returns true wenn eine led der Leben angesteuert werden würde
bool setMatrix(){

  if(richtung)
    j--;//Im Uhrzeigersinn
  else
    j++;//Gegen den Uhrzeigersinn

  if(richtung){
    if(j == -1){
      j = 7;//zurück auf 7 setzen und i++ damit die nächste Reihe angesteuert wird
      i++;
    }
  }
  else{
    if(j == 8){
      j = 0;
      i--;
    }
  }

  if(richtung){
    if(i == 8)
      i = 0;
  }
  else{
    if(i == -1)
      i = 7;
  }

  if((j == 1 || j == 3 || j == 5)&&
      (i == 0 || i == 2 || i == 4 || i == 6)) return true;//Leds der Leben diese überspringen und nichts machen
  leds.set_pixel(i,j, HIGH);
  for(int f = 0; f < dela; f++){//Forschleife, da bei einem hohen delay ein taster druck nicht untergeht
    if(analogRead(pin1) >= 500)//deshalb alle 10ms schauen ob ein taster druck erfolgt ist 
      p[0] = true;
    if(analogRead(pin2) >= 500)
      p[1] = true;
    if(analogRead(pin3) >= 500)
      p[2] = true;
    if(analogRead(pin4) >= 500)
      p[3] = true;
    delay(10);
  }
  leds.set_pixel(i,j, LOW);
  return false;
}
///Findet den Spielmodus
int Modus(){
  if(analogRead(spielwahl1) >= groesser)
    return 1;
  else if(analogRead(spielwahl2) >= groesser)
    return 2;
  return 0;
}

void loop() {
  spielModus = Modus();
  int spieler = 0;
  for(int g = 0; g<4; g++){
    if(leben[g] > 0)
      spieler++;
  }
  anzahlSpieler = spieler;
  if(spieler == 0){//Wenn kein spieler gefunden wurde setup aufrufen
    setup();      //sollte eigentlich nicht vorkommen
    return;
  }else if(spieler == 1){//Es gibt nur noch einen Spieler 
  if(leben[0] > 0)      //Gewinner finden und Spiel beenden
      ende(0);
    else if(leben[1] > 0)
      ende(1);
    else if(leben[2] > 0)
      ende(2);
    else //if(leben[3] > 0)
      ende(3);
  }
  
  if(durchlaeufe >= anzahlSpieler){//Wenn so oft gedrückt wurde, wie es spieler gibt das Delay verringern
    durchlaeufe = 0;
    dela--;
  }
  eingaengeTesten();
  if(setMatrix())//Matrix setzen, wenn true zurück kommt return, da eine Lebensled gesetzt würde
    return;
  //ledBeiFehler ändert sich bei den unterschiedlichen Richtungen
  //Da immer nach den 3 leds, in denen gedrückt werden kann überprüft werden soll ob ein Spieler nicht gedrückt hat
  int ledBeiFehler;
  if(richtung)
    ledBeiFehler = 2;
  else
    ledBeiFehler = 7;

  if(j == ledBeiFehler)//Led ist hinter einem Spieler angekommen
  {
    for(int f = 0; f < 4; f++){//Für alle Spieler schauen, hinter welchem Spieler die led ist
      if(i == f * 2){
        if(!p[f])//wenn der Spieler nicht gedrückt hat hat er einen Fehler
          fehler(f);//f ist der Spieler
        else{
          if(leben[f] > 0){//der Spieler hat richtig gedrückt und er hat noch leben
            randLed();
            p[f] = false;//taster zurücksetzen
            durchlaeufe++;
          }
        }
      }
    }
  }
  for(int f = 0; f < 4; f++){//für alle Spieler schauen ob sie vor den Leds gedrückt haben
    if(p[f]){//der spieler hat gedrückt hätte aber noch nicht drücken dürfen
      if(i != f * 2)//true wenn Led nicht in dem Feld bei den richtien Leds ist
        fehler(f);
      else
        if(!(j >= 1 && j <= 6))//ist direkt auf einer led vor oder hinter den 3 richtigen leds
          fehler(f);
    }
  }
  lebenSetzen();//lebensanzeige setzen
}

void lebenSetzen(){
  for(int f = 0; f < 4; f++){
    int reihe1 = f * 2;
    switch(leben[f]){
      case 3://leben leds setzen jeh nachdem, wieviele leben der Spieler noch hat
        leds.set_pixel(reihe1, 1, HIGH);
        leds.set_pixel(reihe1, 3, HIGH);
        leds.set_pixel(reihe1, 5, HIGH);
        break;
      case 2:
        leds.set_pixel(reihe1, 1, HIGH);
        leds.set_pixel(reihe1, 3, HIGH);
        leds.set_pixel(reihe1, 5, LOW);
        break;
      case 1:
        leds.set_pixel(reihe1, 1, HIGH);
        leds.set_pixel(reihe1, 3, LOW);
        leds.set_pixel(reihe1, 5, LOW);
        break;
      case 0:
        leds.set_pixel(reihe1, 1, LOW);
        leds.set_pixel(reihe1, 3, LOW);
        leds.set_pixel(reihe1, 5, LOW);
        break;
    }
  }
}

void fehler(int player){
  if(leben[player] > 0)
  {
    leben[player]--;//leben einen weniger machen
    for(int h = 0; h<5; h++){//anzeigen, dass ein Spieler einen fehler gemacht hat
      leds.fill();//alle leds an und aus machen 5 mal
      delay(100);
      leds.clear();
      delay(100);
    }
    lebenSetzen();//leben neu setzen da alle aus sind
    randLed();
    for(int h = 0; h<5; h++){//den pixel bei dem danach wieder angefangen wird aufblinken lassen
      leds.set_pixel(i, j, HIGH);
      delay(50);
      leds.set_pixel(i, j, LOW);
      delay(50);
    }
    p[player] = false;//bool auf false setzen
    dela++;
  }
}
void eingaengeTesten(){//Alleinig für den Seriellenmonitor
  int i1 = analogRead(pin1);
  int i2 = analogRead(pin2);
  int i3 = analogRead(pin3);
  int i4 = analogRead(pin4);
  Serial.print(j);
  Serial.print("\t");
  Serial.print(i);
  Serial.print("\t");
  Serial.print(i1);
  Serial.print("\t");
  Serial.print(i2);
  Serial.print("\t");
  Serial.print(i3);
  Serial.print("\t");
  Serial.print(i4);
  Serial.print("\t");
  Serial.print(analogRead(4));
  Serial.print("\t");
  Serial.print(analogRead(5));
  Serial.print("\n");
}

void randLed(){
  if(spielModus == 0)//return da in Spielmodus 0 nichts random gemacht wird
    return;
  if(spielModus == 2){//wenn spielmodus ist gleich 2 random richtung setzen
    delay(1);//vorher setzen damit die random Led richtig gesetzt wird
    int ran = random(0, 2);
    if(ran == 0)
      richtung = false;
    else
      richtung = true;
  }
  if(spielModus >= 1){//Neue Random Led wenn der Spielmodus größer 0 ist
    delay(1);//delay da es passiert ist, dass immer i und j gleich waren
    i = random(0, 8);
    delay(1);
    j = random(0, 8);
    if(i == 0 || i == 2 || i == 4 || i == 6 ){
      //wenn die random Led direkt vor den drei leds wo man drücken muss ist, wird die led verschoben
      //da man es kaum schaffen kann da zu drücken
      if(richtung){
        if(i == 7)
          i = 0;
        else
          i++;
        j = 7;
      }
      else {
        if(i == 0)
          i = 7;
        else
          i--;
        j = 0;
      }
    }
  }
  for(int h = 0; h<5; h++){
    leds.set_pixel(i, j, HIGH);
    delay(50);
    leds.set_pixel(i, j, LOW);
    delay(50);
  }
}

void ende(int sieger){//Sieg Sequenz
  for(int g = 0; g < 20; g++){
    leben[sieger] = 3;
    lebenSetzen();
    delay(100);
    leben[sieger] = 0;
    lebenSetzen();
    delay(100);
  }
  setup();
}
