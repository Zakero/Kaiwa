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

/******************************************************************************
 * Defines
 */


/******************************************************************************
 * Includes
 */
// ANSI/POSIX
#include <stdio.h>
#include <string.h>

// Qt
#include <QApplication>
#include <QSettings>
#include <QString>
#include <QStringList>

// Local
#include "Kaiwa.h"


/******************************************************************************
 * Namespaces
 */


/******************************************************************************
 * Functions: Private
 */

/**
 * \brief Parse the command-line arguments.
 *
 * The command-line args will be parsed and the values will be placed in 
 * "temporary" settings.
 */
static void parseArgs()
{
	/**
	 * \internal
	 * This structure is used to hold all the valid command line args as 
	 * well as the associated QSettings group/key values.
	 */
	struct
	{	const char* name;
		const bool  requires_value;
		const char* section;
		const char* group;
		const char* key;
	} param[] = // name                    reqv  section    group       key
	{	{ "-network-listener-address", true, "Network", "Listener", "Address" }
	,	{ "-network-listener-port",    true, "Network", "Listener", "Port"    }
	,	{ NULL, false, NULL, NULL, NULL },
	};

	// Remove any previous settings
	Kaiwa::clearCommandLineValues();

	// Get the args
	QStringList arg_list = qApp->arguments();
	for(int arg_index = 0; arg_index < arg_list.size(); arg_index++)
	{
		// If the arg is prefixed with "--" remove one of the "-"
		QString temp(arg_list.at(arg_index));
		if(temp.startsWith("--"))
		{
			temp.remove(0, 1);
		}

		// Split the arg into name and value
		QStringList key_value = temp.split('=');
		QString arg_name = key_value.at(0);
		QString arg_value = key_value.value(1);

		// Reassemble the value in-case it contained any ='s
		for(int i = 2; i < key_value.size(); i++)
		{
			arg_value.append("=").append(key_value.at(i));
		}

		for(int i = 0; param[i].name != NULL; i++)
		{
			if(arg_name != param[i].name)
			{
				continue;
			}

			// Save the value in to the settings
			if(param[i].requires_value)
			{
				if(arg_value.isEmpty() || arg_value.isNull())
				{
					if((arg_index + 1) >= arg_list.size())
					{
						fprintf(stderr
							, "Parameter \"%s\" missing value\n"
							, param[i].name
							);
						exit(-1);
					}

					arg_index++;
					arg_value = arg_list.at(arg_index);
				}

				Kaiwa::putCommandLineValue(param[i].section
					, param[i].group
					, param[i].key
					, arg_value
					);
			}
			else
			{
				Kaiwa::putCommandLineValue(param[i].section
					, param[i].group
					, param[i].key
					, QVariant()
					);
			}
		}
	}
}


/******************************************************************************
 * Functions: Public
 */

/**
 * \brief The main().
 *
 * This function will start the Kaiwa application.
 */
int main(int arc, char** argv)
{
	QApplication app(arc, argv);
	parseArgs();
	
	Kaiwa kaiwa;
	kaiwa.show();

	return app.exec();
}
