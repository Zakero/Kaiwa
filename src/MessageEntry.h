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

#ifndef MESSAGEENTRY_H
#define MESSAGEENTRY_H

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
#include "Message.h"


/******************************************************************************
 * Namespaces
 */


/******************************************************************************
 * Forward Declarations
 */
class QLineEdit;
class QPushButton;


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
 * \brief A widget to entry chat messages.
 *
 * This Widget consists of 2 parts, the text entry and the "send" button.
 * \code
 * +-----------------------------+
 * | +----------------+ +------+ |
 * | | Text Entry     | | Send | |
 * | +----------------+ +------+ |
 * +-----------------------------+
 * \endcode
 */
class MessageEntry
	: public QWidget
{
	Q_OBJECT

	public:
		/**
		 * \brief Constructor
		 *
		 * Create a new instance of the MessageEntry Widget.
		 */
		MessageEntry(QWidget* parent = 0 //!< The parent widget.
			);

	signals:
		/**
		 * \brief Send the entered message.
		 */
		void send(const Message& //!< The message.
			);

	public slots:
		/**
		 * \brief Set the text of this widget.
		 *
		 * The contents of the text entry will be set to the provided 
		 * string.  The current contents will be lost.
		 */
		void setText(const QString& //!< The new text.
			);

	private slots:
		/**
		 * \brief Enable the Send Button.
		 */
		void enableSendButton();

		/**
		 * \brief A message is ready to be sent.
		 */
		void send();

	private:
		QLineEdit* line_edit;
		QPushButton* send_button;
};

#endif
