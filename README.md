# technical_test_jellysmack

## Compilation

Do in a console:  
$> cmake  
$> make  

## Usage

Do in a console:  
$> ./tech_test_JS ./tests/asset_1.mp4  

'Escap' allow to quit during execution.

## Option

Do in a console:  
$> ./tech_test_JS ./tests/asset_1.mp4 IM  

IM allow to see each image of video flux. Do 'Enter' to go to next image.  

## Problems
If you have problems with opencv2/xfeature2d.hpp, do in a console :  
$> sudo add-apt-repository ppa:ignaciovizzo/opencv3-nonfree  
$> sudo apt install libopencv-dev  
And then in your working directory :  
$> cmake -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules
