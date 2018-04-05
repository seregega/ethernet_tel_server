#!/usr/bin/perl

use strict;

open(OUTPUT, "> httpd-fsdata.c");
print OUTPUT "\#include \"stddef.h\"\n";
print OUTPUT "\#include \"httpd-fsdata.h\"\n";


chdir("httpd-fs");

opendir(DIR, ".");
my @files =  grep { !/^\./ && !/(CVS|~)/ } readdir(DIR);
closedir(DIR);

my $file;

foreach $file (@files) {  
   
    if(-d $file && $file !~ /^\./) {
      print "Processing directory $file\n";
      opendir(DIR, $file);
      my @newfiles =  grep { !/^\./ && !/(CVS|~)/ } readdir(DIR);
      closedir(DIR);
      printf "Adding files @newfiles\n";
      @files = (@files, map { $_ = "$file/$_" } @newfiles);
      next;
    }
}

my @fvars;
my @pfiles;

foreach $file (@files) {
  if(-f $file) {
    print "Adding file $file\n";
    
    open(FILE, $file) || die "Could not open file $file\n";

    $file =~ s-^-/-;
    my $fvar = $file;
    $fvar =~ s-/-_-g;
    $fvar =~ s-\.-_-g;
    # for AVR, add PROGMEM here
    print(OUTPUT "static const char data".$fvar."[] = {\n");
    print(OUTPUT "\t/* $file */\n\t");
    my $j;
    for($j = 0; $j < length($file); $j++) {
        printf(OUTPUT "%#02x, ", unpack("C", substr($file, $j, 1)));
    }
    printf(OUTPUT "0,\n");
    
    
    my $i = 0;
    my $data;
    while(read(FILE, $data, 1)) {
        if($i == 0) {
          print(OUTPUT "\t");
        }
        printf(OUTPUT "%#02x, ", unpack("C", $data));
        $i++;
        if($i == 10) {
          print(OUTPUT "\n");
          $i = 0;
        }
    }
    print(OUTPUT "0};\n\n");
    close(FILE);
    push(@fvars, $fvar);
    push(@pfiles, $file);
  }
}

my $i;
my $prevfile;
for($i = 0; $i < @fvars; $i++) {
    my $file = $pfiles[$i];
    my $fvar = $fvars[$i];

    if($i == 0) {
        $prevfile = "NULL";
    } else {
        $prevfile = "(struct httpd_fsdata_file *)&file" . $fvars[$i - 1];
    }
    print(OUTPUT "const struct httpd_fsdata_file file".$fvar."[] = {{$prevfile, (char*)data$fvar, ");
    print(OUTPUT "(char*)data$fvar + ". (length($file) + 1) .", ");
    print(OUTPUT "sizeof(data$fvar) - ". (length($file) + 1) ."}};\n\n");
}

print(OUTPUT "#define HTTPD_FS_ROOT file$fvars[$i - 1]\n\n");
print(OUTPUT "#define HTTPD_FS_NUMFILES $i\n");
