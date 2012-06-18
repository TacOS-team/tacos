/**
 * \file term.h
 * \brief Gestion du termios
 * \author Bul0t Team
 * \date mai 2012
 */

#ifndef TERM_H_
#define TERM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/** 
 * \fn int save_termios (struct termios *save)
 * \brief Sauve la configuration du termios courant
 * \param ter
 */
int save_termios (struct termios *save);

/*
 * \fn int restore_termios (struct termios *old)
 * \brief Reéablit les parametres precedents
 */
int restore_termios (struct termios *old);

#endif
