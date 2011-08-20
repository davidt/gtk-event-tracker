gtk-event-tracker
=================

This is a GTK+ module which allows an application to watch what the user is
doing. This is useful for when you want to add "phone-home" functionality to
gather data on how people actually use your software. There's an excellent talk
that you can find via google which explains how Microsoft used such data to
inform the Office 2007 user interface redesign.

Using gtk-event-tracker
-----------------------

After loading the gtk-event-tracker module (via the $GTK\_MODULES environment
variable, for instance), you can collect the event data by creating a log
handler for the "gtk-event-tracker" domain.

For example::

    static void event_log_handler(const gchar *log_domain,
                                  GLogLevelFlags log_level,
                                  const gchar *message,
                                  gpointer user_data)
    {
            /*
             * TODO: Handle your event here. This usually means saving it to a
             * list of some sort (in-memory or on-disk) which can then be
             * reported to a central location later. It could also just be used
             * as an additional layer of application logging.
             */
    }


    void init()
    {
            g_log_set_handler("gtk-event-tracker", G_LOG_LEVEL_DEBUG,
                              event_log_handler, NULL);
    }


The contents of the log message will be one of the following::

    GtkAction activated-via-menu action-name
    GtkAction activated-via-mnemonic action-name
    GtkAction activated-via-shortcut action-name
    GtkAction activated-via-keyboard action-name
    GtkAction activated-via-toolbar action-name
    GtkAction activated action-name

In these cases, "action-name" will be the name of the GtkAction which got
triggered. The module does a best-effort job of determining the source of the
emission, but it's sometimes racy. In general, the plain "activated" log message
should be pretty rare. One notable exception is that this module does not
differentiate activations via proxy widgets.
