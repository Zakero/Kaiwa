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
#include <QHostAddress>
#include <QPushButton>
#include <QVBoxLayout>

// Local
#include "Kaiwa.h"
#include "MessageEntry.h"
#include "MessageView.h"
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


/******************************************************************************
 * Class Variables: Kaiwa
 */


/******************************************************************************
 * Class Methods: Kaiwa
 */

Kaiwa::Kaiwa()
	: QMainWindow()
	, message_entry(new MessageEntry())
	, message_view(new MessageView())
	, network(new Network(this))
{
	connect(
		message_entry, SIGNAL(send(const Message&)),
		message_view, SLOT(addMessage(const Message&))
		);
	connect(
		message_entry, SIGNAL(send(const Message&)),
		network, SLOT(sendMessage(const Message&))
		);
	connect(
		network, SIGNAL(recievedMessage(const Message&)),
		message_view, SLOT(addMessage(const Message&))
		);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(message_view, 1);
	layout->addWidget(message_entry);

	QWidget* message_area = new QWidget();
	message_area->setLayout(layout);

	setCentralWidget(message_area);

	network->listen(QHostAddress("127.0.0.1"), 0xCAFE);

	if(network->isListening() == false)
	{
		QPushButton* button = new QPushButton("Connect");
		connect(button, SIGNAL(clicked()), this, SLOT(makeConnection()));

		setMenuWidget(button);
	}
}

void Kaiwa::makeConnection()
{
	network->connectTo(QHostAddress("127.0.0.1"), 0xCAFE);
}

QString Kaiwa::username()
{
	return QString(getlogin());
}
