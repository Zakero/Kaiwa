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

#ifndef Message_H
#define Message_H

/******************************************************************************
 * Defines
 */


/******************************************************************************
 * Includes
 */
//ANSI/POSIX

// Qt
#include <QByteArray>
#include <QDateTime>
#include <QString>

// Local


/******************************************************************************
 * Namespaces
 */


/******************************************************************************
 * Forward Declarations
 */


/******************************************************************************
 * Typedefs
 */


/******************************************************************************
 * Structures
 */


/******************************************************************************
 * Classes
 */
class Message
{
	public:
		Message();
		static bool fromByteArray(QByteArray&, Message&);

		void init(const QString&, const QString&);
		bool isValid() const;

		const QDateTime& createdDateTime() const;
		const QString& username() const;
		const QString& text() const;
		QByteArray toByteArray() const;

		void debug() const;

	private:
		void init(const qint64, const QString&, const QString&);

		static const char MAGIC;

		QDateTime date_time;
		QString user_name;
		QString message;
};

#endif
