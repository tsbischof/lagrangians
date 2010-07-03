#include "gpu_brook.h"
#include "harmonic_oscillator.h"

#include "../../parse_utils.h"
#include "../../image_funcs.h"

#include <brook/Stream.h>
#include <time.h>
#include <omp.h>
#include <stdio.h>

void gpu_brook(Grapher *grapher) {
	int i, row;
    time_t rawtime;
    struct tm * timeinfo;
    char fmttime[100];

	float4 r0[grapher->width];
	unsigned dimensions[] = {grapher->width};
	::brook::Stream<float4> r0Stream(1, dimensions);
	::brook::Stream<float> resultStream(1, dimensions);
	float4 t_limits;
	t_limits[0] = grapher->t_limits[0];
	t_limits[1] = grapher->t_limits[1];
	t_limits[2] = grapher->t_limits[2];
	t_limits[3] = 0;
	float row_temp[grapher->width];

	char *gpu_systems[1] = {"harmonic_oscillator"};

	int kernel;
	if ( (kernel = contains(gpu_systems, grapher->system)) >= 0 ) {
		printf("System %s has a GPU kernel.\n", grapher->system);
	} else {
		printf("Fatal: GPU computation requested, but system %s is not supported.\n", grapher->system);
		exit(1);
	}
	 
	
//#pragma omp parallel for private(row, indicesStream, resultStream) firstprivate(indices)  schedule(dynamic)
// Evidently, Stream does not have a copy operator, so do not use omp for now.
    for (row = 0; row < grapher->height; row++) {
		if ( grapher->finished_rows[row] ) {
			printf("Found finished row %d.\n", row);
		} else {
			time(&rawtime);
        	timeinfo = localtime(&rawtime);
        	strftime(fmttime, 100, "%Y.%m.%d %H:%M:%S", timeinfo);
        	printf("%s: On row %d of %d (%.1f%%).\n", fmttime, row,
           			grapher->height-1, row/(float)(grapher->height-1)*100);

			for (i = 0; i < grapher->width; i++) {
				r0[i][0] = grapher->r0[0];
				r0[i][1] = grapher->r0[1];
				r0[i][2] = grapher->r0[2];
				r0[i][3] = grapher->r0[3];
				
				r0[i][grapher->parm1_index] += row*grapher->parm1_limits[1];
				r0[i][grapher->parm2_index] += i*grapher->parm2_limits[1];
			}

        	r0Stream.read(r0);
			if ( r0Stream.error() ) {
				printf("Error while reading stream: %s\n", 
					r0Stream.errorLog());
			}
			harmonic_oscillator_gpu(r0Stream, t_limits, resultStream);
//			resultStream.write(grapher->image[row]);
			resultStream.write(row_temp);
			for (i = 0; i < grapher->width; i++) {
				grapher->image[row][i] = (double)row_temp[i];
			} 

			write_restart_row(grapher, row);
			grapher->finished_rows[row] = 1;

			if ( resultStream.error() ) {
				printf("Error while computing: %s\n", resultStream.errorLog());
			}

		}
   }
}

