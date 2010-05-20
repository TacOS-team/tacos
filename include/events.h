#ifndef _EVENTS_H_
#define _EVENTS_H_

/**
* @file events.h
*/

#include <time.h>

typedef void *(*callback_t) (void *);

/** 
* @brief structure stockant un evennement à declencher
*/
struct event_t
{
  struct timeval date;
  callback_t callback;
  void *data;
};

/** 
* @brief initialise le support des evenements
*/
void events_init();

/** 
* @brief ajoute un evenement à declencher
* 
* @param call la fonction qui sera lancée
* @param data un pointeur qui sera passé à la fonction
* @param time la fonction sera lancée dans "time" msec
*/
void add_event(callback_t call, void* data, clock_t time);

#endif

