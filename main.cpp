#include <stdlib.h>
#include <iostream>
#include <string>
#include <thread>

using namespace std;

#define NUM_THREADS 5

void *say_helo(void *args)
{
	cout << "HELLO MY CODE" << endl;
	return 0;
}



int main()
{


	thread tids[NUM_THREADS];
	for (int i = 0; i < NUM_THREADS; ++i)
	{
		thread();
	}

	system("pause");
	return 0;
}
