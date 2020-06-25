/* Sur mon montage, les LED et les boutons sont numérotés de gauche à droite et de 0 à 3 */

//variables
int mode = 0; //mode dans le programme
int taille = 0; // taille de la séquence en cours
int posTest = 0; // position de la réponse
int sequence[50]; //tableau de la séquence (50 max)
int duree = 250; //temps pour l'allumage des LED
int valBoutons[4]; // tableau de stockage des valeurs basses des 4 boutons

//constantes
//À faire => tableau de constantes pour les pins des LED: OK
const byte pinLed [4]={2,3,4,5};
const char pinSon = 8; //constante pour le pin du buzzer
const int freqSon[4] = {261, 329, 392, 523}; //tableau de constantes des fréquences de sons

//À faire => constantes pour la lecture du CAN0 => définies dans setup
/*
Sortie(v)=(rPullDown/(rPullDown+rBouton))*vIn
avec rPullDown = 10000 ohms, rBouton = 220 ohms et vIn = 5v
Bouton 0 (LED rouge): R=660 => sortie=5v => CAN=959
Bouton 1 (LED bleue): R=440 => sortie=4,89v => CAN=979
Bouton 2 (LED jaune): R=220 => sortie=4,78v => CAN=1000
Bouton 3 (LED verte): R=0 => sortie=4,69v => CAN=1023
Ceci sans tenir compte de la tolérance des résistances de 1% négligable dans mon cas
Aucun bouton pressé => sortie=0 => CAN=0
Donc le tableau valBoutons[4] ressemblera à [1023,1000,979,959]
*/

/////////////////////////////////////////////////////////////////////////////

void setup() {
	const float rPullDown=10000.0; //résistance de pull_down de 10k
	const float rBouton=220.0; // résistances untilisées pour les boutons: 220 ohms
	const float rMarge=5.0; //pourcentage d'erreur des résistances: 5%
	const float vIn=5; //tension d'entrée: 5v
	
  //Serial.begin(9600);
	
  //À faire => initialisation des nombres aléatoires: OK
  randomSeed(analogRead(5));
  
  //À faire => paramétrage des pins LED et pin buzzer en mode OUTPUT: OK
  for (int i=0; i<4; i++) {
	  pinMode(pinLed[i],OUTPUT);
	  //remplissage du tableau des valeurs basses pour les 4 boutons
	  float valPourcent=rBouton*rMarge/100;
	  //Serial.print("pourcent: "); Serial.println(valPourcent);
	  float valMin=(rPullDown/(rPullDown+i*(rBouton+valPourcent)))*vIn*1000;
    //Serial.print("valmin: "); Serial.println(valMin);
	  //valBoutons[i]=map(int(valMin),0,5000,0,1023);
    //Serial.print("valboutons: "); Serial.println(valBoutons[i]);
    }
    
 /* int valBoutons[]={950,970,990,1003};
   for (int j=0; j<4; j++) {
     Serial.print("valboutons: "); Serial.println(valBoutons[j]);
   }
  */
  pinMode(pinSon,OUTPUT);
  }

////////////////////////////////////////////////////////////////////////////

void loop() {
  //les actions sont réalisées en fonction du mode en cours
  switch (mode) { //on regarde le mode
    case 0: //mode de démarrage
      start(); // appel de la fonction de démarrage
      taille = 0; //on met la séquence à 0
      mode = 1; // on peut passer au mode suivant
      break;
    case 1: //mode de tirage d'une nouvelle séquence
      posTest=0; //on remet le curseur à la position de début de tableau de la séquence
      augmenteSequence(); //appel de la fonction pour allonger la séquence
      mode = 2; // on peut passer au mode suivant
      break;
    case 2: // mode d'écoute de la séquence
      joueSequence(); // on joue la séquence en cours
      posTest = 0; //on initialise la position de réponse
      mode = 3; // on peut passer au mode suivant
      break;
    case 3: // mode de réponse du joueur
      int b = testBouton(); // on appelle la fonction de lecture des boutons
      if (b != -1) { //si bouton appuyé
        compare(b); // appel de la fonction de validation
      }
      break;
  }
}

//----------------------- FONCTIONS --------------------

//fonction de son et lumière pour le démarrage d'une nouvelle séquence
void start() {
  //À faire => en utilisant la fonction sonLum() *** OK
  for (int i=0; i<4; i++) {
	sonLum (i,100); //allume chaque led pendant 100 ms, avec le son correspondant
	}
  delay(500); //petite pause d'1/2 seconde avant de commencer le jeu
}



//fonction pour le son et lumière pour valider une suite de réponses
void valide() {
  //À faire => en utilisant la fonction sonLum() *** OK
  //la séquence est bonne: je rejoue start avant d'allonger la séquence
  start();
}



//fonction de son et lumière pour invalider une suite de réponses
void nonValide() {
  //À faire => en utilisant la fonction sonLum() *** OK
  //le joueur a perdu: je fais la sequence start à l'envers
  for (int i=3; i>=0; i=i-1) {
    sonLum (i,100); //allume chaque led pendant 100 ms, avec le son correspondant
  }
  mode=0; //on recommence
  delay(3000); //après une pause de 3 secondes
}



//fonction qui allume une LED (l) en jouant le son associé pendant une durée (d)
void sonLum(int l, int d) {
  digitalWrite(pinLed[l], HIGH);
  tone(pinSon, freqSon[l], d);
  delay(d);
  digitalWrite(pinLed[l], LOW);
  delay(d);
}



//fonction de tirage aléatoire d'une séquence
void augmenteSequence() {
  sequence[taille] = random(16) / 4; //permet un tirage mieux réparti
  if (taille<50) taille++;
  //A faire: Winner si séquence > 50 =>retour mode 1
}



//fonction qui joue la séquence en cours
void joueSequence() {
  //À faire => en utilisant une boucle et la fonction sonLum(): OK
  for (int i=0; i<taille; i++) {
	 sonLum(sequence[i],duree);
    }
}



//fonction qui teste l'état des boutons et retourne le numéro de bouton appuyé
int testBouton() {
  //À faire => Cette fonction doit utiliser la fonction sonLum() et retourner le numéro du bouton appuyé ou -1 si aucun bouton n'est appuyé: ok
  int val=analogRead(A0); //lecture de la pin A0, valeur entre 0 et 1023
  int i=map(val,950,1023,0,3); // si on lit une valeur > 950, pour exclure les parasites, c'est qu'un bouton est pressé
  if (i>=0 && i<4) {
    sonLum(i,duree); //et on l'allume avec le son correspondant
    return i;
    }
  else {
	  return -1;
  }
}



//fonction qui compare la réponse avec la séquence en cours
void compare(int b) {
  /*
  * À faire => soit on n'est pas au bout de la séquence et on continue avec l'appui suivant, (cas 1) :ok
  * soit  on est au bout de la séquence et on appelle la fonction valide() et on retourne au mode 1, (cas 2): ok
  * soit le joueur a fait une erreur et on appelle la fonction nonValide() et on retourne au mode 0 (cas 3): ok
  */
  
  if ((sequence[posTest]==b) && (posTest+1<taille)) { //cas 1: bouton valide, on continue     
    posTest++;   
  }
  
  else if (sequence[posTest]==b) { //cas 2: séquence valide
    valide();
    mode=1;
  }
  
  else { //cas 3: Erreur !
    nonValide();
    mode=0;   
  }
}
