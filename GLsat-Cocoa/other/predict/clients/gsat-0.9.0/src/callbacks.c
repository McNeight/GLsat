/*
 * gsat - a realtime satellite tracking graphical frontend to predict
 *
 * Copyright (C) 2001 by Xavier Crehueras, EB3CZS
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Look at the README for more information on the program.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dlfcn.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "globals.h"

void show_status( const gchar *statusmsg );
int get_response(int sock, char *buf);
int connect_server( void );
void disconnect_server( void );
int open_downlink_plugin( char * plugin );
int open_uplink_plugin( char * plugin );
void search_plugins( void );
void save_preferences( void );

gboolean
mainwindow_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  gtk_main_quit();

  return FALSE;
}


gboolean
maparea_expose_event                   (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
  /* Copy internal buffer to screen area */
  gdk_draw_pixmap(widget->window,
		  widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		  drawmap,
		  event->area.x, event->area.y,
		  event->area.x, event->area.y,
		  event->area.width, event->area.height);

  return FALSE;
}

void
combo_entry1_changed                   (GtkEditable     *editable,
                                        gpointer         user_data)
{
  int i;

  /* copy original map to draw map */
  if( drawmap )
    gdk_draw_pixmap(drawmap,yellow_gc,sourcemap,0,0,0,0,MAPSIZEX,MAPSIZEY);

  /* Reset dots array */
  for( i=0; i<MAXDOTS; i++ ) {
    dots[i].x=0;
    dots[i].y=0;
  }
  ndots=0;
  newsat=1;
  lastel=0;

/*    doprediction=TRUE; */
}

void
on_cb_connect_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_set_sensitive( GTK_WIDGET( mainwindow ), FALSE );
  gtk_widget_set_sensitive( GTK_WIDGET( dialog_connect ), TRUE );
  gtk_widget_show( GTK_WIDGET( dialog_connect ) );
}


void
on_cb_disconnect_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget * widget;

  disconnect_server();
  gtk_widget_set_sensitive( GTK_WIDGET( button ), FALSE );
  widget=lookup_widget( mainwindow, "cb_connect" );
  gtk_widget_set_sensitive( widget, TRUE );
  widget=lookup_widget( mainwindow, "tb_footpsat" );
  gtk_widget_set_sensitive( widget, FALSE );
  widget=lookup_widget( mainwindow, "tb_footpqth" );
  gtk_widget_set_sensitive( widget, FALSE );
  widget=lookup_widget( mainwindow, "tb_track" );
  gtk_widget_set_sensitive( widget, FALSE );
}


void
on_tb_grid_toggled                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if( gtk_toggle_button_get_active( togglebutton ) )
    drawgrid=TRUE;
  else
    drawgrid=FALSE;
}


void
on_tb_utctime_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if( gtk_toggle_button_get_active( togglebutton ) )
    timeUTC=TRUE;
  else
    timeUTC=FALSE;
}


void
on_bt_preferences_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget * widget, * widget2;
  int state, n;

  /* refresh plugin list */
  search_plugins();

  /* set up downlink plugin widget */
  widget = lookup_widget( dialog_preferences, "entry_downplugin" );  
  gtk_entry_set_text(GTK_ENTRY(widget),plugindescriptions[0]);
  for(n=0; n<g_list_length( pluginlist ); n++)
    if( strncmp(prefs_down_plugin, pluginfiles[n],256)==0 ) {
      gtk_entry_set_text(GTK_ENTRY(widget),plugindescriptions[n]);
      break;
    }
  
  /* set up downlink plugin config widget */
  downpluginconfig=prefs_down_plugin_config;
  widget = lookup_widget( dialog_preferences, "tx_downpluginconfig" );
  gtk_entry_set_text(GTK_ENTRY(widget),downpluginconfig);

  /* set up uplink plugin widget */
  widget = lookup_widget( dialog_preferences, "entry_upplugin" );  
  gtk_entry_set_text(GTK_ENTRY(widget),plugindescriptions[0]);
  for(n=0; n<g_list_length( pluginlist ); n++)
    if( strncmp(prefs_up_plugin, pluginfiles[n],256)==0) {
      gtk_entry_set_text(GTK_ENTRY(widget),plugindescriptions[n]);
      break;
    }

  /* set up uplink plugin config widget */
  uppluginconfig=prefs_up_plugin_config;
  widget = lookup_widget( dialog_preferences, "tx_uppluginconfig" );
  gtk_entry_set_text(GTK_ENTRY(widget),uppluginconfig);

  /* set up grid widget */
  if( prefs_grid[0]=='1' ) {
    widget2 = lookup_widget( dialog_preferences, "rd_prefs_gridon" );
    state=TRUE;
  }
  else {
    widget2 = lookup_widget( dialog_preferences, "rd_prefs_gridoff" );
    state=FALSE;
  }
  widget=lookup_widget( mainwindow, "tb_grid" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), state );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget2 ), TRUE );

  /* set up utctime widget */
  if( prefs_utctime[0]=='1' ) {
    widget2 = lookup_widget( dialog_preferences, "rd_prefs_utc" );
    state=TRUE;
  }
  else {
    widget2 = lookup_widget( dialog_preferences, "rd_prefs_local" );
    state=FALSE;
  }
  widget=lookup_widget( mainwindow, "tb_utctime" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), state );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget2 ), TRUE );

  /* set up satellite footprint widget */
  if( prefs_satfootprint[0]=='1' ) {
    widget2 = lookup_widget( dialog_preferences, "rd_prefs_fpsaton" );
    state=TRUE;
  }
  else {
    widget2 = lookup_widget( dialog_preferences, "rd_prefs_fpsatoff" );
    state=FALSE;
  }
  widget=lookup_widget( mainwindow, "tb_footpsat" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), state );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget2 ), TRUE );

  /* set up qth footprint widget */
  if( prefs_qthfootprint[0]=='1' ) {
    widget2 = lookup_widget( dialog_preferences, "rd_prefs_fpqthon" );
    state=TRUE;
  }
  else {
    widget2 = lookup_widget( dialog_preferences, "rd_prefs_fpqthoff" );
    state=FALSE;
  }
  widget=lookup_widget( mainwindow, "tb_footpqth" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), state );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget2 ), TRUE );

  /* set up track widget */
  if( prefs_qthfootprint[0]=='1' ) {
    widget2 = lookup_widget( dialog_preferences, "rd_prefs_trackon" );
    state=TRUE;
  }
  else {
    widget2 = lookup_widget( dialog_preferences, "rd_prefs_trackoff" );
    state=FALSE;
  }
  widget=lookup_widget( mainwindow, "tb_track" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), state );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget2 ), TRUE );

  /* set up AOS and LOS command */
  widget = lookup_widget( dialog_preferences, "tx_aoscommand" );
  gtk_entry_set_text(GTK_ENTRY(widget),prefs_aos_command);
  widget = lookup_widget( dialog_preferences, "tx_loscommand" );
  gtk_entry_set_text(GTK_ENTRY(widget),prefs_los_command);

  /* Disable main window and show preferences dialog */
  gtk_widget_set_sensitive( GTK_WIDGET( mainwindow ), FALSE );
  gtk_widget_set_sensitive( GTK_WIDGET( dialog_preferences ), TRUE );
  gtk_widget_show( GTK_WIDGET( dialog_preferences ) );
}


void
on_bt_prefs_apply_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget * widget;
  char * plugin;
  int listidx;
  char filename[256];
  GList * list;
  int i,state;
  char * dummy;

  gtk_widget_set_sensitive( GTK_WIDGET( mainwindow ), TRUE );
  gtk_widget_set_sensitive( GTK_WIDGET( dialog_preferences ), FALSE );
  gtk_widget_hide( GTK_WIDGET( dialog_preferences ) );

  /* Setup downlink plugin */
  widget = lookup_widget( dialog_preferences, "entry_downplugin" );
  plugin=(char *)gtk_entry_get_text(GTK_ENTRY(widget));
  widget = lookup_widget( dialog_preferences, "tx_downpluginconfig" );
  downpluginconfig=(char *)gtk_entry_get_text(GTK_ENTRY(widget));

  list=g_list_first( pluginlist );
  listidx=0;
  for( i=0; i<g_list_length( pluginlist ); i++ ) {
    if(strncmp( list->data,plugin, 29 )==0) {
      listidx=i;
      break;
    }
    list=g_list_next( list );
  }
  if( listidx > 0 ) {
    strcpy( filename, pluginsdir );
    strcat( filename, pluginfiles[ listidx ] );
    open_downlink_plugin( filename );
  }
  strncpy( prefs_down_plugin, pluginfiles[ listidx ], 255 );
  strncpy( prefs_down_plugin_config, downpluginconfig, 255 );

  /* Setup uplink plugin */
  widget = lookup_widget( dialog_preferences, "entry_upplugin" );
  plugin=(char *)gtk_entry_get_text(GTK_ENTRY(widget));
  widget = lookup_widget( dialog_preferences, "tx_uppluginconfig" );
  uppluginconfig=(char *)gtk_entry_get_text(GTK_ENTRY(widget));

  list=g_list_first( pluginlist );
  listidx=0;
  for( i=0; i<g_list_length( pluginlist ); i++ ) {
    if(strncmp( list->data,plugin, 29 )==0) {
      listidx=i;
      break;
    }
    list=g_list_next( list );
  }
  if( listidx > 0 ) {
    strcpy( filename, pluginsdir );
    strcat( filename, pluginfiles[ listidx ] );
    open_uplink_plugin( filename );
  }
  strncpy( prefs_up_plugin, pluginfiles[ listidx ], 255 );
  strncpy( prefs_up_plugin_config, uppluginconfig, 255 );

  widget = lookup_widget( dialog_preferences, "rd_prefs_gridon" );
  if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) == TRUE ) {
    state=TRUE;
    prefs_grid[0]='1';
  }
  else {
    state=FALSE;
    prefs_grid[0]='0';
  }
  widget=lookup_widget( mainwindow, "tb_grid" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), state );

  widget = lookup_widget( dialog_preferences, "rd_prefs_utc" );
  if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) == TRUE ) {
    state=TRUE;
    prefs_utctime[0]='1';
  }
  else {
    state=FALSE;
    prefs_utctime[0]='0';
  }
  widget=lookup_widget( mainwindow, "tb_utctime" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), state );

  widget = lookup_widget( dialog_preferences, "rd_prefs_fpsaton" );
  if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) == TRUE ) {
    state=TRUE;
    prefs_satfootprint[0]='1';
  }
  else {
    state=FALSE;
    prefs_satfootprint[0]='0';
  }
  widget=lookup_widget( mainwindow, "tb_footpsat" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), state );

  widget = lookup_widget( dialog_preferences, "rd_prefs_fpqthon" );
  if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) == TRUE ) {
    state=TRUE;
    prefs_qthfootprint[0]='1';
  }
  else {
    state=FALSE;
    prefs_qthfootprint[0]='0';
  }
  widget=lookup_widget( mainwindow, "tb_footpqth" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), state );

  widget = lookup_widget( dialog_preferences, "rd_prefs_trackon" );
  if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) == TRUE ) {
    state=TRUE;
    prefs_track[0]='1';
  }
  else {
    state=FALSE;
    prefs_track[0]='0';
  }
  widget=lookup_widget( mainwindow, "tb_track" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), state );

  /* Setup AOS and LOS commands */
  widget = lookup_widget( dialog_preferences, "tx_aoscommand" );
  dummy=(char *)gtk_entry_get_text(GTK_ENTRY(widget));
  strncpy( prefs_aos_command, dummy, 1024 );
  widget = lookup_widget( dialog_preferences, "tx_loscommand" );
  dummy=(char *)gtk_entry_get_text(GTK_ENTRY(widget));
  strncpy( prefs_los_command, dummy, 1024 );

  save_preferences();
}


void
on_bt_prefs_cancel_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_set_sensitive( GTK_WIDGET( mainwindow ), TRUE );
  gtk_widget_set_sensitive( GTK_WIDGET( dialog_preferences ), FALSE );
  gtk_widget_hide( GTK_WIDGET( dialog_preferences ) );
}


void
on_bt_con_connect_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget * widget;

  gtk_widget_set_sensitive( GTK_WIDGET( mainwindow ), TRUE );
  gtk_widget_set_sensitive( GTK_WIDGET( dialog_connect ), FALSE );
  gtk_widget_hide( GTK_WIDGET( dialog_connect ) );

  widget = lookup_widget( dialog_connect, "tx_con_server" );
  predicthost=(char *)gtk_entry_get_text(GTK_ENTRY(widget));
  widget = lookup_widget( dialog_connect, "tx_con_port" );
  predictport=(char *)gtk_entry_get_text(GTK_ENTRY(widget));

  /* Setup status bar */
  sprintf(statusmsg,"Connecting to predict server %s ...", predicthost);
  show_status(statusmsg);

  if(connect_server()==0) {
    /* if connected, disable connect button enable disconnect button */
    widget=lookup_widget( mainwindow, "cb_connect" );
    gtk_widget_set_sensitive( widget, FALSE );
    widget=lookup_widget( mainwindow, "cb_disconnect" );
    gtk_widget_set_sensitive( widget, TRUE );
    widget=lookup_widget( mainwindow, "tb_footpsat" );
    gtk_widget_set_sensitive( widget, TRUE );
    widget=lookup_widget( mainwindow, "tb_footpqth" );
    gtk_widget_set_sensitive( widget, TRUE );
    widget=lookup_widget( mainwindow, "tb_track" );
    gtk_widget_set_sensitive( widget, TRUE );

    connected=TRUE;
  }
  else {
    show_status("Error connecting to predict server.");
    connected=FALSE;
  }
}


void
on_bt_con_cancel_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_set_sensitive( GTK_WIDGET( mainwindow ), TRUE );
  gtk_widget_set_sensitive( GTK_WIDGET( dialog_connect ), FALSE );
  gtk_widget_hide( GTK_WIDGET( dialog_connect ) );
}


gboolean
on_dialog_preferences_delete_event     (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  gtk_widget_set_sensitive( GTK_WIDGET( mainwindow ), TRUE );
  gtk_widget_set_sensitive( GTK_WIDGET( dialog_preferences ), FALSE );
  gtk_widget_hide( GTK_WIDGET( dialog_preferences ) );

  return TRUE;
}


gboolean
on_dialog_connect_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  gtk_widget_set_sensitive( GTK_WIDGET( mainwindow ), TRUE );
  gtk_widget_set_sensitive( GTK_WIDGET( dialog_connect ), FALSE );
  gtk_widget_hide( GTK_WIDGET( dialog_connect ) );

  return TRUE;
}


void
on_tb_footpsat_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if( gtk_toggle_button_get_active( togglebutton ) )
    satfootprint=TRUE;
  else
    satfootprint=FALSE;
}


void
on_tb_footpqth_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if( gtk_toggle_button_get_active( togglebutton ) )
    qthfootprint=TRUE;
  else
    qthfootprint=FALSE;
}


void
on_tb_track_toggled                    (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if( gtk_toggle_button_get_active( togglebutton ) )
    drawtrack=TRUE;
  else
    drawtrack=FALSE;
}


void
on_cb_updoppler_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if( gtk_toggle_button_get_active( togglebutton ) ) {
    if( uppluginenable==TRUE ) {
      if( (*plugin_open_rig_uplink)( uppluginconfig )==TRUE )
	enableupdoppler=TRUE;
    }
  }
  else {
    if( uppluginenable==TRUE ) {
      (*plugin_close_rig_uplink)();
      enableupdoppler=FALSE;
    }
  }
}


void
on_cb_dwdoppler_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if( gtk_toggle_button_get_active( togglebutton ) ) {
    if( downpluginenable==TRUE ) {
      if( (*plugin_open_rig_downlink)( downpluginconfig )==TRUE )
	enabledowndoppler=TRUE;
    }
  }
  else {
    if( downpluginenable==TRUE ) {
      (*plugin_close_rig_downlink)();
      enabledowndoppler=FALSE;
    }
  }
}
