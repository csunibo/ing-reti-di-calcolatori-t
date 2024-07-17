/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "opfile_x.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

int *
aggiungi_stanza_1(AggStanza *argp, CLIENT *clnt)
{
	static int clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, AGGIUNGI_STANZA,
		(xdrproc_t) xdr_AggStanza, (caddr_t) argp,
		(xdrproc_t) xdr_int, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ListaStanze *
elimina_utente_1(char **argp, CLIENT *clnt)
{
	static ListaStanze clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, ELIMINA_UTENTE,
		(xdrproc_t) xdr_wrapstring, (caddr_t) argp,
		(xdrproc_t) xdr_ListaStanze, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

int *
aggiungi_utente_1(AggUtente *argp, CLIENT *clnt)
{
	static int clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, AGGIUNGI_UTENTE,
		(xdrproc_t) xdr_AggUtente, (caddr_t) argp,
		(xdrproc_t) xdr_int, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

int *
elimina_stanza_1(char **argp, CLIENT *clnt)
{
	static int clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, ELIMINA_STANZA,
		(xdrproc_t) xdr_wrapstring, (caddr_t) argp,
		(xdrproc_t) xdr_int, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ListaStanze *
visualizza_utente_1(char **argp, CLIENT *clnt)
{
	static ListaStanze clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, VISUALIZZA_UTENTE,
		(xdrproc_t) xdr_wrapstring, (caddr_t) argp,
		(xdrproc_t) xdr_ListaStanze, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}
