#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "Pipe.H"

int main(int argc, char* argv[]) {



	Pipe myPipe((const char*)argv[1]);


	int ns=myPipe.getNumSamples();
	float* smps = myPipe.getSamples();

	int renorm = atoi(argv[2]);
	int dur = atoi(argv[3]);
	int16_t * rsm = new int16_t[ns];
	for(int i=0; i< ns; ++i) {
	  rsm[i]=(int16_t)(smps[i] * renorm);
	}

	for(int i =0; i<dur;++i) {
	fwrite(rsm, sizeof(int16_t), ns, stdout);
	}
	
	return 0;
}
