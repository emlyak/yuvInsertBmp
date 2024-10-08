## About
Simple program for overlaying an .bmp image on a yuv420 video sequence. 

## Building

```sh
mkdir build;
cd build;
cmake ..;
cmake --build .;
```

## Using

Use this syntax to run program:

```sh
./yuv_modder <Path/to/YUV/video> <Path/to/BMP/image> <Path/to/output/directory>
```

Example:
``` sh
./yuv_modder ../akiyo_cif.yuv ../sample_2.bmp ../
```
