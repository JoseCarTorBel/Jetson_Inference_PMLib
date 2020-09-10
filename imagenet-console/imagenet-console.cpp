#include "imageNet.h"
#include "loadImage.h"
#include "cudaFont.h"
#include "tensorNet.h"
#include "commandLine.h"
#include "/home/nvidia/software/PMLib/Python/client/pmlib.h"

#define ENERGY
// main entry point
int main( int argc, char** argv )
{
	if(argc < 2){
		printf("example usage: ./imagenet-console model=network batch_size=batchsize");
	}
	
	commandLine cmdLine(argc, argv);
	const char* precision = cmdLine.GetString("precision");
	printf("Precision: %s \n",precision);
	int runbatchsize = cmdLine.GetInt("batch_size");
	printf("RUNBATCHSIZE-> %d\n",runbatchsize);
	// AÑADIR CÓDIGO TENSORNET PARA PRECISION
	
	//precisionType TYPE = precisionTypeFromStr( precision );

	// create imageNe
	imageNet* net = imageNet::Create(argc, argv);
	if( !net )
	{
		printf("imagenet-console:   failed to initialize imageNet\n");
		return 0;
	}
	
	
	net->EnableProfiler();
	net->EnableDebug();
	printf("\n===============Main::imageNet created.==============\n");
		
	
	float* imgCPU    = NULL;
	float* imgCUDA   = NULL;
	int    imgWidth  = 0;
	int    imgHeight = 0;
        char imgFilename[100];

	for(int i = 0; i < runbatchsize; i++)
	{
                sprintf(imgFilename, "pictures/ILSVRC2012_val_00000%03d.JPEG", i);
		if( !loadImageRGBA(imgFilename, (float4**)&imgCPU, (float4**)&imgCUDA, &imgWidth, &imgHeight) )
		{
			printf("failed to load image '%s'\n", (char*)imgFilename);
			return 0;
		}
		net->ClassifyCuda(i, imgCUDA, imgWidth, imgHeight);
	}
	printf("\n===============Main::image loaded.==================\n");
	// classify image
	float confidence = 0.0f;

#ifdef ENERGY
	server_t server;
	counter_t counter;
	line_t lines;
	int i, frequency= 0, aggregate= 1;
	pm_set_server("127.0.0.1", 6526, &server);
        pm_set_lines("0-11", &lines);
        pm_create_counter("Jetson-TX2", lines, !aggregate, frequency, server, &counter);
  	pm_start_counter(&counter);
#endif

	for(int reps=0; reps<20; reps++)
	  net->ClassifyBatch(runbatchsize);

#ifdef ENERGY
	pm_stop_counter(&counter);
	pm_get_counter_data(&counter);
        pm_calculate_energy(counter, lines, -1);	
	//pm_print_data_text("out.txt", counter, lines, -1);
	pm_finalize_counter(&counter);
#endif

	// show the results
	printf("\nshutting down...\n");
	CUDA(cudaFreeHost(imgCPU));
	delete net;
	return 0;
}
