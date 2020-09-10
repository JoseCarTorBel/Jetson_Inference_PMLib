/*
 * Copyright (c) 2017, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
 
#include "imageNet.h"
#include "cudaMappedMemory.h"
#include "cudaResize.h"
#include "commandLine.h"


// constructor
imageNet::imageNet() : tensorNet()
{
	mCustomClasses = 0;
	mOutputClasses = 0;
}

// destructor
imageNet::~imageNet()
{}

// Create
imageNet* imageNet::Create( imageNet::NetworkType networkType, uint32_t maxBatchSize )
{
	printf("imageNet::Create_1...\n");
	imageNet* net = new imageNet();
	
	if( !net )
		return NULL;
	
	if( !net->init(networkType, maxBatchSize) )
	{
		printf("imageNet -- failed to initialize.\n");
		return NULL;
	}
	
	return net;
}

// Create
imageNet* imageNet::Create( const char* prototxt_path, const char* model_path, const char* mean_binary,
							const char* class_path, const char* input, const char* output, uint32_t maxBatchSize )
{
	printf("imageNet::Create_2...\n");
	imageNet* net = new imageNet();
	
	if( !net )
		return NULL;
	
	if( !net->init(prototxt_path, model_path, mean_binary, class_path, input, output, maxBatchSize) )
	{
		printf("imageNet -- failed to initialize.\n");
		return NULL;
	}
	
	return net;
}

// Create
imageNet* imageNet::Create( int argc, char** argv )
{
	printf("imageNet::Create...\n");

	commandLine cmdLine(argc, argv);
	const char* modelName = cmdLine.GetString("model");
	if( !modelName )
	{
		if( argc == 2 )
			modelName = argv[1];
		else if( argc == 4 )
			modelName = argv[3];
		else
			modelName = "googlenet";
	}

	imageNet::NetworkType type = imageNet::GOOGLENET;

	int maxBatchSize = cmdLine.GetInt("batch_size");	
	if( maxBatchSize < 1 )	maxBatchSize = 2;
	printf("batch_size:%u\n", maxBatchSize);

	if( strcasecmp(modelName, "alexnet") == 0 )
                type = imageNet::ALEXNET;
        else if( strcasecmp(modelName, "googlenet") == 0 )
                type = imageNet::GOOGLENET;
        else if( strcasecmp(modelName, "googlenet-12") == 0 || strcasecmp(modelName, "googlenet_12") == 0 )
                type = imageNet::GOOGLENET_12;
        else if( strcasecmp(modelName, "resnet-18") == 0 || strcasecmp(modelName, "resnet_18") == 0 || strcasecmp(modelName, "resnet18") == 0 )
                type = imageNet::RESNET_18;
        else if( strcasecmp(modelName, "resnet-50") == 0 || strcasecmp(modelName, "resnet_50") == 0 || strcasecmp(modelName, "resnet50") == 0 )
                type = imageNet::RESNET_50;
        else if( strcasecmp(modelName, "resnet-101") == 0 || strcasecmp(modelName, "resnet_101") == 0 || strcasecmp(modelName, "resnet101") == 0 )
                type = imageNet::RESNET_101;
        else if( strcasecmp(modelName, "resnet-152") == 0 || strcasecmp(modelName, "resnet_152") == 0 || strcasecmp(modelName, "resnet152") == 0 )
                type = imageNet::RESNET_152;
        else if( strcasecmp(modelName, "vgg-16") == 0 || strcasecmp(modelName, "vgg_16") == 0 || strcasecmp(modelName, "vgg16") == 0 )
                type = imageNet::VGG_16;
        else if( strcasecmp(modelName, "vgg-19") == 0 || strcasecmp(modelName, "vgg_19") == 0 || strcasecmp(modelName, "vgg19") == 0 )
                type = imageNet::VGG_19;
        else if( strcasecmp(modelName, "inception-v4") == 0 || strcasecmp(modelName, "inception_v4") == 0 || strcasecmp(modelName, "inceptionv4") == 0 )
                type = imageNet::INCEPTION_V4;

	else
	{
		const char* prototxt = cmdLine.GetString("prototxt");
		const char* labels   = cmdLine.GetString("labels");
		const char* input    = cmdLine.GetString("input_blob");
		const char* output   = cmdLine.GetString("output_blob");
		const char* out_bbox = cmdLine.GetString("output_bbox");
		
		if( !input ) 	input   = IMAGENET_DEFAULT_INPUT;
		if( !output )  output   = IMAGENET_DEFAULT_OUTPUT;
		
		return imageNet::Create(prototxt, modelName, NULL, labels, input, output, maxBatchSize);
	}

	// create from pretrained model
	return imageNet::Create(type, maxBatchSize);
}
		
// init
bool imageNet::init( imageNet::NetworkType networkType, uint32_t maxBatchSize )
{
	printf("imageNet::init...\n");

	  if( networkType == imageNet::ALEXNET )
                return init( "networks/alexnet.prototxt", "networks/bvlc_alexnet.caffemodel", NULL, "networks/ilsvrc12_synset_words.txt", IMAGENET_DEFAULT_INPUT, IMAGENET_DEFAULT_OUTPUT, maxBatchSize);
        else if( networkType == imageNet::GOOGLENET )
                return init( "networks/googlenet.prototxt", "networks/bvlc_googlenet.caffemodel", NULL, "networks/ilsvrc12_synset_words.txt", IMAGENET_DEFAULT_INPUT, IMAGENET_DEFAULT_OUTPUT, maxBatchSize);
        else if( networkType == imageNet::GOOGLENET_12 )
                return init( "networks/GoogleNet-ILSVRC12-subset/deploy.prototxt", "networks/GoogleNet-ILSVRC12-subset/snapshot_iter_184080.caffemodel", NULL, "networks/GoogleNet-ILSVRC12-subset/labels.txt", IMAGENET_DEFAULT_INPUT, "softmax", maxBatchSize );
        else if( networkType == imageNet::RESNET_18 )
                return init( "networks/ResNet-18/deploy.prototxt", "networks/ResNet-18/ResNet-18.caffemodel", NULL, "networks/ilsvrc12_synset_words.txt", IMAGENET_DEFAULT_INPUT, IMAGENET_DEFAULT_OUTPUT, maxBatchSize ); 
        else if( networkType == imageNet::RESNET_50 )
                return init( "networks/ResNet-50/deploy.prototxt", "networks/ResNet-50/ResNet-50.caffemodel", NULL, "networks/ilsvrc12_synset_words.txt", IMAGENET_DEFAULT_INPUT, IMAGENET_DEFAULT_OUTPUT, maxBatchSize); 
        else if( networkType == imageNet::RESNET_101 )
                return init( "networks/ResNet-101/deploy.prototxt", "networks/ResNet-101/ResNet-101.caffemodel", NULL, "networks/ilsvrc12_synset_words.txt", IMAGENET_DEFAULT_INPUT, IMAGENET_DEFAULT_OUTPUT, maxBatchSize);      
        
        else if( networkType == imageNet::RESNET_152 )
                return init( "networks/ResNet-152/deploy.prototxt", "networks/ResNet-152/ResNet-152.caffemodel", NULL, "networks/ilsvrc12_synset_words.txt", IMAGENET_DEFAULT_INPUT, IMAGENET_DEFAULT_OUTPUT, maxBatchSize);      
        
        else if( networkType == imageNet::VGG_16 )
                return init( "networks/VGG-16/deploy.prototxt", "networks/VGG-16/VGG-16.caffemodel", NULL, "networks/ilsvrc12_synset_words.txt", IMAGENET_DEFAULT_INPUT, IMAGENET_DEFAULT_OUTPUT, maxBatchSize);
        else if( networkType == imageNet::VGG_19 )
                return init( "networks/VGG-19/deploy.prototxt", "networks/VGG-19/VGG-19.caffemodel", NULL, "networks/ilsvrc12_synset_words.txt", IMAGENET_DEFAULT_INPUT, IMAGENET_DEFAULT_OUTPUT, maxBatchSize);          
        else if( networkType == imageNet::INCEPTION_V4 )
                return init( "networks/Inception-v4/deploy.prototxt", "networks/Inception-v4/Inception-v4.caffemodel", NULL, "networks/ilsvrc12_synset_words.txt", IMAGENET_DEFAULT_INPUT, IMAGENET_DEFAULT_OUTPUT, maxBatchSize );
        
        else
                return false;

}



// init
bool imageNet::init(const char* prototxt_path, const char* model_path, const char* mean_binary, const char* class_path, const char* input, const char* output, uint32_t maxBatchSize )
{
	if( !prototxt_path || !model_path || !class_path || !input || !output )
		return false;

	printf("\n");
	printf("imageNet -- loading classification network model from:\n");
	printf("         -- prototxt     %s\n", prototxt_path);
	printf("         -- model        %s\n", model_path);
	printf("         -- class_labels %s\n", class_path);
	printf("         -- input_blob   '%s'\n", input);
	printf("         -- output_blob  '%s'\n", output);
	printf("         -- batch_size   %u\n\n", maxBatchSize);

	/*
	 * load and parse googlenet network definition and model file
	 */
	if( !tensorNet::LoadNetwork( prototxt_path, model_path, mean_binary, input, output, maxBatchSize ) )
	{
		printf("failed to load %s\n", model_path);
		return false;
	}

	//printf(LOG_GIE "%s loaded\n", model_path);

	/*
	 * load synset classnames
	 */
	mOutputClasses = DIMS_C(mOutputs[0].dims); //defined in tensorNet.h
	//printf(LOG_GIE "%u mOutputClasses\n", mOutputClasses);
	if( !loadClassInfo(class_path) || mClassSynset.size() != mOutputClasses || mClassDesc.size() != mOutputClasses )
	{
		printf("imageNet -- failed to load synset class descriptions  (%zu / %zu of %u)\n", mClassSynset.size(), mClassDesc.size(), mOutputClasses);
		return false;
	}
	
	printf("---------------imageNet::init end.-----------------------\n");
	return true;
}
			

// loadClassInfo
bool imageNet::loadClassInfo( const char* filename )
{
	if( !filename )
		return false;
	
	FILE* f = fopen(filename, "r");
	
	if( !f )
	{
		printf("imageNet -- failed to open %s\n", filename);
		return false;
	}
	
	char str[512];

	while( fgets(str, 512, f) != NULL )
	{
		const int syn = 9;  // length of synset prefix (in characters)
		const int len = strlen(str);
		
		if( len > syn && str[0] == 'n' && str[syn] == ' ' )
		{
			str[syn]   = 0;
			str[len-1] = 0;
	
			const std::string a = str;
			const std::string b = (str + syn + 1);
	
			//printf("a=%s b=%s\n", a.c_str(), b.c_str());

			mClassSynset.push_back(a);
			mClassDesc.push_back(b);
		}
		else if( len > 0 )	// no 9-character synset prefix (i.e. from DIGITS snapshot)
		{
			char a[10];
			sprintf(a, "n%08u", mCustomClasses);

			//printf("a=%s b=%s (custom non-synset)\n", a, str);
			mCustomClasses++;

			if( str[len-1] == '\n' )
				str[len-1] = 0;

			mClassSynset.push_back(a);
			mClassDesc.push_back(str);
		}
	}
	
	fclose(f);
	
	printf("imageNet -- loaded %zu class info entries\n", mClassSynset.size());
	
	if( mClassSynset.size() == 0 )
		return false;
	
	return true;
}

// from imageNet.cu
cudaError_t cudaPreImageNetMean(float4* input, size_t inputWidth, size_t inputHeight, float* output, size_t outputWidth, size_t outputHeight, const float3& mean_value );

cudaError_t cudaPreImageNetMeanBatch(int numBatch, float4* input, size_t inputWidth, size_t inputHeight, float* output, size_t outputWidth, size_t outputHeight, const float3& mean_value );
							
// Classify
int imageNet::Classify( float* rgba, uint32_t width, uint32_t height, float* confidence )
{
	if( !rgba || width == 0 || height == 0 )
	{
		printf("imageNet::Classify( 0x%p, %u, %u ) -> invalid parameters\n", rgba, width, height);
		return -1;
	}

	
	// downsample and convert to band-sequential BGR
	if( CUDA_FAILED(cudaPreImageNetMean((float4*)rgba, width, height, mInputCUDA, mWidth, mHeight,
								 make_float3(104.0069879317889f, 116.66876761696767f, 122.6789143406786f))) )
	{
		printf("imageNet::Classify() -- cudaPreImageNetMean failed\n");
		return -1;
	}


	printf("mWidth=%d,mHeight=%d\n",mWidth,mHeight);	
	// process with GIE
	void* inferenceBuffers[] = { mInputCUDA, mOutputs[0].CUDA };
	
	mContext->execute(1, inferenceBuffers);
	
	//CUDA(cudaDeviceSynchronize());
	PROFILER_REPORT();
	
	
	// determine the maximum class
	int classIndex = -1;
	float classMax = -1.0f;
	
	printf("mOutputClasses:%d;\n",mOutputClasses);
	//printf("moutput:%f;\n",mOutputs[0].CPU);
	for( size_t n=0; n < mOutputClasses; n++ )
	{
		const float value = mOutputs[0].CUDA[n];
		
		if( value >= 0.01f )
			printf("class %04zu - %f  (%s)\n", n, value, mClassDesc[n%mOutputClasses].c_str());

		if( value > classMax )
		{
			classIndex = n;
			classMax   = value;
		}
	}

	if( confidence != NULL )
		*confidence = classMax;
	
	//printf("\nmaximum class:  #%i  (%f) (%s)\n", classIndex, classMax, mClassDesc[classIndex].c_str());
	return classIndex;
}

// Classify
void imageNet::ClassifyCuda(int numBatch, float* rgba, uint32_t width, uint32_t height)
{
	
	if( !rgba || width == 0 || height == 0 )
	{
		printf("imageNet::Classify( 0x%p, %u, %u ) -> invalid parameters\n", rgba, width, height);
	}
	
	// downsample and convert to band-sequential BGR
	if( CUDA_FAILED(cudaPreImageNetMeanBatch(numBatch, (float4*)rgba, width, height, mInputCUDA, mWidth, mHeight,
								 make_float3(104.0069879317889f, 116.66876761696767f, 122.6789143406786f))) )
	{
		printf("imageNet::Classify() -- cudaPreImageNetMean failed\n");
	}

} 
void imageNet::ClassifyBatch(int batchsize)
{	
	
	// process with GIE
	void* inferenceBuffers[] = { mInputCUDA, mOutputs[0].CUDA };
	
	mContext->execute( batchsize, inferenceBuffers);
	
	//CUDA(cudaDeviceSynchronize());
	
	
	PROFILER_REPORT(); //The network total time;
	// determine the maximum class
	int classIndex = -1;
	float classMax = -1.0f;
	
	for( int i = 0; i < batchsize; i++)
	{
		for( size_t n = i * mOutputClasses; n < (i + 1) * mOutputClasses; n++)
		{
			const float value = mOutputs[0].CUDA[n];	
			if( value >= 0.01f )
				printf("class %04zu - %f  (%s)\n", n, value, mClassDesc[n%mOutputClasses].c_str());
			if( value > classMax )
			{
				classIndex = n;
				classMax   = value;
			}
		}
	}	
}






