struct square_in {
	long argl;
};
struct square_out {
	long resl;
};
program SQUARE_PROG {
	version SQUARE_VERS {
	square_out SQUAREPROC(square_in) = 1; /* procedure = 1 */
	} = 1; /* version number = 1 */
} = 0x31230000; /* program number */
