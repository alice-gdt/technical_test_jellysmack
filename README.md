# technical_test_jellysmack

## Compilation

Do in your working directory:  
$> cmake  
$> make  

## Usage

For example, do in your working directory:  
$> ./tech_test_JS ./tests/asset_1.mp4 lib_assets/

'Escap' allow to quit during execution.

## Option

Do in a console:  
$> ./tech_test_JS ./tests/asset_1.mp4 IM  

IM allow to see each comparison between a frame and an asset. Do 'Enter' to go to next image.  

## Problems
If you have problems with opencv2/xfeature2d.hpp, do in a console :  
$> sudo add-apt-repository ppa:ignaciovizzo/opencv3-nonfree  
$> sudo apt install libopencv-dev  
And then in your working directory :  
$> cmake -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules

## Complementary note
The term "asset" *needs* to be in the filename of the images in your lib directory if you want them to be processed and compared.

## Not done
The orientation is not printed.  
The bonus has not been done.
