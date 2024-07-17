/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _XFACTOR_H_RPCGEN
#define _XFACTOR_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

#define NUM_GIUDICI 12
#define MAX_NAME_SIZE 128

struct Giudice {
	char nomeGiudice[MAX_NAME_SIZE];
	int punteggioTot;
};
typedef struct Giudice Giudice;

struct Output {
	Giudice classificaGiudici[NUM_GIUDICI];
};
typedef struct Output Output;

struct Input {
	char nomeCandidato[MAX_NAME_SIZE];
	char tipoOp;
};
typedef struct Input Input;

#define OPERATION 0x20000013
#define OPERATIONVERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define CLASSIFICA_GIUDICI 1
extern  Output * classifica_giudici_1(void *, CLIENT *);
extern  Output * classifica_giudici_1_svc(void *, struct svc_req *);
#define ESPRIMI_VOTO 2
extern  int * esprimi_voto_1(Input *, CLIENT *);
extern  int * esprimi_voto_1_svc(Input *, struct svc_req *);
extern int operation_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define CLASSIFICA_GIUDICI 1
extern  Output * classifica_giudici_1();
extern  Output * classifica_giudici_1_svc();
#define ESPRIMI_VOTO 2
extern  int * esprimi_voto_1();
extern  int * esprimi_voto_1_svc();
extern int operation_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_Giudice (XDR *, Giudice*);
extern  bool_t xdr_Output (XDR *, Output*);
extern  bool_t xdr_Input (XDR *, Input*);

#else /* K&R C */
extern bool_t xdr_Giudice ();
extern bool_t xdr_Output ();
extern bool_t xdr_Input ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_XFACTOR_H_RPCGEN */