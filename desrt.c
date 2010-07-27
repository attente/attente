#include <gtk/gtk.h>

#define TYPE_LABEL_WINDOW label_window_get_type()


typedef struct _LabelWindow LabelWindow;
typedef struct _LabelWindowClass LabelWindowClass;

struct _LabelWindow
{
  GtkWindow parent_instance;

  GtkLabel *label;
};

struct _LabelWindowClass
{
  GtkWindowClass parent_class;
};

GType label_window_get_type (void);

G_DEFINE_TYPE (LabelWindow, label_window, GTK_TYPE_WINDOW);

static void
label_window_init (LabelWindow *window)
{
  window->label = GTK_LABEL (gtk_label_new ());
  gtk_container_add (GTK_CONTAINER (window),
                     GTK_WIDGET (window->label));
  gtk_widget_show (GTK_WIDGET (window->label));
}

static void
label_window_class_init (LabelWindowClass *class)
{
}

void
label_window_set_string (LabelWindow *label,
                         const gchar *string)
{
  gtk_label_set_text (label->label, string);
  g_object_notify (G_OBJECT (label), "string");
}

const gchar *
label_window_get_string (LabelWindow *label)
{
  return gtk_label_get_text (label->label);
}

enum
{
  PROP_NONE,
  PROP_STRING
}

static void
label_window_get_property (GObject *object, guint prop_id,
                           GValue *value, GParamSpec *pspec)
{
  LabelWindow label = (LabelWindow *) object;

  switch (prop_id)
    {
    case PROP_STRING:
      g_value_set_string (value, label_window_get_string (label));
      break;

    default:
      g_assert_not_reached ();
    }
}

static void
label_window_set_property (GObject *object, guint prop_id,
                           const GValue *value, GParamSpec *pspec)
{
  LabelWindow label = (LabelWindow *) object;

  switch (prop_id)
    {
    case PROP_STRING:
      label_window_set_string (label, g_value_get_string (value));
      break;

    default:
      g_assert_not_reached ();
    }
}

LabelWindow *
label_window_new (const gchar *string)
{
  return g_object_new (TYPE_LABEL_WINDOW,
                       "string", string,
                       NULL);
}

int
main (void)
{
  LabelWindow *w;

  gtk_init (NULL, NULL);
  w = label_window_new ("foo");
  gtk_widget_show (GTK_WIDGET (w));
  gtk_main ();

  return 0;
}
