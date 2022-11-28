# Can a machine learning model that reads IMU data and deployed from the Edge Impulse Studio run on a SubCore? #
**The answer is yes!**

<HR>

# Can a IMU based model run on the MainCore and a different IMU based model run on a SubCore? #
**The answer is no!**

The programs below show how to run the IMU based model on the Sony Spresense MainCore and SubCore1.
*Sidenote: A ML model that reads the Spresense camera or microphones cannot run on a SubCore.*

* `Spresense-Dual-Model-IMU.ino`
  * This program uses compiler directives that will program the MainCore to read the KX126 IMU and the SubCore1 to read the KX224 IMU.
  * This program uses an Edge Impulse deployed Arduino library. The docs for creating the library are [here](https://docs.edgeimpulse.com/docs/tutorials/continuous-motion-recognition). Go [here](https://studio.edgeimpulse.com/public/14299/latest) to create and deploy the library.

* `Spresense-MainCore.ino`
  * This program starts SubCore1. The main loop() is there just to print messages coming from the SubCore1.
	
* Verify `Spresense-Dual-Model-IMU.ino` works with KX126.
  * MainCore with KX126 <-- Spresense-Dual-Model-IMU.ino modified to not start any SubCores.
  * SubCore  N/A        <-- MainCore does not start the SubCore.
	
* Verify `Spresense-Dual-Model-IMU.ino` works with KX224.
  * MainCore without IMU code <-- Spresense-MainCore.ino
  * SubCore  with    KX224    <-- Spresense-Dual-Model-IMU.ino
	
* Verify `Spresense-Dual-Model-IMU.ino` works with separate Models running on seperate cores.
  * I was seeing some I2C errors so I implemented hardware handshaking wherein the MainCore and SubCore1 would share the I2C bus, i.e., when one Core was using the I2C bus the other would wait until the bus was idle. This allowed both cores to run without throwing memory core dump error messages. But ultimately I could not get both models working in parallel on different cores.

  * At one point I got a **deadbeef** error :)
    * `up_dumpstate: ERROR: Stack pointer is not within the allocated stack`
    * `up_stackdump: 0d03c8c0: 74696e69 00000000 deadbeef deadbeef deadbeef deadbeef deadbeef deadbeef`
	
Happy Programming
