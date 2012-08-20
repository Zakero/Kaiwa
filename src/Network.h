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

#ifndef NETWORK_LISTENER_H_H
#define NETWORK_LISTENER_H_H

/******************************************************************************
 * Defines
 */


/******************************************************************************
 * Includes
 */
//ANSI/POSIX

// Qt
#include <QList>
#include <QObject>
#include <QTimer>
#include <QTabWidget>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpServer>

#include <QString>

// Local
#include "Message.h"


/******************************************************************************
 * Namespaces
 */


/******************************************************************************
 * Forward Declarations
 */
class QLineEdit;
class QTcpSocket;


/******************************************************************************
 * Typedefs
 */


/******************************************************************************
 * Structures
 */


/******************************************************************************
 * Classes
 */

class Network
	: public QObject
{
	Q_OBJECT

	public:
		Network(QObject* = 0);

		bool isListening() const;
		QWidget* settings();
	
	signals:
		void recievedMessage(const Message&);

	public slots:
		void connectTo(const QHostAddress&, quint16);
		void sendMessage(const Message& //!< The message.
			);

	private slots:
		void connectionRequest();
		void checkConnections();
		void readData();

		void verifyAddress(const QString&);

	private:
		void initSettings();
		QWidget* initSettingsListener();

		QTimer             pending_timer;
		QTcpServer         server_socket;
		QTabWidget         settings_widget;
		QLineEdit*         addr_line_edit;
		QList<QTcpSocket*> sockets_connected;
		QList<QTcpSocket*> connections_pending;
		quint16            listener_port;
};

#endif
