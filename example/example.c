#include "robopoly.h"
#include "lcam.h"

int main (void)
{
	lcam_initport();
	lcam_setup();
	while(1)
	{
		digitalWrite(C,2,1);
		waitms(200);
		digitalWrite(C,2,0);
		waitms(200);
	}
}
