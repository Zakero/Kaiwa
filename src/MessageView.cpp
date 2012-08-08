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
#include <QChar>
#include <QString>
#include <QTextEdit>
#include <QTime>

// Local
#include "MessageView.h"

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
 * Class Variables: MessageView
 */

/******************************************************************************
 * Class Methods: MessageView
 */

/**
 * \internal
 * Establish Signal/Slot connections and initial widget state.
 */
MessageView::MessageView(QWidget* parent)
	: QWidget(parent)
	, text_edit(new QTextEdit(this))
{
}

/**
 * \internal
 */
void MessageView::addMessage(const QTime& time, const QString& message)
{
	QString string;

	string.append(time.toString()).append(" - ").append(message);
	text_edit->append(string);

	return;
}

/**
 * \internal
 */
void MessageView::setHistorySize(int message_count)
{
}
