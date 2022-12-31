. ../../config.ini
TFLITE_BSP_PATH=$GITHUB_TFLITE_PATH/tflite-micro/tensorflow/lite/micro/tools/make/targets/andesevb/ae350
TFLITE_MAKE=$GITHUB_TFLITE_PATH/tflite-micro/tensorflow/lite/micro/tools/make/Makefile

TARGET_MAKE=$EVB_MAKE_FILE

if [ "$cpu" = "d45" ]; then
	cp $TARGET_ANDES_PATH/andescore_d45.tgz $TARGET_ANDES_PATH/andescore.tgz
elif [ "$cpu" = "d25" ]; then
    cp $TARGET_ANDES_PATH/andescore_d25.tgz $TARGET_ANDES_PATH/andescore.tgz
elif [ "$cpu" = "nx27v" ]; then
    cp $TARGET_ANDES_PATH/andescore_nx27v.tgz $TARGET_ANDES_PATH/andescore.tgz
else
	echo "No CPU option..."
fi

CHECK_DIR=$GITHUB_TFLITE_PATH
if [ -d ${CHECK_DIR}* ] 2>/dev/null; then
	echo "the folder ${CHECK_DIR} is exist"
else
	bash -c "\
    mkdir $GITHUB_TFLITE_PATH;\
    pushd $GITHUB_TFLITE_PATH;\
    git clone https://github.com/tensorflow/tflite-micro.git;\
    cd tflite-micro;\
	git checkout -b master-mlperf a30942eb03efc379e9e80279a44e39078344e5fa;\
    tar zxvf $TARGET_ANDES_PATH/andescore.tgz;\
    popd;\
    echo 'done'"
fi

bash -c "\
    mkdir $GITHUB_TFLITE_PATH;\
    pushd $GITHUB_TFLITE_PATH;\
    cd tflite-micro;\
    tar zxvf $TARGET_ANDES_PATH/andescore.tgz;\
    popd;\
    echo 'done'"


echo "TARGET_PATH=${TARGET_MAKE}"
echo "TFLITE_PATH=${TFLITE_MAKE}"

if [ "$cpu" = "d45" ]; then
#delete
	#sed -i "/CCFLAGS += -mtune=andes-45-series/{n;d}" $TARGET_MAKE
	sed -i "/CCFLAGS += -mtune=andes-45-series/d" $TARGET_MAKE
	#sed -i "/CXXFLAGS += -mtune=andes-45-series/{n;d}" $TARGET_MAKE
	sed -i "/CXXFLAGS += -mtune=andes-45-series/d" $TARGET_MAKE
elif [ "$cpu" = "d25" ]; then
#delete
	#sed -i "/CCFLAGS += -mtune=andes-45-series/{n;d}" $TARGET_MAKE
	sed -i "/CCFLAGS += -mtune=andes-45-series/d" $TARGET_MAKE
	#sed -i "/CXXFLAGS += -mtune=andes-45-series/{n;d}" $TARGET_MAKE
	sed -i "/CXXFLAGS += -mtune=andes-45-series/d" $TARGET_MAKE
#modify
elif [ "$cpu" = "nx27v" ]; then
	echo "CPU: nx27v"
else
	echo "No CPU option..."
fi

if [ "$opt" = "1" ]; then
	echo "opt=1"
	sed -i 's/-O0/-O3/g' $TARGET_MAKE
else
	echo "opt=0"
	sed -i 's/-O3/-O0/g' $TARGET_MAKE
fi

cp $GITHUB_TFLITE_PATH/tflite-micro/tensorflow/lite/micro/tools/make/targets/andesevb/ae350/lib/libae350.a.evb $GITHUB_TFLITE_PATH/tflite-micro/tensorflow/lite/micro/tools/make/targets/andesevb/ae350/lib/libae350.a
echo "lib is exist"


