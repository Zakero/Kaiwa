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
#include <QVBoxLayout>

// Local
#include "Kaiwa.h"
#include "MessageEntry.h"
#include "MessageView.h"


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
{
	connect(
		message_entry, SIGNAL(send(const QTime&, const QString&)),
		message_view, SLOT(addMessage(const QTime&, const QString&))
		);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(message_view, 1);
	layout->addWidget(message_entry);

	QWidget* message_area = new QWidget();
	message_area->setLayout(layout);

	setCentralWidget(message_area);
}