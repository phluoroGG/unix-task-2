#include "square.h"
square_out *
squareproc_1_svc(square_in *inp, struct svc_req *rgstp)
{
	static square_out out;
	out.resl = inp->argl * inp->argl;
	return (&out);
}
