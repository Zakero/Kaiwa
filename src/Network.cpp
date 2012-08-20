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

#include <QGroupBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QSlider>
#include <QPushButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSpinBox>
#include <QGraphicsColorizeEffect>

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
static void setupListener(QTcpServer& socket, quint16& port)
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
	port = value.toUInt(0, 0);

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
	, settings_widget()
	, addr_line_edit(new QLineEdit())
	, sockets_connected()
	, connections_pending()
	, listener_port(0)
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

	setupListener(server_socket, listener_port);

	initSettings();
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

// Settings
QWidget* Network::settings()
{
	return &settings_widget;
}

void Network::initSettings()
{
	//settings_widget.addTab(initSettingsConnctions(), tr("Connections"));
	settings_widget.addTab(initSettingsListener(), tr("Listener"));
}

QWidget* Network::initSettingsListener()
{
	// Listener - Address
	QRadioButton* addr_ip4_local = new QRadioButton(tr("IPv4 Localhost"));
	QRadioButton* addr_ip4_any  = new QRadioButton(tr("IPv4 Any Address"));
	QRadioButton* addr_ip6_local = new QRadioButton(tr("IPv6 Localhost"));
	QRadioButton* addr_ip6_any  = new QRadioButton(tr("IPv6 Any Address"));
	QRadioButton* addr_specific = new QRadioButton(tr("A Specific Address"));
	QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect();
	effect->setColor(QColor(0xff, 0x00, 0x00));
	effect->setStrength(1.0);
	effect->setEnabled(false);
	addr_line_edit->setGraphicsEffect(effect);

	QGridLayout* layout_address = new QGridLayout();
	layout_address->addWidget(addr_ip4_local, 0, 0);
	layout_address->addWidget(addr_ip4_any,   1, 0);
	layout_address->addWidget(addr_ip6_local, 0, 1);
	layout_address->addWidget(addr_ip6_any,   1, 1);
	layout_address->addWidget(addr_specific,  2, 0, 1, 2);
	layout_address->addWidget(addr_line_edit, 3, 0, 1, 2);

	QGroupBox* group_address = new QGroupBox(tr("Address"));
	group_address->setLayout(layout_address);

	// Listener - Port
	QSlider* port_slider = new QSlider(Qt::Horizontal);
	port_slider->setRange(0x0000, 0xffff);
	QSpinBox* port_spinbox = new QSpinBox();
	port_spinbox->setSpecialValueText("Any Available Port");
	port_spinbox->setRange(0x0000, 0xffff);

	QVBoxLayout* layout_port = new QVBoxLayout();
	layout_port->addWidget(port_slider);
	layout_port->addWidget(port_spinbox);
	//layout_port->addStretch(1);

	QGroupBox* group_port = new QGroupBox(tr("Port"));
	group_port->setLayout(layout_port);

	// Listener - Actions
	QPushButton* set_default = new QPushButton(tr("Set As Default"));
	QPushButton* use_now = new QPushButton(tr("Use These Settings"));

	QHBoxLayout* layout_button = new QHBoxLayout();
	layout_button->addWidget(set_default);
	layout_button->addWidget(use_now);

	QVBoxLayout* vbox = new QVBoxLayout();
	vbox->addWidget(group_address);
	vbox->addWidget(group_port);
	vbox->addStretch(1);
	vbox->addLayout(layout_button);

	QWidget* listener = new QWidget();
	listener->setLayout(vbox);

	// Connections
	connect(addr_ip4_local, SIGNAL(clicked(bool)), addr_line_edit, SLOT(setDisabled(bool)));
	connect(addr_ip4_any,   SIGNAL(clicked(bool)), addr_line_edit, SLOT(setDisabled(bool)));
	connect(addr_ip6_local, SIGNAL(clicked(bool)), addr_line_edit, SLOT(setDisabled(bool)));
	connect(addr_ip6_any,   SIGNAL(clicked(bool)), addr_line_edit, SLOT(setDisabled(bool)));
	connect(addr_specific,  SIGNAL(clicked(bool)), addr_line_edit, SLOT(setEnabled(bool)));
	connect(addr_line_edit, SIGNAL(textEdited(const QString&)), this, SLOT(verifyAddress(const QString&)));

	connect(port_slider, SIGNAL(valueChanged(int)), port_spinbox, SLOT(setValue(int)));
	connect(port_spinbox, SIGNAL(valueChanged(int)), port_slider, SLOT(setValue(int)));

	// Initial Values
	QString current_address = server_socket.serverAddress().toString();
	QString ip4_localhost = QHostAddress(QHostAddress::LocalHost).toString();
	QString ip4_any       = QHostAddress(QHostAddress::Any).toString();
	QString ip6_localhost = QHostAddress(QHostAddress::LocalHostIPv6).toString();
	QString ip6_any       = QHostAddress(QHostAddress::AnyIPv6).toString();

	if(current_address == ip4_localhost)
	{
		addr_ip4_local->click();
	}
	else if(current_address == ip4_any)
	{
		addr_ip4_any->click();
	}
	else if(current_address == ip6_localhost)
	{
		addr_ip6_local->click();
	}
	else if(current_address == ip6_any)
	{
		addr_ip6_any->click();
	}
	else
	{
		addr_specific->click();
		addr_line_edit->setText(current_address);
	}

	port_slider->setValue(listener_port);

	return listener;
}

void Network::verifyAddress(const QString& address)
{
	QHostAddress host_address;

	if(host_address.setAddress(address))
	{
		addr_line_edit->graphicsEffect()->setEnabled(false);
	}
	else
	{
		addr_line_edit->graphicsEffect()->setEnabled(true);
	}
}
