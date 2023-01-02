As of 27-Dec-2022 this program has not been released.

**_Program Description_**

  **Seeing What the Classifier() Saw in a FOMO Model**

  This [program](https://github.com/DeveloperMarcial/Edge_Impulse/tree/trunk/Arduino_EdgeImpulse/Sony_Spresense/Spresense-Vision/Spresense-VisionModel_SdCard) runs an *EdgeImpulse* (EI) FOMO Machine Learning (ML) model that is looking to find one or more screws
  in a single image captured by a *Sony* Spresense Camera module connected to a *Sony* Spresense Main board. When one or more FOMOed
  objects are found, then a BMP is written to a SD card on the *Sony* Spresense Extension board with the centroid of the 
  predicated object location overlayed on the BMP in a rectangular black outline. The size and location of the centroid rectangle is an output on the *EdgeImpulse* FOMO model inference routine. (Note: On Windows 10 the Microsoft Photos app displays the BMPs. On Ubuntu the Image View app will not display the BMPs and says the BMP header is corrupt.)

  **_Background_**
  
Being able to see were the *EdgeImpulse* Classifier() placed bounding boxes in a *EdgeImpulse* FOMO model is very useful to see why your Model is giving False Positives.

  The *EdgeImpulse* Python code [here](https://github.com/edgeimpulse/linux-sdk-python/blob/master/examples/image/classify-image.py) imports the object "ImageImpulseRunner".
  The code uses the *Runner* to classify an image passed in as a command line argument. If the classified segmented image has bounding boxes, then the code draws
  bounding boxes as determined by the Classifier() on to the image and then saves the image as a JPEG.
  **_The bounding boxes in this case seem to be identifying the location and size of the FOMOed object (not the centroid of the predicted object)._**
  
  As a trial before creating this program, I used this Python *Runner* tool on a Model trained to identify swimming pools. Upon running the Python tool,
  I found FOMOed images with green awnings attached to a house, and blue tarps covering hay piles.
  So now I can add new classes that are not pools to improve the Model's accuarcy.
  As a side note, I trained the model on aerial photos taken from a spring/summer campaign.
  Swimming pools were identified and bounding boxes drawn on the training images.
  On deployment I ran the Model against an very large unknown dataset and found 1 or 2 pools but there should have been a few hundred pools found.
  This failure was becasue the unknown dataset was taken in a winter campaign were the trees are bare of leaves, the grass in brown,
  and swimming pools are either empty or covered on black or brown tarps making them look totally different to the Model but not to
  most humans since we use other *context* clues to make out observations. Perhaps adding, for example, to the training dataset a context clue would be the distance from a structrue, like a house, to the swimmimg pool that should be normally under 100 feet. This should eliminate blue tarps in a farmers field from being identified a swimming pools.

**_Program Implementation_**

  **There is a big discrepency in the reliability of the model performance
  depending on what software gets deployed to the *Spresense*.**

  _Case 1: Arduino Library_
  
  When the model runs on the *Sony* Spresense using an *EdgeImpulse* deployed *Arduino* library
  the Model seems to be having trouble recognizing screw objects in a repeatable manner.
  Every now and then it will recognize a screw as an object in an image and save it as a BMP with BB centroid. Even more rarely does it recognize
  2 screws in the same image when 2 screws are present.
  At times it will recognize 1 of the 2 screws present in the frame.
  The EI Studio Training shows F1=100% accuarcy and the Test dataset shows F1=90% accuarcy. [See EdgeImpulse Studio Project ID: 173213, Spresese FOMO Screws.]

  This program is intended to save the image that got FOMOed so that we can see where the
  Classifier drew centroid of the bounding boxes. The code is optimized to save the images as grayscale BMPs
  and does not handle color BMPs. If you want to save color BMPs, see this [code](https://github.com/DeveloperMarcial/Edge_Impulse/tree/trunk/Arduino_EdgeImpulse/Sony_Spresense/Spresense-Vision/Spresense-CameraToSdCardAsBMP)
  that can handle grayscale _and_ color BMPs but is more compilcated than it needs to be in that it
  contains many compiler directives and additional code so that it can handle either case.

  _Case 2: EdgeImpulse Firmware_
  
  Confusingly, when the *Sony* Spresense is flashed with the *EdgeImpulse* Spresense firmware,
  connected to the *EI* Studio, and then a Live Classification is run,
  the results in the *EI* Studio are always correct, aka, recognizing 1 screw when 1 screw is present, or
  recognizing 2 screws when 2 screws are present, or recognizing 3 screw when 3 screws are present with a F1 score of 90% or better.

**_Questions_**

  Q1. Is there other example code to draw bounding boxes and saves the Framebuffer as a JPEG (or any image format)
      in C++ or more specifically in Arduino code with a Sony Spresense?

  Q2. Is the difference in Bounding Box size between the Python code and the Arduino Library code
      due to the assumed difference in CPU speed and avaliable memory so each code base
      got coded differently?
      
**_TODO_**      

* Investigate the code differences between the *Arduino* Library and the *EdgeImpulse* Spresense Firmware to see why the *Arduino* implementation is not *reliably identifying* individuals screws in a single image.

* Try running this Model on a different MCU, like an *Arduino* Portenta, to see how this particular FOMO model performs on device. (It may be that the *EI* model deployed MUST be a float model as was the case when I tested the *EdgeImpulse* Spresense Firmware and then went on and implemented an Arduino program running on a *Sony Spresense* using a a [keyphrase detection (float not an int8) Model](https://github.com/DeveloperMarcial/Edge_Impulse/tree/trunk/Arduino_EdgeImpulse/Sony_Spresense/Spresense-AudioModel/Code/Spresense-AudioModel).

|Filename / Folder Name | Description |
| --- | --- |
| Example BMPs with BB Centroid | Example BMPs output my this program as deployed on a *Sony* Spresense |
| Example JPGs with BB| Example files used in the implementation of the Python code that runs a FOMO model and saves the results with BBs as a JPG. |
| ei-spresense-fomo-screws-arduino-1.0.5_int8.zip | The *Arduino* library used for this program |
| ei-spresense-fomo-screws-arduino-1.0.6_float.zip | The *Arduino* library used for this program but will not run on the *Sony* Spresense due to its RAM requirements. |
| Misc Color Routines.md| Ignore this file. (Routines that may be useful when converting this program to save color BMPs. |
| README.md| This file. |
| SaveModifiedBMP.h | Global variables for `SaveModifiedBMP.ino` that are used in the main Arduino INO file `Spresense-VisionModel_SdCard.ino`. |
| SaveModifiedBMP.ino | This file holds the routines needed to create a BMP file and save it to an SD card. This file is broken out to make the main INO easer to use and to isolate the routines needed for BMPs. |
| spresense-fomo-screws-linux-x86_64-v2_float.eim| The *EdgeImpluse* Mode (EIM) file used in the Python code that writes JPGs with embedded BBs. |
| spresense-fomo-screws-linux-x86_64-v2_int8.eim| The *EdgeImpluse* Mode (EIM) file used in the Python code that writes JPGs with embedded BBs. |
| Spresense-VisionModel_SdCard.ino | The main *Arduino* file that runs the *EdgeImpulse* ML model. |
| | |
