/*--------------------------------------------------------------------------*/
/* Module MultiPWM															*/
/* Implémentation de NB_PWM canaux PWM										*/
/* Résolutions identiques : (2^RESOLUTION) bits								*/
/* Horloges indépendantes : Vecteur CLkIn									*/
/* Latchs indépendant sur fronts descendants : vecteur nLatch				*/
/* oe indépendants : vecteur oe												*/
/* Rapports cycliques indépendants, sur vecteur Ton							*/
/* Polarité positive : la sortie est à 1 durant les Ton premières périodes	*/
/*                     de ClkIn												*/
/* Nécessite le module Verilog modPWM										*/ 
/* (c) Alexis ROLLAND - 2017 												*/
/*--------------------------------------------------------------------------*/
module modMultiPWM ( ClkIn, PWMout, nLatch, Ton, oe);

parameter NB_PWM = 8;				// Nombre de canaux PWM
parameter RESOLUTION = 8;			// Nombre de bits des registres de comptage. 8 bits = PWM 256 pas de résolution, 10bits = PWM 1024 pas de résolution...


input[(NB_PWM-1):0] ClkIn;
input[(NB_PWM-1):0] nLatch;
input[(NB_PWM-1):0] oe; 

input[(RESOLUTION-1):0] Ton;

output[(NB_PWM-1):0] PWMout;

genvar i;

generate for ( i = 0; i < NB_PWM ; i = i+1 )
    begin: 
		inst modPWM #(RESOLUTION) PWM_i (ClkIn[i], PWMout[i], Ton, nLatch[i],oe[i]);
    end
endgenerate

endmodule
