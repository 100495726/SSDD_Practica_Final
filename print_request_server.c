
#include "print_request.h"

//Esta es la funcion definida para la impresión por pantalla de peticiones con su marca de tiempo
bool_t
print_request_1_svc(Tupla arg1, int *result,  struct svc_req *rqstp)
{
	bool_t retval = TRUE;

	if (arg1.filename[0] == '\0') {
        printf("%s %s %s\n", arg1.username, arg1.operation, arg1.time);
    } 
	else {
        printf("%s %s %s %s\n", arg1.username, arg1.operation, arg1.filename, arg1.time);
    }
	
	return retval;
}

int
print_request_prog_1_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free (xdr_result, result);

	return 1;
}
