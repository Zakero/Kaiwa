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
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QString>
#include <QTableWidget>
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
 */
MessageView::MessageView(QWidget* parent)
	: QWidget(parent)
	, table(new QTableWidget())
{
	this->table->setColumnCount(2);
	this->table->verticalHeader()->setVisible(false);

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(this->table);
	this->setLayout(layout);
}

/**
 * \internal
 */
void MessageView::addMessage(const QTime& time, const QString& message)
{
	int row = table->rowCount();

	table->insertRow(row);
	table->setCellWidget(row, 0, new QLabel(time.toString()));
	table->setCellWidget(row, 1, new QLabel(message));
	table->resizeColumnsToContents();

	table->scrollToItem(table->item(row, 0));

	return;
}

/**
 * \internal
 */
void MessageView::setHistorySize(int message_count)
{
	message_count = message_count;
}
