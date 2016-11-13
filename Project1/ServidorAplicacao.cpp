#include <iostream>
// Bibliotecas Boost
#include <boost/thread.hpp>
// Bibliotecas do projeto
#include "gateway_historiador.h"
#include "Servidor_Historiador.h"

using namespace std;

int main()
{
	//boost::thread thread_gateway_historiador{ thread_procedimento_gateway_historiador };
	boost::thread thread_servidor_historiador{ thread_procedimento_servidor_historiador };

	//thread_gateway_historiador.join();
	thread_servidor_historiador.join();

	system("PAUSE");
	return 0;
}