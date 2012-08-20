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
#include <QAbstractItemView>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QString>
//#include <QTableWidget>
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
,	TimeReceived
,	UserName
,	MessageText
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
	, table()
	, max_rows(5)
{
	table.setColumnCount(ColumnCount);
	table.verticalHeader()->setVisible(false);
	table.resizeColumnsToContents();
	table.setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	table.setShowGrid(false);

	table.setHorizontalHeaderItem(Index
		, new QTableWidgetItem("#")
		);
	table.setHorizontalHeaderItem(TimeSent
		, new QTableWidgetItem("Time Sent")
		);
	table.setHorizontalHeaderItem(TimeReceived
		, new QTableWidgetItem("Time Received")
		);
	table.setHorizontalHeaderItem(UserName
		, new QTableWidgetItem("User")
		);
	table.setHorizontalHeaderItem(MessageText
		, new QTableWidgetItem("Message")
		);

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(&table);
	setLayout(layout);
}

/**
 * \internal
 * Individual columns have the following properties based on name
 * - column index
 * - is visible
 * - flags (editable, dragable, etc..able)
 * - alignment
 */
void MessageView::addMessage(const Message& message)
{
	static Qt::ItemFlags flags[] =
	{	Qt::NoItemFlags    // Index
	,	Qt::ItemIsEnabled  // TimeSent
	,	Qt::ItemIsEnabled  // TimeReceived
	,	Qt::ItemIsEnabled  // UserName
	,	Qt::ItemIsEnabled  // MessageText
		| Qt::ItemIsSelectable
		| Qt::ItemIsDragEnabled
	};

	int row = table.rowCount();

	table.insertRow(row);

	table.setItem(row, Index, new QTableWidgetItem(QString::number(row + 1)));
	table.item(row, Index)->setFlags(flags[Index]);
	table.item(row, Index)->setTextAlignment(Qt::AlignRight | Qt::AlignTop);

	table.setItem(row, TimeSent, new QTableWidgetItem(message.createdDateTime().time().toString()));
	table.item(row, TimeSent)->setFlags(flags[TimeSent]);
	table.item(row, TimeSent)->setTextAlignment(Qt::AlignHCenter | Qt::AlignTop);

	table.setItem(row, TimeReceived, new QTableWidgetItem(message.receivedDateTime().time().toString()));
	table.item(row, TimeReceived)->setFlags(flags[TimeReceived]);
	table.item(row, TimeReceived)->setTextAlignment(Qt::AlignHCenter | Qt::AlignTop);

	table.setItem(row, UserName, new QTableWidgetItem(message.username()));
	table.item(row, UserName)->setFlags(flags[UserName]);
	table.item(row, UserName)->setTextAlignment(Qt::AlignRight | Qt::AlignTop);

	table.setItem(row, MessageText, new QTableWidgetItem(message.text()));
	table.item(row, MessageText)->setFlags(flags[MessageText]);
	table.item(row, MessageText)->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);

	table.resizeColumnsToContents();
	table.resizeRowsToContents();

	table.scrollToBottom();

	return;
}

/**
 * \internal
 */
void MessageView::setHistorySize(int message_count)
{
	message_count = message_count;
}
