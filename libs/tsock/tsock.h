#ifndef TSOCK_H
#define TSOCK_H

#include <sys/cdefs.h>

__BEGIN_DECLS

#include <sys/types.h>

/**
 * Créée une socket serveur en écoute sur le point de rendez-vous spécifié.
 *
 * @param path Point de rendez-vous
 *
 * @return numéro du descripteur de fichier associé, négatif en cas d'erreur
 */
int tsock_listen(const char *path);
    
/**
 * Créée une socket client connectée à la socket serveur en écoute sur le
 * point de rendez-vous spécifié.
 *
 * @param path Point de rendez-vous
 *
 * @return numéro du descripteur de fichier associé, négatif en cas d'erreur
 */
int tsock_connect(const char *path);

/**
 * Attend une connexion sur la socket serveur spécifiée. Créée et retourne
 * une nouvelle socket serveur associée au nouveau client.
 *
 * @param server Socket serveur en attente d'une connexion
 *
 * @return numéro du descripteur de fichier associé à la socket créée,
 * négatif en cas d'erreur
 */
int tsock_accept(int tsock);

/**
 * Lit depuis une socket.
 *
 * @param tsock Numéro du descripteur de fichier associé à la socket
 * @param buffer Buffer où stocker les données lues
 * @param count Nombre maximum d'octets à lire
 *
 * @return le nombre d'octets lus (0 si la connexion a été fermée,
 * -1 en cas d'erreur)
 */
ssize_t tsock_read(int tsock, void *buffer, size_t len);

/**
 * Écrit sur une socket.
 *
 * @param tsock Numéro du descripteur de fichier associé à la socket
 * @param buffer Buffer qui contient les données à écrire
 * @param count Nombre maximum d'octets à écrire
 *
 * @return le nombre d'octets écrits (0 si aucune écriture, -1 en cas d'erreur).
 */
ssize_t tsock_write(int tsock, void *buffer, size_t len);

/**
 * Ferme une socket (et la connexion associée).
 *
 * @param tsock Numéro du descripteur de fichier associé à la socket
 *
 * @return 0 en cas de succès
 */
void tsock_close(int tsock);

/**
 * Passe une socket en mode non bloquant.
 *
 * @param tsock Numéro du descripteur de fichier associé à la socket
 *
 * @return 0 en cas de succès
 */
int tsock_set_nonblocking(int tsock);

/**
 * Passe une socket en mode bloquant.
 *
 * @param tsock Numéro du descripteur de fichier associé à la socket
 *
 * @return 0 en cas de succès
 */
int tsock_set_blocking(int tsock);

__END_DECLS

#endif
