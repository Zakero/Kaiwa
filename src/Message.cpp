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

/**
 * \internal
 * The constructor does nothing to set values to it's members.  This is for API 
 * consistency.
 * 
 * The user_name and message could be passed to the constructor and the 
 * created could be set with out any problems.  However, passing a QByteArray 
 * would be problematic because what happens if the QByteArray is not valid?  
 * What happens to the Message, because it is now in an unknown state.
 *
 * By separating the object construction from the object initialization, this 
 * problem becomes an non-issue.
 */
Message::Message()
	: created()
	, received()
	, user_name()
	, message()
{
}

/**
 * \internal
 *
 * \see init(const qint64, const QString&, const QString&)
 */
bool Message::init(const QString& user, const QString& text)
{
	if(isValid())
	{
		// Don't init a valid message;
		return false;
	}

	return init(QDateTime::currentMSecsSinceEpoch(), user, text);
}

/**
 * \internal
 * A copy of the byte_array is made so that the original remains intact.  We 
 * don't know what the caller intends to do with the byte_array after we are 
 * done with it, so we can't assume that we can do what we want with it.
 *
 * The order of the ByteArray data is:
 * -# QDateTime (in milli-seconds from the Epoch)
 * -# QString of the username terminated by a '\0'.
 * -# QString of the message text terminated by a '\0'.
 *
 * \see init(const qint64, const QString&, const QString&)
 */
bool Message::init(const QByteArray& byte_array, int* bytes_read)
{
	if(byte_array.at(0) != Message::MAGIC || isValid())
	{
		return false;
	}

	QByteArray data(byte_array);

	/**
	 * \{
	 * \internal
	 * Read in the time stamp value (milli-secs) 1 byte at a time in order 
	 * to reconstruct the original value.
	 */
	int len = 0;
	data.remove(0, sizeof(Message::MAGIC));
	len += sizeof(Message::MAGIC);

	qint64 mask = 0x00ff;
	qint64 msecs = 0;
	for(unsigned int i = 0; i < sizeof(qint64); i++)
	{
		int s = i * 8;
		qint64 c = data.at(i) & mask;
		msecs |= (c << s);
	}
	data.remove(0, sizeof(qint64));
	len += sizeof(qint64);
	/**
	 * \}
	 */

	QString user_name(data);
	data.remove(0, user_name.size() + 1);
	len += user_name.size() + 1;

	QString text(data);
	data.remove(0, text.size() + 1);
	len += text.size() + 1;

	if(bytes_read != 0)
	{
		*bytes_read = len;
	}

	received.setMSecsSinceEpoch(QDateTime::currentMSecsSinceEpoch());

	return init(msecs, user_name, text);
}

/**
 * \internal
 * Update the internal data using the provided values.
 *
 * \bug Values are not checked for sanity.  An empty user or text QString 
 * should not be allowed.  The UI currently does some checks, but this should 
 * not be relied upon because what if the data came from a faulty UI or a 
 * QByteArray.
 */
bool Message::init(const qint64 msecs, const QString& user, const QString& text)
{
	created.setMSecsSinceEpoch(msecs);

	user_name.clear();
	user_name.append(user);

	message.clear();
	message.append(text);

	return true;
}

/**
 * \internal
 * An empty QDateTime is invalid.  Use this to determine if the Message has 
 * been initialized or not.
 */
bool Message::isValid() const
{
	return created.isValid();
}

/**
 * \internal
 * No need to supply createDate() and createTime() methods because those 
 * objects can be provided by the QDateTime object.
 */
const QDateTime& Message::createdDateTime() const
{
	return created;
}

/**
 * \internal
 * "Received" actually means when the Message was initialized from a 
 * QByteArray.
 * 
 * \see init(const QByteArray&, int*)
 */
const QDateTime& Message::receivedDateTime() const
{
	return received;
}

/**
 * \internal
 * Use the same API convention as Qt, like QLineText.
 */
const QString& Message::text() const
{
	return message;
}

/**
 * \internal
 * 
 */
const QString& Message::username() const
{
	return user_name;
}

/**
 * \internal
 * The order of the ByteArray data is:
 * -# QDateTime (in milli-seconds from the Epoch)
 * -# QString of the username terminated by a '\0'.
 * -# QString of the message text terminated by a '\0'.
 *
 * Storing the QDateTime milli-second value requires a bit of work.  The value 
 * is writen in Least-Significant byte order first.  So a value of 0xAABBCCDD 
 * will be added to the QByteArray as:
 * \code
 * byte_array.append(0xDD);
 * byte_array.append(0xCC);
 * byte_array.append(0xBB);
 * byte_array.append(0xAA);
 * \endcode
 *
 * This provides the following benefits:
 * - Maintaining the bit-mask is easier
 * - No change to the process if the size of the value changes
 * - Reconstructing the value is very simple (shift and add)
 */
QByteArray Message::toByteArray(QByteArray& byte_array) const
{
	byte_array.append(Message::MAGIC);
	qint64 mask = 0x00ff;
	qint64 msecs = created.toMSecsSinceEpoch();
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

/**
 * \internal
 * 
 */
void Message::debug() const
{
	printf("%d Message\n{\n", getpid());
	printf("\tCreated: %s\n", created.toString().toStdString().c_str());
	printf("\tUserName: %s\n", user_name.toStdString().c_str());
	printf("\tText: %s\n", message.toStdString().c_str());
	printf("}\n");
}
