/**
 * @file apps.h
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
 *
 * @section LICENSE
 *
 * Copyright (C) 2010 - TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */

#ifndef APPS_H
#define APPS_H

int main_fiinou(int argc, char* argv[] );
int launch_pres();
int launch_matrix();
int test_gui();

int semaphore_task(int argc, char** argv );
int test_semaphores();
void test_kmalloc();
void test_memory_reserve_page_frame();
int calc_pi();
int pi(int argc , char** argv );
int test_task1(int argc, char** argv);
int test_task();
int test_mouse_task();
int test_mouse();
int test_scanf();
int test_fgets();
int test_fputs();
int test_fwrite();
int test_fread();
int test_fseek();
int test_write_serial();
int test_read_serial();
int test_ansi();
int snake_main();
int cube_launcher();
int noxeyes(); 
int test_elf();
int test_ctype();
int test_stdio();
int test_ofd_flags_rdonly();
int test_ofd_flags_wronly();
void test_send_packet();

#endif //APPS_H
