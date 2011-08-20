#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

static guint signal__gtk_action__activate,
             signal__gtk_menu_item__activate,
             signal__gtk_tool_button__clicked;

static guint last_event_time;
static enum {
   NONE,
   MENU_ITEM,
   MENU_ITEM_VIA_MNEMONIC,
   MENU_ITEM_VIA_SHORTCUT,
   MENU_ITEM_VIA_KEYBOARD,
   TOOL_ITEM,
} last_event_type;

static void log_entry(gchar *message, ...)
{
   va_list ap;
   va_start(ap, message);
   g_logv("gtk-event-tracker", G_LOG_LEVEL_DEBUG, message, ap);
   va_end(ap);
}

static void default_log_handler(const gchar *log_domain,
                                GLogLevelFlags log_level,
                                const gchar *message,
                                gpointer user_data)
{
   // Do nothing
}

static gboolean emission_hook_cb(GSignalInvocationHint *hint,
                                 guint n_param_values,
                                 const GValue *param_values,
                                 gpointer data)
{
   GObject *object;
   guint32 eventTime;
   GdkEvent *event;

   eventTime = gtk_get_current_event_time();

   // Ignore events not caused by user input
   if (GDK_CURRENT_TIME == eventTime) {
      return TRUE;
   }

   object = g_value_get_object(&param_values[0]);
   event = gtk_get_current_event();

   if (GTK_IS_ACTION(object)) {
      GtkAction *action = GTK_ACTION(g_value_get_object(&param_values[0]));

      if (last_event_time != eventTime) {
         return TRUE;
      }

      if (GDK_KEY_PRESS == event->type) {
         GdkEventKey *key = (GdkEventKey*)(event);
         if (GDK_MOD1_MASK == key->state) {
            last_event_type = MENU_ITEM_VIA_MNEMONIC;
         } else if (0 != key->state) {
            last_event_type = MENU_ITEM_VIA_SHORTCUT;
         } else {
            last_event_type = MENU_ITEM_VIA_KEYBOARD;
         }
      }

      switch (last_event_type) {
      case MENU_ITEM:
         log_entry("GtkAction activated-via-menu %s",
                   gtk_action_get_name(action));
         break;
      case MENU_ITEM_VIA_MNEMONIC:
         log_entry("GtkAction activated-via-mnemonic %s",
                   gtk_action_get_name(action));
         break;
      case MENU_ITEM_VIA_SHORTCUT:
         log_entry("GtkAction activated-via-shortcut %s",
                   gtk_action_get_name(action));
         break;
      case MENU_ITEM_VIA_KEYBOARD:
         log_entry("GtkAction activated-via-keyboard %s",
                   gtk_action_get_name(action));
         break;
      case NONE:
         log_entry("GtkAction activated %s",
                   gtk_action_get_name(action));
         break;
      case TOOL_ITEM:
         log_entry("GtkAction activated-via-toolbar %s",
                   gtk_action_get_name(action));
      default:
         break;
      }

      last_event_type = NONE;
      last_event_time = eventTime;
   } else if (GTK_IS_MENU_ITEM(object)) {
      last_event_time = eventTime;
      last_event_type = MENU_ITEM;
   } else if (GTK_IS_TOOL_BUTTON(object)) {
      last_event_time = eventTime;
      last_event_type = TOOL_ITEM;
   }

   return TRUE;
}

static void install_hook(GType type, const char *signal, guint *sn)
{
   GTypeClass *type_class = g_type_class_ref(type);
   *sn = g_signal_lookup(signal, type);
   g_signal_add_emission_hook(*sn, 0, emission_hook_cb, NULL, NULL);
   g_type_class_unref(type_class);
}

G_MODULE_EXPORT void gtk_module_init(gint argc, char *argv[])
{
   install_hook(GTK_TYPE_ACTION,      "activate", &signal__gtk_action__activate);
   install_hook(GTK_TYPE_MENU_ITEM,   "activate", &signal__gtk_menu_item__activate);
   install_hook(GTK_TYPE_TOOL_BUTTON, "clicked",  &signal__gtk_tool_button__clicked);
   g_log_set_handler("gtk-event-tracker", G_LOG_LEVEL_DEBUG, default_log_handler, NULL);
   last_event_type = NONE;
   last_event_time = 0;
}
