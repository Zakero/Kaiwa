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
#include <QButtonGroup>
#include <QByteArray>
#include <QColor>
#include <QGraphicsColorizeEffect>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QVBoxLayout>
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
static void buildHostname(const QHostAddress& host_address, const quint16& port, QString& hostname)
{
	hostname.clear();
	hostname.append(host_address.toString())
		.append(" : ")
		.append(QString::number(port))
		;
}

static void setStateConnected(QListWidgetItem* item)
{
	QBrush brush = item->foreground();
	QColor color = brush.color();

	color.setAlpha(0xff);

	brush.setColor(color);
	item->setForeground(brush);
}

static void setStatePending(QListWidgetItem* item)
{
	QBrush brush = item->foreground();
	QColor color = brush.color();

	color.setAlpha(0x77);

	brush.setColor(color);
	item->setForeground(brush);
}

/******************************************************************************
 * Class Variables: Network
 */


/******************************************************************************
 * Class Methods: Network
 */

/**
 * \internal
 *
 */
Network::Network(QObject* parent)
	: QObject(parent)
	, pending_timer(this)
	, server_socket(this)
	, settings_widget()
	, settings_connection_address(0)
	, settings_connection_list(0)
	, settings_connection_new(0)
	, settings_connection_port(0)
	, settings_listener_addr_type(0)
	, settings_listener_address(0)
	, settings_listener_port(0)
	, settings_listener_set_default(0)
	, settings_listener_use_now(0)
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

	initListener();
	initSettings();
}

/**
 * \internal
 * \brief Check the pending connections.
 *
 * Every time the internal timer fires, this method will be called to check the 
 * status of all pending connections.  If a socket is connect, then that socket 
 * will be removed from the "pending" list and added to the "connected" list.  
 * The socket will also be connected to "readData()".
 */
void Network::checkConnections()
{
	int i = 0;
	while(i < connections_pending.size())
	{
		QTcpSocket* socket = connections_pending.at(i);

		QAbstractSocket::SocketState socket_state = socket->state();
		if(socket_state == QAbstractSocket::ConnectedState)
		{
			printf("%d - Socket is connected:\n\tlocal: %s:%u\n\tpeer: %s:%u\n"
				, getpid()
				, toStr(socket->localAddress().toString())
				, socket->localPort()
				, toStr(socket->peerAddress().toString())
				, socket->peerPort()
				);

			connections_pending.removeAt(i);

			QString hostname;
			buildHostname(socket->peerAddress(), socket->peerPort(), hostname);
			QList<QListWidgetItem*> items =
				settings_connection_list->findItems(hostname, Qt::MatchFixedString);
			if(items.size() > 0)
			{
				setStateConnected(items.at(0));
			}
			
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

/**
 * \internal
 * When the server_socket gets a new connection, this slot is triggered.  The 
 * socket is removed from the server_socket and added to connections pending.
 */
void Network::connectionRequest()
{
	QTcpSocket* socket = server_socket.nextPendingConnection();
	connections_pending.append(socket);
}

/**
 * \internal
 * \brief Initialize the Server Socket.
 *
 * The server socket is used to listener for connection requests.  If a NULL 
 * address is passed, then the stored value will be used.  If there is not 
 * stored value, or if the stored value is invalid, use any address.
 *
 * If a NULL port number is passed, then the stored value will be used.  If 
 * there is not stored value, or if the stored value is invalid, use any 
 * available port number.
 *
 * \note Currently any and all connections are accepted.
 */
void Network::initListener(QHostAddress* address //!< The Host Address
	, quint16* port //!< The Port number
	)
{
	QString value;
	QHostAddress host_address;

	if(address != 0)
	{
		host_address = *address;
	}
	else
	{
		value = Kaiwa::getValue("Network", "Listener", "Address").toString();
		value = Kaiwa::getCommandLineValue("Network", "Listener", "Address", value).toString();

		if(host_address.setAddress(value) == false)
		{
			host_address = QHostAddress::Any;
		}
	}

	if(port)
	{
		listener_port = *port;
	}
	else
	{
		value = Kaiwa::getValue("Network", "Listener", "Port").toString();
		value = Kaiwa::getCommandLineValue("Network", "Listener", "Port", value).toString();
		listener_port = value.toUInt(0, 0);
	}

	if(server_socket.isListening())
	{
		server_socket.close();
	}

	server_socket.listen(host_address, listener_port);

//printf("listening to: %s:%x\n", toStr(server_socket.serverAddress().toString()), server_socket.serverPort());
}

// Message handling
/**
 * \internal
 * \brief Read data from the sockets
 *
 * When one socket signals that it has data to be read, all the sockets are 
 * checked.  If the socket has data, then all the data is read in, converted 
 * into a Message, then signals that a Message is ready.
 */
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

/**
 * \internal
 * 
 */
void Network::sendMessage(const Message& message)
{
	QByteArray data;

	message.toByteArray(data);

	for(int i = 0; i < sockets_connected.size(); i++)
	{
		sockets_connected.at(i)->write(data);
	}
}

/**
 * \internal
 *
 */
QWidget* Network::settings()
{
	return &settings_widget;
}

/**
 * \internal
 * \brief Initialize the Settings widget.
 */
void Network::initSettings()
{
	settings_widget.addTab(initSettingsConnections(), tr("Connections"));
	settings_widget.addTab(initSettingsListener(), tr("Listener"));
}


/******************************************************************************
 * Settings - Connections
 */

QWidget* Network::initSettingsConnections()
{
	settings_connection_address = new QLineEdit();
	QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect();
	effect->setColor(QColor(0xff, 0x00, 0x00));
	effect->setStrength(1.0);
	effect->setEnabled(false);
	settings_connection_address->setGraphicsEffect(effect);

	QLabel* address_label = new QLabel(tr("Address:"));
	address_label->setBuddy(settings_connection_address);

	settings_connection_port = new QSpinBox();
	settings_connection_port->setRange(0x0001, 0xffff);
	QLabel* port_label = new QLabel(tr("Port:"));
	port_label->setBuddy(settings_connection_port);

	QPushButton* clear = new QPushButton(tr("Clear"));
	settings_connection_new = new QPushButton(tr("Connect To Host"));
	settings_connection_new->setEnabled(false);

	QHBoxLayout* buttons = new QHBoxLayout();
	buttons->addWidget(clear);
	buttons->addWidget(settings_connection_new);

	settings_connection_list = new QListWidget();

	QGridLayout* grid_layout = new QGridLayout();
	grid_layout->addWidget(address_label,               0, 0, Qt::AlignRight);
	grid_layout->addWidget(settings_connection_address, 0, 1);
	grid_layout->addWidget(port_label,                  1, 0, Qt::AlignRight);
	grid_layout->addWidget(settings_connection_port,    1, 1);
	grid_layout->addLayout(buttons,                     2, 0, 1, 2);
	grid_layout->addWidget(settings_connection_list,    3, 0, 1, 2);

	QWidget* connection_list = new QWidget();
	connection_list->setLayout(grid_layout);

	QSplitter* connections = new QSplitter(Qt::Horizontal);
	connections->setChildrenCollapsible(false);
	connections->addWidget(connection_list);
	connections->addWidget(new QLabel(""));

	connect(
		clear, SIGNAL(clicked()),
		this, SLOT(settingsConnectionClearHost())
		);
	connect(
		settings_connection_address, SIGNAL(textChanged(const QString&)),
		this, SLOT(settingsConnectionVerifyAddress(const QString&))
		);
	connect(
		settings_connection_new, SIGNAL(clicked()),
		this, SLOT(settingsConnectionEstablish())
		);

	return connections;
}

void Network::settingsConnectionClearHost()
{
	settings_connection_address->clear();
}

void Network::settingsConnectionEstablish()
{
	QHostAddress host_address(settings_connection_address->text());
	quint16 port = settings_connection_port->value();

	QString hostname;
	buildHostname(host_address, port, hostname);

	QListWidgetItem* item = new QListWidgetItem(hostname);
	setStatePending(item);
	settings_connection_list->addItem(item);

	QTcpSocket* socket = new QTcpSocket();
	socket->connectToHost(host_address, port);

	connections_pending.append(socket);
}

void Network::settingsConnectionVerifyAddress(const QString& address)
{
	QHostAddress host_address;

	if(host_address.setAddress(address))
	{
		settings_connection_address->graphicsEffect()->setEnabled(false);
		settings_connection_new->setEnabled(true);
	}
	else
	{
		if(address.isEmpty())
		{
			settings_connection_address->graphicsEffect()->setEnabled(false);
		}
		else
		{
			settings_connection_address->graphicsEffect()->setEnabled(true);
		}

		settings_connection_new->setEnabled(false);
	}
}

/******************************************************************************
 * Settings - Listener
 */
/**
 * \internal
 * \brief Initialize the Listener settings.
 */
QWidget* Network::initSettingsListener()
{
	// Listener - Address

	// Create the buttons
	QRadioButton* addr_ip4_local = new QRadioButton(tr("IPv4 Localhost"));
	QRadioButton* addr_ip4_any  = new QRadioButton(tr("IPv4 Any Address"));
	QRadioButton* addr_ip6_local = new QRadioButton(tr("IPv6 Localhost"));
	QRadioButton* addr_ip6_any  = new QRadioButton(tr("IPv6 Any Address"));
	QRadioButton* addr_specific = new QRadioButton(tr("A Specific Address"));

	settings_listener_address = new QLineEdit();
	QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect();
	effect->setColor(QColor(0xff, 0x00, 0x00));
	effect->setStrength(1.0);
	effect->setEnabled(false);
	settings_listener_address->setGraphicsEffect(effect);

	// Layout the buttons
	QGridLayout* layout_address = new QGridLayout();
	layout_address->addWidget(addr_ip4_local, 0, 0);
	layout_address->addWidget(addr_ip4_any,   1, 0);
	layout_address->addWidget(addr_ip6_local, 0, 1);
	layout_address->addWidget(addr_ip6_any,   1, 1);
	layout_address->addWidget(addr_specific,  2, 0, 1, 2);
	layout_address->addWidget(settings_listener_address, 3, 0, 1, 2);

	// Display the buttons
	QGroupBox* group_address = new QGroupBox(tr("Address"));
	group_address->setLayout(layout_address);

	// Group the buttons
	settings_listener_addr_type = new QButtonGroup(group_address);
	settings_listener_addr_type->addButton(addr_ip4_local, QHostAddress::LocalHost);
	settings_listener_addr_type->addButton(addr_ip4_any, QHostAddress::Any);
	settings_listener_addr_type->addButton(addr_ip6_local, QHostAddress::LocalHostIPv6);
	settings_listener_addr_type->addButton(addr_ip6_any, QHostAddress::AnyIPv6);
	settings_listener_addr_type->addButton(addr_specific, QHostAddress::Null);

	// Listener - Port
	QSlider* port_slider = new QSlider(Qt::Horizontal);
	port_slider->setRange(0x0000, 0xffff);
	settings_listener_port = new QSpinBox();
	settings_listener_port->setSpecialValueText("Any Available Port");
	settings_listener_port->setRange(0x0000, 0xffff);

	QVBoxLayout* layout_port = new QVBoxLayout();
	layout_port->addWidget(port_slider);
	layout_port->addWidget(settings_listener_port);

	QGroupBox* group_port = new QGroupBox(tr("Port Number"));
	group_port->setLayout(layout_port);

	// Listener - Actions
	settings_listener_set_default = new QPushButton(tr("Set As Default"));
	settings_listener_use_now = new QPushButton(tr("Use These Settings"));

	QHBoxLayout* layout_button = new QHBoxLayout();
	layout_button->addWidget(settings_listener_set_default);
	layout_button->addWidget(settings_listener_use_now);

	QVBoxLayout* vbox = new QVBoxLayout();
	vbox->addWidget(group_address);
	vbox->addWidget(group_port);
	vbox->addStretch(1);
	vbox->addLayout(layout_button);

	QWidget* listener = new QWidget();
	listener->setLayout(vbox);

	// Connections
	connect(settings_listener_address, SIGNAL(textEdited(const QString&)), this, SLOT(settingsListenerVerifyAddress(const QString&)));
	connect(settings_listener_address, SIGNAL(textEdited(const QString&)), this, SLOT(settingsListenerUpdateButtons()));
	connect(settings_listener_addr_type, SIGNAL(buttonClicked(int)), this, SLOT(settingsListenerUpdateButtons()));

	connect(settings_listener_port, SIGNAL(valueChanged(int)), this,         SLOT(settingsListenerUpdateButtons()));
	connect(settings_listener_port, SIGNAL(valueChanged(int)), port_slider, SLOT(setValue(int)));
	connect(port_slider, SIGNAL(valueChanged(int)), settings_listener_port, SLOT(setValue(int)));

	connect(settings_listener_set_default, SIGNAL(clicked()), this, SLOT(settingsListenerSetDefaults()));
	connect(settings_listener_set_default, SIGNAL(clicked()), this, SLOT(settingsListenerUpdateButtons()));

	connect(settings_listener_use_now, SIGNAL(clicked()), this, SLOT(settingsListenerUseNow()));
	connect(settings_listener_use_now, SIGNAL(clicked()), this, SLOT(settingsListenerUpdateButtons()));

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
		settings_listener_address->setText(current_address);
	}

	settings_listener_port->setValue(listener_port);

	return listener;
}

/**
 * \internal
 * \brief Check if the address is valid.
 *
 * The address in the settings_listener_address widget is checked for validity.  
 * If it is valid then its graphics effect is disabled.  However, if the 
 * address is not valid, then the graphics effect is enabled.
 */
void Network::settingsListenerVerifyAddress(const QString& address)
{
	QHostAddress host_address;

	if(host_address.setAddress(address))
	{
		settings_listener_address->graphicsEffect()->setEnabled(false);
	}
	else
	{
		if(address.isEmpty())
		{
			settings_listener_address->graphicsEffect()->setEnabled(false);
		}
		else
		{
			settings_listener_address->graphicsEffect()->setEnabled(true);
		}
	}
}

/**
 * \internal
 * \brief Update the state of the buttons.
 *
 * As the various listener settings are changed, some widgets may become valid 
 * or invalid.  This method will update the visual state of the widgets.
 */
void Network::settingsListenerUpdateButtons()
{
	QString new_address;
	quint16 new_port = settings_listener_port->value();

	int address_type = settings_listener_addr_type->checkedId();
	switch(address_type)
	{
		case QHostAddress::Null:
			settings_listener_address->setEnabled(true);
			settingsListenerVerifyAddress(settings_listener_address->text());
			if(!settings_listener_address->graphicsEffect()->isEnabled())
			{
				new_address = settings_listener_address->text();
			}
			break;
		case QHostAddress::LocalHost:
			settings_listener_address->setEnabled(false);
			new_address = QHostAddress(QHostAddress::LocalHost).toString();
			break;
		case QHostAddress::Any:
			settings_listener_address->setEnabled(false);
			new_address = QHostAddress(QHostAddress::Any).toString();
			break;
		case QHostAddress::LocalHostIPv6:
			settings_listener_address->setEnabled(false);
			new_address = QHostAddress(QHostAddress::LocalHostIPv6).toString();
			break;
		case QHostAddress::AnyIPv6:
			settings_listener_address->setEnabled(false);
			new_address = QHostAddress(QHostAddress::AnyIPv6).toString();
			break;
	}

	if(new_address.isNull() || new_address.isEmpty())
	{
		settings_listener_use_now->setEnabled(false);
		settings_listener_set_default->setEnabled(false);
		return;
	}

	QString current_address = server_socket.serverAddress().toString();
	quint16 current_port = listener_port;

	if(new_address == current_address && new_port == current_port)
	{
		settings_listener_use_now->setEnabled(false);
	}
	else
	{
		settings_listener_use_now->setEnabled(true);
	}

	QString default_address = Kaiwa::getValue("Network", "Listener", "Address").toString();
	quint16 default_port = Kaiwa::getValue("Network", "Listener", "Port", 0).toUInt();

	if(new_address == default_address && new_port == default_port)
	{
		settings_listener_set_default->setEnabled(false);
	}
	else
	{
		settings_listener_set_default->setEnabled(true);
	}
}

/**
 * \internal
 * \brief Store the current Listener settings.
 */
void Network::settingsListenerSetDefaults()
{
	QString address;

	int address_type = settings_listener_addr_type->checkedId();
	switch(address_type)
	{
		case QHostAddress::Null:
			address = settings_listener_address->text();
			break;
		case QHostAddress::LocalHost:
			address = QHostAddress(QHostAddress::LocalHost).toString();
			break;
		case QHostAddress::Any:
			address = QHostAddress(QHostAddress::Any).toString();
			break;
		case QHostAddress::LocalHostIPv6:
			address = QHostAddress(QHostAddress::LocalHostIPv6).toString();
			break;
		case QHostAddress::AnyIPv6:
			address = QHostAddress(QHostAddress::AnyIPv6).toString();
			break;
	}

	Kaiwa::putValue("Network", "Listener", "Address", address);
	Kaiwa::putValue("Network", "Listener", "Port", settings_listener_port->value());
}

/**
 * \internal
 * \brief Use the current Listener setting values.
 */
void Network::settingsListenerUseNow()
{
	QHostAddress host_address;

	int address_type = settings_listener_addr_type->checkedId();
	switch(address_type)
	{
		case QHostAddress::Null:
			host_address = QHostAddress(settings_listener_address->text());
			break;
		case QHostAddress::LocalHost:
			host_address = QHostAddress(QHostAddress::LocalHost);
			break;
		case QHostAddress::Any:
			host_address = QHostAddress(QHostAddress::Any);
			break;
		case QHostAddress::LocalHostIPv6:
			host_address = QHostAddress(QHostAddress::LocalHostIPv6);
			break;
		case QHostAddress::AnyIPv6:
			host_address = QHostAddress(QHostAddress::AnyIPv6);
			break;
	}

	listener_port = settings_listener_port->value();

	initListener(&host_address, &listener_port);
}
