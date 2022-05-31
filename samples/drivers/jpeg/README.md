# JPEG
## Overview
***
**jpeg** codec driver example project shows how to convert **YUV picture data** into **RGB data** and **RGB data** into **YUV data**.
- decoding function: extract the sample JPG picture data source, the **huffmin**, **huffbase**, **huffsymbol**, **huffenc**, **decoder Q value** table information and **YUV pixel stream** data in the picture data, decode the **YUV data** into **RGB data** through the hardware JPEG module, and display the original picture information through the **LCD**.
- encoding function: encode the picture **rgb565 pixel stream** data and **huffmin**, **huffbase**, **huffsymbol**, **huffenc**, **encoder Q value** table information through the hardware JPEG module, encode the **RGB data** into **YUV data** , then restore it to RGB data through the decoding process, and display the original picture information through the **LCD screen**

## Board Setting
***
- Connect **LCD screen** to development board ``lcd1``

## Running the example
***
When the project runs correctly, the LCD screen will display the picture information of green bamboo correctly, and the serial port terminal will output the following information:
```
> jpeg example
> start decoding
> decoding done
> start encoding and decoding
> encoded 937
> encoding and decoding done
```

