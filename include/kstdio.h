#ifndef _KSTDIO_H_
#define _KSTDIO_H_

/** 
 * @brief Affiche un message sur l'écran.
 *
 * Affiche un message sur l'écran. Attention, il n'y a pas d'appel système de
 * fait !
 *
 * @see printf
 *	@see fprintf
 *	@see sprintf
 *	@see snprintf
 *	@see vprintf
 *	@see vfprintf
 *	@see vsprintf
 *	@see vsnprintf
 * 
 * @param format chaîne de format précisant le format de conversion pour la
 * sortie.
 * @param ... liste variable d'arguments à afficher.
 */
void kprintf(const char *format, ...);

#endif
