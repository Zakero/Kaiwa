/******************************************************************************
 * Copyright 2012 Andrew Moore
 *
 * This file is part of Kaiwa.
 *
 * Kaiwa is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kaiwa is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kaiwa.  If not, see <http://www.gnu.org/licenses/>.
 */

/******************************************************************************
 * Defines
 */


/******************************************************************************
 * Includes
 */
// ANSI/POSIX

// Qt
#include <QByteArray>
#include <QTime>
#include <QtNetwork/QTcpSocket>

// Local
#include "Kaiwa.h"
#include "Network.h"


/******************************************************************************
 * Namespaces
 */


/******************************************************************************
 * Functions: Public
 */


/******************************************************************************
 * Functions: Private
 */
static void setupListener(QTcpServer& socket)
{
	QString value;

	value = Kaiwa::getSetting("Network", "Listener", "Address").toString();
	QHostAddress host_address;
	if(value.isNull() || value.isEmpty())
	{
		host_address = QHostAddress::Any;
	}
	else
	{
		host_address = QHostAddress(value);
	}

	value = Kaiwa::getSetting("Network", "Listener", "Port").toString();
	quint16 port = value.toUInt(0, 0);

	socket.listen(host_address, port);

	/*
	printf("socket: %s:%x\n"
		, toStr(socket.serverAddress().toString())
		, socket.serverPort()
		);
	*/
}


/******************************************************************************
 * Class Variables: Network
 */


/******************************************************************************
 * Class Methods: Network
 */

Network::Network(QObject* q_object)
	: QObject(q_object)
	, pending_timer(this)
	, server_socket(this)
	, sockets_connected()
	, connections_pending()
{
	// Server Socket
	connect(
		&server_socket, SIGNAL(newConnection()),
		this, SLOT(connectionRequest())
		);

	// Pending Connection Timer
	connect(
		&pending_timer, SIGNAL(timeout()),
		this, SLOT(checkConnections())
		);
	pending_timer.start(1000);

	setupListener(server_socket);
}

void Network::checkConnections()
{
	int i = 0;
	while(i < connections_pending.size())
	{
		QTcpSocket* socket = connections_pending.at(i);

		QAbstractSocket::SocketState socket_state = socket->state();
		if(socket_state == QAbstractSocket::ConnectedState)
		{
			printf("%d - Socket is connected: %s:%X %d\n"
				, getpid()
				, socket->localAddress().toString().toStdString().c_str()
				, socket->localPort()
				, socket->state()
				);

			connections_pending.removeAt(i);
			
			connect(
				socket, SIGNAL(readyRead()),
				this, SLOT(readData())
				);

			sockets_connected.append(socket);
		}
		else
		{
			i++;
		}
	}
}

// Incoming connections
bool Network::isListening() const
{
	//return server_socket.isListening();
	return (server_socket.serverPort() == 0xCAFE);
}

void Network::connectionRequest()
{
	QTcpSocket* socket = server_socket.nextPendingConnection();
	connections_pending.append(socket);
}

// Outgoing connections
void Network::connectTo(const QHostAddress& address, quint16 port)
{
	QTcpSocket* socket = new QTcpSocket();
	socket->connectToHost(address, port);

	connections_pending.append(socket);
}

// Message handling
void Network::readData()
{
	for(int i = 0; i < sockets_connected.size(); i++)
	{
		QTcpSocket* socket = sockets_connected.at(i);

		if(socket->bytesAvailable() > 0)
		{
			QByteArray data = socket->readAll();

			int message_len;
			Message message;
			message.init(data, &message_len);

			emit recievedMessage(message);

			data.remove(0, message_len);
		}
	}
}

void Network::sendMessage(const Message& message)
{
	QByteArray data;

	message.toByteArray(data);

	for(int i = 0; i < sockets_connected.size(); i++)
	{
		sockets_connected.at(i)->write(data);
	}
}
