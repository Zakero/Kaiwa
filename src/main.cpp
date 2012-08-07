#include <QApplication>

#include "ChatEntry.h"

int main(int arc, char** argv)
{
	QApplication app(arc, argv);
	
	ChatEntry chat_entry;
	chat_entry.show();

	return app.exec();
}
