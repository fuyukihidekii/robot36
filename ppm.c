/*
robot36 - encode and decode images using SSTV in Robot 36 mode
Written in 2011 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mmap_file.h"
#include "img.h"

typedef struct {
	void (*close)(img_t *);
	uint8_t *pixel;
	int width;
	int height;
	void *p;
	size_t size;
} ppm_t;

void close_ppm(img_t *img)
{
	ppm_t *ppm = (ppm_t *)img;
	munmap_file(ppm->p, ppm->size);
}

int open_ppm_read(img_t **p, char *name) {
	ppm_t *ppm = (ppm_t *)malloc(sizeof(ppm_t));
	ppm->close = close_ppm;

	if (!mmap_file_ro(&(ppm->p), name, &(ppm->size))) {
		fprintf(stderr, "couldnt open image file %s\n", name);
		free(ppm);
		return 0;
	}

	char *chr = (char *)ppm->p;
	size_t index = 0;
	if (ppm->size < 10 || chr[index++] != 'P' || chr[index++] != '6') {
		fprintf(stderr, "unsupported image file\n");
		munmap_file(ppm->p, ppm->size);
		free(ppm);
		return 0;
	}

	char buff[16];
	int integer[3];
	for (int n = 0; n < 3; n++) {
		for (; index < ppm->size; index++) {
			if (chr[index] >= '0' && chr[index] <= '9')
				break;
			if (chr[index] == '#')
				for (; index < ppm->size && chr[index] != '\n'; index++);
		}
		for (int i = 0; i < 16; i++)
			buff[i] = 0;
		for (int i = 0; index < ppm->size && i < 15; i++, index++) {
			buff[i] = chr[index];
			if (buff[i] < '0' || buff[i] > '9') {
				buff[i] = 0;
				break;
			}
		}
		if (index >= ppm->size) {
			fprintf(stderr, "broken image file\n");
			munmap_file(ppm->p, ppm->size);
			free(ppm);
			return 0;
		}
		integer[n] = atoi(buff);
		index++;
	}

	if (0 == integer[0] || 0 == integer[1] || integer[2] != 255) {
		fprintf(stderr, "unsupported image file\n");
		munmap_file(ppm->p, ppm->size);
		free(ppm);
		return 0;
	}
	ppm->width = integer[0];
	ppm->height = integer[1];

	ppm->pixel = (uint8_t *)ppm->p + index;

	*p = (img_t *)ppm;

	return 1;
}

int open_ppm_write(img_t **p, char *name, int width, int height)
{
	ppm_t *ppm = (ppm_t *)malloc(sizeof(ppm_t));
	ppm->close = close_ppm;

	char head[32];
	snprintf(head, 32, "P6 %d %d 255\n", width, height);
	ppm->size = strlen(head) + width * height * 3;

	if (!mmap_file_rw(&(ppm->p), name, ppm->size)) {
		fprintf(stderr, "couldnt open image file %s\n", name);
		free(ppm);
		return 0;
	}

	memcpy(ppm->p, head, strlen(head));
	ppm->pixel = (uint8_t *)ppm->p + strlen(head);
	memset(ppm->pixel, 0, width * height * 3);

	*p = (img_t *)ppm;

	return 1;
}

