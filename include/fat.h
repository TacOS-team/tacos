#ifndef _FAT_H_
#define _FAT_H_

// Init FAT (a faire dans le main)
void mount_FAT12 ();

// Fonctions pour le Shell
void print_Boot_Sector ();		// fonction de debug
void print_path ();				// fonction de debug
void change_dir (char * name);
void list_segments (int mode);			
void print_working_dir ();		

#endif
