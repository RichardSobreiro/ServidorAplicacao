#include <iostream> 
#include <fstream>
#include <windows.h>
// Bibliotecas Boost
#include <boost/interprocess/ipc/message_queue.hpp>
// Bibliotecas do projeto
#include "Servidor_Historiador.h"

using namespace std;
using namespace boost::interprocess;

void thread_procedimento_servidor_historiador()
{
	try {
		// Abre a fila de mensagens servidor de aplicação para o historiador
		message_queue fila_servidor_historiador
		(
			open_only,									// Cria a fila de mensagens
			"servapp_historiador"						// Nome da fila de mensagens
		);

		// Abre a fila de mensagens historiador para o servidor de aplicação
		message_queue fila_historiador_servidor
		(
			open_only,									// Cria a fila de mensagens
			"historiador_servapp"						// Nome da fila de mensagens
		);

		while (true)
		{
			struct historical_data_request_t data_request;
			struct historical_data_reply_t data_reply;
			
			fila_servidor_historiador.send(&data_request, sizeof(struct historical_data_request_t), 1);
		}

	}
	catch (interprocess_exception &ex) {
		// Apaga fila anterior caso a mesma ainda exista
		message_queue::remove("historiador_servapp");
		message_queue::remove("servapp_historiador");
		cout << ex.what() << endl;
		return;
	}

	// Apaga fila anterior caso a mesma ainda exista
	message_queue::remove("historiador_servapp");
	message_queue::remove("servapp_historiador");
}

void imprime_position_t(struct position_t& nova_posicao)
{
	cout << "---------------------------------" << endl;
	cout << "ID: " << nova_posicao.id << endl;
	cout << "Latitude: " << nova_posicao.latitude << endl;
	cout << "Longitude: " << nova_posicao.longitude << endl;
	cout << "Velocidade: " << nova_posicao.speed << endl;
	struct tm *tm = new struct tm;
	gmtime_s(tm, &(nova_posicao.timestamp));
	char date[30];
	strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", tm);
	cout << "Timestamp: " << date << endl;
	cout << "---------------------------------" << endl;
}