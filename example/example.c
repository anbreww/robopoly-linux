#include "robopoly.h"
#include "lcam.h"

// Ce code de test fait clignoter la LED du PRisme (Pin C2)
#define DELAY 200

int main (void)
{
	lcam_initport();
	lcam_setup();

	while(1)
	{
		digitalWrite(C,2,1);
		waitms(DELAY);
		digitalWrite(C,2,0);
		waitms(DELAY);
	}

	// never reached

}
