/**
 * @file   azm_gtk_sample.c
 * @brief  
 * @date   11/04/18
 * @author dhkang
 *
 * all this code comes from zetcode.com
 *
 * History:
 *   - 2018-04-11 dhkang initial create
 */

/* include header files */
#include <gtk/gtk.h>
#include <math.h>
#include "aurora.h"

#define  AZM_GTK_SAMPLE6    1

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
#ifdef  AZM_GTK_SAMPLE1
int azm_gtk_sample_main(int argc, char **argv)
{
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
#endif

#ifdef  AZM_GTK_SAMPLE2
static void do_drawing(cairo_t *);

struct {
  int count;
  double coordx[100];
  double coordy[100];
} glob;

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  do_drawing(cr);

  return FALSE;
}

int titleshow = 1;

static void do_drawing(cairo_t *cr)
{
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_set_line_width(cr, 0.5);

  int i, j;
  for (i = 0; i <= glob.count - 1; i++) {
    for (j = 0; j <= glob.count - 1; j++) {
      cairo_move_to(cr, glob.coordx[i], glob.coordy[i]);
      cairo_line_to(cr, glob.coordx[j], glob.coordy[j]);
    }
  }

  glob.count = 0;
  cairo_stroke(cr);

  if (titleshow) {
  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 40.0);

  cairo_move_to(cr, 10.0, 50.0);
  cairo_show_text(cr, "Gauron graphic test");
  }
}

static gboolean clicked(GtkWidget *widget, GdkEventButton *event,
    gpointer user_data)
{
  if (event->button == 1) {
    glob.coordx[glob.count] = event->x;
    glob.coordy[glob.count++] = event->y;
  }
  if (event->button == 3) {
    gtk_widget_queue_draw(widget);
  }

  return TRUE;
}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *darea;

  glob.count = 0;
  
  gtk_init(&argc, &argv);
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(window, "button-press-event",
      G_CALLBACK(clicked), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
  gtk_window_set_title(GTK_WINDOW(window), "Lines"); 

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE3
static void do_drawing(cairo_t *, GtkWidget *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  do_drawing(cr, widget);

  return FALSE;
}

float count = 0;

static void do_drawing(cairo_t *cr, GtkWidget *widget)
{
  GtkWidget *win = gtk_widget_get_toplevel(widget);

  int width, height;
  gtk_window_get_size(GTK_WINDOW(win), &width, &height);

  cairo_set_line_width(cr, 9);
  cairo_set_source_rgb(cr, 0.69+count, 0.19+count, 0);

  cairo_translate(cr, width/2, height/2);
  cairo_arc(cr, 0, 0, 50, 0, 2*M_PI);
  cairo_stroke_preserve(cr);

  cairo_set_source_rgb(cr, 0.3+count, 0.4+count, 0.6);
  cairo_fill(cr);
}

static gboolean clicked(GtkWidget *widget, GdkEventButton *event,
    gpointer user_data)
{
  count += 0.1;
  if (event->button == 3) {
    count = 0;
  }
  gtk_widget_queue_draw(widget);

  return TRUE;
}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *darea;

  gtk_init(&argc, &argv);
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(window, "button-press-event",
      G_CALLBACK(clicked), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
  gtk_window_set_title(GTK_WINDOW(window), "Fill & stroke"); 

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE4
static void do_drawing(cairo_t *, GtkWidget *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  do_drawing(cr, widget);

  return FALSE;
}

float count = 0;

static void do_drawing(cairo_t *cr, GtkWidget *widget)
{
  int y = 30;

  cairo_set_source_rgba(cr, 0, 0, 0, 1); 

  static const double dashed1[] = {4.0, 21.0, 2.0};
  static int len1 = array_size(dashed1); 

  static const double dashed2[] = {14.0, 6.0};
  static int len2 = array_size(dashed2); 

  static const double dashed3[] = {1.0};

  cairo_set_line_width(cr, 5);

  cairo_set_dash(cr, dashed1, len1, 0);
  cairo_move_to(cr, 40, y); 
  cairo_line_to(cr, 100, y); 
  cairo_stroke(cr);

  y += 20;
  cairo_set_dash(cr, dashed2, len2, 0);
  cairo_move_to(cr, 40, y); 
  cairo_line_to(cr, 100, y); 
  cairo_stroke(cr);

  y += 20;
  cairo_set_dash(cr, dashed3, 1, 0);
  cairo_move_to(cr, 40, y); 
  cairo_line_to(cr, 100, y); 
  cairo_stroke(cr);

  cairo_set_line_width(cr, 10);
  cairo_set_dash(cr, NULL, 0, 0);

  y += 20;
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT); 
  cairo_move_to(cr, 30, y); 
  cairo_line_to(cr, 150, y); 
  cairo_stroke(cr);

  y += 20;
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND); 
  cairo_move_to(cr, 30, y); 
  cairo_line_to(cr, 150, y); 
  cairo_stroke(cr);

  y += 20;
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE); 
  cairo_move_to(cr, 30, y); 
  cairo_line_to(cr, 150, y); 
  cairo_stroke(cr);

  cairo_set_line_width(cr, 1.5);

  cairo_move_to(cr, 30, 80); 
  cairo_line_to(cr, 30, 140); 
  cairo_stroke(cr);

  cairo_move_to(cr, 150, 80); 
  cairo_line_to(cr, 150, 140); 
  cairo_stroke(cr);

  cairo_move_to(cr, 155, 80); 
  cairo_line_to(cr, 155, 140); 
  cairo_stroke(cr);

  cairo_set_source_rgb(cr, 0.1, 0, 0); 
  cairo_set_line_width(cr, 14);

  cairo_translate(cr, 200, 0);
  cairo_rectangle(cr, 30, 30, 100, 100);
  cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER); 
  cairo_stroke(cr);

  cairo_rectangle(cr, 160, 30, 100, 100);
  cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL); 
  cairo_stroke(cr);

  cairo_rectangle(cr, 100, 160, 100, 100);
  cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND); 
  cairo_stroke(cr);
}

static gboolean clicked(GtkWidget *widget, GdkEventButton *event,
    gpointer user_data)
{
  count += 0.1;
  if (event->button == 3) {
    count = 0;
  }
  gtk_widget_queue_draw(widget);

  return TRUE;
}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *darea;

  gtk_init(&argc, &argv);
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(window, "button-press-event",
      G_CALLBACK(clicked), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 300);
  gtk_window_set_title(GTK_WINDOW(window), "Dash Lines"); 

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE5
static void do_drawing(cairo_t *, GtkWidget *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  do_drawing(cr, widget);

  return FALSE;
}

float count = 0;

int points[11][2] = {
  {0, 85},
  {75, 75},
  {100, 10},
  {125, 75},
  {200, 85},
  {150, 125},
  {160, 190},
  {100, 150},
  {40, 190},
  {50, 125},
  {0, 85}
};

static void do_drawing(cairo_t *cr, GtkWidget *widget)
{
  int y = 30;

  cairo_set_source_rgb(cr, 0.6, 0.6, 0.6); 
  cairo_set_line_width(cr, 1);

  cairo_rectangle(cr, 20, 20, 120, 80);
  cairo_rectangle(cr, 180, 20, 80, 80);
  cairo_stroke_preserve(cr);
  cairo_fill(cr);

  cairo_arc(cr, 330, 60, 40, 0, 2*M_PI);
  cairo_stroke_preserve(cr);
  cairo_fill(cr);

  cairo_arc(cr, 90, 160, 40, M_PI/4, M_PI);
  cairo_close_path(cr);
  cairo_stroke_preserve(cr);
  cairo_fill(cr);

  cairo_translate(cr, 220, 180);
  cairo_scale(cr, 1, 0.7);
  cairo_arc(cr, 0, 0, 50, 0, 2*M_PI);
  cairo_stroke_preserve(cr);
  cairo_fill(cr);


  cairo_translate(cr, -220, 30);
  cairo_scale(cr, 1, 10.0/7.0);

  cairo_set_source_rgb(cr, 0.5, 0.5, 0.1); 
  gint i;
  for (i = 0; i < 10; i++) {
    cairo_line_to(cr, points[i][0], points[i][1]);
  }
  cairo_close_path(cr);
  cairo_stroke_preserve(cr);
  cairo_fill(cr);

  cairo_set_source_rgb(cr, 0, 0.3, 0); 
  cairo_move_to(cr, 240, 40);
  cairo_line_to(cr, 240, 160);
  cairo_line_to(cr, 350, 160);
  cairo_close_path(cr);
  cairo_stroke_preserve(cr);
  cairo_fill(cr);

  cairo_set_source_rgb(cr, 1, 0, 0.5); 
  cairo_move_to(cr, 380, 40);
  cairo_line_to(cr, 380, 160);
  cairo_line_to(cr, 450, 160);
  cairo_curve_to(cr, 440, 155, 380, 145, 380, 40);
  cairo_stroke_preserve(cr);
  cairo_fill(cr);

}

static gboolean clicked(GtkWidget *widget, GdkEventButton *event,
    gpointer user_data)
{
  count += 0.1;
  if (event->button == 3) {
    count = 0;
  }
  gtk_widget_queue_draw(widget);

  return TRUE;
}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *darea;

  gtk_init(&argc, &argv);
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(window, "button-press-event",
      G_CALLBACK(clicked), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
  gtk_window_set_title(GTK_WINDOW(window), "Shapes"); 

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE6
static void do_drawing(cairo_t *, GtkWidget *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  do_drawing(cr, widget);

  return FALSE;
}

float count = 0;

cairo_surface_t *surface1;
cairo_surface_t *surface2;
cairo_surface_t *surface3;
cairo_surface_t *surface4;

static void create_surfaces()
{
  surface1 = cairo_image_surface_create_from_png("images/blueweb.png");
  surface2 = cairo_image_surface_create_from_png("images/maple.png");
  surface3 = cairo_image_surface_create_from_png("images/crack.png");
  surface4 = cairo_image_surface_create_from_png("images/chocolate.png");
}
static void destroy_surfaces()
{
  cairo_surface_destroy(surface1);
  cairo_surface_destroy(surface2);
  cairo_surface_destroy(surface3);
  cairo_surface_destroy(surface4);
}

static void do_drawing(cairo_t *cr, GtkWidget *widget)
{
  cairo_pattern_t *patter1;
  cairo_pattern_t *patter2;
  cairo_pattern_t *patter3;
  cairo_pattern_t *patter4;


  patter1 = cairo_pattern_create_for_surface(surface1);
  patter2 = cairo_pattern_create_for_surface(surface2);
  patter3 = cairo_pattern_create_for_surface(surface3);
  patter4 = cairo_pattern_create_for_surface(surface4);

  cairo_set_source(cr, patter1);
  cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
  cairo_rectangle(cr, 20, 20, 100, 100);
  cairo_fill(cr);

  cairo_set_source(cr, patter2);
  cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
  cairo_rectangle(cr, 150, 20, 100, 100);
  cairo_fill(cr);

  cairo_set_source(cr, patter3);
  cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
  cairo_rectangle(cr, 20, 140, 100, 100);
  cairo_fill(cr);

  cairo_set_source(cr, patter4);
  cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
  cairo_rectangle(cr, 150, 140, 100, 100);
  cairo_fill(cr);

  cairo_pattern_destroy(patter1);
  cairo_pattern_destroy(patter2);
  cairo_pattern_destroy(patter3);
  cairo_pattern_destroy(patter4);
}

static gboolean clicked(GtkWidget *widget, GdkEventButton *event,
    gpointer user_data)
{
  count += 0.1;
  if (event->button == 3) {
    count = 0;
  }
  gtk_widget_queue_draw(widget);

  return TRUE;
}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *darea;

  gtk_init(&argc, &argv);
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(window, "button-press-event",
      G_CALLBACK(clicked), NULL);

  create_surfaces();

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
  gtk_window_set_title(GTK_WINDOW(window), "Shapes"); 

  gtk_widget_show_all(window);

  gtk_main();

  destroy_surfaces();
  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE7
static void do_drawing(cairo_t *, GtkWidget *);

static void draw_gradient1(cairo_t *);
static void draw_gradient2(cairo_t *);
static void draw_gradient3(cairo_t *);
static void draw_gradient4(cairo_t *);
static void draw_gradient5(cairo_t *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  draw_gradient1(cr);
  draw_gradient2(cr);
  draw_gradient3(cr);
  draw_gradient4(cr);
  draw_gradient5(cr);

  return FALSE;
}

static void draw_gradient1(cairo_t *cr)
{
  cairo_pattern_t *pat1;
  pat1 = cairo_pattern_create_linear(0.0, 0.0, 350.0, 350.0);

  gdouble j;
  gint count = 1;

  for (j = 0.1; j < 1; j += 0.1) {
    if ((count % 2)) {
      cairo_pattern_add_color_stop_rgb(pat1, j, 0, 0, 0);
    } else {
      cairo_pattern_add_color_stop_rgb(pat1, j, 1, 0, 0);
    }
    count++;
  }

  cairo_rectangle(cr, 20, 20, 300, 100);
  cairo_set_source(cr, pat1);
  cairo_fill(cr);

  cairo_pattern_destroy(pat1);
}

static void draw_gradient2(cairo_t *cr)
{
  cairo_pattern_t *pat2;
  pat2 = cairo_pattern_create_linear(0.0, 0.0, 350.0, 0.0);

  gdouble j;
  gint count = 1;

  for (j = 0.05; j < 0.95; j += 0.025) {
    if ((count % 2)) {
      cairo_pattern_add_color_stop_rgb(pat2, j, 0, 0, 0);
    } else {
      cairo_pattern_add_color_stop_rgb(pat2, j, 0, 0, 1);
    }
    count++;
  }

  cairo_rectangle(cr, 20, 140, 300, 100);
  cairo_set_source(cr, pat2);
  cairo_fill(cr);

  cairo_pattern_destroy(pat2);
}

static void draw_gradient3(cairo_t *cr)
{
  cairo_pattern_t *pat3;
  pat3 = cairo_pattern_create_linear(20.0, 260.0, 20.0, 360.0);

  cairo_pattern_add_color_stop_rgb(pat3, 0.1, 0, 0, 0);
  cairo_pattern_add_color_stop_rgb(pat3, 0.5, 1, 1, 0);
  cairo_pattern_add_color_stop_rgb(pat3, 0.9, 0, 0, 0);

  cairo_rectangle(cr, 20, 260, 300, 100);
  cairo_set_source(cr, pat3);
  cairo_fill(cr);

  cairo_pattern_destroy(pat3);
}

static void draw_gradient4(cairo_t *cr)
{
  cairo_pattern_t *pat4;

  cairo_set_source_rgba(cr, 0, 0, 0, 1);
  cairo_set_line_width(cr, 12);
  cairo_translate(cr, 400, 60);

  pat4 = cairo_pattern_create_radial(30, 30, 10, 30, 30, 90);

  cairo_pattern_add_color_stop_rgba(pat4, 0, 1, 1, 1, 1); 
  cairo_pattern_add_color_stop_rgba(pat4, 1, 0.6, 0.6, 0.6, 1); 

  cairo_set_source(cr, pat4);
  cairo_arc(cr, 0, 0, 40, 0, M_PI*2);
  cairo_fill(cr);

  cairo_pattern_destroy(pat4);
}
static void draw_gradient5(cairo_t *cr)
{
  cairo_pattern_t *pat5;

  cairo_translate(cr, 120, 0);

  pat5 = cairo_pattern_create_radial(0, 0, 10, 0, 0, 40);

  cairo_pattern_add_color_stop_rgb(pat5, 0, 1, 1, 0); 
  cairo_pattern_add_color_stop_rgb(pat5, 0.8, 0, 0, 0); 

  cairo_set_source(cr, pat5);
  cairo_arc(cr, 0, 0, 40, 0, M_PI*2);
  cairo_fill(cr);

  cairo_pattern_destroy(pat5);
}

static gboolean clicked(GtkWidget *widget, GdkEventButton *event,
    gpointer user_data)
{
  gtk_widget_queue_draw(widget);

  return TRUE;
}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *darea;

  gtk_init(&argc, &argv);
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(window, "button-press-event",
      G_CALLBACK(clicked), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
  gtk_window_set_title(GTK_WINDOW(window), "Gradients"); 

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE8
static void do_drawing(cairo_t *, GtkWidget *);
static void do_drawing2(cairo_t *, GtkWidget *);

struct {
  gboolean timer;
  gdouble alpha;
  gdouble size;
} glob;

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{

  do_drawing(cr, widget);
  do_drawing2(cr, widget);

  return FALSE;
}
static void do_drawing(cairo_t *cr, GtkWidget *widget)
{
  /*
  gint i;
  for (i = 1; i <= 10; i++) {
    cairo_set_source_rgba(cr, 0, 0, 1, i*0.1);
    cairo_rectangle(cr, 50*i, 20, 40, 40);
    cairo_fill(cr);
  }
  */

  cairo_text_extents_t extents;

  GtkWidget *win = gtk_widget_get_toplevel(widget);

  gint width, height;
  gtk_window_get_size(GTK_WINDOW(win), &width, &height);

  gint x = width/2;
  gint y = height/2;

  cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
  cairo_paint(cr);

  cairo_select_font_face(cr, "Courier",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);

  glob.size += 0.8;

  if (glob.size > 20) {
    glob.alpha -= 0.01;
  }

  cairo_set_font_size(cr, glob.size);
  cairo_set_source_rgb(cr, 0, 0, 0);

  cairo_text_extents(cr, "Aurora", &extents);
  cairo_move_to(cr, x - extents.width/2, y);
  cairo_text_path(cr, "Aurora");
  cairo_clip(cr);

  cairo_paint_with_alpha(cr, glob.alpha);

  if (glob.alpha <= 0) {
    glob.timer = FALSE;
  }
}
int count = 0;

static void do_drawing2(cairo_t *cr, GtkWidget *widget)
{  
  static gdouble const trs[8][8] = {
      { 0.0, 0.15, 0.30, 0.5, 0.65, 0.80, 0.9, 1.0 },
      { 1.0, 0.0,  0.15, 0.30, 0.5, 0.65, 0.8, 0.9 },
      { 0.9, 1.0,  0.0,  0.15, 0.3, 0.5, 0.65, 0.8 },
      { 0.8, 0.9,  1.0,  0.0,  0.15, 0.3, 0.5, 0.65},
      { 0.65, 0.8, 0.9,  1.0,  0.0,  0.15, 0.3, 0.5 },
      { 0.5, 0.65, 0.8, 0.9, 1.0,  0.0,  0.15, 0.3 },
      { 0.3, 0.5, 0.65, 0.8, 0.9, 1.0,  0.0,  0.15 },
      { 0.15, 0.3, 0.5, 0.65, 0.8, 0.9, 1.0,  0.0, }
  };

  GtkWidget *win = gtk_widget_get_toplevel(widget);
  
  gint width, height;
  gtk_window_get_size(GTK_WINDOW(win), &width, &height);

  cairo_translate(cr, width/2, height/2);

  gint i = 0;
  for (i = 0; i < 8; i++) {
      cairo_set_line_width(cr, 3);
      cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
      cairo_set_source_rgba(cr, 0, 0, 0, trs[count%8][i]);

      cairo_move_to(cr, 0.0, -10.0);
      cairo_line_to(cr, 0.0, -40.0);
      cairo_rotate(cr, M_PI/4);

      cairo_stroke(cr);
  }   
}

static gboolean time_handler(GtkWidget *widget)
{
  count += 1;
  if (!glob.timer) return FALSE;
  gtk_widget_queue_draw(widget);

  return TRUE;
}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *darea;

  gtk_init(&argc, &argv);
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
  gtk_window_set_title(GTK_WINDOW(window), "Gradients"); 

  glob.timer = TRUE;
  glob.alpha = 1.0;
  glob.size = 1.0;

  g_timeout_add(100, (GSourceFunc) time_handler, (gpointer) window);

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE9
static void do_drawing(cairo_t *cr, gint x, gint w, gint h, cairo_operator_t);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  cairo_operator_t oper[] = {
    CAIRO_OPERATOR_DEST_OVER,
    CAIRO_OPERATOR_DEST_IN,
    CAIRO_OPERATOR_OUT,
    CAIRO_OPERATOR_ADD,
    CAIRO_OPERATOR_ATOP,
    CAIRO_OPERATOR_DEST_ATOP,
  };

  gint width, height;
  GtkWidget *win = gtk_widget_get_toplevel(widget);

  gtk_window_get_size(GTK_WINDOW(win), &width, &height);

  gint i;
  gint x, y;
  for (x = 20, y = 20, i = 0; i < 6; x += 80, i++) {
    do_drawing(cr, x, width, height, oper[i]);
  }

  return FALSE;
}
static void do_drawing(cairo_t *cr, gint x, gint w, gint h, cairo_operator_t op)
{
  cairo_t *first_cr, *second_cr;
  cairo_surface_t *first, *second;

  first = cairo_surface_create_similar(cairo_get_target(cr),
      CAIRO_CONTENT_COLOR_ALPHA, w, h);
  second = cairo_surface_create_similar(cairo_get_target(cr),
      CAIRO_CONTENT_COLOR_ALPHA, w, h);

  first_cr = cairo_create(first);
  cairo_set_source_rgb(first_cr, 0, 0, 0.4);
  cairo_rectangle(first_cr, x, 20, 50, 50);
  cairo_fill(first_cr);

  second_cr = cairo_create(second);
  cairo_set_source_rgb(second_cr, 0.5, 0.5, 0);
  cairo_rectangle(second_cr, x+10, 40, 50, 50);
  cairo_fill(second_cr);

  cairo_set_operator(first_cr, op);
  cairo_set_source_surface(first_cr, second, 0, 0);
  cairo_paint(first_cr);

  cairo_set_source_surface(cr, first, 0, 0);
  cairo_paint(cr);

  cairo_surface_destroy(first);
  cairo_surface_destroy(second);

  cairo_destroy(first_cr);
  cairo_destroy(second_cr);

}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *darea;

  gtk_init(&argc, &argv);
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
  gtk_window_set_title(GTK_WINDOW(window), "Compositing operations"); 

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE10
static void do_drawing(cairo_t *cr, GtkWidget *); 

struct {
  cairo_surface_t *image;
} glob;

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  do_drawing(cr, widget);

  return FALSE;
}
static void do_drawing(cairo_t *cr, GtkWidget *widget)
{
  static gint pos_x = 128;
  static gint pos_y = 128;
  static gint radius = 40;
  static gint delta[] = {3, 3};

  GtkWidget *win = gtk_widget_get_toplevel(widget);

  gint width, height;
  gtk_window_get_size(GTK_WINDOW(win), &width, &height);

  if (pos_x < 0 + radius) {
    delta[0] = rand() % 4 + 5;
  } else if (pos_x > width - radius) {
    delta[0] = -(rand() % 4 + 5);
  }

  if (pos_y < 0 + radius) {
    delta[1] = rand() % 4 + 5;
  } else if (pos_y > width - radius) {
    delta[1] = -(rand() % 4 + 5);
  }

  pos_x += delta[0];
  pos_y += delta[1];

  cairo_set_source_surface(cr, glob.image, 1, 1);
  cairo_arc(cr, pos_x, pos_y, radius, 0, 2*M_PI);
  cairo_clip(cr);
  cairo_paint(cr);
}

static gboolean time_handler(GtkWidget *widget)
{
  gtk_widget_queue_draw(widget);
  return TRUE;
}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *darea;
  gint width, height;

  gtk_init(&argc, &argv);

  glob.image = cairo_image_surface_create_from_png("images/Seoul26.png");
  width = cairo_image_surface_get_width(glob.image);
  height = cairo_image_surface_get_height(glob.image);
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), width+2, height+2);
  gtk_window_set_title(GTK_WINDOW(window), "clip image"); 

  g_timeout_add(100, (GSourceFunc) time_handler, (gpointer) window);

  gtk_widget_show_all(window);

  gtk_main();

  cairo_surface_destroy(glob.image);

  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE11
static void do_drawing(cairo_t *cr, cairo_surface_t *); 

struct {
  cairo_surface_t *surface;
} glob;

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  do_drawing(cr, glob.surface);

  return FALSE;
}
static void do_drawing(cairo_t *cr, cairo_surface_t *surface)
{
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_mask_surface(cr, surface, 0, 0);
  cairo_fill(cr);
}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *darea;
  gint width, height;

  gtk_init(&argc, &argv);

  glob.surface = cairo_image_surface_create_from_png("images/South_korea.png");
  width = cairo_image_surface_get_width(glob.surface);
  height = cairo_image_surface_get_height(glob.surface);
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), width, height);
  gtk_window_set_title(GTK_WINDOW(window), "clip image"); 


  gtk_widget_show_all(window);

  gtk_main();

  cairo_surface_destroy(glob.surface);

  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE12
static void do_drawing(cairo_t *cr); 

struct {
  cairo_surface_t *image;
  cairo_surface_t *surface;
  gboolean  timer;
  gint  img_width;
  gint  img_height;
} glob;

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  do_drawing(cr);

  return FALSE;
}
static gboolean time_handler(GtkWidget *widget)
{
  if (!glob.timer) return FALSE;

  gtk_widget_queue_draw(widget);

  return TRUE;
}

static void do_drawing(cairo_t *cr)
{
  cairo_t *ic;
  static gint h = 0;
  
  ic = cairo_create(glob.surface);

  cairo_rectangle(ic, 0, 0, glob.img_width, h);
  cairo_fill(ic);

  h += 1;
  if (h == glob.img_height) glob.timer = FALSE;

  cairo_set_source_surface(cr, glob.image, 10, 10);
  cairo_mask_surface(cr, glob.surface, 10, 10);

  cairo_destroy(ic);
}

static void init_vars()
{
  glob.timer = TRUE;
  glob.image = cairo_image_surface_create_from_png("images/Seoul26.png");
  glob.img_width = cairo_image_surface_get_width(glob.image);
  glob.img_height = cairo_image_surface_get_height(glob.image);
  glob.surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
      glob.img_width, glob.img_height);
}

static void cleanup()
{
  cairo_surface_destroy(glob.image);
  cairo_surface_destroy(glob.surface); 
}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *darea;

  gtk_init(&argc, &argv);

  init_vars();
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), glob.img_width+20, glob.img_height+20);
  gtk_window_set_title(GTK_WINDOW(window), "blind down"); 

  g_timeout_add(15, (GSourceFunc)time_handler, (gpointer)window);

  gtk_widget_show_all(window);

  gtk_main();

  cleanup();

  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE13
static void do_drawing(cairo_t *cr, GtkWidget *widget); 

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  do_drawing(cr, widget);

  return FALSE;
}

int points[11][2] = { 
  { 0, 85 }, 
  { 75, 75 }, 
  { 100, 10 }, 
  { 125, 75 }, 
  { 200, 85 },
  { 150, 125 }, 
  { 160, 190 },
  { 100, 150 }, 
  { 40, 190 },
  { 50, 125 },
  { 0, 85 } 
};

static void do_drawing(cairo_t *cr, GtkWidget *widget)
{
  cairo_save(cr);
  cairo_set_source_rgb(cr, 0.2, 0.3, 0.8);
  cairo_rectangle(cr, 10, 10, 30, 30);
  cairo_fill(cr);

  cairo_translate(cr, 20, 20);
  cairo_set_source_rgb(cr, 0.8, 0.3, 0.2);
  cairo_rectangle(cr, 0, 0, 30, 30);
  cairo_fill(cr);

  cairo_translate(cr, 30, 30);
  cairo_set_source_rgb(cr, 0.8, 0.8, 0.2);
  cairo_rectangle(cr, 0, 0, 30, 30);
  cairo_fill(cr);

  cairo_translate(cr, 40, 40);
  cairo_set_source_rgb(cr, 0.3, 0.8, 0.8);
  cairo_rectangle(cr, 0, 0, 30, 30);
  cairo_fill(cr);


  cairo_translate(cr, 40, 40);

  cairo_save(cr);

  cairo_matrix_t matrix;

  cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);
  cairo_rectangle(cr, 20, 30, 80, 50);
  cairo_fill(cr);
  cairo_matrix_init(&matrix,
      1.0, 0.5,
      0.0, 1.0,
      0.0, 0.0);

  cairo_transform(cr, &matrix);
  cairo_rectangle(cr, 130, 30, 80, 50);
  cairo_fill(cr);

  cairo_restore(cr);

  //cairo_matrix_invert(&matrix);
  //cairo_transform(cr, &matrix);
  cairo_translate(cr, 200, 200);

  cairo_set_source_rgb(cr, 0.2, 0.3, 0.8);
  cairo_rectangle(cr, 0, 0, 90, 90);
  cairo_fill(cr);

  cairo_scale(cr, 0.6, 0.6);
  cairo_set_source_rgb(cr, 0.8, 0.3, 0.2);
  cairo_rectangle(cr, 30, 30, 90, 90);
  cairo_fill(cr);

  cairo_scale(cr, 0.8, 0.8);
  cairo_set_source_rgb(cr, 0.8, 0.8, 0.2);
  cairo_rectangle(cr, 50, 50, 90, 90);
  cairo_fill(cr);

  cairo_restore(cr);

  GtkWidget *win = gtk_widget_get_toplevel(widget);
  gint width, height;
  gtk_window_get_size(GTK_WINDOW(win), &width, &height);

  cairo_set_line_width(cr, 0.5);
  cairo_translate(cr, width/2, height/2);
  cairo_arc(cr, 0, 0, 120, 0, 2*M_PI);
  cairo_stroke(cr);

  gint i;
  for (i = 0; i < 36; i++) {
    cairo_save(cr);
    cairo_rotate(cr, i*M_PI/36);
    cairo_scale(cr, 0.3, 1);
    cairo_arc(cr, 0, 0, 120, 0, 2*M_PI);
    cairo_restore(cr);
    cairo_stroke(cr);
  }

  static gdouble angle = 0;
  static gdouble scale = 1;
  static gdouble delta = 0.01;

  cairo_save(cr);

  cairo_set_source_rgb(cr, 0, 0.44, 0.7);
  cairo_set_line_width(cr, 1);

  //cairo_translate(cr, width/2, height/2);
  cairo_rotate(cr, angle);
  cairo_scale(cr, scale, scale);

  for (i = 0; i < 10; i++) {
    cairo_line_to(cr, points[i][0], points[i][1]);
  }

  cairo_close_path(cr);
  cairo_fill(cr);
  cairo_stroke(cr);

  cairo_restore(cr);

  if (scale < 0.01) {
    delta = -delta;
  } else if (scale > 0.99) {
    delta = -delta;
  }

  scale += delta;
  angle += 0.01;
}

static gboolean time_handler(GtkWidget *widget)
{
  gtk_widget_queue_draw(widget);

  return TRUE;
}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *darea;

  gtk_init(&argc, &argv);

  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
  gtk_window_set_title(GTK_WINDOW(window), "transformation"); 

  g_timeout_add(10, (GSourceFunc) time_handler, (gpointer)window);

  gtk_widget_show_all(window);

  gtk_main();


  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE14
static void do_drawing(cairo_t *, GtkWidget *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{

  do_drawing(cr, widget);

  return FALSE;
}
static void do_drawing(cairo_t *cr, GtkWidget *widget)
{
  cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);

  cairo_select_font_face(cr, "Purisa",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);

  cairo_set_font_size(cr, 13);

  cairo_move_to(cr, 20, 30);
  cairo_show_text(cr, "Most relationships seem so transitory");  
  cairo_move_to(cr, 20, 60);
  cairo_show_text(cr, "They're all good but not the permanent one");

  cairo_move_to(cr, 20, 120);
  cairo_show_text(cr, "Who doesn't long for someone to hold");

  cairo_move_to(cr, 20, 150);
  cairo_show_text(cr, "Who knows how to love you without being told");
  cairo_move_to(cr, 20, 180);
  cairo_show_text(cr, "Somebody tell me why I'm on my own");
  cairo_move_to(cr, 20, 210);
  cairo_show_text(cr, "If there's a soulmate for everyone"); 

  GtkWidget *win = gtk_widget_get_toplevel(widget);

  cairo_text_extents_t extents;
  int width, height;
  gtk_window_get_size(GTK_WINDOW(win), &width, &height);

  cairo_select_font_face(cr, "Courier",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);

  cairo_set_font_size(cr, 60);

  cairo_text_extents(cr, "Aurora", &extents);

  cairo_move_to(cr, width/2 - extents.width/2, 250);
  cairo_show_text(cr,  "Aurora");

  cairo_select_font_face(cr, "Serif",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 50);

  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_move_to(cr, 20, 350);
  cairo_show_text(cr,  "Aurora");

  cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
  cairo_move_to(cr, 23, 353);
  cairo_show_text(cr,  "Aurora");

  cairo_pattern_t *pat;

  /*
  cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
  cairo_paint(cr);
  */

  gint h = 90;
  
  cairo_select_font_face(cr, "Serif", CAIRO_FONT_SLANT_ITALIC,
      CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, h);

  pat = cairo_pattern_create_linear(0, 15, 0, h*0.8);
  cairo_pattern_set_extend(pat, CAIRO_EXTEND_REPEAT);
  cairo_pattern_add_color_stop_rgb(pat, 0.0, 1, 0.6, 0);
  cairo_pattern_add_color_stop_rgb(pat, 0.5, 1, 0.3, 0);

  cairo_move_to(cr, 200, 350);
  cairo_text_path(cr,  "Aurora");
  cairo_set_source(cr, pat);
  cairo_fill(cr);

  cairo_select_font_face(cr, "Serif", CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 13);

  const int n_glyphs = 20 * 35;
  cairo_glyph_t glyphs[n_glyphs];

  gint i = 0;
  gint x, y;
  
  for (y = 0; y < 20; y++) {
    for (x = 0; x < 35; x++) {
      glyphs[i] = (cairo_glyph_t) {i, x*15 + 20, y*18 + 20};
      i++;
    }
  }

  cairo_set_source_rgb(cr, 0,0,0);
  cairo_translate(cr, 0, 400);
  cairo_show_glyphs(cr, glyphs, n_glyphs);
}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *darea;

  gtk_init(&argc, &argv);
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 800);
  gtk_window_set_title(GTK_WINDOW(window), "Gradients"); 

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE15
static void do_drawing(cairo_t *cr, GtkWidget *); 

struct {
  cairo_surface_t *image;
} glob;

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  do_drawing(cr, widget);

  return FALSE;
}
static void do_drawing(cairo_t *cr, GtkWidget *widget)
{
  cairo_set_source_surface(cr, glob.image, 10, 10);
  cairo_paint(cr);
}

static gboolean time_handler(GtkWidget *widget)
{
  gtk_widget_queue_draw(widget);
  return TRUE;
}

static void draw_mark()
{
  cairo_t *ic;
  ic = cairo_create(glob.image);
  cairo_set_font_size(ic, 11);

  cairo_set_source_rgb(ic, 0.9, 0.9, 0.9);
  cairo_move_to(ic, 20, 30);
  cairo_show_text(ic, " Aurora (TM) ");
  cairo_stroke(ic);
}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *darea;
  gint width, height;

  gtk_init(&argc, &argv);

  glob.image = cairo_image_surface_create_from_png("images/Seoul26.png");
  width = cairo_image_surface_get_width(glob.image);
  height = cairo_image_surface_get_height(glob.image);

  draw_mark();
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  darea = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), width+20, height+20);
  gtk_window_set_title(GTK_WINDOW(window), "Image"); 

  //g_timeout_add(100, (GSourceFunc) time_handler, (gpointer) window);

  gtk_widget_show_all(window);

  gtk_main();

  cairo_surface_destroy(glob.image);

  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE16
static void do_drawing(cairo_t *cr, GtkWidget *); 

static void tran_setup(GtkWidget *win)
{
  GdkScreen *screen;
  GdkVisual *visual;

  gtk_widget_set_app_paintable(win, TRUE);
  screen = gdk_screen_get_default();
  visual = gdk_screen_get_rgba_visual(screen);

  if (visual != NULL && gdk_screen_is_composited(screen)) {
    gtk_widget_set_visual(win, visual);
  }
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  do_drawing(cr, widget);

  return FALSE;
}
static void do_drawing(cairo_t *cr, GtkWidget *widget)
{
  cairo_set_source_rgba(cr, 0.2, 0.2, 0.2, 0.4);
  cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(cr);
}

static gboolean time_handler(GtkWidget *widget)
{
  gtk_widget_queue_draw(widget);
  return TRUE;
}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *darea;
  gint width = 640, height = 480;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  tran_setup(window);

  darea = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(window), darea);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

  g_signal_connect(G_OBJECT(darea), "draw",
      G_CALLBACK(on_draw_event), NULL);
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), width, height);
  gtk_window_set_title(GTK_WINDOW(window), "Transparent Window"); 

  //g_timeout_add(100, (GSourceFunc) time_handler, (gpointer) window);

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE17
int azm_gtk_sample_main(int argc, char **argv)
{
  gint width = 640, height = 480;

  gtk_init(&argc, &argv);

  GdkWindow *root_win = gdk_get_default_root_window();

  width = gdk_window_get_width(root_win);
  height = gdk_window_get_height(root_win);


  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
      width, height);

  GdkPixbuf *pb = gdk_pixbuf_get_from_window(root_win, 0, 0, width, height);

  cairo_t *cr = cairo_create(surface);
  gdk_cairo_set_source_pixbuf(cr, pb, 0, 0);
  cairo_paint(cr);

  cairo_surface_write_to_png(surface, "images/scrdump.png");

  cairo_destroy(cr);
  cairo_surface_destroy(surface);

  return 0;
}
#endif

#ifdef  AZM_GTK_SAMPLE18

static void do_drawing(cairo_t *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, 
    gpointer user_data)
{      
  do_drawing(cr);  

  return FALSE;
}

static void do_drawing(cairo_t *cr)
{
   cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
   cairo_paint(cr);
   cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
}

static void setup(GtkWidget *win)
{        
  gtk_widget_set_app_paintable(win, TRUE);
  gtk_window_set_type_hint(GTK_WINDOW(win), GDK_WINDOW_TYPE_HINT_DOCK);
  gtk_window_set_keep_below(GTK_WINDOW(win), TRUE);
  
  GdkScreen *screen = gdk_screen_get_default();
  GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
  
  if (visual != NULL && gdk_screen_is_composited(screen)) {
      gtk_widget_set_visual(win, visual);
  }  
}

int azm_gtk_sample_main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *lbl;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  
  setup(window);
  
  lbl = gtk_label_new("Aurora, Embedded Application Sofware Framework");
  
  PangoFontDescription *fd = pango_font_description_from_string("Serif 20");
  gtk_widget_modify_font(lbl, fd);  
  gtk_container_add(GTK_CONTAINER(window), lbl);  
  
  GdkColor color;
  gdk_color_parse("white", &color);
  gtk_widget_modify_fg(lbl, GTK_STATE_NORMAL, &color);
  
  g_signal_connect(G_OBJECT(window), "draw", 
      G_CALLBACK(on_draw_event), NULL); 
  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL);

  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 350, 250); 

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
#endif
/* === end of AZM_GTK_SAMPLE_C === */
