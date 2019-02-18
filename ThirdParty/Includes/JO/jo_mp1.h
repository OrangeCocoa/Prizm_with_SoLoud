/* public domain Simple, Minimalistic MPEG Layer 1 decoder - http://jonolick.com
 *
 * Revision History:
 * 	1.00 (2014-26-1) Initial release.
 *
 * Basic usage:
 * 	int hz, channels, outputSize;
 * 	short *output;
 * 	jo_read_mp1(input, inputSize, output, outputSize, hz, channels);
 * 	// Do something with the data here
 * 	free(output);
 *
 * 	*/

#ifndef JO_INCLUDE_MP1_H
#define JO_INCLUDE_MP1_H

bool jo_read_mp1(const void *input, int inputSize, short *&output, int &outputSize, int &hz, int &channels);

#endif // JO_INCLUDE_MP1_H