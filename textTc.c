#include <stdio.h>
#include <stdlib.h> // For portable exit and success definitions
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <unistd.h> // For getopt, which parses CL options (https://www.gnu.org/software/libc/manual/html_node/Getopt.html)

void ascii_to_bin(int curr, int space);
void ascii_to_hex(int curr, int space);
void bin_to_ascii_or_hex(char *byte, char encoding_flag, int space);
void hex_to_ascii_or_bin(char *byte, char encoding_flag, int space);
void display_help();

int main(int argc, char *argv[]) {
	FILE *input;
	int curr, a_flag = 0, b_flag = 0, f_flag = 0, s_flag = 0, x_flag = 0, byte_index = 0;
	char byte[9];
	
	// Collect execution options
	while ((curr = getopt(argc, argv, "abfhxs2:")) != -1) {
		switch (curr) {
			case 'a':
				a_flag = 1;
				break;
			case 'b':
				b_flag = 1;
				break;
			case 'f':
				f_flag = 1;
				break;
			case 'h':
				display_help();
			case 's':
				s_flag = 1;
				break;
			case 'x':
				x_flag = 1;
				break;
			case '2':
				if ((strcmp(optarg, "a")) == 0) {
					if (a_flag) {
						fprintf(stderr, "Converting ASCII to ASCII...? Err, okay, done!\n");
						return EXIT_FAILURE;
					}
					a_flag = 2;
				}
				if ((strcmp(optarg, "b")) == 0) {
					if (b_flag) {
						fprintf(stderr, "Converting binary to binary...? Err, okay, done!\n");
						return EXIT_FAILURE;
					}
					b_flag = 2;
				}
				if ((strcmp(optarg, "x")) == 0) {
					if (x_flag) {
						fprintf(stderr, "Converting hex to hex...? Err, okay, done!\n");
						return EXIT_FAILURE;
					}
					x_flag = 2;
				}
				break;
			case '?':
				if (isprint(optopt)) {
					fprintf(stderr, "Unknown option \'-%c\'.\n", optopt);
				}
				else {
					fprintf(stderr, "Unknown option character \'\\x%x\'.\n", optopt);
				}
				return EXIT_FAILURE;
			default:
				return EXIT_FAILURE;
		}
	}
	
	// Quit if there's some logical flag conflict
	if ((a_flag == b_flag) || (a_flag == x_flag) || (b_flag == x_flag)) {
		fprintf(stderr, "Need a set of TWO ENCODING arguments separated by a \"2\" (e.g., \"-a2b\") to know what to transcode to!\n(Note: If using other flags (\"-f\", \"-s\"), use them with a \"-\" and separated by a space from any arg sequences before it.)\n");
		return EXIT_FAILURE;
	}
	
	// Open input file
	input = fopen("input.txt", "r");
	if (input == NULL) {
		input = fopen("input.txt", "w");
		if (input == NULL) {
			perror("Missing input.txt!");
		}
		else {
			printf("Put text to transcode in input.txt!\n");
		}
		return EXIT_FAILURE;
	}
	
	// Set output to a file if the file flag was indicated
	if (f_flag) {
		if ((freopen("output.txt", "w", stdout)) == NULL) {
			perror("Problem redirecting STDOUT to output.txt...");
		}
	}
	
	// Read input file
	while ((curr = fgetc(input)) != EOF) {
		// If transcoding FROM ASCII
		if (a_flag == 1) {
			// if the current char is printable,
			if (curr > 31) {
				// convert it to the indicated encoding.
				if (b_flag == 2) {
					ascii_to_bin(curr, s_flag);
				}
				else if (x_flag == 2) {
					ascii_to_hex(curr, s_flag);
				}
			}
		}
		
		// If transcoding FROM BINARY
		else if (b_flag == 1) {
			// store each set of eight bits into byte[].
			if (curr == '0' || curr == '1') {
				byte[byte_index++] = curr;
			}
			// After processing a byte,
			if (byte_index == 8) {
				// terminate the byte string with a null char,
				byte[byte_index] = '\0';
				// convert the byte,
				if (a_flag == 2) {
					bin_to_ascii_or_hex(byte, 'a', s_flag);
				}
				else if (x_flag == 2) {
					bin_to_ascii_or_hex(byte, 'x', s_flag);
				}
				// and reset byte_index for the next byte.
				byte_index = 0;
			}
		}
		
		// If transcoding FROM HEX
		else if (x_flag == 1) {
			// store each set of two hex chars into byte[].
			if ((curr > 47 && curr < 58) || (curr > 64 && curr < 71) || (curr > 96 && curr < 123)) {
				byte[byte_index++] = curr;
			}
			// After processing a pair of hex chars,
			if (byte_index == 2) {
				// terminate the hex string with a null char,
				byte[byte_index] = '\0';
				// convert the hex chars,
				if (a_flag == 2) {
					hex_to_ascii_or_bin(byte, 'a', s_flag);
				}
				else if (b_flag == 2) {
					hex_to_ascii_or_bin(byte, 'b', s_flag);
				}
				// and reset byte_index.
				byte_index = 0;
			}
		}
	}
	
	printf("\n");
	fclose(input);
	fclose(stdout);
	return EXIT_SUCCESS;
}

/*****************************************************************************
 * ascii_to_bin: Converts a char to a series of eight binary bits and then   *
 * 				 writes them to output optionally followed by a space		 *
 *****************************************************************************/
void ascii_to_bin(int curr, int space) {
	unsigned int mask = 1 << 7;
	int i;
	char bit;
	
	// AND with a mask to get each bit for all eight bits
	for (i = 0; i < 8; i++) {
		bit = (curr & mask) == 0 ? '0' : '1';
		// Output bit
		printf("%c", bit);
		// Shift to the next right bit
		mask >>= 1;
	}
	// Output space according to argument
	if (space) {
		printf("%c", ' ');
	}
}

/*****************************************************************************
 * ascii_to_hex: Writes a char to output as a string of two hex chars        *
 *				 optionally followed by a space								 *
 *****************************************************************************/
void ascii_to_hex(int curr, int space) {
	printf("%02x", curr);
	if (space) {
		printf("%c", ' ');
	}
}

/*****************************************************************************
 * bin_to_ascii: Converts a string of eight bits to an ASCII char or a hex   *
 *			     sequence, depending on encoding_flag, optionally followed   *
 *				 by a space and writes it to output							 *
 *****************************************************************************/
void bin_to_ascii_or_hex(char *byte, char encoding_flag, int space) {
	unsigned int result = 0;
	int exponent = 7;
	
	// Convert byte to decimal
	while (*byte) {
		if (*byte == '1') {
			result += (unsigned int) pow(2, exponent);
		}
		byte++;
		exponent--;
	}
	// If encoding_flag is 'a',
	if (encoding_flag == 'a') {
		// output as ASCII.
		printf("%c", result);
	}
	// If it's 'x',
	else if (encoding_flag == 'x') {
		// output as hex.
		printf("%02x", result);
		if (space) {
			printf("%c", ' ');
		}
	}
}

/*****************************************************************************
 * hex_to_ascii_or_bin: Converts a string of two hex chars to an ASCII char  *
 *				        or a binary string optionally followed by a space    *
 *					    and then writes it output							 *
 *****************************************************************************/
void hex_to_ascii_or_bin(char *byte, char encoding_flag, int space) {
	unsigned int result = 0;
	int exponent = 1, coeff = 0;
	
	while (*byte) {
		switch (*byte) {
			case 'a':
			case 'A':
				coeff = 10;
				break;
			case 'b':
			case 'B':
				coeff = 11;
				break;
			case 'c':
			case 'C':
				coeff = 12;
				break;
			case 'd':
			case 'D':
				coeff = 13;
				break;
			case 'e':
			case 'E':
				coeff = 14;
				break;
			case 'f':
			case 'F':
				coeff = 15;
				break;
			default:
				coeff = *byte - '0';
		}
		result += (coeff * (unsigned int) pow(16, exponent));
		byte++;
		exponent--;
	}
	if (encoding_flag == 'a') {
		printf("%c", result);
	}
	else if (encoding_flag == 'b') {
		ascii_to_bin(result, space);
	}
}

/*****************************************************************************
 * display_help: Prints program instructions and exits					     *
 *****************************************************************************/
void display_help() {
	printf("This program transcodes between text sequences of binary or hex bytes and ASCII characters.\n");
	printf("================================================================================\n");
	printf("To use, put the text to be decoded in a file named \"input.txt\" in the same\n");
	printf("directory as the executable, then run with the appropriate arguments; e.g.:\n\n");
	printf("\t\"textTc.exe -a2b\"\tto transcode the text FROM ASCII TO BINARY\n");
	printf("\t\"textTc.exe -b2a\"\tto transcode the text FROM BINARY TO ASCII\n");
	printf("\t\"textTc.exe -x2b\"\tto transcode the text FROM HEX TO BINARY\n");
	printf("\t\"textTc.exe -x2b -f\"\tto write the transcoded text TO AN \"output.txt\" FILE\n");
	printf("\t\"textTc.exe -x2b -s\"\tto SEPARATE transcoded hex or binary bytes by a SPACE\n");
	printf("\t\"textTc.exe -h\"\tto display these HELP instructions\n");
	exit(EXIT_SUCCESS);
}


