/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "square.h"

bool_t
xdr_square_in (XDR *xdrs, square_in *objp)
{
	register int32_t *buf;

	 if (!xdr_long (xdrs, &objp->argl))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_square_out (XDR *xdrs, square_out *objp)
{
	register int32_t *buf;

	 if (!xdr_long (xdrs, &objp->resl))
		 return FALSE;
	return TRUE;
}
