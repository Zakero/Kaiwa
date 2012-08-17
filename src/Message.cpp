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

// Local
#include "Message.h"


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
 * Class Variables: Message
 */
const char Message::MAGIC = 1;


/******************************************************************************
 * Class Methods: Message
 */
Message::Message()
	: date_time()
	, user_name()
	, message()
{
}

void Message::init(const QString& user, const QString& text)
{
	if(isValid())
	{
		// Don't init a valid message;
		return;
	}

	init(QDateTime::currentMSecsSinceEpoch(), user, text);
}

void Message::init(const qint64 msecs, const QString& user, const QString& text)
{
	date_time.setMSecsSinceEpoch(msecs);

	user_name.clear();
	user_name.append(user);

	message.clear();
	message.append(text);
}

bool Message::isValid() const
{
	return date_time.isValid();
}

const QDateTime& Message::createdDateTime() const
{
	return date_time;
}

const QString& Message::text() const
{
	return message;
}

const QString& Message::username() const
{
	return user_name;
}

QByteArray Message::toByteArray() const
{
	QByteArray byte_array;

	byte_array.append(Message::MAGIC);
	qint64 mask = 0x00ff;
	qint64 msecs = date_time.toMSecsSinceEpoch();
	for(unsigned int i = 0; i < sizeof(qint64); i++)
	{
		int s = i * 8;
		char c = ((msecs & (mask << s)) >> s);
		byte_array.append(c);
	}

	byte_array.append(user_name).append('\0');
	byte_array.append(message).append('\0');

	return byte_array;
}

bool Message::fromByteArray(QByteArray& byte_array, Message& message)
{
	if(byte_array.at(0) != Message::MAGIC
		|| message.isValid())
	{
		return false;
	}

	byte_array.remove(0, sizeof(Message::MAGIC) + 0);

	qint64 mask = 0x00ff;
	qint64 msecs = 0;
	for(unsigned int i = 0; i < sizeof(qint64); i++)
	{
		int s = i * 8;
		qint64 c = byte_array.at(i) & mask;
		msecs |= (c << s);
	}
	byte_array.remove(0, sizeof(qint64));

	QString user_name(byte_array);
	byte_array.remove(0, user_name.size() + 1);

	QString text(byte_array);
	byte_array.remove(0, text.size() + 1);

	message.init(msecs, user_name, text);

	return true;
}

void Message::debug() const
{
	printf("%d Message\n{\n", getpid());
	printf("\tDateTime: %s\n", date_time.toString().toStdString().c_str());
	printf("\tUserName: %s\n", user_name.toStdString().c_str());
	printf("\tText: %s\n", message.toStdString().c_str());
	printf("}\n");
}
