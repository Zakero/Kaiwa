/******************************************************************************
 * Defines
 */


/******************************************************************************
 * Includes
 */
// ANSI/POSIX
#include <stdio.h>

// Qt
#include <QChar>
#include <QString>
#include <QTextEdit>
#include <QTime>

// Local
#include "ChatView.h"

/******************************************************************************
 * Active Namespaces
 */


/******************************************************************************
 * Functions: Public
 */


/******************************************************************************
 * Functions: Private
 */


/******************************************************************************
 * Class Variables: ChatView
 */

/******************************************************************************
 * Class Methods: ChatView
 */

/**
 * \internal
 * Establish Signal/Slot connections and initial widget state.
 */
ChatView::ChatView(QWidget* parent)
	: QWidget(parent)
	, text_edit(new QTextEdit(this))
{
}

/**
 * \internal
 */
void ChatView::addMessage(const QTime& time, const QString& message)
{
	QString string;

	string.append(time.toString()).append(" - ").append(message);
	text_edit->append(string);

	return;
}

/**
 * \internal
 */
void ChatView::setHistorySize(int message_count)
{
}
