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
#include <QHeaderView>
#include <QLabel>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTime>

// Local
#include "MessageView.h"


/******************************************************************************
 * Active Namespaces
 */


/******************************************************************************
 * Global Variables
 */
enum ColumnNames
{	Index
,	TimeSent
,	UserName
,	Message
,	ColumnCount
};


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
	, max_rows(5)
{
	this->table->setColumnCount(ColumnCount);
	this->table->verticalHeader()->setVisible(false);

	this->table->setHorizontalHeaderItem(Index
		, new QTableWidgetItem("#")
		);

	this->table->setHorizontalHeaderItem(TimeSent
		, new QTableWidgetItem("Sent")
		);
	this->table->setHorizontalHeaderItem(UserName
		, new QTableWidgetItem("UserName")
		);
	this->table->setHorizontalHeaderItem(Message
		, new QTableWidgetItem("Message")
		);

	table->resizeColumnsToContents();

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(this->table);
	this->setLayout(layout);
}

/**
 * \internal
 * Individual columns have the following properties based on name
 * - column index
 * - is visible
 * - flags (editable, dragable, etc..able)
 */
void MessageView::addMessage(const QTime& time, const QString& username, const QString& message)
{
	static Qt::ItemFlags flags[] =
	{	Qt::NoItemFlags    // Index
	,	Qt::ItemIsEnabled  // TimeSent
	,	Qt::ItemIsEnabled  // UserName
	,	Qt::ItemIsEnabled  // Message
		| Qt::ItemIsSelectable
		| Qt::ItemIsDragEnabled
	};

	int row = table->rowCount();

	table->insertRow(row);

	table->setItem(row, Index
		, new QTableWidgetItem(
			QString::number(row)
			)
		);
	table->item(row, 0)->setFlags(flags[Index]);

	// Set Margin...
	QLabel* label;
	label = new QLabel(time.toString());
	label->setMargin(3);
	table->setCellWidget(row, TimeSent, label);

	table->setItem(row, UserName, new QTableWidgetItem(username));
	table->item(row, UserName)->setFlags(flags[UserName]);

	table->setItem(row, Message, new QTableWidgetItem(message));
	table->item(row, Message)->setFlags(flags[Message]);

	table->resizeColumnsToContents();
	table->resizeRowsToContents();

	table->scrollToItem(table->item(row, Index));

	return;
}

/**
 * \internal
 */
void MessageView::setHistorySize(int message_count)
{
	message_count = message_count;
}
