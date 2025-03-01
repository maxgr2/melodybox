#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "sys/cm4.h"
#include "sys/devices.h"
#include "sys/init.h"
#include "sys/clock.h"
#include "systick.h"
#include "adc.h"
#include "sys/button.h"
#include <stdlib.h>

typedef enum {
  DO, RE, MI, FA, SOL, LA, SI, PAUSE, LA3, SI3, SOL3, RE5, SOL5, MI5, FA5, DO5, RE_DIESE, SOL_DIESE
} Note;

typedef struct{
  Note note;
  int temps;
}musique;


int periods =500; //ARR
int prescaler =42; //ne pas modifier trouvé grâce à des tests a l'oscilloscope
int bpm=350; //bpm de la musique
volatile int choix_musique=1; //choix de la musique
volatile int joue=0; //si on joue ou pas musique en pause ?

volatile int flags=0; //compteur pour le systick

//---------------------------------------Configuration--------------------------------------------------------
void configuration_TIM2();
void configuration_TIM4();
void configuration_4_switch ();
void configuration_potentiometre ();

//--------------------------------------Remplissage des musiques----------------------------------------
void remplir_au_clair_de_la_lune (musique *au_claire_de_la_lunes);
void remplir_berceuse(musique *berceuse);
void remplir_mario(musique *mario);
void remplir_letter_a_elise(musique *letter_a_elise);
void remplir_fort_boyard(musique *fort_boyard);
void remplir_scoobidou (musique* scobidou);

//--------------------------------------Jouer une note-----------------------------------------------------
void pause (int tmps);
void fonction_joue_une_note (int freq, int tmps);
void jouer (musique * music, int i);


void EXTI9_5_Handler(){
  EXTI.PR |= (1<<8);
  if (joue==0){
    joue=1;
  }else{
    joue=0;
  }
  choix_musique = (GPIOB.IDR>>3) & 0xF;
}


uint32_t mesure_potentiometre ();
void SysTick_Handler() {
  flags++;
}


int main() {
  musique au_claire_de_la_lune[41];
  musique mario[32];
  musique berceuse[43]; // Taille ajustée pour la berceuse
  musique lettre_a_elise[16];
  musique fort_boyard[64]; // Taille ajustée pour Fort Boyard
  musique scubidou[35];

  remplir_au_clair_de_la_lune(au_claire_de_la_lune);
  remplir_mario(mario);
  remplir_berceuse(berceuse);
  remplir_letter_a_elise(lettre_a_elise);
  remplir_fort_boyard(fort_boyard);
  remplir_scoobidou(scubidou);

  SysTick_init(1000);
  enable_GPIOB();
  button_init();
  button_irq_init();
  configuration_TIM4();
  configuration_potentiometre();
  
  int i = 0;
  int choix_precedent = 22; // valeur impossible décider arbitrairement 

  while (1) {
    if (joue == 1) {
      i++;
      if (choix_precedent != choix_musique) {
        i = 0;
      }
      switch (choix_musique) {
        case 0:
          jouer(au_claire_de_la_lune, i);
          if (i == 37) {
            pause(5000);
            i = i % 37;
          }
          break;
        case 2:
          jouer(mario, i);
          if (i == 32) {
            pause(5000);
            i = i % 32;
          }
          break;
        case 1:
          jouer(berceuse, i);
          if (i == 43) {
            pause(5000);
            i = i % 43;
          }
          break;
        case 3:
          jouer(lettre_a_elise, i);
          if (i == 16) {
            pause(5000);// pas de pause pour la lettre à elise 
            i = i % 16;
          }
          break;
        case 4:
          jouer(fort_boyard, i);
          if (i == 64) {
            pause(5000);
            i = i % 64;
          }
          case 5:
          jouer(scubidou, i);
          bpm=150;
          if (i == 18) {
            pause(5000);
            i = i % 18;
          }
          break;
        default:
          pause(1);
          break;
      }

      choix_precedent = choix_musique;
    }
  }
  return 0;
}

void pause (int tmps){
  TIM4.CCR4=0;
  TIM4.ARR=100;
  while(flags<tmps){}
  flags=0;
}

void fonction_joue_une_note (int freq, int tmps){
    TIM4.ARR =  1000000 /freq;
    TIM4.CCR4 = mesure_potentiometre ();
      while (flags < tmps) {
    }
  flags = 0;
  return;
}
//permet de jouer une note en fonction de la fréquence et du temps
void jouer (musique * music, int i){
  switch (music[i].note)
      {
      case DO:
        fonction_joue_une_note(523,music[i].temps*bpm);
        break;
        case RE:
          fonction_joue_une_note(585,music[i].temps*bpm);
        break;
      case MI:
        fonction_joue_une_note(659,music[i].temps*bpm);
      break;
      case FA:
        fonction_joue_une_note(699,music[i].temps*bpm);
      break;
      case SOL:
        fonction_joue_une_note(784,music[i].temps*bpm);
      break;
      case LA:
        fonction_joue_une_note(880,music[i].temps*bpm);
      break;
      case SI:
        fonction_joue_une_note(988,music[i].temps*bpm);
      break;
      case LA3:
        fonction_joue_une_note(440,music[i].temps*bpm);
        break;
      case SI3:
        fonction_joue_une_note(494,music[i].temps*bpm);
        break;
        case SOL3: //si3
        fonction_joue_une_note(392,music[i].temps*bpm);
        break;
      case RE5:
        fonction_joue_une_note(1175,music[i].temps*bpm);
        break;
      case SOL5:
        fonction_joue_une_note(1568,music[i].temps*bpm);
        break;
      case MI5:
        fonction_joue_une_note(1319,music[i].temps*bpm);
        break;
      case FA5:
        fonction_joue_une_note(1397,music[i].temps*bpm);
        break;
      case DO5: //DO5
        fonction_joue_une_note(1047,music[i].temps*bpm);
        break;
      case RE_DIESE:
        fonction_joue_une_note(1244,music[i].temps*bpm);
        break;
      case SOL_DIESE:
        fonction_joue_une_note(830,music[i].temps*bpm);
      break;
      
      default:
        break;
      }
      pause(10);
}

void remplir_au_clair_de_la_lune (musique *au_claire_de_la_lunes){
  au_claire_de_la_lunes[0].note=DO;
  au_claire_de_la_lunes[0].temps=1;
  au_claire_de_la_lunes[1].note=DO;
  au_claire_de_la_lunes[1].temps=1;
  au_claire_de_la_lunes[2].note=DO;
  au_claire_de_la_lunes[2].temps=1;
  au_claire_de_la_lunes[3].note=RE;
  au_claire_de_la_lunes[3].temps=1;
  au_claire_de_la_lunes[4].note=MI;
  au_claire_de_la_lunes[4].temps=2;
  au_claire_de_la_lunes[5].note=RE;
  au_claire_de_la_lunes[5].temps=2;
  au_claire_de_la_lunes[6].note=DO;
  au_claire_de_la_lunes[6].temps=1;
  au_claire_de_la_lunes[7].note=MI;
  au_claire_de_la_lunes[7].temps=1;
  au_claire_de_la_lunes[8].note=RE;
  au_claire_de_la_lunes[8].temps=1;
  au_claire_de_la_lunes[9].note=RE;
  au_claire_de_la_lunes[9].temps=1;
  au_claire_de_la_lunes[10].note=DO;
  au_claire_de_la_lunes[10].temps=4;
  au_claire_de_la_lunes[11].note=DO;
  au_claire_de_la_lunes[11].temps=1;
  au_claire_de_la_lunes[12].note=DO;
  au_claire_de_la_lunes[12].temps=1;
  au_claire_de_la_lunes[13].note=DO;
  au_claire_de_la_lunes[13].temps=1;
  au_claire_de_la_lunes[14].note=RE;
  au_claire_de_la_lunes[14].temps=1;
  au_claire_de_la_lunes[15].note=MI;
  au_claire_de_la_lunes[15].temps=2;
  au_claire_de_la_lunes[16].note=RE;
  au_claire_de_la_lunes[16].temps=2;
  au_claire_de_la_lunes[17].note=DO;
  au_claire_de_la_lunes[17].temps=1;
  au_claire_de_la_lunes[18].note=MI;
  au_claire_de_la_lunes[18].temps=1;
  au_claire_de_la_lunes[19].note=RE;
  au_claire_de_la_lunes[19].temps=1;
  au_claire_de_la_lunes[20].note=RE;
  au_claire_de_la_lunes[20].temps=1;
  au_claire_de_la_lunes[21].note=DO;
  au_claire_de_la_lunes[21].temps=4;
  au_claire_de_la_lunes[22].note=RE;
  au_claire_de_la_lunes[22].temps=1;
  au_claire_de_la_lunes[21].note=RE;
  au_claire_de_la_lunes[21].temps=1;
  au_claire_de_la_lunes[22].note=RE;
  au_claire_de_la_lunes[22].temps=1;
  au_claire_de_la_lunes[23].note=RE;
  au_claire_de_la_lunes[23].temps=1;
  au_claire_de_la_lunes[24].note=LA3;
  au_claire_de_la_lunes[24].temps=2;
  au_claire_de_la_lunes[25].note=LA3;
  au_claire_de_la_lunes[25].temps=2;
  au_claire_de_la_lunes[26].note=RE;
  au_claire_de_la_lunes[26].temps=1;
  au_claire_de_la_lunes[27].note=DO;
  au_claire_de_la_lunes[27].temps=1;
  au_claire_de_la_lunes[28].note=SI3;
  au_claire_de_la_lunes[28].temps=1;
  au_claire_de_la_lunes[29].note=LA3;
  au_claire_de_la_lunes[29].temps=1;
  au_claire_de_la_lunes[30].note=SOL3;
  au_claire_de_la_lunes[30].temps=1;
  au_claire_de_la_lunes[31].note=DO;
  au_claire_de_la_lunes[31].temps=1;
  au_claire_de_la_lunes[32].note=DO;
  au_claire_de_la_lunes[32].temps=1;
  au_claire_de_la_lunes[33].note=DO;
  au_claire_de_la_lunes[33].temps=1;
  au_claire_de_la_lunes[34].note=RE;
  au_claire_de_la_lunes[34].temps=1;
  au_claire_de_la_lunes[35].note=MI;
  au_claire_de_la_lunes[35].temps=2;
  au_claire_de_la_lunes[36].note=RE;
  au_claire_de_la_lunes[36].temps=2;
  au_claire_de_la_lunes[37].note=DO;
  au_claire_de_la_lunes[37].temps=4;
  

}

void remplir_berceuse(musique *berceuse) {
  berceuse[0].note = MI;
  berceuse[0].temps = 1;
  berceuse[1].note = MI;
  berceuse[1].temps = 1;
  berceuse[2].note = SOL;
  berceuse[2].temps = 6;
  berceuse[3].note = MI;
  berceuse[3].temps = 1;
  berceuse[4].note = MI;
  berceuse[4].temps = 2;
  berceuse[5].note = SOL;
  berceuse[5].temps = 6;
  berceuse[6].note = MI;
  berceuse[6].temps = 1;
  
  berceuse[7].note = SOL;
  berceuse[7].temps = 1;
  berceuse[8].note = DO5;
  berceuse[8].temps = 2;
  berceuse[9].note = SI;
  berceuse[9].temps = 2;
  berceuse[10].note = LA;
  berceuse[10].temps = 1;
  berceuse[11].note = LA;
  berceuse[11].temps = 2;
  berceuse[12].note = SOL;
  berceuse[12].temps = 2;
  berceuse[13].note = RE;
  berceuse[13].temps = 2;
  
  berceuse[14].note = RE;
  berceuse[14].temps = 1;
  berceuse[15].note = MI;
  berceuse[15].temps = 1;
  berceuse[16].note = FA;
  berceuse[16].temps = 4;
  berceuse[17].note = RE;
  berceuse[17].temps = 1;
  berceuse[18].note = FA;
  berceuse[18].temps = 1;
  berceuse[19].note = SI;
  berceuse[19].temps = 1;
  berceuse[20].note = LA;
  berceuse[20].temps = 1;
  
  berceuse[21].note = SI;
  berceuse[21].temps = 2;
  berceuse[22].note = DO5;
  berceuse[22].temps = 4;
  berceuse[23].note = DO;
  berceuse[23].temps = 1;
  berceuse[24].note = DO;
  berceuse[24].temps = 1;
  berceuse[25].note = DO5;
  berceuse[25].temps = 4;
  berceuse[26].note = LA;
  berceuse[26].temps = 1;
  berceuse[27].note = FA;
  berceuse[27].temps = 4;
  berceuse[28].note = SOL;
  berceuse[28].temps = 4;
  berceuse[29].note = MI;
  berceuse[29].temps = 1;
  berceuse[30].note = DO;
  berceuse[30].temps = 1;
  berceuse[31].note = FA;
  berceuse[31].temps = 2;
  berceuse[32].note = SOL;
  berceuse[32].temps = 2;
  berceuse[33].note = LA;
  berceuse[33].temps = 2;
  berceuse[34].note = SOL;
  berceuse[34].temps = 4;
  berceuse[35].note = DO;
  berceuse[35].temps = 1;
  berceuse[36].note = DO;
  berceuse[36].temps = 1;
  berceuse[37].note = DO5;
  berceuse[37].temps = 4;
  berceuse[38].note = LA;
  berceuse[38].temps = 1;
  berceuse[39].note = FA;
  berceuse[39].temps = 4;
  berceuse[40].note = SOL;
  berceuse[40].temps = 4;
  berceuse[41].note = MI;
  berceuse[41].temps = 2;
  berceuse[42].note = RE;
  berceuse[42].temps = 2;
  berceuse[43].note = DO;
  berceuse[43].temps = 6;
  

}

void remplir_mario(musique *mario) {
  mario[0]  = (musique){MI, 1}; mario[1]  = (musique){PAUSE, 1};
  mario[2]  = (musique){MI, 1}; mario[3]  = (musique){PAUSE, 1};
  mario[4]  = (musique){MI, 1}; mario[5]  = (musique){PAUSE, 1};
  mario[6]  = (musique){DO, 1}; mario[7]  = (musique){PAUSE, 1};
  mario[8]  = (musique){MI, 1}; mario[9]  = (musique){PAUSE, 1};
  mario[10] = (musique){SOL, 1}; mario[11] = (musique){PAUSE, 3};
  
  mario[12] = (musique){SOL, 1}; mario[13] = (musique){PAUSE, 3};
  mario[14] = (musique){DO, 1}; mario[15] = (musique){PAUSE, 3};
  mario[16] = (musique){MI, 1}; mario[17] = (musique){PAUSE, 3};
  mario[18] = (musique){LA, 1}; mario[19] = (musique){PAUSE, 1};
  mario[20] = (musique){SI, 1}; mario[21] = (musique){PAUSE, 1};
  mario[22] = (musique){LA, 1}; mario[23] = (musique){PAUSE, 1};

  mario[24] = (musique){SOL, 2}; mario[25] = (musique){MI, 2};
  mario[26] = (musique){DO, 2}; mario[27] = (musique){SOL, 2};
  mario[28] = (musique){MI, 2}; mario[29] = (musique){PAUSE, 1};
  mario[30] = (musique){LA, 2}; mario[31] = (musique){SI, 2};
  mario[32] = (musique){LA, 2}; mario[33] = (musique){SOL, 2};
}


void remplir_letter_a_elise(musique *letter_a_elise){
  letter_a_elise[0].note=MI5;
  letter_a_elise[0].temps=1;
  letter_a_elise[1].note=RE_DIESE;
  letter_a_elise[1].temps=1;
  letter_a_elise[2].note=MI5;
  letter_a_elise[2].temps=1;
  letter_a_elise[3].note=RE_DIESE;
  letter_a_elise[3].temps=1;
  letter_a_elise[4].note=MI5;
  letter_a_elise[4].temps=1;
  letter_a_elise[5].note=SI;
  letter_a_elise[5].temps=1;
  letter_a_elise[6].note=RE5;
  letter_a_elise[6].temps=1;
  letter_a_elise[7].note=DO5;
  letter_a_elise[7].temps=1;
  letter_a_elise[8].note=LA;
  letter_a_elise[8].temps=2;
  letter_a_elise[9].note=DO;
  letter_a_elise[9].temps=1;
  letter_a_elise[10].note=MI;
  letter_a_elise[10].temps=1;
  letter_a_elise[11].note=LA;
  letter_a_elise[11].temps=1;
  letter_a_elise[12].note=SI;
  letter_a_elise[12].temps=1;
  letter_a_elise[13].note=MI;
  letter_a_elise[13].temps=1;
  letter_a_elise[14].note=SOL_DIESE;
  letter_a_elise[14].temps=1;
  letter_a_elise[15].note=SI;
  letter_a_elise[15].temps=1;
  letter_a_elise[16].note=DO5;
  letter_a_elise[16].temps=1;
}

void remplir_fort_boyard(musique *fort_boyard) {
  fort_boyard[0] = (musique){MI, 2};
  fort_boyard[1] = (musique){SOL, 2};
  fort_boyard[2] = (musique){LA, 2};
  fort_boyard[3] = (musique){SI, 2};
  fort_boyard[4] = (musique){DO5, 2};
  fort_boyard[5] = (musique){SI, 2};
  fort_boyard[6] = (musique){LA, 2};
  fort_boyard[7] = (musique){SOL, 2};
  fort_boyard[8] = (musique){MI, 2};
  fort_boyard[9] = (musique){SOL, 2};
  fort_boyard[10] = (musique){LA, 2};
  fort_boyard[11] = (musique){SI, 2};
  fort_boyard[12] = (musique){DO5, 2};
  fort_boyard[13] = (musique){SI, 2};
  fort_boyard[14] = (musique){LA, 2};
  fort_boyard[15] = (musique){SOL, 2};
  fort_boyard[16] = (musique){MI, 2};
  fort_boyard[17] = (musique){SOL, 2};
  fort_boyard[18] = (musique){LA, 2};
  fort_boyard[19] = (musique){SI, 2};
  fort_boyard[20] = (musique){DO5, 2};
  fort_boyard[21] = (musique){SI, 2};
  fort_boyard[22] = (musique){LA, 2};
  fort_boyard[23] = (musique){SOL, 2};
  fort_boyard[24] = (musique){MI, 2};
  fort_boyard[25] = (musique){SOL, 2};
  fort_boyard[26] = (musique){LA, 2};
  fort_boyard[27] = (musique){SI, 2};
  fort_boyard[28] = (musique){DO5, 2};
  fort_boyard[29] = (musique){SI, 2};
  fort_boyard[30] = (musique){LA, 2};
}


void remplir_scoobidou (musique* scobidou){
    scobidou[0] = (musique){RE, 1};
    scobidou[1] = (musique){MI, 1};
    scobidou[2] = (musique){RE, 1};
    scobidou[3] = (musique){MI, 1};
    scobidou[4] = (musique){DO, 6};
    scobidou[5] = (musique){DO, 1};
    scobidou[6] = (musique){MI, 1};
    scobidou[7] = (musique){MI, 1};
    scobidou[8] = (musique){RE, 1};
    scobidou[9] = (musique){DO, 1};
    scobidou[10] = (musique){PAUSE, 4};
    scobidou[11] = (musique){DO, 2};
    scobidou[12] = (musique){RE, 2};
    scobidou[13] = (musique){MI, 2};
    scobidou[14] = (musique){MI, 2};
    scobidou[15] = (musique){RE, 2};
    scobidou[16] = (musique){DO, 2};
    scobidou[17] = (musique){PAUSE, 2};
    scobidou[18] = (musique){DO, 1};

}
void configuration_TIM4(){
  enable_GPIOB();
  
  // Configuration de PB9 en mode alternatif TIM4_CH4
  GPIOB.MODER &= ~(3 << 18); // Effacer les bits
  GPIOB.MODER |= (2 << 18);  // Mode alternatif (10)

  // Sélection de l'AF2 (TIM4_CH4) pour PB9
  GPIOB.AFRH &= ~(0xF << (4 * 1)); // Effacer les bits
  GPIOB.AFRH |= (2 << (4 * 1));   // AF2 (TIM4)

  // Configuration en sortie push-pull, pas de pull-up/doewn
  GPIOB.OTYPER &= ~(1 << 9);
  //GPIOB.PUPDR &= ~(3 << 18);

  // Activation du Timer 4
  clock_init(); // Devrait activer APB1
  enable_TIM4();
  // Configuration du TIM4
  TIM4.PSC = prescaler;  // Prescaler pour ajuster la fréquence
  TIM4.ARR = periods;     // Fixe la période du signal

  // Configuration en mode PWM1 sur le canal 4
  TIM4.CCMR2 &= ~(0b111 << 12); // Effacer les bits de mode
  TIM4.CCMR2 |= (0b110 << 12);  // PWM Mode 1 0b111 devrait fonctionner aussi
  //TIM4.CCMR2 |= (1 << 11);  // Activer la précharge fonctionne sans est il est pas vraiment utile
  TIM4.CCR4 =0; // 50% de rapport cyclique Sur un cycle combien de temps il passe en haut ou en bas

  // Activation de la sortie PWM sur CH4
  TIM4.CCER |= (1 << 12);  // Activer la sortie
  TIM4.CCER &= ~(1 << 13); // Active en position haute

  // Mode Center-Aligned Mode 3
  TIM4.CR1 &= ~(3 << 5);  // Effacer CMS
  TIM4.CR1 |= (3 << 5);   // CMS = 11 (Center-aligned Mode 3) 

  // Démarrer le Timer
  TIM4.CR1 |= (1 << 0); //CEN (counter enable clock) OBLIGATOIRE
}


void configuration_potentiometre (){
  enable_GPIOB();
  GPIOB.MODER |= 0b11; // configure reception des données en analogiques
  GPIOB.OTYPER |= 0b1; //configuation drain ouvert on peut recevoir des données à tous moment

  enable_ADC1();
  ADC1.SMPR2 &=~ 0b111; //cycle de mesure
  ADC1.SMPR2 |= 7<<24;
  ADC1.SQR3 |=8; //on mesure le canal 8
  ADC1.SQR1 &= ~ (0b01111) << 20;  // on mesure un seul canal 1-1=0
  ADC_common.CCR |= 0b010 <<16;
  //ADC1.CR1 &= ~(3<<24); //configure la resolution 64 ou plus
  //ADC1.CR1 |= (3<<24);
  //ADC1.CR1 |= 1<<8 ;
  ADC1.CR2|=1; //configuation de ADON ADC allumé

}


// Fonction de mesure du potentiomètre
uint32_t mesure_potentiometre (){
  ADC1.DR &= ~0b111111111111111; //on remet à zero avant de lancer une mesure
  ADC1.SR &= ~0b101; // remet à 0 si pas fait pas le programme
  ADC1.CR2 |= 1<<30; //SWSTART on lance une mesure
  while ((ADC1.SR & 0b010) ==0){} // test si mesure fini
  return (TIM4.ARR*(ADC1.DR & 0b111111111111111)*750)/(4096*1000); //retourne la valeur pour CCR4

}

