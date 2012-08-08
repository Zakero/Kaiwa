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
	/*
	connect(
		this, SLOT(setText(const QString&)),
		line_edit, SLOT(setText(const QString&))
		);
	*/

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
 */
void ChatEntry::enableSendButton()
{
	if(line_edit->text().isEmpty())
	{
		return;
	}

	send_button->setEnabled(true);

	return;
}

/**
 * \internal
 * There is more to sending a message than just data collection:
 * - Get the current text from `line_edit`.
 * - [TODO] Get the current time.
 * - [TODO] Generate the "send" signal.
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

	printf("%s\n", line_edit->text().toStdString().c_str());
	line_edit->clear();
}

/**
 * \internal
 * Pass the string to `line_edit`.
 */
void ChatEntry::setText(const QString& string)
{
	line_edit->setText(string);
}
