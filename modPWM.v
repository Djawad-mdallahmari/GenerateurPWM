/*--------------------------------------------------------------------------*/
/* Module modPWM															*/
/* Implémentation d'un canal PWM											*/
/* Résolutions : (2^RESOLUTION) bits										*/
/* Horloge : CLkIn															*/
/* Latch : signal nLatch, actif sur front descendant						*/
/* Rapport cyclique sur vecteur Ton											*/
/* Polarité positive : la sortie est à 1 durant les Ton premières périodes	*/
/*                     de ClkIn												*/
/* Mode d'emploi : 															*/
/* 	Rappel : 	Tpwm = (Résolution en pas) x Tclkin							*/
/*				Résolution en pas = 2^RESOLUTION							*/
/* 				Rapport cyclique en sortie = Ton / Résolution en pas		*/
/*																			*/
/* Prise en compte de Ton sur front descendant de nLatch					*/ 
/* oe : activation/désactivation sortie (oe = 1 : sortie ok, oe = 0 : HiZ   */ 
/* (c) Alexis ROLLAND - 2017 												*/
/*--------------------------------------------------------------------------*/
module modPWM(ClkIn, PWMout,Ton,nLatch,oe);

parameter RESOLUTION = 10;			// Nombre de bits des regsitres de comptage. 10bits = PWM 1024 pas de résolution
			
input ClkIn,nLatch,oe;
input[(RESOLUTION-1):0] Ton;

output PWMout;

reg PWMout_local;

reg[(RESOLUTION-1):0] NbTon, NbTclkIn;

initial
begin
	NbTon <=0;
	NbTclkIn <=0;
end


always@(negedge(nLatch))
begin
	NbTon = Ton;
end

always@(negedge(ClkIn))
begin
	if (NbTclkIn < NbTon) PWMout_local <= 1'b1;
	else PWMout_local <= 1'b0;
	
	NbTclkIn <= NbTclkIn + 10'd1;
	
end

// pilotage sortie
bufif1(PWMout,PWMout_local,oe);


endmodule

