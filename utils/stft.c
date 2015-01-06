/*
Copyright 2015 Ahmet Inan <xdsopl@googlemail.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 */

#include <math.h>
#include <complex.h>
#include <stdio.h>

double gauss(double n, double N, double o)
{
	return exp(- 1.0/2.0 * pow((n - (N - 1.0) / 2.0) / (o * (N - 1.0) / 2.0), 2.0));
}

int main()
{
	const int N = 1 << 14;
	printf("/* code generated by 'utils/stft.c' */\n");
	printf("static const int stft_N = %d;\n", N);
	printf("static const float stft_w[%d] = {\n", N);
	double sum = 0.0;
	for (int n = 0; n < N; n++)
		sum += gauss(n, N|1, 0.2);
	for (int n = 0; n < N; n++) {
		printf("\t%g%s\n", gauss(n, N, 0.2) / sum, n < (N-1) ? "," : "");
	}
	printf("};\n");
	return 0;
}