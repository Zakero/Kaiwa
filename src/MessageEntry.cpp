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
#include <stdio.h>

// Qt
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTime>

// Local
#include "ChatEntry.h"

/******************************************************************************
 * Active Namespaces
 */


/******************************************************************************
 * Functions: Public
 */


/******************************************************************************
 * Functions: Private
 */


/******************************************************************************
 * Class Variables: ChatEntry
 */

/******************************************************************************
 * Class Methods: ChatEntry
 */

/**
 * \internal
 * Establish Signal/Slot connections and initial widget state.
 */
ChatEntry::ChatEntry(QWidget* parent)
	: QWidget(parent)
	, line_edit(new QLineEdit())
	, send_button(new QPushButton(tr("Send")))
{
	connect(
		line_edit, SIGNAL(textChanged(const QString&)),
		this, SLOT(enableSendButton())
		);
	connect(
		line_edit, SIGNAL(returnPressed()),
		this, SLOT(send())
		);

	send_button->setEnabled(false);
	connect(
		send_button, SIGNAL(clicked()),
		this, SLOT(send())
		);

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(line_edit);
	layout->addWidget(send_button);

	this->setLayout(layout);
}

/**
 * \internal
 * The following conditions must be meet in-order to enable the send 
 * send_button:
 * - The `line_edit` is not empty
 * - The `line_edit` contains something other than whitespace
 */
void ChatEntry::enableSendButton()
{
	if(line_edit->text().trimmed().isEmpty())
	{
		send_button->setEnabled(false);
		return;
	}

	send_button->setEnabled(true);

	return;
}

/**
 * \internal
 * There is more to sending a message than just data collection:
 * - Get the current text from `line_edit`.
 * - Get the current time.
 * - Generate the "send" signal.
 * - Clear the data from `line_edit`
 * - Disable the `send_button`
 */
void ChatEntry::send()
{
	if(send_button->isEnabled() == false)
	{
		return;
	}

	send_button->setEnabled(false);

	QString message = line_edit->text();
	line_edit->clear();

	emit send(QTime::currentTime(), message);
}

/**
 * \internal
 * Pass the string to `line_edit`.
 */
void ChatEntry::setText(const QString& string)
{
	line_edit->setText(string);
}
