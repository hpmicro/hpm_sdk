. ../../config.ini

TARGET_MAKE=$EVB_MAKE_FILE

#copy
cp -r ../api $GITHUB_TFLITE_PATH/tflite-micro/tensorflow/lite/micro/examples/mlperf_libnn
cp -r ../ic $GITHUB_TFLITE_PATH/tflite-micro/tensorflow/lite/micro/examples/mlperf_libnn
cp -r ../util $GITHUB_TFLITE_PATH/tflite-micro/tensorflow/lite/micro/examples/mlperf_libnn
cp ../main.cc $GITHUB_TFLITE_PATH/tflite-micro/tensorflow/lite/micro/examples/mlperf_libnn
cp ../Makefile.inc $GITHUB_TFLITE_PATH/tflite-micro/tensorflow/lite/micro/examples/mlperf_libnn

#modify
cp ../submitter_implemented.cpp $GITHUB_TFLITE_PATH/tflite-micro/tensorflow/lite/micro/examples/mlperf_libnn/submitter_implemented.cc
cp ../api/internally_implemented.cpp $GITHUB_TFLITE_PATH/tflite-micro/tensorflow/lite/micro/examples/mlperf_libnn/api/internally_implemented.cc


cd $GITHUB_TFLITE_PATH/tflite-micro

if [ "$cpu" = "nx27v" ]; then
	make -f tensorflow/lite/micro/tools/make/Makefile TARGET=andes_evb OPTIMIZED_KERNEL_DIR=andes_libnn TARGET_ARCH=rv64p BUILD_TYPE=$run_mode mlperf_libnn
else
	make -f tensorflow/lite/micro/tools/make/Makefile TARGET=andes_evb OPTIMIZED_KERNEL_DIR=andes_libnn TARGET_ARCH=rv32p BUILD_TYPE=$run_mode mlperf_libnn
fi

cd ../../

