#ifndef CHATENTRY_H
#define CHATENTRY_H

#include <QWidget>

class QLineEdit;
class QPushButton;
//class QString;
//class QTime;

class ChatEntry
	: public QWidget
{
	Q_OBJECT

	public:
		ChatEntry(QWidget* parent = 0);

	/*
	signals:
		void send(const QTime&, const QString&);

	*/
	private slots:
		void enableButton();
		void send();
		void setText(const QString&);

	private:
		QLineEdit* line_edit;
		QPushButton* button;
};

#endif
