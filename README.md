



#  GAL-OSNMA-SIM

 GAL-OSNMA-SIM generates GALILEO baseband signal data streams (E1B/E1C). The baseband signal can be converted to RF using software-defined radio (SDR) platforms, such as [HackRF](https://github.com/mossmann/hackrf/wiki).
This projet has been inspired from [GPS-SDR-SIM](https://github.com/osqzss/gps-sdr-sim). Many thanks to [Takuji Ebinuma](https://github.com/osqzss/)


### Configure the project
By default, the software is linked with the library [libhackrf](https://github.com/greatscottgadgets/hackrf). You can disable this option by undefined HACKRFLINKED in include/constant.h


### Building the project
A makefile is provided with the project. The makefile support Windows, Linux and MacOS. Edit the file Makefile and modify the path according to your setup.
```
$ make all
```

### OSNMA test vectors files

The  gal-osnma-sim software has been tested using the official ICD test vectors from the [Receiver Guidelines for the Test Phase v1.1](https://www.gsc-europa.eu/sites/default/files/sites/all/files/Galileo_OSNMA_Receiver_Guidelines_for_the_Test_Phase_v1.1.pdf) available [here](https://www.gsc-europa.eu/sites/default/files/sites/all/files/osnma_annex_2.zip).

The test vectors **must be reformatted** using, for instance, the python script [reformat_test_vector.py](https://github.com/Algafix/OSNMA/blob/master/tests/icd_test_vectors/reformat_test_vectors.py)  available with the project [OSNMAlib](https://github.com/Algafix/OSNMA). Thanks  [Algafix](https://github.com/Algafix/OSNMA/commits?author=Algafix). 


### Generating the GALILEO OSNMA signal file

The software has been designed to generate on the fly the I/Q samples and send them to the HackRF. The multithreading implementation allows a realtime signal generation on a PC equiped with a processor such as Intel® Core:tm:i7. Nevertheless, you can also generate I/Q file and replay it later on your SDR device.

**Our test setup:**

```mermaid
graph LR
A(PC) --> B(HackRF)
B --> C(att. 30dB)
C --> D(GNSS Rx)
```
**Notes**: An external TCXO Clock Oscillator Module (0.5PPM) is installed on our HackRF.

You don't need additional files than the test vectors. The ephemeris are extracted from the navigation messages. You have to set the location of the simulation (latitude,Longitude ,altitude). 

```
Specify at least -v <testvector>
Usage:
  -v <testvector>  Test vector file
  -l <location>    Lat,Lon,Alt (default 48.8435155,2.4297700,60)
  -o <output>      I/Q sampling data file (default: osnma.bin)
  -s <frequency>   Sampling frequency [Hz] (default: 2600000)
  -b <iq_bits>     I/Q data format [8/16] (default: 8)
  -k               HackRF transfer
  -d <duration>    Scenario duration [s] (default : 0 -> whole scenario duration)
  -h help
```
The signal generation date/time is the same as the test vector scenario
```
> ./gal-osnma-sim -l 48.8435155,2.4297700,60 -t ./tv/configuration_A/13_DEC_2020_GST_09_00_01_fixed.csv -k
```

### Transmitting the samples

#### HackRF:

The output data have to be 8-bit signed I/Q samples.
```
> ./gal-osnma-sim -l 48.8435155,2.4297700,60 -t ./tv/configuration_A/13_DEC_2020_GST_09_00_01_fixed.csv
```
You can use `hackrf_transfer` tool in the HackRF host software. 

```
> hackrf_transfer -t osnma.bin -f 1575420000 -s 2600000 -a 1 -x 0
```



### License

Copyright &copy; 2023 FDC  
Distributed under the [MIT License](http://www.opensource.org/licenses/mit-license.php).
