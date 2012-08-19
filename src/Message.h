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

/**
 * \brief A message.
 *
 * A Message is a simple text message that can be sent to another Kaiwa 
 * application.  The contents of a Message are;
 * - When it was created.
 * - Who created it.
 * - The text.
 */
class Message
{
	public:
		/**
		 * \brief Constructor.
		 *
		 * Create a new instance of a Message.  The Message will be in 
		 * an "invalid" state until it has been initialized.
		 *
		 * \see init()
		 * \see isValid()
		 */
		Message();

		/**
		 * \name Intialization
		 * \{
		 */
		/**
		 * \brief Initialize the Message.
		 *
		 * Use the provided username and text to initialize this 
		 * Message.  The current time will be used for "created" time 
		 * stamp.
		 *
		 * If the Message has already been initialized, then calling 
		 * this method will not do anything.
		 *
		 * \retval  true  The Message is initialized.
		 * \retval  false  The Message was not initialized.
		 */
		bool init(const QString&  //!< The user name.
			, const QString&  //!< The message text.
			);

		/**
		 * \brief Initialize the Message.
		 *
		 * Use the provided QByteArray to initialize this Message.  If 
		 * an int pointer is provided, the number of bytes that are 
		 * read from the QByteArray will be placed in the pointer.
		 *
		 * Reconstructing a Message from a QByteArray will set the 
		 * received time stamp as well.
		 *
		 * If the Message has already been initialized, then calling 
		 * this method will not do anything.  Also, if the QByteArray 
		 * is not properly formatted, this method will not do anything.
		 *
		 * \retval  true  The Message is initialized.
		 * \retval  false  The Message was not initialized.
		 *
		 * \see toByteArray()
		 */
		bool init(const QByteArray&  //!< The QByteArray of data.
			, int* = 0           //!< The number of bytes read.
			);

		/**
		 * \}
		 */

		/**
		 * \name Contents
		 * \{
		 */
		/**
		 * \brief The creation Date and Time.
		 *
		 * This method will return the Date and Time of when the 
		 * message was initialized with data.
		 *
		 * \return  The QDateTime.
		 */
		const QDateTime& createdDateTime() const;

		/**
		 * \brief The user that created the Message.
		 *
		 * \return  The username.
		 */
		const QString& username() const;

		/**
		 * \brief The contents of the message.
		 *
		 * \return  The message text.
		 */
		const QString& text() const;

		/**
		 * \brief The received Date and Time.
		 *
		 * This method will return the Date and Time of when the 
		 * message was received.  If the message has not been received 
		 * then an invalid QDateTime object will be returned.
		 *
		 * \return The QDateTime.
		 */
		const QDateTime& receivedDateTime() const;
		/**
		 * \}
		 */

		/**
		 * \name State
		 * \{
		 */
		/**
		 * \brief Has this Message been initialized?
		 *
		 * \retval  true  The Message has been initialized.
		 * \retval  false  The Message has not been initialized.
		 */
		bool isValid() const;

		/**
		 * \}
		 */

		/**
		 * \name Conversion
		 * \{
		 */
		/**
		 * \brief Convert to a QByteArray.
		 *
		 * The internal data of this Message will be appended to the 
		 * provided QByteArray.
		 *
		 * \retval  The provided QByteArray.
		 */
		QByteArray toByteArray(QByteArray& //!< Used to store data.
			) const;

		/**
		 * \}
		 */

		/**
		 * \brief Dump the Message to stdout.
		 *
		 * The internal data of this Message will be printed to 
		 * <code>stdout</code> in JSON format.
		 */
		void debug() const;

	private:
		/**
		 * \brief Initialize the Message.
		 */
		bool init(const qint64    //!< Milliseconds from Epoch.
			, const QString&  //!< The user name.
			, const QString&  //!< The message text.
			);

		static const char MAGIC; //!< Used for QByteArray conversion

		QDateTime created;  //!< When the message was created.
		QDateTime received; //!< When the message was created.
		QString user_name;  //!< Who created the message.
		QString message;    //!< The contents of the message.
};

#endif
