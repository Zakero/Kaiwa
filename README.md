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

Road Map
--------
 - _v0.3_
   - Communication via N nodes
     - `N >= 3`
     - `N1 <-> N2 <-> N3`
       - Automatic link repair if `N2` drops
         - `N1 <-> N3`
 - _v0.2_
   - Communication between 2 nodes
     - Simple point-2-point message passing
     - Alert when connection is dropped
   - Network Protocol
     - Connect.Open
     - Connect.Close
     - Connect.Status
     - Message.Text
 - __v0.1__
   - Simple UI
     - Text entry at the bottom of the window
     - Log at the top of the window

Key:
 - _Future Release_
 - __In-Progress__
 - Old Release

Bugs
----
 - None yet...

Future
------
 - Secure network connections
 - Multi-Channel support
 - Send data to user (ie: images)
 - Bot Support
   - BanBot
     Drop messages from a user or group of users
   - LogBot
     Record all message traffic to a log file
   - UrlBot
     Display the HTML title of URLs
