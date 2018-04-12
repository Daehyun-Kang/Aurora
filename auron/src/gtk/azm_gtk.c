/**
 * @file   azm_gtk_sample.c
 * @brief  
 * @date   11/04/18
 * @author dhkang
 *
 * @copyright 
 * Copyright (c) Fware, 2013-2017 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.\n\n
 * 
 * You should have received a copy of  the LICENSE with this file.\n\n
 * 
 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History:
 *   - 2018-04-11 dhkang initial create
 */

/* include header files */
#include <gtk/gtk.h>
#include <math.h>
#include "aurora.h"

#define AZM_TEST_GTK_SAMPLE   1

#ifdef AZM_TEST_GTK_SAMPLE  
  #define CALL_AZM_GTK_SAMPLE(argc, argv) {\
    extern int azm_gtk_sample_main(int, char **);\
    \
    return azm_gtk_sample_main(argc, argv);\
    }
#else
  #define CALL_AZM_GTK_SAMPLE(argc, argv) 
#endif

/* declare global variables */

/* declare static variables */


/* implement static functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */


/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
int azm_gtk_main(int argc, char **argv)
{
  
  CALL_AZM_GTK_SAMPLE(argc, argv); 

  GtkBuilder *builder;
  GtkWidget *window;
  GError  *error = NULL;
  GtkLabel *label;
  GtkButton *button;
  GtkEntry *entry;

  gtk_init(&argc, &argv);

  builder = gtk_builder_new();
  if (!gtk_builder_add_from_file(builder, "cfg/azm_gtk.glade", &error))
  {
    g_warning("%s", error->message);
    g_free(error);
    return 1;
  }
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
  label = GTK_LABEL(gtk_builder_get_object(builder, "label1"));
  button = GTK_BUTTON(gtk_builder_get_object(builder, "button1"));
  entry = GTK_ENTRY(gtk_builder_get_object(builder, "entry1"));

  gtk_builder_connect_signals(builder, label);

  g_object_unref(G_OBJECT(builder));

  gtk_label_set_text(label, "Hello world!");

  gtk_widget_show(window);

  gtk_main();

  return 0;
}

G_MODULE_EXPORT void on_window1_destroy()
{
  gtk_main_quit();
}

G_MODULE_EXPORT void button1_clicked_cb(GtkButton *button, GtkLabel *label)
{
  static int toggle = 0;
  if (toggle) {
    toggle = 0;
    gtk_label_set_text(label, "Hello world!");
  } else {
    toggle = 1;
    gtk_label_set_text(label, "");
  }
}
G_MODULE_EXPORT void on_entry1_changed(GtkEntry *entry, GtkLabel *label)
{
  const gchar *text = gtk_entry_get_text(entry);
  gtk_label_set_text(label, text);
}
G_MODULE_EXPORT void on_scale1_value_changed(GtkRange *range,
    gpointer user_data)
{
  gtk_widget_queue_draw(GTK_WIDGET(user_data));
}

G_MODULE_EXPORT gboolean on_drawingarea1_draw(GtkWidget *widget,
    cairo_t *cr,
    gpointer user_data)
{
    gdouble marks;
    char text[10];
     
    static PangoLayout *layout = NULL;  // Retained between calls
    PangoFontDescription *desc;
    int width,height;

    gdouble value;

    // Do some pango setup on the first call.
    if(layout == NULL) {
      layout = gtk_widget_create_pango_layout(widget, "");
      desc = pango_font_description_from_string ("Sans Bold 15");
      az_printf("font descr: %p\n", desc);
      pango_layout_set_font_description (layout, desc);
      pango_font_description_free (desc);
    }

     cairo_set_source_rgb(cr, 0, 0, 0);
     cairo_set_line_width(cr, 1.0);

     // Set origin as middle of bottom edge of the drawing area.
     // Window is not resizable so "magic numbers" will work here. 
     cairo_translate(cr,200,190);
 

     // Draw the black radial scale marks and labels
     for(marks = 0.0; marks <= 100.0; marks += 10.0)
     {
        // Draw the radial marks
        cairo_move_to(cr, 150.0 * cos(M_PI * marks * 0.01),   -150.0 * sin(M_PI * marks * 0.01) );
        cairo_line_to(cr, 160.0 * cos(M_PI * marks * 0.01),   -160.0 * sin(M_PI * marks * 0.01) );
 
        // Set the text to print
         sprintf(text,"%2.0f", marks);
         pango_layout_set_text (layout, text, -1);

        // Redo layout for new text
        pango_cairo_update_layout (cr, layout);
 
        // Retrieve the size of the rendered text
         pango_layout_get_size (layout, &width, &height);

         // Position the text at the end of the radial marks
       cairo_move_to(cr, 
                       (-180.0 * cos(M_PI *marks * 0.01)) - ((double)width /PANGO_SCALE / 2.0),
                       (-180.0 * sin(M_PI *marks * 0.01)) - ((double)height/PANGO_SCALE / 2.0));
        // Render the text
         pango_cairo_show_layout (cr, layout);
    }
    cairo_stroke(cr);
 
    // Retrieve the new slider value
    value = gtk_range_get_value(GTK_RANGE(user_data));
    az_printf("value : %f\n", value);
    //value = 30;
 
     cairo_set_source_rgb(cr, 1.0, 0, 0);
     cairo_set_line_width(cr, 1.5);

    // Draw the meter pointer
    cairo_move_to(cr, 0.0,0.0);
     cairo_line_to(cr, -150.0 * cos(M_PI *value * 0.01),  -150.0 * sin(M_PI *value * 0.01) );
    cairo_stroke(cr);  
 
    return FALSE;
}

/* === end of AZM_GTK_C === */
