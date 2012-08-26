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
#include <QHostAddress>
#include <QList>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QTabWidget>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpServer>

// Local
#include "Message.h"


/******************************************************************************
 * Namespaces
 */


/******************************************************************************
 * Forward Declarations
 */
class QButtonGroup;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QSpinBox;
class QTcpSocket;
class QVBoxLayout;
class QGridLayout;


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
	
	signals:
		/**
		 * \brief A Message has been recieved.
		 */
		void recievedMessage(const Message& //!< The message.
			);

	public slots:
		/**
		 * \brief Send a Message.
		 */
		void sendMessage(const Message& //!< The message.
			);

	private slots:
		void checkConnections();
		void connectionRequest();
		void readData();
		void removeSocket();

		void settingsConnectionAddressVerify(const QString&);
		void settingsConnectionClearHost();
		void settingsConnectionEstablish();
		void settingsConnectionRemove();
		void settingsConnectionShowInfo(QListWidgetItem*);

		void settingsListenerAddressTypeUpdate();
		void settingsListenerAddressVerify(const QString&);
		void settingsListenerPortVerify(int);
		void settingsListenerUpdateButtons();
		void settingsListenerSetDefaults();
		void settingsListenerUseNow();

	private:
		bool initListener();
		bool initListener(const QHostAddress&, const int&);
		void initSettings();
		QWidget* initSettingsConnections();
		QWidget* initSettingsListener();

		QTimer             pending_timer;
		QTcpServer         server_socket;
		QHostAddress       default_address;
		quint16            default_port;

		QTabWidget         settings_widget;
		QLineEdit*         settings_connection_address;
		QListWidget*       settings_connection_list;
		QPushButton*       settings_connection_add;
		QSpinBox*          settings_connection_port;
		QPushButton*       settings_connection_remove;
		QButtonGroup*      settings_listener_addr_type;
		QHostAddress       settings_listener_address;
		QLineEdit*         settings_listener_address_edit;
		QLabel*            settings_listener_address_error;
		int                settings_listener_port;
		QSpinBox*          settings_listener_port_edit;
		QLabel*            settings_listener_port_error;
		QPushButton*       settings_listener_set_default;
		QPushButton*       settings_listener_use_now;
		QList<QTcpSocket*> sockets_connected;
		QList<QTcpSocket*> connections_pending;
		quint16            listener_port;
};

#endif
