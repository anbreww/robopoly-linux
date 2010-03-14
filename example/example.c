#include "robopoly.h"

int main (void)
{
	while(1)
	{
		digitalWrite(A,2,1);
		waitms(200);
		digitalWrite(A,2,0);
		waitms(200);
	}
}
