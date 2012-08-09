Kaiwa
=====

A distributed internet chat application.
"Kaiwa" (kai-wa) is the Japanese word for "communication".

Why Another Chat Program
------------------------

Most IRC type programs, like X-Chat, rely on a client-server model.  And if the
server becomes unavailable for some reason, communication among the clients
will be lost.

The goal of this project is to experiment with how to make a distributed
communication system.

The User Interface will be based on __Qt4__.

Developers
----------

### Building The Project

The first time you clone or make any invasive changes, run the setup script:  
`> ./setup.sh`  
This will create a new QT Project file and a new Makefile.

If any changes need to be make to the QT Project file, place these changes in
setup script instead.  This will allow qmake to do the heavy lifting when new
source code is added or moved around.  Then the script will add the finishing
touches.

And finally: 
`make && ./Kaiwa`

### Generating The Documentation

To create the documentation, you will need [Doxygen][].  And if you want to
make the graphs, you will also need [Dot][Graphviz].

Use the following command to generate the documentation:  
`> doxygen Doxyfile`


[Doxygen]:  http://www.stack.nl/~dimitri/doxygen "Doxygen"
[Graphviz]: http://www.graphviz.org/             "Graphviz"
