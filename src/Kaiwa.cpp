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
#include <QSettings>
#include <QTabWidget>
#include <QToolBox>
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
const char* toStr(const QString& q_string)
{
	return q_string.toStdString().c_str();
}


/******************************************************************************
 * Functions: Private
 */


/******************************************************************************
 * Class Variables: Kaiwa
 */


/******************************************************************************
 * Class Methods: Kaiwa
 */

Kaiwa::Kaiwa(QWidget* parent)
	: QMainWindow(parent)
	, message_entry(new MessageEntry())
	, message_view(new MessageView())
	, network(this)
{
	QToolBox* tool_box = new QToolBox();

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(message_view, 1);
	layout->addWidget(message_entry);

	QWidget* message_area = new QWidget();
	message_area->setLayout(layout);
	/*
	QTabWidget* channels = new QTabWidget();
	channels->setTabPosition(QTabWidget::West);
	channels->setTabShape(QTabWidget::Triangular);
	channels->addTab(message_area,  "Foo");
	channels->addTab(new QWidget(), "Bar");
	channels->addTab(new QWidget(), "Blah");
	tool_box->addItem(channels, "Channels");
	*/
	tool_box->addItem(message_area, "Messages");

	QTabWidget* settings = new QTabWidget();
	settings->setTabPosition(QTabWidget::West);
	//settings->setTabShape(QTabWidget::Triangular);
	//settings->addTab(new QWidget(), "User");
	settings->addTab(network.settings(), "Network");
	//settings->addTab(new QWidget(), "Bots");
	//settings->addTab(new QWidget(), "About");
	tool_box->addItem(settings, "Settings");
	setCentralWidget(tool_box);

	connect(
		message_entry, SIGNAL(send(const Message&)),
		message_view, SLOT(addMessage(const Message&))
		);
	connect(
		message_entry, SIGNAL(send(const Message&)),
		&network, SLOT(sendMessage(const Message&))
		);
	connect(
		&network, SIGNAL(recievedMessage(const Message&)),
		message_view, SLOT(addMessage(const Message&))
		);

	/*
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(message_view, 1);
	layout->addWidget(message_entry);

	QWidget* message_area = new QWidget();
	message_area->setLayout(layout);

	setCentralWidget(message_area);

	if(network->isListening() == false)
	{
		QPushButton* button = new QPushButton("Connect");
		connect(button, SIGNAL(clicked()), this, SLOT(makeConnection()));

		setMenuWidget(button);
	}
	*/
}

void Kaiwa::makeConnection()
{
	network.connectTo(QHostAddress("127.0.0.1"), 0xCAFE);
}

QString Kaiwa::username()
{
	return QString(getlogin());
}

/******************************************************************************
 * Application Values
 */

/**
 * \internal
 *
 */
QVariant Kaiwa::getValue(const QString& section, const QString& group, const QString& key, const QVariant& default_value)
{
	QSettings settings("Kaiwa");

	settings.beginGroup(section);
	settings.beginGroup(group);

	QVariant value = settings.value(key, default_value);

	settings.endGroup();
	settings.endGroup();

	return value;
}

/**
 * \internal
 *
 */
void Kaiwa::putValue(const QString& section, const QString& group, const QString& key, const QVariant& value)
{
	QSettings settings("Kaiwa");

	settings.beginGroup(section);
	settings.beginGroup(group);

	settings.setValue(key, value);

	settings.endGroup();
	settings.endGroup();
}

/**
 * \internal
 *
 */
void Kaiwa::removeValue(const QString& section, const QString& group, const QString& key)
{
	QSettings settings("Kaiwa");

	settings.beginGroup(section);
	settings.beginGroup(group);

	settings.remove(key);

	settings.endGroup();
	settings.endGroup();
}

/**
 * \internal
 *
 */
void Kaiwa::clearCommandLineValues()
{
	QSettings settings("Kaiwa");

	settings.beginGroup("Command-Line");
	settings.remove("");
	settings.endGroup();
}

/**
 * \internal
 *
 */
QVariant Kaiwa::getCommandLineValue(const QString& section, const QString& group, const QString& key, const QVariant& default_value)
{
	QSettings settings("Kaiwa");

	settings.beginGroup("Command-Line");
	settings.beginGroup(section);
	settings.beginGroup(group);

	QVariant value = settings.value(key, default_value);

	settings.endGroup();
	settings.endGroup();
	settings.endGroup();

	return value;
}

/**
 * \internal
 *
 */
void Kaiwa::putCommandLineValue(const QString& section, const QString& group, const QString& key, const QVariant& value)
{
	QSettings settings("Kaiwa");

	settings.beginGroup("Command-Line");
	settings.beginGroup(section);
	settings.beginGroup(group);

	settings.setValue(key, value);

	settings.endGroup();
	settings.endGroup();
	settings.endGroup();
}
