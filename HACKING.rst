=================
Developer's Guide
=================


This document currently contains random remarks instead of being comprehensive
developer guide, but it's valuable to give hints on some non-obvious parts of
the design and implementation.


Transport vs Stream
-------------------

* The transport is user-visible entity, when user calls ``xs_bind()`` or
  ``xs_connect()``, he get's transport id
* Transport has multiple underlying streams, e.g. TCP or unix socket after
  ``xs_bind()`` has no connections, and when connection is accepted, a stream
  is created for each connection
* Each transport implementation has a name e.g. "inproc", "tcp", which is a
  prefix of URI provided by user
* Transport is in charge for choosing stream implementation, it can preform a
  handshake and then choose a protocol, or it can provide different
  implementation depending on whether IOCP available. Depending on the nature
  of the underlying protocol it can create streams on connection basis (like
  with TCP), single stream for everything (multicast), or stream per host (UDP)
* Stream is in charge for sending and receiving messages, it also has support
  for shutting down a connection, and that is pretty much it. As it operates on
  already opened socket it can be reused between similar underlying protocols
  (e.g. TCP and unix sockets).
* The pattern plugin interface manages streams not transports, load balancing
  is made between streams, as well as duplicating messages for pub-sub.
* Stream plugins are not registered at the moment, any transport implementation
  is free to return any stream plugin structure (However, we may reconsider
  this point if we figure out how to combine different transports with
  different streams)

xs_recv()/xs_send() Wake Up
---------------------------

* We create an xs_signal (eventfd or socketpair) per application thread (keep
  it in threadlocals)
* Each time the application thread is about to block, it registers itself in
  sock->listeners list and sleeps in xs_signal_wait
* xs_sock_update_state() is API for pattern plugin to wakeup listeners
* In theory same applies to xs_poll, it can register single eventfd in
  multiple sockets and wait for events
* Having a list of listeners allows to have multiple xs_recv/xs_send in
  multiple threads simultaneously
* We do not wakeup anything if nobody is listening on the socket
* We do not wait for signal in case socket is readable/writable (this may need
  to be tweaked for socketpair)

