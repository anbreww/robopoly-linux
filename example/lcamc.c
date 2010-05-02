//**************************************************************//
//* Fonctions C pour la caméra linéaire TSL3301                 //
//* Auteur(s): Eric Seuret                                      //
//* Date: 23-05-2009 (04-05-2007)                               //
//* Version: 0.11                                               //
//**************************************************************//


//23-05-2009: Modification de lcam_stop pour gérer l'erreur de timeout (Christophe Winter)

#include "lcam.h"
#include <avr/io.h>
#include "robopoly.h"

void lcam_stop(unsigned char *image){

		lcam_endintegration();

		if(lcam_readout() != 0)
		{
			// Reset complétement la caméra si elle a planté
			lcam_reset();
			lcam_setup();
		}
		else
		{
			// Sinon on lit les pixels
			lcam_read(image);
		}

}

