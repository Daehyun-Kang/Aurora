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
#undef AZM_TEST_GTK_SAMPLE 

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


GtkWidget * find_child(GtkWidget *parent, const gchar *name)
{
  if (g_strcasecmp(gtk_widget_get_name((GtkWidget *)parent), (gchar *)name) == 0) {
    return parent;
  }

  if (GTK_IS_BIN(parent)) {
    GtkWidget *child = gtk_bin_get_child(GTK_BIN(parent));
    return find_child(child, name);
  }
  if (GTK_IS_CONTAINER(parent)) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(parent));
    while ((children = g_list_next(children)) != NULL) {
      GtkWidget *widget = find_child(children->data, name);
      if (widget != NULL) { 
        return widget;
      }
    }
  }

  return NULL;
}
#define N_PROBES      4

#define N_SAMPLES     200
#define N_POINTS      500
typedef struct {
  int x, y;
} az_point_t;
typedef struct {
  int        state;
  int        srcno;
  int64_t    time_us[N_POINTS];
  int        values[N_POINTS];
  az_point_t points[N_POINTS];
  int64_t toffset;
  int push, pop;
  int64_t   x, y;
} az_disp_sig_t;
struct { 
  int width;
  int height;
  int64_t interval_us;
  uint64_t  stime_us;
  uint64_t time_us;
  double source_freq;
  double source_x[N_SAMPLES], source_y[N_SAMPLES];
  int64_t timespan_us;

  uint8_t is_odd;
  az_disp_sig_t  disp_sig[N_PROBES];
  int org_x;

  double x_scale;

  double t_scale;
  double t_scale_n;

  GtkLabel *label;
  GtkScale *scale;
} da_info = { 
  .width = 500, 
  .height = 500, 
  .source_freq = 0.2, 
  .timespan_us = 5000000, //5000, 
  .interval_us = 100000, //100,
  .stime_us = 0,
  .time_us = 0,
  .disp_sig = {0},
  .org_x = 0,
  .is_odd = 0,
  .x_scale = 1.0,
  .t_scale = 1.0,
  .t_scale_n = 1.0,
};
void az_write_sample(int srcno, uint64_t tstamp_us, double value_norm)
{
  double dy = da_info.height/10.0;
  az_disp_sig_t *disp_sig; 
  int probeno = 0;
  
  for (disp_sig = &da_info.disp_sig[probeno]; probeno < N_PROBES; probeno++,disp_sig++)
  {
    if (disp_sig->srcno == srcno) break;
  }
  if (probeno >= N_PROBES) {
    //az_printf("srcno %d no probes\n", srcno);
    return; 
  }

  if (disp_sig->state == 0) {
    //az_printf("srcno %d  state = %d\n", srcno, disp_sig->state);
    return;
  }

  if (disp_sig->toffset == 0) {
    disp_sig->toffset = da_info.time_us - tstamp_us;
  }
  tstamp_us += disp_sig->toffset;
  
  disp_sig->time_us[disp_sig->push] = tstamp_us;
  disp_sig->values[disp_sig->push++] = value_norm*2*dy + dy*5;
  if (disp_sig->push == array_size(disp_sig->values)) disp_sig->push = 0;

  gtk_widget_set_sensitive(da_info.scale, FALSE);
}
int get_sample_value(int64_t time_us)
{
  az_disp_sig_t *disp_sig; 

  disp_sig = &da_info.disp_sig[1];

  int j = disp_sig->pop;
  while (j < disp_sig->push) {
    if (disp_sig->time_us[j] < time_us) {
      disp_sig->y = disp_sig->values[j];
    } else {
      disp_sig->y = disp_sig->values[j];
      break;
    }
    if (++j == array_size(disp_sig->values)) {
      j = 0;
    }
    disp_sig->pop = j;
  }
  return disp_sig->y;
}

static double gen_sample(double us)
{
  return sin(2*M_PI * us * da_info.source_freq / 1E6); 
}

static gboolean time_handler(GtkWidget *widget)
{
  #ifdef  CHECK_DRAWINGAREAD
  GtkDrawingArea *drawingArea;
  gchar *name = "drawingArea1";
  drawingArea = find_child(widget, name); 

  if (drawingArea) {
    gtk_widget_set_size_request(drawingArea, da_info.width, da_info.height + da_info.count * 2);
    gtk_widget_queue_draw(drawingArea);
    //az_printf("widget %s found, %s\n", name, gtk_widget_get_name(widget));
  } else {
    az_printf("widget %s not found, %s\n", name, gtk_widget_get_name(widget));
  }

  da_info.count++;
  #endif

  int j;
  double dt_us = da_info.interval_us / array_size(da_info.source_x);
  double us;
  int64_t x, y;
  az_disp_sig_t *disp_sig = &da_info.disp_sig[1];
  if (da_info.t_scale_n != da_info.t_scale) {
    da_info.t_scale = da_info.t_scale_n;
    da_info.timespan_us = (int64_t)((double)da_info.timespan_us * da_info.t_scale);
    az_printf("timespan changed to %ld us\n", da_info.timespan_us);
  }

  if (da_info.time_us == 0) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    da_info.stime_us = ts.tv_sec * 1000000 + ts.tv_nsec / 1000; 
    da_info.time_us = da_info.interval_us; 
    disp_sig = &da_info.disp_sig[1];
    for (j = 0; j < array_size(disp_sig->values); j++) {
      disp_sig->points[j].x = j; 
      disp_sig->points[j].y = da_info.height/2;
      disp_sig->time_us[j] = 0; 
      disp_sig->values[j] = -1; 
    }
    disp_sig->x = da_info.interval_us * array_size(disp_sig->points) / da_info.timespan_us;
    disp_sig = &da_info.disp_sig[0];
    for (j = 0; j < array_size(disp_sig->points); j++) {
      disp_sig->points[j].x = j;
      disp_sig->points[j].y = 0;
    }
    for (j = 0; j < array_size(da_info.source_x); j++) {
      us = da_info.source_x[j] = dt_us * j; 
      da_info.source_y[j] = gen_sample(us);

      x = array_size(disp_sig->points) * us / da_info.timespan_us;
      x = x % array_size(disp_sig->points); 
      disp_sig->points[x].y = (da_info.source_y[j] + 1)*da_info.height / 2; 
    }
  } else {
    int64_t time_us;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    time_us = da_info.time_us;
    da_info.is_odd =  (da_info.time_us / da_info.timespan_us) & 0x1;
    da_info.time_us = ts.tv_sec * 1000000 + ts.tv_nsec / 1000 - da_info.stime_us; 
    disp_sig = &da_info.disp_sig[0];
    for (j = 0; j < array_size(da_info.source_x); j++) {
      us = da_info.source_x[j] = da_info.source_x[j] + da_info.interval_us; 
      da_info.source_y[j] = gen_sample(us);

      x = array_size(disp_sig->points) * us / da_info.timespan_us;
      x = x % array_size(disp_sig->points); 
      disp_sig->points[x].y = (da_info.source_y[j] + 1)*da_info.height / 2; 
    }

    disp_sig = &da_info.disp_sig[1];
    int64_t nsamples = (da_info.time_us - time_us) * array_size(disp_sig->points) / da_info.timespan_us; 
    int64_t dt = 0;
    if (nsamples > 0) dt =  (da_info.time_us - time_us)/nsamples; 
    for (j = 0, x = disp_sig->x; j < nsamples; j++) {
      disp_sig->points[x].y = get_sample_value(time_us);
      time_us += dt;
      if (++x == array_size(disp_sig->points)) x = 0;
    }
    disp_sig->x = x;
  }

  da_info.org_x += (array_size(disp_sig->points) * da_info.interval_us / da_info.timespan_us); 
  if (da_info.org_x >= array_size(disp_sig->points)) da_info.org_x = 0; 

  
  char tstamp[16];
  static int cur = 0;
  if (da_info.time_us - cur >= 100*1000) { 
    cur = da_info.time_us; 
    sprintf(tstamp, "%d.%d", cur/1000000, cur%1000000); 
    gtk_label_set_text(da_info.label, tstamp); 
    gtk_widget_queue_draw(widget);
  }
  gtk_widget_queue_draw(da_info.label);

  return TRUE;
}

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
  GtkDrawingArea *drawingArea;
  GtkScale  *scale;

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
  drawingArea = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "drawingarea1"));
  scale = GTK_SCALE(gtk_builder_get_object(builder, "scale1"));

  gtk_widget_set_name(drawingArea, "drawingArea1");

  gint width, height;
  gtk_window_get_size(GTK_WINDOW(window), &width, &height);

  az_printf("width:%d height:%d\n", width, height);

  gtk_builder_connect_signals(builder, label);

  g_object_unref(G_OBJECT(builder));

  gtk_label_set_text(label, "0.0");
  gtk_range_set_range(GTK_RANGE(scale), 0, 2);
  gtk_range_set_value(GTK_RANGE(scale), 1);

  da_info.label = label;
  g_timeout_add(da_info.interval_us/1000, (GSourceFunc) time_handler, (gpointer)drawingArea);

  da_info.scale = scale;

  gtk_widget_set_size_request(drawingArea, da_info.width, da_info.height);
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
static void az_set_srcno(int index, gchar *text)
{
  da_info.disp_sig[index].srcno = strtol(text, NULL, 10);
  az_printf("set probe %d source thread no %d\n",  
      index, da_info.disp_sig[index].srcno);
  da_info.disp_sig[index].state = 1; 
}
G_MODULE_EXPORT void on_entry1_changed(GtkEntry *entry, GtkWidget *widget)
{
  const gchar *text = gtk_entry_get_text(entry);
  az_set_srcno(0, text);
  GtkRadioButton *rb = GTK_RADIO_BUTTON(widget);
  gtk_toggle_button_set_active(rb, 1);
  az_printf("rb%d active %d\n", 0, gtk_toggle_button_get_active(rb));
}
G_MODULE_EXPORT void on_entry2_changed(GtkEntry *entry, GtkWidget *widget)
{
  const gchar *text = gtk_entry_get_text(entry);
  az_set_srcno(1, text);
  GtkRadioButton *rb = GTK_RADIO_BUTTON(widget);
  gtk_toggle_button_set_active(rb, 1);
  az_printf("rb%d active %d\n", 1, gtk_toggle_button_get_active(rb));
}
G_MODULE_EXPORT void on_entry3_changed(GtkEntry *entry, GtkWidget *widget)
{
  const gchar *text = gtk_entry_get_text(entry);
  az_set_srcno(2, text);
  GtkRadioButton *rb = GTK_RADIO_BUTTON(widget);
  gtk_toggle_button_set_active(rb, 1);
  az_printf("rb%d active %d\n", 2, gtk_toggle_button_get_active(rb));
}
G_MODULE_EXPORT void on_entry4_changed(GtkEntry *entry, GtkWidget *widget)
{
  const gchar *text = gtk_entry_get_text(entry);
  az_set_srcno(3, text);
  GtkRadioButton *rb = GTK_RADIO_BUTTON(widget);
  gtk_toggle_button_set_active(rb, 1);
  az_printf("rb%d active %d\n", 3, gtk_toggle_button_get_active(rb));
}
G_MODULE_EXPORT void on_scale1_value_changed(GtkRange *range,
    gpointer user_data)
{
  gdouble value = gtk_range_get_value(GTK_RANGE(range));
  if (value == 1.0) {
    da_info.t_scale_n = 1.0;
  } else if (value < 1.0) {
    value = ceil(value * 10);
    da_info.t_scale_n = value * 0.1; 
  } else {
    da_info.t_scale_n = ceil((value + 0.09 - 1.0) * 10.0);
  }
  az_printf("scale changed to : %f\n", da_info.t_scale_n);
  //gtk_widget_queue_draw(GTK_WIDGET(user_data));
}

static gboolean draw_clock(GtkWidget *widget,
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

G_MODULE_EXPORT gboolean on_drawingarea1_draw(GtkWidget *widget,
    cairo_t *cr,
    gpointer user_data)
{
  
  /*
  gint width, height;
  GtkWidget *win = gtk_widget_get_toplevel(widget);

  gtk_window_get_size(GTK_WINDOW(win), &width, &height);

  az_printf("top window width:%d height:%d\n", width, height);

  gtk_widget_get_size_request(widget, &width, &height);

  az_printf("draw area width:%d height:%d\n", width, height);
  */

  //if (da_info.time_us == 0) {
    cairo_set_source_rgb(cr, 1, 1, 1); 
    cairo_paint(cr);
  //}

  cairo_set_source_rgb(cr, 0, 0, 0); 

  int j, k, end;
  az_disp_sig_t *disp_sig;

  //disp_sig = &da_info.disp_sig[0];
  disp_sig = &da_info.disp_sig[1];
  int remained = array_size(disp_sig->points)- da_info.org_x;

  cairo_scale(cr, da_info.x_scale, 1.0);
  cairo_translate(cr, -da_info.org_x, 0);
  if (da_info.time_us > da_info.timespan_us) {
    j = da_info.org_x;
    cairo_move_to(cr, disp_sig->points[j].x,disp_sig->points[j].y);
    for (++j; j < array_size(disp_sig->points); j++) {
      //az_printf("%d %d\n", da_info.x[j], da_info.y[j]);
      cairo_line_to(cr, disp_sig->points[j].x,disp_sig->points[j].y);
    }
  }

  cairo_translate(cr, da_info.org_x+remained, 0);
  end = da_info.org_x;
  for (j = 0; j < end; j++) {
    //az_printf("%d %d\n", da_info.x[j], da_info.y[j]);
    cairo_line_to(cr, disp_sig->points[j].x,disp_sig->points[j].y);
  }
  cairo_stroke(cr);
  cairo_translate(cr, -remained, 0);

}

/* === end of AZM_GTK_C === */
