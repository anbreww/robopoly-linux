//**************************************************************//
//* Fonctions C pour la caméra linéaire TSL3301                 //
//* Auteur(s): Eric Seuret                                      //
//* Date: 04-05-2007                                            //
//* Version: 0.1                                                //
//**************************************************************//


#include "lcam.h"

void lcam_stop(unsigned char *image){

		lcam_endintegration();
		lcam_readout();
		lcam_read(image);

}

