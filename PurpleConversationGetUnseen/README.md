This plugin exposes pidgin-specific PurpleConversation's data with key "unseen-count":

<pre>
[desktop] ~> purple-send PurpleGetConversations
method return sender=:1.548 -> dest=:1.553 reply_serial=2
   array [
      int32 35010
      int32 20282
   ]
[desktop] ~> purple-send PurpleConversationGetUnseen int32:35010
method return sender=:1.548 -> dest=:1.555 reply_serial=2
   int32 4
</pre>

Build Instructions
======

* Download and extract pidgin source from http://www.pidgin.im/download/source/
* Put `dbus-unseen-count.c` and `Makefile.am.diff` to `libpurple/plugins` and run `patch -p0 < Makefile.am.patch`
* Run `automake`, `./configure` and `make` in pidgin source directory
* Run `make dbus-unseen-count.so` in `libpurple/plugins`
* Run `sudo cp dbus-unseen-count.so /usr/lib/purple-2`
