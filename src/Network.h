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
class QButtonGroup;
class QLineEdit;
class QPushButton;
class QSlider;
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
		/**
		 * \brief Constructor
		 */
		Network(QObject* = 0 //!< The Parent Object
			);

		/**
		 * \brief The settings interface.
		 *
		 * The widget the provides all the controls to manage this 
		 * object will be return by this method.
		 *
		 * \return The widget.
		 */
		QWidget* settings();

		/**
		 * \deprecated
		 */
		bool isListening() const;
	
	signals:
		/**
		 * \brief A Message has been recieved.
		 */
		void recievedMessage(const Message& //!< The message.
			);

	public slots:
		/**
		 * \deprecated
		 */
		void connectTo(const QHostAddress&, quint16);

		/**
		 * \brief Send a Message.
		 */
		void sendMessage(const Message& //!< The message.
			);

	private slots:
		void checkConnections();
		void connectionRequest();
		void readData();

		void settingsListenerVerifyAddress(const QString&);
		void settingsListenerUpdateButtons();
		void settingsListenerSetDefaults();
		void settingsListenerUseNow();

	private:
		void initListener(QHostAddress* = 0, quint16* = 0);
		void initSettings();
		QWidget* initSettingsListener();

		QTimer             pending_timer;
		QTcpServer         server_socket;
		QTabWidget         settings_widget;
		QButtonGroup*      settings_listener_addr_type;
		QLineEdit*         settings_listener_address;
		QSlider*           settings_listener_port;
		QPushButton*       settings_listener_set_default;
		QPushButton*       settings_listener_use_now;
		QList<QTcpSocket*> sockets_connected;
		QList<QTcpSocket*> connections_pending;
		quint16            listener_port;
};

#endif
