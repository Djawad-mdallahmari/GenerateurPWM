/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <stdio.h>
#include "unistd.h"
#include "altera_avalon_pio_regs.h"
#include "system.h"
#include "altera_nios2_gen2_irq.h"

/**
 * Structure caractéristique d'un cannal PWM
 */
typedef struct{
	//int numChannel;
	int rapportCyclique;
	char etat; //Actif(1) ou Haute Impédence(0)
}channel_t;

/**
 * ISR routine d'interruption pour les boutons
 */
void btnInterruptionHandler(void* isr_context);

/**
 * Fonction qui implémente les comportements à l'appuie des boutons + ou -
 */
void processingPlusOrMinus(char sign);

//Variables globables
char btnPlusPressed = 0;
char btnMinusPressed = 0;
char btnDisableChannelPressed = 0;


channel_t cannauxPWM[24];
char currentSelectedPWM = 0;
long latchValue = 1;
int tonValue = 0x200; // Par defaut 512
long oeValue = 0xF;
FILE * fp;

int main()
{
	printf("Program start!\n");

	//Init de tous les cannaux
	for(int i=0; i<24; i++){
		cannauxPWM[i].etat = 1;
		cannauxPWM[i].rapportCyclique = 512;
	}

	fp =  fopen ("/dev/lcd_16207_0", "w");

	//init interruption
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BTNGPIO_BASE, 0x7);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BTNGPIO_BASE, 0);
	alt_ic_isr_register(BTNGPIO_IRQ_INTERRUPT_CONTROLLER_ID,BTNGPIO_IRQ,btnInterruptionHandler,NULL,NULL);

	//Oe, Ton et Latch
	//Au depart tous actif à 50% de RC
	IOWR_ALTERA_AVALON_PIO_DATA(OEGPIO_BASE, 0xFFFFFF);
	IOWR_ALTERA_AVALON_PIO_DATA(TONGPIO_BASE, 0x200);
	IOWR_ALTERA_AVALON_PIO_DATA(LATCHGPIO_BASE, 0xFFFFFF);
	IOWR_ALTERA_AVALON_PIO_DATA(LATCHGPIO_BASE, 0x0);
	IOWR_ALTERA_AVALON_PIO_DATA(LATCHGPIO_BASE, 0xFFFFFF);

	while(1){
		//Traitement suite aux interruptions boutons
		if(btnPlusPressed == 1){ // Si bouton + appué
			processingPlusOrMinus('+');
			btnPlusPressed = 0; //RAZ pour la routine
		}else if(btnMinusPressed == 1){ // Si bouton - appué
			processingPlusOrMinus('-');
			btnMinusPressed = 0;
		}else if(btnDisableChannelPressed == 1){ // Si bouton désactivation d'un channel (mise en haute impédance) appuyé
			//OE
			if(cannauxPWM[currentSelectedPWM].etat==1){
				cannauxPWM[currentSelectedPWM].etat = 0;
			}else{
				cannauxPWM[currentSelectedPWM].etat = 1;
			}
			//oeValue ^= 1 << pow(2,currentSelectedPWM); //Masque XOR pour basculer le bit voulu
			oeValue ^= (0x1 << currentSelectedPWM); //Masque XOR pour basculer le bit voulu

			fprintf(fp,"CH:%d  RP:%d\n",currentSelectedPWM,cannauxPWM[currentSelectedPWM].rapportCyclique);
			fprintf(fp,"Etat:%s\n",cannauxPWM[currentSelectedPWM].etat == 1? "ACTIF":"INACTIF");
			IOWR_ALTERA_AVALON_PIO_DATA(OEGPIO_BASE, oeValue);
			btnDisableChannelPressed = 0;
		}

	}

	return 0;
}

void processingPlusOrMinus(char sign){
	if(IORD_ALTERA_AVALON_PIO_DATA(SWITCHGPIO_BASE)&0b1){ //Si SW0 en haut (=Changement de canal)

		if(IORD_ALTERA_AVALON_PIO_DATA(SWITCHGPIO_BASE)&0b10){ //Par pas de 10
			currentSelectedPWM = (sign=='+')? currentSelectedPWM + 10 : currentSelectedPWM - 10;
		}else{ //Par pas de 1
			currentSelectedPWM = (sign=='+')? currentSelectedPWM + 1 : currentSelectedPWM - 1;
		}
		if(currentSelectedPWM > 24){
			currentSelectedPWM = 23; //Dernier canal
		}else if(currentSelectedPWM < 0){
			currentSelectedPWM = 0;
		}

	}else{ // Si SW0 en bas (=Changement du rapport cyclique du canal actuellement affiché)

		if(IORD_ALTERA_AVALON_PIO_DATA(SWITCHGPIO_BASE)&0b10){ //Par pas de 10
			cannauxPWM[currentSelectedPWM].rapportCyclique = (sign=='+')?
					cannauxPWM[currentSelectedPWM].rapportCyclique + 10 : cannauxPWM[currentSelectedPWM].rapportCyclique - 10;
		}else{ //Par pas de 1
			cannauxPWM[currentSelectedPWM].rapportCyclique = (sign=='+')?
					cannauxPWM[currentSelectedPWM].rapportCyclique + 1 : cannauxPWM[currentSelectedPWM].rapportCyclique - 1;
		}
		//Si depasse valeur max
		if(cannauxPWM[currentSelectedPWM].rapportCyclique > 1024){
			//MAJ du rapport cyclique du canal courant
			cannauxPWM[currentSelectedPWM].rapportCyclique = 1023;
		}
		//Ton
		tonValue = cannauxPWM[currentSelectedPWM].rapportCyclique;
		//latch (sélection du bon canal à changer)
		latchValue = pow(2,currentSelectedPWM); //sélection du bon canal (2 puissance currentSelectecPWM)

	}
	//Affichage des infos sur le canal courant
	/*printf("CH:%d  RP:%d  Etat:%s \n",currentSelectedPWM
			,cannauxPWM[currentSelectedPWM].rapportCyclique
			,cannauxPWM[currentSelectedPWM].etat == 1? "ACTIF":"INACTIF");*/

	fprintf(fp,"CH:%d  RP:%d\n",currentSelectedPWM,cannauxPWM[currentSelectedPWM].rapportCyclique);
	fprintf(fp,"Etat:%s\n",cannauxPWM[currentSelectedPWM].etat == 1? "ACTIF":"INACTIF");
	//Ton
	IOWR_ALTERA_AVALON_PIO_DATA(TONGPIO_BASE, tonValue);
	//Latch
	IOWR_ALTERA_AVALON_PIO_DATA(LATCHGPIO_BASE, latchValue);
	IOWR_ALTERA_AVALON_PIO_DATA(LATCHGPIO_BASE, 0x0);
}

void btnInterruptionHandler(void* isr_context){
	int btnAppuye = (int) isr_context;
	btnAppuye = IORD_ALTERA_AVALON_PIO_EDGE_CAP(BTNGPIO_BASE);

	int switchAppuye = (int) IORD_ALTERA_AVALON_PIO_DATA(SWITCHGPIO_BASE);


	if(btnAppuye & 0b100){
		btnDisableChannelPressed = 1;
	}else if(btnAppuye & 0b010){
		btnMinusPressed = 1;
	}else if(btnAppuye & 0b001){
		btnPlusPressed = 1;
	}

	//acquittement de l'interruption en remettant le registre edgecapture à 0
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BTNGPIO_BASE, 0);
	//lecture pour un délai par sécurité? (voir doc)
	IORD_ALTERA_AVALON_PIO_EDGE_CAP(BTNGPIO_BASE);
}

