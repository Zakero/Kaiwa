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

#ifndef KAIWA_H
#define KAIWA_H

/******************************************************************************
 * Defines
 */


/******************************************************************************
 * Includes
 */
//ANSI/POSIX

// Qt
#include <QMainWindow>
#include <QVariant>

// Local
#include "Network.h"


/******************************************************************************
 * Namespaces
 */


/******************************************************************************
 * Forward Declarations
 */

class MessageEntry;
class MessageView;


/******************************************************************************
 * Typedefs
 */


/******************************************************************************
 * Structures
 */

/**
 * \deprecated
 * This function is for debugging purposes only.
 */
const char* toStr(const QString& //!< The QString to convert.
	);

/******************************************************************************
 * Classes
 */

class Kaiwa
	: public QMainWindow
{
	Q_OBJECT

	public:
		/**
		 * \brief Constructor.
		 */
		Kaiwa();

		/**
		 * \brief The user name.
		 *
		 * \retval  The name of the user.
		 */
		static QString username();

		/**
		 * \brief Get the requested settings.
		 *
		 * The Kaiwa application settings will be checked for the 
		 * requested values.  If the Section, Group, Key is not found, 
		 * then the provided default value is returned.  If the value 
		 * was found in the "temporary" settings, then that value will 
		 * be removed.
		 */
		static QVariant getSetting(const QString& //!< Section
			, const QString&                  //!< Group
			, const QString&                  //!< Key
			, const QVariant& = QVariant()    //!< Default Value
			);

	private slots:
		/**
		 * \brief Make a network connection.
		 */
		void makeConnection();

	private:
		MessageEntry* message_entry;
		MessageView* message_view;
		Network network;
};
#endif
