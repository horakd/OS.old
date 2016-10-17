#include <stdio.h>
#include <stdlib.h>
#include <thread>

#include "utils.h"
#include "kernel.h"

std::thread kernel;

//init
int main(int argc, void* argv)
{
	/* 
	inicializace z�kladn�ch struktur
		pcb
		filehandlers
		fronta �kol� pro kernel
	n�sledn� spu�t�n� vl�kna kernelu
	*/
	u_printMessage("Start main");
	u_printMessage("Kernel init");
	k_init();
	u_printMessage("Kernel start");
	//kernel = std::thread(k_run);
	u_printMessage("Kernel running");
	//kernel.join();
	u_printMessage("Kernel stop");
	//uklid


	u_printMessage("Stop main");
	system("pause");
	return 0;
}