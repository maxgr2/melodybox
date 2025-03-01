#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "sys/cm4.h"
#include "sys/devices.h"
#include "sys/init.h"
#include "sys/clock.h"
#include "systick.h"
#include "adc.h"




int periods =1;
int prescaler =42;
int puissance =999;

volatile int flags=0;
void configuration_TIM2();
void configuration_TIM4();


void re (int tmps);
void doe (int tmps);
void fa (int tmps);
void mi (int tmps);
void sol (int tmps);
void la (int tmps);
void si (int tmps);



uint32_t mesure_potentiometre ();
void SysTick_Handler() {
  flags++;
}

void  Led_init(){
    GPIOA.MODER = (GPIOA.MODER & ~(0x3FFF<<8)) | (0x1555<<8);
    GPIOA.OTYPER &= ~(0x7F<<4);
    GPIOA.OSPEEDR |= (0x3FFF<<8);
  }


void led_tri_set(uint32_t status) { //0 rouge 1 vert 2 bleu plus s'allume pas'
  GPIOA.ODR = (GPIOA.ODR & ~(7<<8) ) | (1<<(8+status));
}


int main() {

  SysTick_init(1000);
  enable_GPIOA();
  enable_Vrefint();
  configuration_TIM4();  
  while(1){


    doe(500);

    re(500);

    mi (500);

    fa (500);
    sol(500);
    la(500);
    si(500);



    /*
    if (flags>=500 && pause==0){
      TIM4.CCR4=0; //arrête le temps de haut bas il reste à la meme position donc plus de son car buzzer piezoelectrique
      TIM4.EGR|=1;
      flags=0;
      pause=1;
      TIM4.ARR=periods; //modifie la fréquence
      TIM4.EGR|=1;
    }
    if (flags>=500 && pause==1){
      TIM4.ARR=periods;
      TIM4.EGR|=1;
      TIM4.CCR4 = (puissance* periods) / 100; //CCR4 doit adapaté sa valeur en fonction de la valeur de ARR
      flags=0;
      pause=2;
    }
    if (flags>=500 && pause==2){
      TIM4.ARR=500;
      TIM4.CCR4=(puissance* 500) / 100;
      TIM4.EGR|=1;
      flags=0;
      pause=3;

    }
    if (flags>=500 && pause==3){
      TIM4.ARR=periods;
      TIM4.CCR4 = (puissance* periods) / 100; //CCR4 doit adapaté sa valeur en fonction de la valeur de ARR
      TIM4.EGR|=1;
      flags=0;
      pause=0;
    }
    */

  }
  return 0;
} 

void configuration_TIM2(){ 
  enable_GPIOB();
  
  // Configuration PB9 en mode alternatif (TIM2_CH2, AF1)
  GPIOB.MODER &= ~(3 << 18); // Effacer les bits pour PB9
  GPIOB.MODER |= (2 << 18);  // Mode alternatif (10)

  GPIOB.AFRH &= ~(0xF << (4 * 1)); // Effacer les bits
  GPIOB.AFRH |= (1 << (4 * 1));   // AF1 (TIM2_CH2)

  // Configuration en sortie push-pull, pas de pull-up/down
  GPIOB.OTYPER &= ~(1 << 9);
  GPIOB.PUPDR &= ~(3 << 18);

  // Activation du Timer 2
  enable_TIM2();

  // Configuration du TIM2
  TIM2.PSC = prescaler;  // Prescaler pour ajuster la fréquence
  TIM2.ARR = periods;     // Fixe la période du signal
  TIM2.EGR |= 1 << 0;     // Mise à jour des registres

  // Configuration en mode PWM1 sur le canal 2
  TIM2.CCMR1 &= ~(0b111 << 12);  // Effacer les anciens bits
  TIM2.CCMR1 |= (0b110 << 12);  // PWM Mode 1
  TIM2.CCMR1 |= (1 << 11);  // Activer la précharge
  TIM2.CCR2 = (50 * periods) / 100; // Rapport cyclique de 50%

  // Activation de la sortie PWM sur CH2
  TIM2.CCER |= (1 << 4);  // Activer la sortie
  TIM2.CCER &= ~(1 << 5); // Pas d'inversion du signal

  // Démarrer le Timer
  TIM2.CR1 |= (1 << 0); // Activer le Timer
}


/*
void configuration_TIM2(){ //
  enable_GPIOB();
  GPIOB.MODER &= ~(3 << 18); // Effacer les bits
  GPIOB.MODER |= (2 << 18);  // Mode alternatif (10)

  GPIOB.AFRH &= ~(0xF << (4 * 1)); // Effacer les bits
  GPIOB.AFRH |= (1<< (4 * 1));   // AF1 TIM2

  // Configuration en sortie push-pull, pas de pull-up/down
  GPIOB.OTYPER &= ~(1 << 9);

  // Activation du Timer 4
  enable_TIM2();

  // Configuration du TIM2
  TIM2.PSC = prescaler;  // Prescaler pour ajuster la fréquence
  TIM2.ARR = periods;     // Fixe la période du signal
  TIM2.EGR |=1 <<0;

  // Configuration en mode PWM1 sur le canal 4
  TIM2.CCMR1 |= (0b110 << 12);  // PWM Mode 1 0b111 devrait fonctionner aussi
  TIM2.CCMR1 |= (1 << 11);  // Activer la précharge
  TIM2.CCR4 = (50 * periods) / 100; // 75% de rapport cyclique

  // Activation de la sortie PWM sur CH2
  TIM2.CCER |= 1<<4;
  TIM2.CCER &= ~(1 << 5);

  TIM2.CR1 &= ~(3 << 5);  // Effacer CMS
  TIM2.CR1 |= (3 << 5);   // CMS = 11 (Center-aligned Mode 3) 

  // Démarrer le Timer
  TIM2.CR1 |= (1 << 0); //CEN (counter enable clock) OBLIGATOIRE

}*/


void doe (int tmps){
  int freq=1000000/523;
  TIM4.ARR=freq;
  TIM4.CCR4=(puissance* freq) / 1000;
  while (flags<tmps){
  }
  flags=0;
  return;

}
void re (int tmps){
  int freq=1000000/585;
  TIM4.ARR=freq;
  TIM4.CCR4=(puissance* freq) / 1000;
  while (flags<tmps){
  }
  flags=0;
  return;

}
void mi (int tmps){
  int freq=1000000/659;
  TIM4.ARR=freq;
  TIM4.CCR4=(puissance* freq) / 1000;
  while (flags<tmps){
  }
  flags=0;
  return;
}
void fa (int tmps){
  int freq=1000000/699;
  TIM4.ARR=freq;
  TIM4.CCR4=(puissance* freq) / 1000;
  while (flags<tmps){
  }
  flags=0;
  return;
}
void sol (int tmps){
  int freq=1000000/784;
  TIM4.ARR=freq;
  TIM4.CCR4=(puissance* freq) / 1000;
  while (flags<tmps){
  }
  flags=0;
  return;
}
void la (int tmps){
  int freq=1000000/880;
  TIM4.ARR=freq;
  TIM4.CCR4=(puissance* freq) / 1000;
  while (flags<tmps){
  }
  flags=0;
  return;
}
void si (int tmps){
  int freq=1000000/988;
  TIM4.ARR=freq;
  TIM4.CCR4=(puissance* freq) / 1000;
  while (flags<tmps){
  }
  flags=0;
  return;
}


void pause (int tmps){
  TIM4.CCR4=0;
  while(flags<tmps){}
  flags=0;
}



void configuration_TIM4(){
  enable_GPIOB();
  
  // Configuration de PB9 en mode alternatif TIM4_CH4
  GPIOB.MODER &= ~(3 << 18); // Effacer les bits
  GPIOB.MODER |= (2 << 18);  // Mode alternatif (10)

  // Sélection de l'AF2 (TIM4_CH4) pour PB9
  GPIOB.AFRH &= ~(0xF << (4 * 1)); // Effacer les bits
  GPIOB.AFRH |= (2 << (4 * 1));   // AF2 (TIM4)

  // Configuration en sortie push-pull, pas de pull-up/down
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
  TIM4.CCR4 = (90*periods) / 100; // 50% de rapport cyclique Sur un cycle combien de temps il passe en haut ou en bas

  // Activation de la sortie PWM sur CH4
  TIM4.CCER |= (1 << 12);  // Activer la sortie
  TIM4.CCER &= ~(1 << 13); // Active en position haute

  // Mode Center-Aligned Mode 3
  TIM4.CR1 &= ~(3 << 5);  // Effacer CMS
  TIM4.CR1 |= (3 << 5);   // CMS = 11 (Center-aligned Mode 3) 

  // Démarrer le Timer
  TIM4.CR1 |= (1 << 0); //CEN (counter enable clock) OBLIGATOIRE
}

uint32_t mesure_potentiometre (){
  ADC1.DR &= ~0b111111111111111; //on remet à zero avant de lancer une mesure
  ADC1.SR &= ~0b101; // remet à 0 si pas fait pas le programme
  ADC1.CR2 |= 1<<30; //SWSTART on lance une mesure
  while ((ADC1.SR & 0b010) ==0){} // test si mesure fini
  return ADC1.DR & 0b111111111111111;

}
