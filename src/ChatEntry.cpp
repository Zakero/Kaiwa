#include <stdio.h>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTime>

#include "ChatEntry.h"

ChatEntry::ChatEntry(QWidget* parent)
	: QWidget(parent)
	, line_edit(new QLineEdit())
	, button(new QPushButton(tr("Send")))
{
	connect(
		line_edit, SIGNAL(textChanged(const QString&)),
		this, SLOT(enableButton())
		);
	connect(
		line_edit, SIGNAL(returnPressed()),
		this, SLOT(send())
		);
	connect(
		this, SLOT(setText(const QString&)),
		line_edit, SLOT(setText(const QString&))
		);

	button->setEnabled(false);
	connect(
		button, SIGNAL(clicked()),
		this, SLOT(send())
		);

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(line_edit);
	layout->addWidget(button);

	this->setLayout(layout);
}

void ChatEntry::enableButton()
{
	if(line_edit->text().isEmpty())
	{
		return;
	}

	button->setEnabled(true);

	return;
}

void ChatEntry::send()
{
	printf("%s\n", line_edit->text().toStdString().c_str());

	button->setEnabled(false);
	line_edit->clear();
}

void ChatEntry::setText(const QString& string)
{
	line_edit->setText(string);
}
