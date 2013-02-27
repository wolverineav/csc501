#include <math.h>

double pow(double x, int y){
        int j;
        double result=1.0f;
        if(0 == x) return 0;
        if(-1 == x) return (0 == (y%2)) ? 1 : -1;

        for(j=1; j<=y; j++){
                result=result*x;
        }
        return result;
}

double log(double x){

        double result, p;
        int i;

	p = 1-x;
	result = p;
	for(i=2; i<41; i++){
		result = result + pow(-1,i)+(pow(p,i)/i);
	}

	return 1-result;
}

double expdev(double lambda){
	double x;
	do{
		x = (double) rand()/RAND_MAX;
	}while(x == 0.0);
	return -log(x)/lambda;
}
