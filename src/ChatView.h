#ifndef CHATVIEW_H
#define CHATVIEW_H

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
class QTextEdit;


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
 * - Time [TODO]
 *   When the message was sent (not recieved).
 * - User [TODO]
 *   The name of the user that sent the message.
 * - Message
 *   The message that was sent.
 */
class ChatView
	: public QWidget
{
	Q_OBJECT

	public:
		/**
		 * \brief Constructor
		 *
		 * Create a new instance of the ChatView Widget.
		 */
		ChatView(QWidget* = 0 //!< The parent widget.
			);

	signals:

	public slots:
		void addMessage(const QTime&, const QString&);

		/**
		 * \brief Determine how many message to hold.
		 *
		 * The contents of the text entry will be set to the provided 
		 * string.  The current contents will be lost.
		 */
		void setHistorySize(int = 100 //!< The number messages
			);

	private slots:

	private:
		QTextEdit* text_edit;
};

#endif
