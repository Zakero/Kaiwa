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

#include <QApplication>
#include <QVBoxLayout>

#include "MessageEntry.h"
#include "MessageView.h"

int main(int arc, char** argv)
{
	QApplication app(arc, argv);
	
	MessageEntry* chat_entry = new MessageEntry();
	MessageView* chat_view = new MessageView();

	QObject::connect(
		chat_entry, SIGNAL(send(const QTime&, const QString&)),
		chat_view, SLOT(addMessage(const QTime&, const QString&))
		);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(chat_view);
	layout->addWidget(chat_entry);

	QWidget* window = new QWidget();
	window->setLayout(layout);
	window->adjustSize();
	window->show();

	return app.exec();
}
