/**
 * @file clock.h
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
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
 * @brief Gestion de l'horloge et du temps en général.
 */

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <types.h>
#include <ksyscall.h>

#define CLOCKS_PER_SEC 1000 /**< Nombre de secondes par tick d'horloge. */
#define USEC_PER_SEC 1000000 /**< Nombre de us par secondes. */

struct timeval{
	long int tv_sec;
	long int tv_usec;
};

struct timespec{
	long int tv_sec;
	long int tv_nsec;
};

struct tm {
	int tm_sec;   /**< seconds after the minute — [0, 60] */
	int tm_min;   /**< minutes after the hour — [0, 59] */
	int tm_hour;  /**< hours since midnight — [0, 23] */
	int tm_mday;  /**< day of the month — [1, 31] */
	int tm_mon;   /**< months since January — [0, 11] */
	int tm_year;  /**< years since 1900 */
	int tm_wday;  /**< days since Sunday — [0, 6] */
	int tm_yday;  /**< days since January 1 — [0, 365] */
	int tm_isdst; /**< Daylight Saving Time flag (1 true, 0 false, neg unavailable) */
};

typedef long int time_t;
typedef long int clock_t;

/**
 * @brief Comparaison de 2 temps.
 * Comparaison de 2 temps.
 *
 * @return -1, 1 ou 0 en fonction du résultat.
 */
int compare_times(struct timeval a, struct timeval b);

/**
 * @brief Initialisation de l'horloge.
 * Initialisation de l'horloge.
 */
void clock_init();

/**
 * @brief Incrémente le temps système.
 * Ajoute une milliseconde au temps système, ne met rien à jour au dela du 
 * jour actuel.
 */
void clock_tick();

/**
 * @brief Retourne le temps système.
 * Renvoie la valeur actuelle du temps système en nombre de ticks (1 tick =
 * 1ms).
 */
clock_t get_clock();

/**
 * @brief Syscall handler pour get_clock.
 * Syscall handler pour get_clock.
 *
 * @param clock Un pointeur pour y stocker la valeur de l'horloge système.
 */
SYSCALL_HANDLER1(sys_getclock, clock_t* clock);

/**
 * @brief Retourne la date actuelle en secondes.
 * Retourne la date actuelle en secondes.
 *
 * @return la date en secondes.
 */
time_t get_date();

/**
 * @brief Syscall handler pour get_date.
 * Syscall handler pour get_date.
 *
 * @param date Un pointeur pour y stocker la date.
 */
SYSCALL_HANDLER1(sys_getdate, time_t* date);

/**
 * @brief Ajoute des us à un temps.
 * Ajoute des us à un temps contenu dans une structure timeval.
 * 
 * @param t La structure qui contient un temps.
 * @param usec Le nombre de us à ajouter.
 */
void timeval_add_usec(struct timeval *t, time_t usec); 

/**
 * @brief Retourne date courante.
 * Retourne une structure timeval qui contient la date courante en seconde et
 * microsecondes.
 *
 * @return une structure timeval.
 */
struct timeval get_tv();

/**
 * @brief Conversion struct tm vers time_t.
 * Conversion d'une date exprimée sous la forme secondes, minutes, heures, jour,
 * mois, année, etc, vers le format time_t.
 * 
 * @return Le temps correspondant.
 */
time_t clock_mktime(struct tm *timep);

void klog_systime();

#endif

