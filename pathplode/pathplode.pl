#! /usr/bin/perl 
#
# Perl implementation of pathplode
#
# Copyright 2003 by Ulf Klaperski
#
# This file is part of pathplode.
# 
###############################################################################
#
# Pathplode is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# Pathplode is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with the pathplode sources; see the file COPYING.  
#
###############################################################################
#
# $Id$
#
###############################################################################

use Getopt::Long qw(:config no_ignore_case);

GetOptions ('help|h!' => \$help,
	    'uniquify|u!' => \$uniquify,
	    'unique|U!' => \$unique,
	    'separator|S' => \$separator,
	    'list|t!' => \$list,
	    'prepend|p=s' => \$prepend,
	    'append|a=s' => \$append,
	    'remove-first|f=s' => \$remove_first,
	    'remove-last|l=s' => \$remove_last,
	    'remove-all|d=s' => \$remove_all,
	    'after|A=s' => \$after,
	    'before|B=s' => \$before,
	    'allow-empty|E' => \$allow_empty,
	    'color|C' => \$color,
	    'preserve-trailing-slash|P' => \$preserve_trailing_slash
	   );


sub help {
  print "Usage: pathplode [options] command <element> <extra-arg> [path]
    Commands:
	--append  <element>: put element at end of list
	--prepend <element>: put element at beginning of list
	--remove-first <regexp>: remove the first matching element 
	--remove-all <regexp>: 
	--uniquify
	--list
    Options:
	--unique: remove multiple entries. Only the entry with the
    		  highest priority is left over.
	--after <regexp>: modify append/prepend to insert new element after
			  element matching regexp. Note that prepend inserts
			  after the first matching element, append after the last
			  matching element.
	--before <regexp>: modify append/prepend to insert new element before
			   element matching regexp. Note that prepend inserts
			   before the first matching element, append before the last
			   matching element.
	--separator: The separator between path elements. Default: ':'
	--preserve-trailing-slash: by default a trailing slash is removed. This changes
				   the behaviour to keep those slashes.
	--allow-empty: Allow empty entries. By default they are removed.
	--color: colorize output - currently only affects the list command.
    	
Examples:
	pathplode --uniquify ~/gnu/bin:/usr/local/bin:/usr/bin:/bin:~/gnu/bin:/usr/bin/X11:/usr/games:/usr/local/bin
		  -> ~/gnu/bin:/usr/local/bin:/usr/bin:/bin:/usr/bin/X11:/usr/games
	pathplode --unique --prepend ~/gnu/bin /usr/local/bin:/usr/bin:/bin:~/gnu/bin:/usr/bin/X11:/usr/games
		  -> ~/gnu/bin:/usr/local/bin:/usr/bin:/bin:/usr/bin/X11:/usr/games
	pathplode --append ~/gnu/bin /usr/local/bin:/usr/bin:/bin:~/gnu/bin:/usr/bin/X11:/usr/games
		  -> /usr/local/bin:/usr/bin:/bin:~/gnu/bin:/usr/bin/X11:/usr/games:~/gnu/bin
	pathplode --prepend ~/bin --after gnu /usr/bin:/bin:~/gnu/bin:/usr/bin/X11:/usr/games:~/gnu/bin
		  -> /usr/bin:/bin:~/gnu/bin:~/bin:/usr/bin/X11:/usr/games:~/gnu/bin
";
  exit shift;
}

###############################################################################
# Commands
###############################################################################
# 
# Each command expects as first argument a reference to an array of the current  
# elements. The result will be passed back by modifying this array.

sub uniquify {
  my $elements_r = shift;
  my %found = ();
  local @new_elements = ();
  foreach $elm (@$elements_r) {
    next if $found{$elm};
    $found{$elm} = 1;
    push @new_elements, $elm;
  }
  @$elements_r = @new_elements;
}

sub prepend {
  my $prepend = shift;
  my $elements_r = shift;
  my ($found, $idx) = (0, 0);
  
  uniquify $elements_r if $unique_before;

  if ($before or $after) {
    my $anchor = ($before ? $before : $after);
    while (!$found and $idx<$#$elements_r) {
      if ($$elements_r[$idx] =~ /$anchor/) {
	$found=1;
	last;
      }
      $idx++;
    }
    
    unless ($found) {
      $idx=0;
      $error=1;
      print STDERR "Anchor element $anchor not found. Prepending at beginning!\n";
    } elsif ($after) {
      $idx++; 
    }
  }
  splice @$elements_r, $idx, 0, $prepend;
  
  uniquify $elements_r if $unique_after;
}

sub append {
  my $append = shift;
  my $elements_r = shift;
  my ($found, $idx) = (0, scalar @$elements_r);
  uniquify $elements_r if $unique_before;

  if ($before or $after) {
    $anchor = ($before ? $before : $after);
    while (!$found and $idx>0) {
      if ($$elements_r[$idx-1] =~ /$anchor/) {
	$found=1;
	last;
      }
      $idx--;
    }
    
    unless ($found) {
      $idx=@$elements;
      $error=1;
      print STDERR "Anchor element $anchor not found. Prepending at beginning!\n";
    } elsif ($before) {
      $idx--;
    }
  }
  splice @$elements_r, $idx, 0, $append;

  uniquify $elements_r if $unique_after;
}

sub remove_first {
  my $remove = shift;
  my $elements_r = shift;
  my ($found, $idx) = (0, 0);
  
  uniquify $elements_r if $unique_before;

  if ($before or $after) {
    $anchor = ($before ? $before : $after);
  } else {
    undef $anchor;
  }

  while (1) {
    if ($$elements_r[$idx] =~ /$remove/) {
      if ((!defined($before) or ($idx!=$#$elements_r and $$elements_r[$idx+1] =~ /$before/) ) 
	  and (!defined($after) or ($idx!=0 and $$elements_r[$idx-1] =~ /$after/))) {
	$found=1;
	last;
      }
    }
    last unless ( ++$idx<@$elements_r );
  }
  
  if ($found) {
    print "Removing $idx\n";
    splice @$elements_r, $idx, 1;
  }
  
  uniquify $elements_r if $unique_after;
}

sub remove_last {
  my $remove = shift;
  my $elements_r = shift;

  uniquify $elements_r if $unique_before;

  ($before, $after) = ($after, $before);
  @$elements_r = reverse @$elements_r;
  remove_first $remove, $elements_r;
  @$elements_r = reverse @$elements_r;

  uniquify $elements_r if $unique_after;

}

sub list {
  my $elements_r = shift;
  my %paths;
  foreach $path (@$elements_r) {
    print $color && $paths{$path}?"\033[1;31m":"","$path\n", $color?"\e[0m":"";
    $paths{$path}=1;
  }
}

###############################################################################
# Main Program
###############################################################################

$help and &help;

$path_in = pop;

defined $separator or $separator = ':';
@commands = ("uniquify",  "prepend", "append", "remove_first", "remove_last", "list");

foreach $cmd (@commands) {
  if ($$cmd) {
    if ($cmd_found) {
      die "Only one command of ", join(" ", @commands), " allowed!\n";
    }
    $cmd_found=1;
    $command=$cmd;
  }
}

if ($unique) {
  $unique_before = 1;
  $unique_after = 1;
}

@elements = split /$separator/, $path_in;
map { s:/$::; } @elements unless $preserve_trailing_slash;

if ($command eq "uniquify") {
  uniquify \@elements;
} elsif ($command eq "prepend") {
  $prepend =~ s:/$:: unless $preserve_trailing_slash;
  prepend $prepend, \@elements;
} elsif ($command eq "append") {
  $append =~ s:/$:: unless $preserve_trailing_slash;
  append $append, \@elements;
} elsif ($command eq "remove_first") {
  remove_first $remove_first, \@elements;
} elsif ($command eq "remove_last") {
  remove_last $remove_last, \@elements;
} elsif ($command eq "list") {
  list \@elements;
  $noprint=1;
}

print join $separator, @elements unless $noprint;
