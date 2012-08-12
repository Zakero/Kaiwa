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

#ifndef MESSAGEVIEW_H
#define MESSAGEVIEW_H

/******************************************************************************
 * Defines
 */


/******************************************************************************
 * Includes
 */
//ANSI/POSIX

// Qt
#include <QWidget>

// Local

/******************************************************************************
 * Namespaces
 */


/******************************************************************************
 * Forward Declarations
 */
class QTableWidget;


/******************************************************************************
 * Typedefs
 */


/******************************************************************************
 * Structures
 */


/******************************************************************************
 * Classes
 */

/**
 * \brief A widget to display chat messages.
 *
 * Display chat messages as well as information about the messages.  This will 
 * be accomplished by using 3 columns:
 * - Sent Time
 *   When the message was sent.
 * - Recieved Time [TODO]
 *   When the message was recieved.
 * - User
 *   The name of the user that sent the message.
 * - Message
 *   The message that was sent.
 */
class MessageView
	: public QWidget
{
	Q_OBJECT

	public:
		/**
		 * \brief Constructor
		 *
		 * Create a new instance of the MessageView Widget.
		 */
		MessageView(QWidget* = 0 //!< The parent widget.
			);

	signals:

	public slots:
		/**
		 * \brief Send the entered message.
		 */
		void addMessage(const QTime& //!< When the message was created.
			, const QString&     //!< The Username.
			, const QString&     //!< The message.
			);

		/**
		 * \brief Determine how many message to hold.
		 *
		 * The contents of the text entry will be set to the provided 
		 * string.  The current contents will be lost.
		 */
		void setHistorySize(int = 5 //!< The number messages
			);

	private slots:

	private:
		QTableWidget* table;
		int max_rows;
};

#endif
