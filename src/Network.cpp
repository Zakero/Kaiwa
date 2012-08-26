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
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHostInfo>
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
static void buildHostname(const QHostAddress& address, const quint16& port, QString& hostname)
{
	hostname.clear();
	hostname.append(address.toString())
		.append("#")
		.append(QString::number(port))
		;
}

static void splitHostname(const QString& hostname, QHostAddress& address, quint16& port)
{
	QStringList parts = hostname.split('#');

	address.setAddress(parts[0]);
	port = parts[1].toUInt();
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
	, default_address()
	, default_port(0)
	, settings_widget()
	, settings_connection_address(0)
	, settings_connection_list(0)
	, settings_connection_add(0)
	, settings_connection_port(0)
	, settings_connection_remove(0)
	, settings_listener_addr_type(0)
	, settings_listener_address()
	, settings_listener_address_edit(0)
	, settings_listener_address_error(0)
	, settings_listener_port(-1)
	, settings_listener_port_edit(0)
	, settings_listener_port_error(0)
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
	initConnection();
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
			else
			{
				new QListWidgetItem(hostname
					, settings_connection_list
					);
			}
			
			connect(
				socket, SIGNAL(readyRead()),
				this, SLOT(readData())
				);
			connect(
				socket, SIGNAL(disconnected()),
				this, SLOT(removeSocket())
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
 * Remove all disconnected sockets from the sockets_connected list.  Also 
 * remove the disconnected sockets from the settings_connection_list.
 */
void Network::removeSocket()
{
	int i = 0;
	while(i < sockets_connected.size())
	{
		QTcpSocket* socket = sockets_connected.at(i);
		if(socket->state() == QAbstractSocket::UnconnectedState)
		{
			sockets_connected.removeAt(i);
			socket->deleteLater();
		}
		else
		{
			i++;
		}
	}

	i = 0;
	while(i < settings_connection_list->count())
	{
		QHostAddress address;
		quint16 port;

		QListWidgetItem* item = settings_connection_list->item(i);
		splitHostname(item->text(), address, port);

		bool found_it = false;

		for(int j = 0; j < sockets_connected.size(); j++)
		{
			QTcpSocket* socket = sockets_connected.at(j);
			if(socket->peerAddress() == address
				&& socket->peerPort() == port
				)
			{
				found_it = true;
				break;
			}
		}

		if(!found_it)
		{
			settings_connection_list->removeItemWidget(item);
			delete item;
		}

		i++;
	}
}

/**
 * \internal
 * Create a new socket using the command line connection values.
 */
bool Network::initConnection()
{
	QString value;

	value = Kaiwa::getCommandLineValue("Network", "Connection", "Address").toString();

	if(value.isNull() || value.isEmpty())
	{
		return false;
	}

	QHostAddress address(value);

	value = Kaiwa::getCommandLineValue("Network", "Connection", "Port").toString();

	quint16 port;
	if(value.isNull() || value.isEmpty())
	{
		port = default_port;

		if(port == 0)
		{
			port = server_socket.serverPort();
		}

		if(port == 0)
		{
			return false;
		}
	}
	else
	{
		port = value.toUInt();
	}

	QTcpSocket* socket = new QTcpSocket();
	socket->connectToHost(address, port);

	connections_pending.append(socket);

	return true;
}

/**
 * \internal
 * \brief Initialize the Server Socket.
 *
 * The server socket is used to listener for connection requests.  If a NULL 
 * address is passed, then the stored value will be used.  If there is not 
 * stored value, or if the stored value is invalid, use any address.
 * <b>\p address is assumed to be valid</b>
 *
 * If a NULL port number is passed, then the stored value will be used.  If 
 * there is not stored value, or if the stored value is invalid, use any 
 * available port number.
 *
 * \note Currently any and all connections are accepted.
 */
bool Network::initListener()
{
	QString value;

	value = Kaiwa::getValue("Network", "Listener", "Address").toString();
	value = Kaiwa::getCommandLineValue("Network", "Listener", "Address", value).toString();

	if(default_address.setAddress(value) == false)
	{
		default_address = QHostAddress::Any;
	}

	value = Kaiwa::getValue("Network", "Listener", "Port").toString();
	value = Kaiwa::getCommandLineValue("Network", "Listener", "Port", value).toString();
	default_port = value.toUInt(0, 0);

	return initListener(default_address, default_port);
}

/**
 * \internal
 * \brief Initialize the Server Socket.
 *
 * The server socket is used to listener for connection requests.  If a NULL 
 * address is passed, then the stored value will be used.  If there is not 
 * stored value, or if the stored value is invalid, use any address.
 * <b>\p address is assumed to be valid</b>
 *
 * If a NULL port number is passed, then the stored value will be used.  If 
 * there is not stored value, or if the stored value is invalid, use any 
 * available port number.
 *
 * \note Currently any and all connections are accepted.
 */
bool Network::initListener(const QHostAddress& address //!< The Host Address
	, const int& port //!< The Port number
	)
{
	listener_port = port;

	if(server_socket.isListening())
	{
		server_socket.close();
	}

	server_socket.listen(address, listener_port);

printf("%d listening(%s): %s:%d\n"
, getpid()
, server_socket.isListening() ? "T" : "F"
, toStr(server_socket.serverAddress().toString())
, server_socket.serverPort()
);

/*
printf("hostname: %s\n", toStr(QHostInfo::localHostName()));

QHostInfo host_info = QHostInfo::fromName("localhost");
QList<QHostAddress> address_list = host_info.addresses();

host_info = QHostInfo::fromName(QHostInfo::localHostName());
address_list.append(host_info.addresses());

for(int i = 0; i < address_list.size(); i++)
{
	printf("%d %s\n", i, toStr(address_list.at(i).toString()));
}
*/

	return server_socket.isListening();
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
	settings_connection_address->setGraphicsEffect(
		Kaiwa::createErrorEffect()
		);

	QLabel* address_label = new QLabel(tr("Address:"));
	address_label->setBuddy(settings_connection_address);

	settings_connection_port = new QSpinBox();
	settings_connection_port->setRange(0x0001, 0xffff);
	QLabel* port_label = new QLabel(tr("Port:"));
	port_label->setBuddy(settings_connection_port);

	QPushButton* clear = new QPushButton(tr("Clear"));
	settings_connection_add = new QPushButton(tr("Connect To Host"));
	settings_connection_add->setEnabled(false);

	QHBoxLayout* buttons = new QHBoxLayout();
	buttons->addWidget(clear);
	buttons->addWidget(settings_connection_add);

	settings_connection_list = new QListWidget();
	settings_connection_list->setSelectionMode(QAbstractItemView::SingleSelection);

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

	QGridLayout* gl = new QGridLayout();
	settings_connection_remove = new QPushButton(tr("Remove Connection"));
	settings_connection_remove->setEnabled(false);
	gl->addWidget(new QLabel(), 0, 0); gl->addWidget(new QLabel(), 0, 1); gl->addWidget(new QLabel(), 0, 2);
	gl->addWidget(new QLabel(), 1, 0);                                    gl->addWidget(new QLabel(), 1, 2);
	gl->addWidget(new QLabel(), 2, 0); gl->addWidget(new QLabel(), 2, 1); gl->addWidget(new QLabel(), 2, 2);
	gl->addWidget(settings_connection_remove, 1, 1);

	QWidget* widget = new QWidget();
	widget->setLayout(gl);
	connections->addWidget(widget);

	connect(
		clear, SIGNAL(clicked()),
		this, SLOT(settingsConnectionClearHost())
		);
	connect(
		settings_connection_address, SIGNAL(textChanged(const QString&)),
		this, SLOT(settingsConnectionAddressVerify(const QString&))
		);
	connect(
		settings_connection_add, SIGNAL(clicked()),
		this, SLOT(settingsConnectionEstablish())
		);
	connect(
		settings_connection_list, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
		this, SLOT(settingsConnectionShowInfo(QListWidgetItem*))
		);
	connect(
		settings_connection_remove, SIGNAL(clicked()),
		this, SLOT(settingsConnectionRemove())
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

void Network::settingsConnectionAddressVerify(const QString& address)
{
	QHostAddress host_address;

	if(host_address.setAddress(address))
	{
		settings_connection_address->graphicsEffect()->setEnabled(false);
		settings_connection_add->setEnabled(true);
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

		settings_connection_add->setEnabled(false);
	}
}

void Network::settingsConnectionRemove()
{
	QListWidgetItem* item = settings_connection_list->currentItem();

	QHostAddress address;
	quint16 port;

	splitHostname(item->text(), address, port);

	int i = 0;
	while(i < sockets_connected.size())
	{
		QTcpSocket* socket = sockets_connected.at(i);

		if(socket->peerAddress() == address
			&& socket->peerPort() == port
			)
		{
			socket->disconnectFromHost();
		}

		i++;
	}
}

void Network::settingsConnectionShowInfo(QListWidgetItem* current_item)
{
	if(current_item)
	{
		settings_connection_remove->setEnabled(true);
	}
	else
	{
		settings_connection_remove->setEnabled(false);
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

	settings_listener_address_edit = new QLineEdit();
	QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect();
	effect->setColor(QColor(0xff, 0x00, 0x00));
	effect->setStrength(1.0);
	effect->setEnabled(false);
	settings_listener_address_edit->setGraphicsEffect(effect);

	settings_listener_address_error = new QLabel();
	settings_listener_address_error->setAlignment(Qt::AlignCenter);
	settings_listener_address_error->setGraphicsEffect(
		Kaiwa::createErrorEffect(true)
		);

	// Layout the buttons
	QGridLayout* layout_address = new QGridLayout();
	layout_address->addWidget(addr_ip4_local, 0, 0);
	layout_address->addWidget(addr_ip4_any,   1, 0);
	layout_address->addWidget(addr_ip6_local, 0, 1);
	layout_address->addWidget(addr_ip6_any,   1, 1);
	layout_address->addWidget(addr_specific,  2, 0, 1, 2);
	layout_address->addWidget(settings_listener_address_edit,  3, 0, 1, 2);
	layout_address->addWidget(settings_listener_address_error, 4, 0, 1, 2);

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
	settings_listener_port_edit = new QSpinBox();
	settings_listener_port_edit->setSpecialValueText(tr("Any Available Port"));
	settings_listener_port_edit->setRange(0x0000, 0xffff);
	settings_listener_port_edit->setGraphicsEffect(
		Kaiwa::createErrorEffect()
		);

	settings_listener_port_error = new QLabel();
	settings_listener_port_error->setAlignment(Qt::AlignCenter);
	settings_listener_port_error->setGraphicsEffect(
		Kaiwa::createErrorEffect(true)
		);

	QVBoxLayout* layout_port = new QVBoxLayout();
	layout_port->addWidget(port_slider);
	layout_port->addWidget(settings_listener_port_edit);
	layout_port->addWidget(settings_listener_port_error);

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

	// Connect: Address
	connect(settings_listener_address_edit, SIGNAL(textEdited(const QString&)), this, SLOT(settingsListenerAddressVerify(const QString&)));
	connect(settings_listener_addr_type, SIGNAL(buttonClicked(int)), this, SLOT(settingsListenerAddressTypeUpdate()));

	connect(settings_listener_port_edit, SIGNAL(valueChanged(int)), this,        SLOT(settingsListenerPortVerify(int)));
	connect(settings_listener_port_edit, SIGNAL(valueChanged(int)), port_slider, SLOT(setValue(int)));
	connect(port_slider, SIGNAL(valueChanged(int)), settings_listener_port_edit, SLOT(setValue(int)));

	connect(settings_listener_set_default, SIGNAL(clicked()), this, SLOT(settingsListenerSetDefaults()));
	connect(settings_listener_use_now, SIGNAL(clicked()), this, SLOT(settingsListenerUseNow()));

	// Initial Values
	if(default_address == QHostAddress::LocalHost)
	{
		addr_ip4_local->click();
	}
	else if(default_address == QHostAddress::Any)
	{
		addr_ip4_any->click();
	}
	else if(default_address == QHostAddress::LocalHostIPv6)
	{
		addr_ip6_local->click();
	}
	else if(default_address == QHostAddress::AnyIPv6)
	{
		addr_ip6_any->click();
	}
	else
	{
		addr_specific->click();
		settings_listener_address_edit->setText(default_address.toString());
	}

	settings_listener_port_edit->setValue(default_port);

	return listener;
}

/**
 * \internal
 * \brief Check if the address is valid.
 *
 * The address in the settings_listener_address_edit widget is checked for 
 * validity.  If it is valid then its graphics effect is disabled.  However, if 
 * the address is not valid, then the graphics effect is enabled.
 */
void Network::settingsListenerAddressVerify(const QString& address)
{
	settings_listener_address.setAddress(address);
	if(settings_listener_address.isNull())
	{ // Bad Address
		if(address.isEmpty())
		{ // Empty is ok
			settings_listener_address_edit->graphicsEffect()->setEnabled(false);
			settings_listener_address_error->clear();
		}
		else
		{
			settings_listener_address_edit->graphicsEffect()->setEnabled(true);
			settings_listener_address_error->setText(tr("Invalid Address"));
		}

		settings_listener_address.clear();
	}
	else
	{ // Good Address
		QTcpServer test_socket;

		if(test_socket.listen(settings_listener_address, 0) == false)
		{ // Bad Address
			settings_listener_address.clear();
			settings_listener_address_edit->graphicsEffect()->setEnabled(true);
			settings_listener_address_error->setText(tr("Address not available"));
		}
		else
		{ // Good Address
			settings_listener_address_edit->graphicsEffect()->setEnabled(false);
			settings_listener_address_error->clear();
		}

		test_socket.close();
	}

	settingsListenerUpdateButtons();
}

/**
 * \internal
 * \brief Update the state of the buttons.
 *
 * As the various listener settings are changed, some widgets may become valid 
 * or invalid.  This method will update the visual state of the widgets.
 */
void Network::settingsListenerAddressTypeUpdate()
{
	int address_type = settings_listener_addr_type->checkedId();
	switch(address_type)
	{
		case QHostAddress::Null:
			settings_listener_address_edit->setEnabled(true);
			settings_listener_address_error->setEnabled(true);
			settingsListenerAddressVerify(
				settings_listener_address_edit->text()
				);
			break;
		case QHostAddress::LocalHost:
			settings_listener_address_edit->setEnabled(false);
			settings_listener_address_error->setEnabled(false);
			settings_listener_address = QHostAddress::LocalHost;
			break;
		case QHostAddress::Any:
			settings_listener_address_edit->setEnabled(false);
			settings_listener_address_error->setEnabled(false);
			settings_listener_address = QHostAddress::Any;
			break;
		case QHostAddress::LocalHostIPv6:
			settings_listener_address_edit->setEnabled(false);
			settings_listener_address_error->setEnabled(false);
			settings_listener_address = QHostAddress::LocalHostIPv6;
			break;
		case QHostAddress::AnyIPv6:
			settings_listener_address_edit->setEnabled(false);
			settings_listener_address_error->setEnabled(false);
			settings_listener_address = QHostAddress::AnyIPv6;
			break;
	}

	settingsListenerUpdateButtons();
}

void Network::settingsListenerPortVerify(int port)
{
	QTcpServer test_socket;

	if(port == server_socket.serverPort()
		|| test_socket.listen(QHostAddress::Any, port) == true)
	{ // Good Port
		settings_listener_port_error->clear();
		settings_listener_port_edit->graphicsEffect()->setEnabled(false);
		settings_listener_port = port;
	}
	else if(test_socket.listen(QHostAddress::Any, port) == true)
	{ // Good Port
		settings_listener_port_error->clear();
		settings_listener_port_edit->graphicsEffect()->setEnabled(false);
		settings_listener_port = port;
	}
	else
	{ // Bad Port
		if(port < 1024)
		// And user is not root
		{
			settings_listener_port_error->setText(tr("Permission Denied"));
		}
		else
		{
			settings_listener_port_error->setText(tr("Port In Use"));
		}

		settings_listener_port_edit->graphicsEffect()->setEnabled(true);
		settings_listener_port = -1;
	}

	test_socket.close();

	settingsListenerUpdateButtons();
}

void Network::settingsListenerUpdateButtons()
{
	if(settings_listener_address.isNull()  // Invalid Address
		|| settings_listener_port < 0  // Invalid Port
		)
	{
		settings_listener_use_now->setEnabled(false);
		settings_listener_set_default->setEnabled(false);
	}
	else
	{
		if(settings_listener_address == server_socket.serverAddress()
			&& settings_listener_port == listener_port
			)
		{
			settings_listener_use_now->setEnabled(false);
		}
		else
		{
			settings_listener_use_now->setEnabled(true);
		}

		if(settings_listener_address == default_address
			&& settings_listener_port == default_port
			)
		{
			settings_listener_set_default->setEnabled(false);
		}
		else
		{
			settings_listener_set_default->setEnabled(true);
		}
	}
}

/**
 * \internal
 * \brief Store the current Listener settings.
 */
void Network::settingsListenerSetDefaults()
{
	default_address = settings_listener_address;
	default_port = settings_listener_port;

	Kaiwa::putValue("Network", "Listener", "Address", settings_listener_address.toString());
	Kaiwa::putValue("Network", "Listener", "Port", settings_listener_port);

	settingsListenerUpdateButtons();
}

/**
 * \internal
 * \brief Use the current Listener setting values.
 */
void Network::settingsListenerUseNow()
{
	initListener(settings_listener_address, settings_listener_port);
	settingsListenerUpdateButtons();
}
