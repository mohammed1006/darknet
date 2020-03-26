sed -i '/GPU=1/c\GPU=0' Makefile;
sed -i '/CUDNN=1/c\CUDNN=0' Makefile;
sed -i '/CUDNN_HALF=1/c\CUDNN_HALF=0' Makefile;
sed -i '/OPENCV=0/c\OPENCV=1' Makefile;
sed -i '/AVX=0/c\AVX=1' Makefile;
