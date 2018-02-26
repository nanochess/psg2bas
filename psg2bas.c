/*
** Convert a log of PSG to a DATA sequence
**
** by Oscar Toledo G.
**
** Creation date: Feb/24/2018.
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

double psg_vol[16];
double psg2_vol[16];
int psg_conv[16];

/*
** Main program
*/
int main(int argc, char *argv[])
{
	FILE *input;
	FILE *output;
	char line[256];
	char buffer[256];
	char buffer2[256];
	char *ap;
	double vol;
	double best_approx;
	int c;
	int d;
	int e;
	int prev_a, curr_a;
	int prev_va, curr_va;
	int prev_b, curr_b;
	int prev_vb, curr_vb;
	int prev_c, curr_c;
	int prev_vc, curr_vc;
	int disabled;

	if (argc < 3) {
		fprintf(stderr, "psg2bas v0.1 http://nanochess.org/\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "Usage: psg2bas [-a] [-b] [-c] psg.txt psg.bas\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "The options allows to disable indicated channel for output\n");
		fprintf(stderr, "\n");
		exit(1);
	}

	/*
	** Volume levels for SN76489
	*/
	vol = 1.0;
	c = 0;
	do {
		psg_vol[c] = vol;
		vol = vol / 1.25893;
	} while (++c < 15) ;
	psg_vol[c] = 0.0;

	/*
	** Volume levels for AY-3-8910
	*/
	vol = 1.0;
	c = 15;
	do {
		psg2_vol[c] = vol;
		vol = vol / 1.41421;
	} while (--c > 0) ;
	psg2_vol[c] = 0.0;

	/*
	** Generate a conversion table
	** input index - SN76489 volume
	** output - AY-3-8910 volume
	*/
	for (c = 0; c < 16; c++) {
		vol = psg_vol[c];
		e = 0;
		best_approx = 10.0;
		for (d = 0; d < 16; d++) {
			if (fabs(vol - psg2_vol[d]) < best_approx) {
				best_approx = fabs(vol - psg2_vol[d]);
				e = d;
			}
		}
		psg_conv[c] = e;
	}

	/*
	** Process arguments
	*/
	disabled = 0;
	c = 1;
	while (c < argc) {
		if (argv[c][0] != '-')
			break;
		d = tolower(argv[c][1]);
		if (d == 'a')
			disabled |= 1;
		else if (d == 'b')
			disabled |= 2;
		else if (d == 'c')
			disabled |= 4;
		else {
			fprintf(stderr, "unknown option in command-line\n");
			exit(1);
		}
		c++;
	}
	if (c >= argc)	{
		fprintf(stderr, "Input filename not provided\n");
		exit(1);
	}
	input = fopen(argv[c], "r");
	if (input == NULL) {
		fprintf(stderr, "Unable to open input %s\n", argv[c]);
		exit(1);
	}
	c++;
	if (c >= argc)	{
		fprintf(stderr, "Output filename not provided\n");
		exit(1);
	}
	output = fopen(argv[c], "w");
	if (output == NULL) {
		fprintf(stderr, "Unable to open output %s\n", argv[c]);
		exit(1);
	}
	prev_a = -1;
	prev_va = 16;
	prev_b = -1;
	prev_vb = 16;
	prev_c = -1;
	prev_vc = 16;
	buffer[0] = '\0';
	e = 0;
	d = 0;
	while (fgets(line, sizeof(line) - 1, input)) {
		if (line[0] != 'A')
			continue;
		sscanf(line, "A=%04x %01x B=%04x %01x C=%04x %01x",
			&curr_a, &curr_va, &curr_b, &curr_vb,
			&curr_c, &curr_vc);
		curr_va = psg_conv[curr_va];
		curr_vb = psg_conv[curr_vb];
		curr_vc = psg_conv[curr_vc];
		c = 0;
		sprintf(buffer2, "\tDATA $0000,");
		ap = buffer2 + 11;
		if ((disabled & 1) == 0 && (prev_va != curr_va || (prev_a != curr_a && prev_va != 0))) {
			sprintf(ap, ",$%04x", curr_va | curr_a << 4);
			ap += 6;
			c |= 1;
			prev_va = curr_va;
			prev_a = curr_a;
		}
		if ((disabled & 2) == 0 && (prev_vb != curr_vb || (prev_b != curr_b && prev_vb != 0))) {
			sprintf(ap, ",$%04x", curr_vb | curr_b << 4);
			ap += 6;
			c |= 2;
			prev_vb = curr_vb;
			prev_b = curr_b;
		}
		if ((disabled & 4) == 0 && (prev_vc != curr_vc || (prev_c != curr_c && prev_vc != 0))) {
			sprintf(ap, ",$%04x", curr_vc | curr_c << 4);
			ap += 6;
			c |= 4;
			prev_vc = curr_vc;
			prev_c = curr_c;
		}
		*ap++ = '\n';
		*ap = '\0';
		if (c) {
			ap = buffer + 7 + sprintf(buffer + 7, "%04x", e | (d << 4));
			*ap = ',';
			fprintf(output, "%s", buffer);
			strcpy(buffer, buffer2);
			d = 1;
			e = c;
		} else {
			d++;
		}
	}
	if (d) {
		ap = buffer + 7 + sprintf(buffer + 7, "%04x", e | (d << 4));
		*ap = ',';
		fprintf(output, "%s", buffer);
	}
	fprintf(output, "\tDATA 0\n");
	fclose(output);
	fclose(input);
	exit(0);
}
