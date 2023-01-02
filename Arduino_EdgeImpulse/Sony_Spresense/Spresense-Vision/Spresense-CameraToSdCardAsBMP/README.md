This program was released on 30-December-2022.

File `Spresense-CameraToSdCardAsBMP.ino` captures images streaming from a camera attached to the Spresense Main board. The images are saved to the Spresense Extension Board SD card in BMP format. Color or grayscale BMPs can be created by toggling `#define` statements at the top of the INO file.

The `Example Saved Images` show what a bounding box looks like. They are not actually highlighting objects with a black outlines rectange in an image.

**TODO:**

The negative value of height is currently hard-coded to -240. This negative encoding make the image normal looking, aka the image is not mirrored.

This negative value affects the following code in the BMP Header called DIB:
"""
//4 Bytes: The bitmap height in pixels (signed integer)
// Negative 240 = 0xFFFF FF10.
	bitmapDIBHeader[8]  = 0x10;	<--Note LSBy comes first.
    bitmapDIBHeader[9]  = 0xFF;
    bitmapDIBHeader[10] = 0xFF;
    bitmapDIBHeader[11] = 0xFF;
"""	