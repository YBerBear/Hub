/////////////////////////////
//
//	interval_test.cpp
//
/////////////////////////////
#include <iostream>
#include <stdio.h>
#include <stdlib.h>            //????? can live without it
#include <malloc.h>
#include <ctime>
#include <fcntl.h>

using namespace std;

void pause(int iDur)
{
	int temp;
	
	temp = time(NULL) + iDur;
	
	while(time(NULL) < temp);
}

int main(int argv, char *argc)
{
	
	time_t time_now, time_reference;
	int i;
	
	time(&time_reference);

	cout << "\nPresent time reference: " << time_reference << endl;
	// do {
		// time(&time_now);
		// cout << cout << "\n Now: " << time_now << endl;
	// }
	// while(difftime(time_now,time_reference) < 60); 
	
	for (i = 0; i < 5; i++) 
	{
		pause (3);
		time(&time_now);
		cout << "\ni = " << i << ", time reference: " << time_reference << ", time now " << time_now << ",  difference: " <<  difftime(time_now,time_reference) << endl;
	}
	
	return 0;
}

