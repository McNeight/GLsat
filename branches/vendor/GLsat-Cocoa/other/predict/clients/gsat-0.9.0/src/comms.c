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

/* Main processing loop and network functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>
#include <math.h>
#include <dlfcn.h>
#include <dirent.h>

#include "support.h"

#include "globals.h"

#ifndef PI
#define PI 3.141592653589793
#endif

/* some prototypes */
int connect_server( void );
void disconnect_server();

double arccos(x,y)
double x, y;
{
  /* This function implements the arccosine function,
     returning a value between 0 and two pi. */

  double result=0.0, fraction;

  fraction=x/y;

  if ((x>0.0) && (y>0.0))
    result=acos(fraction);

  if ((x<0.0) && (y>0.0))
    result=acos(fraction);

  if ((x<0.0) && (y<0.0))
    result=PI+acos(fraction);

  if ((x>0.0) && (y<0.0))
    result=PI+acos(fraction);

  return result;
}

/* Function to open a dialog box displaying error messages. */

void error_dialog(gchar *message)
{

  GtkWidget *dialog, *frame, *box, *vbox1, *vbox2, *vbox3, *label, *okay_button;

  /* Create the widgets */

  dialog = gtk_dialog_new();
  frame = gtk_frame_new(NULL);
  box = gtk_vbox_new( TRUE, 2 );
  vbox1 = gtk_vbox_new( FALSE, 2 );
  vbox2 = gtk_vbox_new( TRUE, 2 );
  vbox3 = gtk_vbox_new( FALSE, 2 );
  label = gtk_label_new (message);
  okay_button = gtk_button_new_with_label("Okay");

  /* Ensure that the dialog box is destroyed when the user clicks ok. */

  gtk_signal_connect_object (GTK_OBJECT (okay_button), "clicked",
			     GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT(dialog));
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->action_area),
		     okay_button);

  /* Add the label, and show everything we've added to the dialog. */

  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), frame);
  gtk_container_add (GTK_CONTAINER (frame), box);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 4);
  gtk_box_pack_start (GTK_BOX(box), vbox1, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX(box), vbox2, TRUE, TRUE, 2);
  gtk_box_pack_start (GTK_BOX(box), vbox3, FALSE, FALSE, 2);
  gtk_container_add (GTK_CONTAINER (vbox2), label);
  gtk_window_set_title( GTK_WINDOW(dialog),"gsat - ERROR");
  gtk_widget_show_all (dialog);
}

void show_status( const gchar * statusmsg )
{
  GtkWidget *widget;
  gint ctxid;

  widget = lookup_widget( mainwindow, "statusbar" );
  ctxid = gtk_statusbar_get_context_id(GTK_STATUSBAR(widget),"Status");
  gtk_statusbar_pop(GTK_STATUSBAR(widget),ctxid);
  gtk_statusbar_push(GTK_STATUSBAR(widget),ctxid,statusmsg);
}

/* Establish network conection */

int connectsock(char *host, char *service, char *protocol)
{
  /* This function is used to connect to the server.  "host" is the
     name host is the name of the computer the server program resides
     on.  "service" is the name of the socket port.  "protocol" is the
     socket protocol.  It should be set to UDP. */

  struct hostent *phe;
  struct servent *pse;
  struct protoent *ppe;
  struct sockaddr_in sin;

  int s, type;

  bzero((char *)&sin,sizeof(struct sockaddr_in));
  sin.sin_family=AF_INET;

  if ((pse=getservbyname(service,protocol)))
    sin.sin_port=pse->s_port;
  else if ((sin.sin_port=htons((unsigned short)atoi(service)))==0) {
    fprintf(stderr,"WARNING: Can't get predict services. Check /etc/services. Trying with default port.\n");
    sin.sin_port=htons(1210);
  }

  if ((phe=gethostbyname(host)))
    bcopy(phe->h_addr,(char *)&sin.sin_addr,phe->h_length);

  else if ((sin.sin_addr.s_addr=inet_addr(host))==INADDR_NONE) {
    fprintf(stderr,"ERROR: Can't get host address.\n");
    error_dialog("ERROR: Can't get host address.\n");
    return -1;
  }

  if ((ppe=getprotobyname(protocol))==0)
    return -1;

  if (strcmp(protocol,"udp")==0)
    type=SOCK_DGRAM;
  else
    type=SOCK_STREAM;

  s=socket(PF_INET,type,ppe->p_proto);

  if (s<0) {
    fprintf(stderr,"ERROR: Can't get socket.\n");
    return -1;
  }

  if (connect(s,(struct sockaddr *)&sin,sizeof(sin))<0) {
    fprintf(stderr,"ERROR: Can't connect to socket.\n");
    return -1;
  }

  return s;
}

/* Send the command to the server */

int send_command(int sock, char *command)
{
  int len;

  len=write(sock,command,strlen(command));

  return len;
}

/* Read from network socket */

int read_socket(int sock, char *buf)
{
  int n;
  struct timeval tv;
  fd_set rfds;
  time_t now;

  FD_ZERO( &rfds );
  FD_SET( sock, &rfds );
  tv.tv_sec=NETTIMEOUT;
  tv.tv_usec=0;

  if( select(sock+1,&rfds,NULL,NULL,&tv))
    n=read(sock,buf,NETBUFSIZE);
/*      n=recvfrom(sock,buf,strlen(buf),0,NULL,NULL); */
  else {
    now=time(NULL);
    fprintf(stderr,"Timeout reading from socket.\n");
    return -1;
  }

  buf[n]='\0';

  return n;
}

/* Get response to command from server */

int get_response(int sock, char *buf)
{
  int len;
  int timeouts;

  len=0;
  timeouts=0;
  while(timeouts<MAXTIMEOUTS) {
    if((len=read_socket(sock,buf))<0) {
      timeouts++;
      if(timeouts==MAXTIMEOUTS) {
	fprintf(stderr,"ERROR: predict server %s is not responding to commands.\n", predicthost);
	return -1;
      }
    }
    else
      break;
  }

  return len;
}

/* Plugin functions */
void close_uplink_plugin( void )
{
  if(uppluginenable==FALSE)
    return;

  /* close rig if it was open before closing plugin */
  if( enableupdoppler==TRUE ) {
    (*plugin_close_rig_uplink)();
    enableupdoppler=FALSE;
  }

  dlclose(upplugin_handle);

  uppluginenable=FALSE;
}

int open_uplink_plugin( char * plugin )
{
  char * error;

  /* if it's already open, close it first */
  if( uppluginenable==TRUE )
    close_uplink_plugin();

  upplugin_handle=dlopen( plugin, RTLD_NOW );
  if( upplugin_handle==NULL )
    if( (error=dlerror()) != NULL ) {
      fprintf(stderr,"Error opening downlink plugin: %s\n",error);
      return FALSE;
    }

  plugin_info_uplink=dlsym(upplugin_handle,"plugin_info");
  if( plugin_info_uplink==NULL )
    if( (error=dlerror()) != NULL ) {
      fprintf(stderr,"Error: plugin_info_uplink: %s\n",error);
      return FALSE;
    }

  plugin_open_rig_uplink=dlsym(upplugin_handle,"plugin_open_rig");
  if( plugin_open_rig_uplink==NULL )
    if( (error=dlerror()) != NULL ) {
      fprintf(stderr,"Error: plugin_open_rig_uplink: %s\n",error);
      return FALSE;
    }

  plugin_close_rig_uplink=dlsym(upplugin_handle,"plugin_close_rig");
  if( plugin_close_rig_uplink==NULL )
    if( (error=dlerror()) != NULL ) {
      fprintf(stderr,"Error: plugin_close_rig_uplink: %s\n",error);
      return FALSE;
    }

  plugin_set_uplink_frequency=dlsym(upplugin_handle,"plugin_set_uplink_frequency");
  if( plugin_set_uplink_frequency==NULL )
    if( (error=dlerror()) != NULL ) {
      fprintf(stderr,"Error: plugin_set_uplink_frequency: %s\n",error);
      return FALSE;
    }

  uppluginenable=TRUE;

  return TRUE;
}

void close_downlink_plugin( void )
{
  if(downpluginenable==FALSE)
    return;

  /* close rig if it was open before closing plugin */
  if( enabledowndoppler==TRUE ) {
    (*plugin_close_rig_downlink)();
    enabledowndoppler=FALSE;
  }

  dlclose(downplugin_handle);

  downpluginenable=FALSE;
}

int open_downlink_plugin( char * plugin )
{
  char * error;

  /* if it's already open, close it first */
  if( downpluginenable==TRUE )
    close_downlink_plugin();

  downplugin_handle=dlopen( plugin, RTLD_NOW );
  if( downplugin_handle==NULL )
    if( (error=dlerror()) != NULL ) {
      fprintf(stderr,"Error opening downlink plugin: %s\n",error);
      return FALSE;
    }

  plugin_info_downlink=dlsym(downplugin_handle,"plugin_info");
  if( plugin_info_downlink==NULL )
    if( (error=dlerror()) != NULL ) {
      fprintf(stderr,"Error: plugin_info_downlink: %s\n",error);
      return FALSE;
    }
  plugin_open_rig_downlink=dlsym(downplugin_handle,"plugin_open_rig");
  if( plugin_open_rig_downlink==NULL )
    if( (error=dlerror()) != NULL ) {
      fprintf(stderr,"Error: plugin_open_rig_downlink: %s\n",error);
      return FALSE;
    }
  plugin_close_rig_downlink=dlsym(downplugin_handle,"plugin_close_rig");
  if( plugin_close_rig_downlink==NULL )
    if( (error=dlerror()) != NULL ) {
      fprintf(stderr,"Error: plugin_close_rig_downlink: %s\n",error);
      return FALSE;
    }
  plugin_set_downlink_frequency=dlsym(downplugin_handle,"plugin_set_downlink_frequency");
  if( plugin_set_downlink_frequency==NULL )
    if( (error=dlerror()) != NULL ) {
      fprintf(stderr,"Error: plugin_set_downlink_frequency: %s\n",error);
      return FALSE;
    }

  downpluginenable=TRUE;

  return TRUE;
}

int check_plugin_file( const struct dirent * plugdir )
{
  return ! strncmp(plugdir->d_name,"plugin_",7);
}

void search_plugins( void )
{
  struct dirent **namelist;
  int n, i;
  GtkWidget * widget;
  char filename[256];
  DIR * dir;

  /* empty plugin list if it was created */
  n = g_list_length(pluginlist);
  if( n > 0 ) {
    pluginlist=g_list_first( pluginlist );
    for( i=0; i<n; i++ )
      pluginlist=g_list_remove( pluginlist, pluginlist->data );
    for( i=0; i<n; i++ ) {
      pluginfiles[i][0]='\0';
      plugindescriptions[i][0]='\0';
    }
  }

  strcpy( pluginfiles[0], "None");
  strcpy( plugindescriptions[0], "No Plugin");
  pluginlist=g_list_append( pluginlist, plugindescriptions[0] );
  
  /* check plugin directory */
  if( (dir=opendir( pluginsdir ))==NULL ) {
    switch( errno ) {
    case EACCES:
      fprintf(stderr, "Error: You don't have access to plugin directory.\n");
      error_dialog("You don't have access to plugin directory.");
      break;
    case ENOENT:
      fprintf(stderr, "Error: Plugin directory doesn't exist.\n");
      error_dialog("Plugin directory doesn't exist.");
      break;
    case ENOTDIR:
      fprintf(stderr, "Error: Plugin directory is not a directory.\n");
      error_dialog("Can't open plugin directory.");
      break;
    default:
      fprintf(stderr, "Error: Can't open plugin directory.\n");
      error_dialog("Can't open plugin directory.");
      break;
    }
    /* Attach plugin list */
    widget=lookup_widget( dialog_preferences, "combo_upplugin" );
    gtk_combo_set_popdown_strings( GTK_COMBO(widget), pluginlist);
    widget=lookup_widget( dialog_preferences, "combo_downplugin" );
    gtk_combo_set_popdown_strings( GTK_COMBO(widget), pluginlist);
    return;
  }
  closedir( dir );

  /* scan plugin directory and create plugin list */
  n = scandir(pluginsdir, &namelist, check_plugin_file, alphasort);
  if (n < 0) {
    perror("scandir");
    return;
  }

  for( i=0; i<n; i++ ) {
    strcpy(filename, pluginsdir);
    strcat(filename, namelist[i]->d_name);
    if(open_downlink_plugin(filename)==TRUE) {
      strncpy( pluginfiles[i+1], namelist[i]->d_name, 29);
      strncpy( plugindescriptions[i+1], (*plugin_info_downlink)(), 29);
      pluginlist=g_list_append( pluginlist, plugindescriptions[i+1] );
      close_downlink_plugin();
    }
  }

  /* Attach plugin list */
  widget=lookup_widget( dialog_preferences, "combo_upplugin" );
  gtk_combo_set_popdown_strings( GTK_COMBO(widget), pluginlist);
  widget=lookup_widget( dialog_preferences, "combo_downplugin" );
  gtk_combo_set_popdown_strings( GTK_COMBO(widget), pluginlist);
}

/* Get prediction data */

#define BUFSIZE 60*MAXDOTS

void get_orbitdata(char *satname)
{
  int i,cnt,sx,sy;
  long int begin,now;
  float slat,slong,saz,sel,phase;
  char buf[BUFSIZE],weekday[4],adate[8],atime[9];

  /* Get satellite data */
  /* Build a command buffer */
  now=time(NULL);
  begin=now-MAXDOTS*60;
  cnt=0;
  
  fprintf(stderr,"Now: %ld From: %ld\n",now,now-MAXDOTS);

  sprintf(buf,"GET_SAT_POS %s %ld +90m\n",satname,begin);
  fprintf(stderr,"Command: %s",buf);

  /* Send the command to the server */
  send_command(netsocket,buf);

  /* Calculate sub-satellite points */

  for( i=0; i<MAXDOTS+1; i++ ) {
    /* Get the response */
    if(get_response(netsocket,buf)==-1) {
      error_dialog("Too many errors receiving response from server, disconnecting");
      disconnect_server();
    }

    if(buf[0]==26)
      break;
    fprintf(stderr,"Index: %d Data: %s",i,buf);

    sscanf(buf,"%ld %s %s %s %f %f %f %f %f",&now,weekday,adate,atime,
	   &slat,&slong,&phase,&saz,&sel);

    if(slong>180.0) {
      slong = slong - 180.0;
      sx = (int)(MAPSIZEX - (slong * MAPSIZEX / 360.0));
    }
    else {
      sx = (int)((MAPSIZEX/2.0) - (slong * MAPSIZEX / 360.0));
    }
    sy = (int)((MAPSIZEY/2.0) - (slat * MAPSIZEY / 180.0));

    /* do not draw repeated dots in the orbit */
    if( dots[cnt-1].x!=sx || dots[cnt-1].y!=sy ) {
      dots[cnt].x=sx;
      dots[cnt].y=sy;
      cnt++;
    }
  }

  fprintf(stderr,"Count: %d\n",cnt);
  ndots=cnt;
}


/* Main processing loop */

gint
timeout_callback( gpointer data )
{
  int i, j, azi, ctxid;
  char buf[NETBUFSIZE], buf2[NETBUFSIZE], buf3[NETBUFSIZE],
    satname[26], *sat, callsign[16], param[30], *dopplerptr;
  long aostime,orbitnumber;
  float az, el, slong, slat, footprint, range, doppler, altitude,
    velocity, qthlat, qthlong, qthalt;
  time_t t, now;
  GtkWidget *widget;
  double 	uplink, downlink, updoppler, downdoppler, dopplershiftup,
    dopplershiftdown, rangelat, rangelong, azimuth, ssplat,
    ssplong, TWOPI, HALFPI, deg2rad=1.74532925199e-02,
    R0=6378.16, beta, num, dem;
  GdkRectangle updatewin;
  gint sx, sy, qthx, qthy;
  GdkPoint footprintdots[360], qthfootprintdots[360];
  char visible;
  static int flash;

  TWOPI=2.0*PI;
  HALFPI=PI/2.0;

  /* Show date and time */
  now = time(NULL);
  if( timeUTC )
    sprintf(param,"%s",asctime(gmtime(&now)));
  else
    sprintf(param,"%s",asctime(localtime(&now)));
  widget = lookup_widget( data, "tx_date" );
  gtk_entry_set_text(GTK_ENTRY(widget),param);

  /* if we are not connected, don't do anything more */
  if( connected == FALSE )
    return TRUE; /* allow more timeouts to ocur */

  /* Get Satellite name from combo entry */
  widget = lookup_widget( data, "combo" );
  sat = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(widget)->entry));
  strncpy( satname, sat, 25 );
  satname[ 26 ] = '\0';

/*    if(doprediction==TRUE) { */
/*      get_orbitdata(satname); */
/*      doprediction=FALSE; */
/*    } */

  /* Get satellite data */
  /* Build a command buffer */
  sprintf(buf,"GET_SAT %s\n",satname);

  /* Send the command to the server */
  send_command(netsocket,buf);

  /* Get the response */
  if(get_response(netsocket,buf)==-1) {
    error_dialog("Too many errors receiving response from server, disconnecting");
    disconnect_server();
    return TRUE;
  }

  /* Get doppler data */
  /* Build a command buffer */
  sprintf(buf2,"GET_DOPPLER %s\n",satname);

  /* Send the command to the server */
  send_command(netsocket,buf2);

  /* Get the response */
  if(get_response(netsocket,buf2)==-1) {
    error_dialog("Too many errors receiving response from server, disconnecting");
    disconnect_server();
    return TRUE;
  }

  /* Parse the satellite data */

  /* The first element of the response is the satellite name.
     It is ended with a '\n' character and may contain spaces. */

  for (i=0; buf[i]!='\n'; i++)
    satname[i]=buf[i];

  satname[i]=0;
  i++;

  /* The rest of the data can be parsed using the sscanf()
     function.  First, the satellite name is removed from
     "buf", and then "buf" is parsed for numerical data
     using an sscanf(). */

  for (j=0; buf[i+j]!=0; j++)
    buf[j]=buf[i+j];

  buf[j]=0;
 
  sscanf(buf,"%f %f %f %f %ld %f %f %f %f %ld %c",
	 &slong, &slat, &az, &el, &aostime, &footprint, &range,
	 &altitude, &velocity, &orbitnumber, &visible);

  /* Get doppler shift */
  sscanf(buf2,"%f",&doppler );

  /* Get qth data */
  /* Build a command buffer */
  sprintf(buf3,"GET_QTH\n");

  /* Send the command to the server */
  send_command(netsocket,buf3);

  /* Get the response */
  if(get_response(netsocket,buf3)==-1) {
    error_dialog("Too many errors receiving response from server, disconnecting");
    disconnect_server();
    return TRUE;
  }

  /* Parse the satellite data */

  /* The first element of the response is the callsign.
     It is ended with a '\n' character and may contain spaces. */

  for (i=0; buf3[i]!='\n'; i++)
    callsign[i]=buf3[i];

  callsign[i]=0;
  i++;

  /* The rest of the data can be parsed using the sscanf()
     function.  First, the callsign is removed from
     "buf3", and then "buf3" is parsed for numerical data
     using an sscanf(). */

  for (j=0; buf3[i+j]!=0; j++)
    buf3[j]=buf3[i+j];

  buf3[j]=0;
 
  sscanf(buf3,"%f %f %f",&qthlat,&qthlong,&qthalt);

  /* Now display all the satellite data on the widgets */
  sprintf(param,"%7.2f %c",(slong>180?360.0-slong:slong),(slong>180?'E':'W'));
  widget = lookup_widget( data, "tx_longitude" );
  gtk_entry_set_text(GTK_ENTRY(widget),param);

  sprintf(param,"%7.2f %c",fabs((double)slat),(slat<0?'S':'N'));
  widget = lookup_widget( data, "tx_latitude" );
  gtk_entry_set_text(GTK_ENTRY(widget),param);

  sprintf(param,"%7.2f",az);
  widget = lookup_widget( data, "tx_azimuth" );
  gtk_entry_set_text(GTK_ENTRY(widget),param);

  sprintf(param,"%+-6.2f",el);
  widget = lookup_widget( data, "tx_elevation" );
  gtk_entry_set_text(GTK_ENTRY(widget),param);

  sprintf(param,"%7.2f km",footprint);
  widget = lookup_widget( data, "tx_footprint" );
  gtk_entry_set_text(GTK_ENTRY(widget),param);

  sprintf(param,"%7.2f km",range);
  widget = lookup_widget( data, "tx_range" );
  gtk_entry_set_text(GTK_ENTRY(widget),param);

  sprintf(param,"%7.2f km",altitude);
  widget = lookup_widget( data, "tx_altitude" );
  gtk_entry_set_text(GTK_ENTRY(widget),param);

  sprintf(param,"%7.2f km/h",velocity);
  widget = lookup_widget( data, "tx_velocity" );
  gtk_entry_set_text(GTK_ENTRY(widget),param);

  sprintf(param,"%ld",orbitnumber);
  widget = lookup_widget( data, "tx_orbit" );
  gtk_entry_set_text(GTK_ENTRY(widget),param);

  /* change label of AOS widget to AOS/LOS */
  widget = lookup_widget( data, "lb_aos" );
  if( el>=0 )
    gtk_label_set_text(GTK_LABEL(widget),"    LOS at");
  else
    gtk_label_set_text(GTK_LABEL(widget),"Next AOS");

  t=(time_t)aostime;
  if( timeUTC )
    sprintf(param,"%s",asctime(gmtime(&t)));
  else
    sprintf(param,"%s",asctime(localtime(&t)));
  widget = lookup_widget( data, "tx_aos" );
  gtk_entry_set_text(GTK_ENTRY(widget),param);

  /* execute AOS and LOS commands */
  if( newsat==0 ) {
    if( (lastel < 0 && el >= 0)||(lastel >=0 && el < 0) ) {
      if( el >= 0 ) {
	if(strlen(prefs_aos_command)>0)
	  system(prefs_aos_command);
      }
      else {
	if(strlen(prefs_los_command)>0)
	  system(prefs_los_command);
      }
    }
  }
  newsat=0;
  lastel=el;

  /* Show doppler correction if elevation > 0 */
  if(el >= 0) {
    widget = lookup_widget( data, "tx_uplink" );
    dopplerptr=(gchar *)gtk_entry_get_text(GTK_ENTRY(widget));
    uplink=atof(dopplerptr);
    if(uplink==0.0)
      uplink=100000.0;
    dopplershiftup=(-uplink)/100000.0*doppler;
    sprintf(param,"%7.2f Hz",dopplershiftup);
    widget = lookup_widget( data, "tx_upshift" );
    gtk_entry_set_text(GTK_ENTRY(widget),param);
    updoppler=uplink+(dopplershiftup/1000.0);
    sprintf(param,"%7.2f kHz",updoppler);
    widget = lookup_widget( data, "tx_updoppler" );
    gtk_entry_set_text(GTK_ENTRY(widget),param);

    /* Set Uplink frequency on the radio */
    if( enableupdoppler==TRUE )
      (*plugin_set_uplink_frequency)(updoppler);

    widget = lookup_widget( data, "tx_downlink" );
    dopplerptr=(gchar *)gtk_entry_get_text(GTK_ENTRY(widget));
    downlink=atof(dopplerptr);
    if(downlink==0.0)
      downlink=100000.0;
    dopplershiftdown=downlink/100000.0*doppler;
    sprintf(param,"%7.2f Hz",dopplershiftdown);
    widget = lookup_widget( data, "tx_downshift" );
    gtk_entry_set_text(GTK_ENTRY(widget),param);
    downdoppler=downlink+(dopplershiftdown/1000.0);
    sprintf(param,"%7.2f kHz",downdoppler);
    widget = lookup_widget( data, "tx_dwdoppler" );
    gtk_entry_set_text(GTK_ENTRY(widget),param);

    /* Set Downlink frequency on the radio */
    if( enabledowndoppler==TRUE )
      (*plugin_set_downlink_frequency)(downdoppler);
  }
  else {
    widget = lookup_widget( data, "tx_upshift" );
    gtk_entry_set_text(GTK_ENTRY(widget),"--------");
    widget = lookup_widget( data, "tx_updoppler" );
    gtk_entry_set_text(GTK_ENTRY(widget),"--------");

    widget = lookup_widget( data, "tx_downshift" );
    gtk_entry_set_text(GTK_ENTRY(widget),"--------");
    widget = lookup_widget( data, "tx_dwdoppler" );
    gtk_entry_set_text(GTK_ENTRY(widget),"--------");
  }

  /* Show satellite visibility in 2nd status bar */
  widget = lookup_widget( data, "satbar" );
  ctxid = gtk_statusbar_get_context_id(GTK_STATUSBAR(widget),"Status");
  switch( visible ) {
  case 'D':
    gtk_statusbar_pop(GTK_STATUSBAR(widget),ctxid);
    gtk_statusbar_push(GTK_STATUSBAR(widget),ctxid,
		       "Satellite is in daylight");
    break;
  case 'N':
    gtk_statusbar_pop(GTK_STATUSBAR(widget),ctxid);
    gtk_statusbar_push(GTK_STATUSBAR(widget),ctxid,
		       "Satellite is in darkness");
    break;
  case 'V':
    gtk_statusbar_pop(GTK_STATUSBAR(widget),ctxid);
    gtk_statusbar_push(GTK_STATUSBAR(widget),ctxid,
		       "Satellite is visible");
    break;
  default:
    gtk_statusbar_pop(GTK_STATUSBAR(widget),ctxid);
    gtk_statusbar_push(GTK_STATUSBAR(widget),ctxid,
		       "No visibility information available");
  }

  /* Graphical representation */
  
  /* Range Circle Calculations by KD2BD */

  if( satfootprint ) {
    ssplat=slat*deg2rad;
    ssplong=slong*deg2rad;
    beta=(0.5*footprint)/R0;

    for (azi=0; azi<360; azi++)
      {
	azimuth=deg2rad*(double)azi;
	rangelat=asin(sin(ssplat)*cos(beta)+cos(azimuth)*sin(beta)*cos(ssplat));
	num=cos(beta)-(sin(ssplat)*sin(rangelat));
	dem=cos(ssplat)*cos(rangelat);

	if (azi==0 && (beta > HALFPI-ssplat))
	  rangelong=ssplong+PI;

	else if (azi==180 && (beta > HALFPI+ssplat))
	  rangelong=ssplong+PI;

	else if (fabs(num/dem)>1.0)
	  rangelong=ssplong;

	else
	  {
	    if ((180-azi)>=0)
	      rangelong=ssplong-arccos(num,dem);
	    else
	      rangelong=ssplong+arccos(num,dem);
	  }

	while (rangelong<0.0)
	  rangelong+=TWOPI;

	while (rangelong>(2.0*PI))
	  rangelong-=TWOPI;
 
	rangelat=rangelat/deg2rad;
	rangelong=rangelong/deg2rad;

	/* Convert range circle data to map-based
	   coordinates and draw on map */

	if (rangelong>180.0)
	  {
	    rangelong=rangelong-180.0;
	    sx=(int)(MAPSIZEX-(rangelong*MAPSIZEX/360.0));
	  }

	else
	  sx=(int)((MAPSIZEX/2.0)-(rangelong*MAPSIZEX/360.0));

	sy=(int)((MAPSIZEY/2.0)-(rangelat*MAPSIZEY/180.0));

	/* store position for Plot Range Circle */
	footprintdots[azi].x=sx;
	footprintdots[azi].y=sy;
      }
  }

  if( qthfootprint ) {
    ssplat=qthlat*deg2rad;
    ssplong=qthlong*deg2rad;
    beta=(0.5*footprint)/R0;

    for (azi=0; azi<360; azi++)
      {
	azimuth=deg2rad*(double)azi;
	rangelat=asin(sin(ssplat)*cos(beta)+cos(azimuth)*sin(beta)*cos(ssplat));
	num=cos(beta)-(sin(ssplat)*sin(rangelat));
	dem=cos(ssplat)*cos(rangelat);

	if (azi==0 && (beta > HALFPI-ssplat))
	  rangelong=ssplong+PI;

	else if (azi==180 && (beta > HALFPI+ssplat))
	  rangelong=ssplong+PI;

	else if (fabs(num/dem)>1.0)
	  rangelong=ssplong;

	else
	  {
	    if ((180-azi)>=0)
	      rangelong=ssplong-arccos(num,dem);
	    else
	      rangelong=ssplong+arccos(num,dem);
	  }

	while (rangelong<0.0)
	  rangelong+=TWOPI;

	while (rangelong>(2.0*PI))
	  rangelong-=TWOPI;

	rangelat=rangelat/deg2rad;
	rangelong=rangelong/deg2rad;

	/* Convert range circle data to map-based
	   coordinates and draw on map */

	if (rangelong>180.0)
	  {
	    rangelong=rangelong-180.0;
	    sx=(int)(MAPSIZEX-(rangelong*MAPSIZEX/360.0));
	  }

	else
	  sx=(int)((MAPSIZEX/2.0)-(rangelong*MAPSIZEX/360.0));

	sy=(int)((MAPSIZEY/2.0)-(rangelat*MAPSIZEY/180.0));

	/* store position for Plot Range Circle */
	qthfootprintdots[azi].x=sx;
	qthfootprintdots[azi].y=sy;
      }
  }

  /* First draw the empty map */
  gdk_draw_pixmap( drawmap, yellow_gc, sourcemap, 0, 0, 0, 0, 700, 350 );

  /* Draw grid if needed */
  if(drawgrid==TRUE) {
    widget=lookup_widget(data,"maparea");
    for(i=-90; i<90; i=i+30) {
      sy = (int)((MAPSIZEY/2.0) - (i * MAPSIZEY / 180.0));
      gdk_draw_line(drawmap,widget->style->black_gc,0,sy,MAPSIZEX,sy);
    }
    for(i=-180; i<180; i=i+30) {
      sx = (int)((MAPSIZEX/2.0) - (i * MAPSIZEX / 360.0));
      gdk_draw_line(drawmap,widget->style->black_gc,sx,0,sx,MAPSIZEY);
    }
  }

  /* Draw orbit */
  if( drawtrack == TRUE )
    if( ndots>0 )
      gdk_draw_points(drawmap,red_gc,dots,ndots);

  /* Draw footprints */
  if( satfootprint==TRUE)
    gdk_draw_points(drawmap,yellow_gc,footprintdots,360);
  if( qthfootprint==TRUE)
    gdk_draw_points(drawmap,cyan_gc,qthfootprintdots,360);

  /* Plot sub-satellite point */

  if(slong>180.0) {
    slong = slong - 180.0;
    sx = (int)(MAPSIZEX - (slong * MAPSIZEX / 360.0));
  }
  else {
    sx = (int)((MAPSIZEX/2.0) - (slong * MAPSIZEX / 360.0));
  }
  sy = (int)((MAPSIZEY/2.0) - (slat * MAPSIZEY / 180.0));

  /* do not draw repeated dots in the orbit */
  if( dots[ndots-1].x!=sx || dots[ndots-1].y!=sy ) {
    if( ndots<MAXDOTS ) {
      dots[ndots].x=sx;
      dots[ndots].y=sy;
      ndots++;
    }
    else {
      for( i=0;i<MAXDOTS-1;i++ ) {
	dots[i].x=dots[i+1].x;
	dots[i].y=dots[i+1].y;
      }
      dots[MAXDOTS-1].x=sx;
      dots[MAXDOTS-1].y=sy;
    }
  }

  /* Plot qth point */
  if(qthlong>180.0) {
    qthlong = qthlong - 180.0;
    qthx = (int)(MAPSIZEX - (qthlong * MAPSIZEX / 360.0));
  }
  else {
    qthx = (int)((MAPSIZEX/2.0) - (qthlong * MAPSIZEX / 360.0));
  }
  qthy = (int)((MAPSIZEY/2.0) - (qthlat * MAPSIZEY / 180.0));

  gdk_draw_rectangle( drawmap, cyan_gc, TRUE, qthx-1, qthy-1, 3, 3 );
  gdk_draw_string( drawmap, drawfont, cyan_gc, qthx+4, qthy+4, callsign );

  /* the flashing square */
  if( flash==0 ) {
    gdk_draw_rectangle( drawmap, purple_gc, TRUE, sx-1, sy-1, 3, 3 );
    flash=1;
  }
  else {
    gdk_draw_rectangle( drawmap, yellow_gc, TRUE, sx-1, sy-1, 3, 3 );
    flash=0;
  }

  /* Force map redraw */
  widget = lookup_widget( data, "maparea" );
  updatewin.x = 0;
  updatewin.y = 0;
  updatewin.width = widget->allocation.width;
  updatewin.height = widget->allocation.height;
  gtk_widget_draw ( widget, &updatewin);

  return TRUE;
}

/* Connect to predict server */

int connect_server( void )
{
  GtkWidget * widget;
  int x, y, z, i;
  char bufr[256];

  /* Get satellite list from server */
  netsocket=connectsock(predicthost,predictport,"udp");

  if (netsocket<0) {
    fprintf(stderr, "ERROR: Can't connect to the predict server on %s.\n", predicthost);
    error_dialog("Can't connect to the predict server.");
    // exit(-1);
    return 1;
  }

  send_command(netsocket,"GET_LIST");

  if(get_response(netsocket,bufr)==-1) {
    error_dialog("Too many errors receiving response from server, disconnecting");
    disconnect_server();
    return 1;
  }

  /* Parse the response and place each name
     in the character array satname[]. */

  for (x=0, y=0, z=0; y<strlen(bufr); y++)
    {
      if (bufr[y]!='\n')
	{
	  satname[z][x]=bufr[y];
	  x++;
	}

      else
	{
	  satname[z][x]=0;
	  z++;
	  x=0;
	}

    }

  for (x=0; x<z; x++)
    satlist=g_list_append(satlist,satname[x]);

  /* Attach satellite list */
  widget=lookup_widget( mainwindow, "combo" );
  gtk_combo_set_popdown_strings( GTK_COMBO(widget), satlist);

  /* Get predict version from server */
  send_command(netsocket,"GET_VERSION");

  if(get_response(netsocket,predictversion)==-1) {
    error_dialog("Too many errors receiving response from server, disconnecting");
    disconnect_server();
    return TRUE;
  }

  for(i=0;i<strlen(predictversion);i++)
    if(predictversion[i]=='\n') {
      predictversion[i]='\0';
      break;
    }

  /* Setup status bar */
  sprintf(statusmsg,"Connected to predict server version %s on %s", \
	  predictversion, predicthost);
  show_status( statusmsg );

  /* Enable main loop processing */
  connected=TRUE;
  lastel=0;

  return 0;
}

void disconnect_server( void )
{
  GtkWidget * widget;
  gint ctxid;
  int i;

  /* if we are not connected, don't do anything more */
  if( connected == FALSE )
    return;

  /* Disable main loop processing */
  connected=FALSE;

  /* Close network socket */
  close(netsocket);

  /* Setup status bar */
  show_status("Not connected");

  /* Clear all data from widgets */
  widget=lookup_widget( mainwindow, "combo" );
  gtk_combo_set_popdown_strings( GTK_COMBO(widget), emptylist);
  satlist=g_list_first( satlist );
  for( i=0; i<NUMSATS; i++ )
    satlist=g_list_remove( satlist, satlist->data );
  for( i=0; i<NUMSATS; i++ )
    satname[i][0]='\0';

  widget = lookup_widget( mainwindow, "tx_longitude" );
  gtk_entry_set_text(GTK_ENTRY(widget),"");

  widget = lookup_widget( mainwindow, "tx_latitude" );
  gtk_entry_set_text(GTK_ENTRY(widget),"");

  widget = lookup_widget( mainwindow, "tx_azimuth" );
  gtk_entry_set_text(GTK_ENTRY(widget),"");

  widget = lookup_widget( mainwindow, "tx_elevation" );
  gtk_entry_set_text(GTK_ENTRY(widget),"");

  widget = lookup_widget( mainwindow, "tx_footprint" );
  gtk_entry_set_text(GTK_ENTRY(widget),"");

  widget = lookup_widget( mainwindow, "tx_range" );
  gtk_entry_set_text(GTK_ENTRY(widget),"");

  widget = lookup_widget( mainwindow, "tx_altitude" );
  gtk_entry_set_text(GTK_ENTRY(widget),"");

  widget = lookup_widget( mainwindow, "tx_velocity" );
  gtk_entry_set_text(GTK_ENTRY(widget),"");

  widget = lookup_widget( mainwindow, "tx_orbit" );
  gtk_entry_set_text(GTK_ENTRY(widget),"");

  widget = lookup_widget( mainwindow, "lb_aos" );
  gtk_label_set_text(GTK_LABEL(widget),"Next AOS");

  widget = lookup_widget( mainwindow, "tx_aos" );
  gtk_entry_set_text(GTK_ENTRY(widget),"");

  widget = lookup_widget( mainwindow, "tx_upshift" );
  gtk_entry_set_text(GTK_ENTRY(widget),"");

  widget = lookup_widget( mainwindow, "tx_updoppler" );
  gtk_entry_set_text(GTK_ENTRY(widget),"");

  widget = lookup_widget( mainwindow, "tx_downshift" );
  gtk_entry_set_text(GTK_ENTRY(widget),"");
  widget = lookup_widget( mainwindow, "tx_dwdoppler" );
  gtk_entry_set_text(GTK_ENTRY(widget),"");

  widget = lookup_widget( mainwindow, "satbar" );
  ctxid = gtk_statusbar_get_context_id(GTK_STATUSBAR(widget),"Status");
  gtk_statusbar_pop(GTK_STATUSBAR(widget),ctxid);
  gtk_statusbar_push(GTK_STATUSBAR(widget),ctxid,
			     "No visibility information available");
}

void open_preferences( void )
{
  char filename[256];
  FILE * fd;
  int n, state;
  GtkWidget *widget;

  strncpy(filename,getenv("HOME"),256);
  strcat(filename,"/.gsat.prefs");

  if((fd=fopen( filename, "r" ))==NULL)
    return;

  prefs_grid[0]=fgetc(fd);
  prefs_utctime[0]=fgetc(fd);
  prefs_satfootprint[0]=fgetc(fd);
  prefs_qthfootprint[0]=fgetc(fd);
  prefs_track[0]=fgetc(fd);
  fgetc(fd);
  fgets(prefs_aos_command, 1024, fd);
  prefs_aos_command[strlen(prefs_aos_command)-1]='\0';
  fgets(prefs_los_command, 1024, fd);
  prefs_los_command[strlen(prefs_los_command)-1]='\0';
  fgets(prefs_down_plugin, 256, fd);
  prefs_down_plugin[strlen(prefs_down_plugin)-1]='\0';
  fgets(prefs_down_plugin_config, 256, fd);
  prefs_down_plugin_config[strlen(prefs_down_plugin_config)-1]='\0';
  fgets(prefs_up_plugin, 256, fd);
  prefs_up_plugin[strlen(prefs_up_plugin)-1]='\0';
  fgets(prefs_up_plugin_config, 256, fd);
  prefs_up_plugin_config[strlen(prefs_up_plugin_config)-1]='\0';
  fclose( fd );

  if(strcmp(prefs_down_plugin, "None")!=0)
    for(n=0; n<g_list_length( pluginlist ); n++) {
      if( strncmp(prefs_down_plugin, pluginfiles[n],255)==0 ) {
	strcpy( filename, pluginsdir );
	strcat( filename, prefs_down_plugin );
	if(open_downlink_plugin( filename )==FALSE)
	  fprintf(stderr,"Openprefs: error opening downlink plugin.\n");
      }
    }
  
  if(strcmp(prefs_up_plugin, "None")!=0)
    for(n=0; n<g_list_length( pluginlist ); n++) {
      if( strncmp(prefs_up_plugin, pluginfiles[n],255)==0) {
	strcpy( filename, pluginsdir );
	strcat( filename, prefs_up_plugin );
	if(open_uplink_plugin( filename )==FALSE)
	  fprintf(stderr,"Openprefs: error opening uplink plugin.\n");
      }
    }

  if( prefs_grid[0]=='1' )
    state=TRUE;
  else
    state=FALSE;
  widget=lookup_widget( mainwindow, "tb_grid" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), state );

  if( prefs_utctime[0]=='1' )
    state=TRUE;
  else
    state=FALSE;
  widget=lookup_widget( mainwindow, "tb_utctime" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), state );

  if( prefs_satfootprint[0]=='1' )
    state=TRUE;
  else
    state=FALSE;
  widget=lookup_widget( mainwindow, "tb_footpsat" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), state );

  if( prefs_qthfootprint[0]=='1' )
    state=TRUE;
  else
    state=FALSE;
  widget=lookup_widget( mainwindow, "tb_footpqth" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), state );

  if( prefs_track[0]=='1' )
    state=TRUE;
  else
    state=FALSE;
  widget=lookup_widget( mainwindow, "tb_track" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), state );
}

void save_preferences( void )
{
  char prefsfile[256];
  FILE * fd;

  strncpy(prefsfile,getenv("HOME"),256);
  strcat(prefsfile,"/.gsat.prefs");

  fd=fopen( prefsfile, "w" );
  fputc(prefs_grid[0], fd);
  fputc(prefs_utctime[0], fd);
  fputc(prefs_satfootprint[0], fd);
  fputc(prefs_qthfootprint[0], fd);
  fputc(prefs_track[0], fd);
  fputc('\n', fd);
  fputs(prefs_aos_command, fd);
  fputc('\n', fd);
  fputs(prefs_los_command, fd);
  fputc('\n', fd);
  fputs(prefs_down_plugin, fd);
  fputc('\n', fd);
  fputs(prefs_down_plugin_config, fd);
  fputc('\n', fd);
  fputs(prefs_up_plugin, fd);
  fputc('\n', fd);
  fputs(prefs_up_plugin_config, fd);
  fputc('\n', fd);
  fclose( fd );
}
