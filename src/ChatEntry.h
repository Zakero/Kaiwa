#ifndef CHATENTRY_H
#define CHATENTRY_H

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
class ChatEntry
	: public QWidget
{
	Q_OBJECT

	public:
		/**
		 * \brief Constructor
		 *
		 * Create a new instance of the ChatEntry Widget.
		 */
		ChatEntry(QWidget* parent = 0 //!< The parent widget.
			);

	/*
	signals:
		void send(const QTime&, const QString&);

	*/
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
		QPushButton* button;
};

#endif
