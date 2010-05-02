#ifndef __lcam_h
#define __lcam_h
//**************************************************************//
//* Header de la librairie c pour la TSL3301                    //
//* Auteur(s): Eric Seuret                                      //
//* Date: 23-05-2009 (04-05-2007)                               //
//* Version: 0.11                                               //
//**************************************************************//

//23-05-2009: Modification de lcam_stop pour gérer l'erreur de timeout (Christophe Winter)

/** \defgroup lcam_h Caméra linéaire
	
	\brief	Librairie pour l'utilisation de la caméra linéaire

	\author Christophe Winter (code)
	\author	Fabrizio Loconte (code)
	\author	Eric Seuret (code+documentation)
	\date	Octobre 2007

*/
/*@{*/


/** \brief Configuration des ports du microcontrôleur
	
	Cette fonction configure les pins sur lesquels la caméra est branchée en entrée ou en sortie. 
	Vous devez l'appeler aprés avoir fait toute vos modification sur les ports, elle ne modifiera que ce qu'elle doit modifier.


*/
void lcam_initport(void);



/** \brief Initialisation de la caméra avec des paramètres standards

	Cette fonction initialise les paramaètre de la caméra (offset et gain) avec des paramètres standard.

*/
void lcam_setup(void);




/** \brief Reset de la camera

	Envoi l'ordre à la caméra de faire un reset

*/
void lcam_reset(void);


/** \brief Début de l'acquisition d'une image

	Cette fonction envoie l'ordre à la caméra de commencer l'acquisition d'une image. Lors de la réception de cette ordre la caméra commence la prise d'une image.

*/
void lcam_startintegration(void);


/** \brief Fin de l'intégration et téléchargement des données

	Cette fonction donne l'ordre à la caméra de stopper l'acquision d'une image et d'envoyer l'image au microcontrôleur. Elle se charge ensuite du téléchargement
	des donnée

	\param image Pointeur vers la zone mémoire ou l'image sera enregistrée (doit contenir 102 pixels)

*/
void lcam_stop(unsigned char *image);

void lcam_endintegration(void);	//Fin de l'intégration
unsigned char lcam_readout(void);		//Préparation à la lecture
void lcam_read(unsigned char *image); //Lecture et sauvegarde dans buffer


/** \brief Fonction pour la recherche d'un pic

	Cette fonction recherche le pic de plus haute valeur sur une image. Cette fonction divise les 102 pixels en 25 zones numérotée de 1 à 25. Le résultat obtenu
	est le numéro de la zone la plus lumineuse ou 0 si aucune zone n'était significativement plus lumineuse que les autres.

	Il faut également noter que cette fonction modifie l'image en mémoire. Une fois appliqué l'image n'est plus la même.

	\param image Pointeur vers la zone mémoire contenant l'image à analyser (doit contenir 102 pixels)
	\return 0 Aucune zone lumineuse détecter
	\return # numéro de la zone la plus lumineuse


*/
unsigned char lcam_getpic(unsigned char *image);
/*@}*/

#endif

/** \addtogroup lcam_h
	\section lcamexemple Exemple de programme documenté

	Il ne faut pas oublier d'ajouter les fichier lcam.s et lcamc.c au projet sous AVR Studio.

	\code

	#include <avr/io.h>
	#include <util/delay.h> //pour les fonctions _delay_ms() et _delay_us()
	#include "lcam.h"

	int main(){
		unsigned char image[102];	//Tableau de 102 valeurs pour pouvoir contenir les 102 pixels
		unsigned char valeur;		//Valeur de retour de la fonction lcam_getpic

		//Initialisation
		lcam_initport();		//Initialisation des pins du port
		lcam_reset();			//Ordonne un reset à la camera
		lcam_setup();			//Configure la caméra
		
		_delay_ms(2);			//attendre 2 ms pour que la logique de la camera puisse se configurer
			

		while(1){
			
			lcam_startintegration();	//On commence l'acquisition
			
			_delay_us(96);		//Ici il faut attendre le temps d'intégration, c-à-d le temps d'exposition.
			_delay_us(96);		//400 us est en général une bonnes valeur
			_delay_us(96);		//Mais comme en photographie cela dépends de divers paramètres dont votre optique
			_delay_us(96);		//et nous vous conseillons de faire quelques tests
			_delay_us(15);		//_delay_us() peut faire au maximum 96us sur un microcontrôleur à 8 Mhz c'est pourquoi nous l'avons répéter
			
			lcam_stop(image);  		//Fin de l'acquisition et téléchargement de l'image
			valeur = lcam_getpic(image); 	//Traitement de l'image 
			

			//////////////////////////////////////////
			//INSEREZ ICI CE QUE VOUS VOULEZ FAIRE
			//AVEC LE RESULTAT
			/////////////////////////////////////////
			
			_delay_ms(30);		//Attendre 100 ms avant la prochaine acquisiton
			_delay_ms(30);		//La caméra ne peut fournir qu'une dizaine d'image par secondes (sinon elle plante)
			_delay_ms(30);		//Il y a plusieur _delay_ms() car cette fonciton peut attendre au maximum 30ms 
			_delay_ms(10);		//sur un microcontrôleur à 8 Mhz
		}


		
		return 0;
	}



	\endcode

	*/


