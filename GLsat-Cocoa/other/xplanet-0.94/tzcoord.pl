#!/usr/bin/perl

# This script prints coordinates of a named timezone in the same
# format that xplanet (see http://xplanet.sourceforge.net) uses in the
# argument of its `-lon/-lat' option.
#
# The first argument is a substring matching the name field of the
# file, the second argument is the path to the zone.tab file.
#
# Copyright (c) 2000 by Marcus Harnisch <marcus.harnisch@gmx.net>
#
# From /usr/share/zoneinfo/zone.tab:
#
# This file contains a table with the following columns:
# 1.  ISO 3166 2-character country code.  See the file `iso3166.tab'.
# 2.  Latitude and longitude of the zone's principal location
#     in ISO 6709 sign-degrees-minutes-seconds format,
#     either +-DDMM+-DDDMM or +-DDMMSS+-DDDMMSS,
#     first latitude (+ is north), then longitude (+ is east).
# 3.  Zone name used in value of TZ environment variable.
# 4.  Comments; present if and only if the country has multiple rows.
#
# Columns are separated by a single tab.
# The table is sorted first by country, then an order within the country that
# (1) makes some geographical sense, and
# (2) puts the most populous zones first, where that does not contradict (1).

# Sorry for this hack, but Getopt::Std would be way
# too expensive for a simple help option
if ( @ARGV < 0 or @ARGV > 2 or $ARGV[0] eq '-h' )
{
   print STDERR <<EOM;
Usage:
  tzcoord.pl [-h] <timezone> [<zone.tab file>]

EOM
  exit 1;
}

my $timezone = shift;
my $zone_tab = shift || '/usr/share/zoneinfo/zone.tab';

open( ZONE_FILE, "< $zone_tab" )
  or die "Couldn't open file $zone_tab: $!";

my ( $countrycode, $isocoord, $name, $comment );
my $found;

while ( <ZONE_FILE> )
{
  s/#.*$//;
  next if /^\s*$/;
  ( $countrycode, $isocoord, $name, $comment ) = split ' ', $_, 4;
  if ( $name =~ /$timezone/i )
  {
    $found = 1;
    last;
  }
}

# Timezone found?
if ( $found == 0 )
{
  print STDERR "No timezone matching $timezone found!\n";
  exit 1;
}

# Check format
unless (    $isocoord =~ /((?:\+|-)\d{4})((?:\+|-)\d{5})/
	 or $isocoord =~ /((?:\+|-)\d{6})((?:\+|-)\d{7})/ )
{
  print STDERR "Coordinates $isocoord are not ISO 6709 compliant!\n";
  exit 1;
}

my $isolatitude  = $1;
my $isolongitude = $2;

my $latitude  = 0;
my $longitude = 0;

# Seconds?
if ( length( $isolatitude ) > 5 )
{
  $latitude  += substr( $isolatitude, -2, 2, '' ) / 3600;
  $longitude += substr( $isolongitude, -2, 2, '' ) / 3600;
}

# Minutes
$latitude  += substr( $isolatitude,  -2, 2, '' ) / 60;
$longitude += substr( $isolongitude, -2, 2, '' ) / 60;

# Degrees
$latitude  += substr( $isolatitude,  1, 2 );
$longitude += substr( $isolongitude, 1, 3 );

# Sign
$latitude  *= -1 if ( substr( $isolatitude,  0, 1 ) eq '-' );
$longitude *= -1 if ( substr( $isolongitude, 0, 1 ) eq '-' );

{
  use locale;
  printf "-lon %.2f -lat %.2f\n", $longitude, $latitude;
}

