# EI_CustomProcessingBlock

**What it Does**

This Edge Impulse Custom Processing Block allows one to implemet:
* a Low Pass Filter and
* a Notch Filter.

New Features are generated based on:
* Mean
* Median
* StDev
* Skew
* Kurtosis

Other statistics can easily be added near line 150.

**Help**

To get this code running follow this [tutorial](https://docs.edgeimpulse.com/docs/edge-impulse-studio/processing-blocks/custom-blocks).

Also watch this [video](https://www.youtube.com/watch?v=7vr4D_zlQTE) which has some good info on cppType.

Other useful related links:

[Forum post about cppType](https://forum.edgeimpulse.com/t/doubt-about-the-custom-preprocessing-block/4805)

[C++ Library](https://docs.edgeimpulse.com/docs/deployment/running-your-impulse-locally)

[Running on a Desktop (handy for troubleshooting Custom Blocks)](https://docs.edgeimpulse.com/docs/deployment/running-your-impulse-locally/running-your-impulse-locally)

**TODO:**

* I am not sure I am implementing the butter() filter correctly. There is an issue of *normailzation*. See [this](https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.butter.html).
* Add a check on filter to make sure user entered parameters make sense. For example, at present the EI GUI allows a float to be enetered into an integer field. The GUI designed is forcing the backend designer to do his work for him. The GUI designer needs to inspect the `parameters.json` file to see what is alllowed in a given GUI field.
* Add error checking
* General cleanup code
* Test with known frequencies. *Until then in `scipy` and `numpy` we trust.*

************************************************************************************
* The MIT License (MIT)                                                            *
*                                                                                  *
* Copyright (c) 2022 M. Marcial                                                    *
* If you use this code, please give me some credit.                                *
*                                                                                  *
* Permission is hereby granted, free of charge, to any person obtaining a copy     *
* of this software and associated documentation files (the "Software"), to deal    *
* in the Software without restriction, including without limitation the rights     *
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
* copies of the Software, and to permit persons to whom the Software is            *
* furnished to do so, subject to the following conditions:                         *
* The above copyright notice shall be included in                                  *    
* all copies or substantial portions of the Software.                              *
*                                                                                  *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR       *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,         *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER           *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,    *
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE    *
* SOFTWARE.                                                                        *
************************************************************************************
