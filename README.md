# Load-Balancer

Load Balancer is an educational project for the Operating System course held by Dr. Mahdi Kargahi.
This project aims to filter files content and to sort them ascending/descending by the field attribute.

## How does it work?
The primary process is the `LoadBalancer` which separates input files into workers' processes.
Then `Worker` processes filter files content by the indicated attributes and send them to the `Presenter` process.
The presenter sort contents and merge them to the last filtered data.
Pipes specify all the interconnections and message passing.
(Unnamed pipes between LoadBalancer and workers, and Named pipe between workers and presenter).

## How to run it?
Run the following commands for in the root directory of each process [LoadBalancer/Worker/Presenter]
```
mkdir build
cd build
cmake ..
make
./[executable_file]
```

## Inputs
After running `LoadBalancer` executable file your input should have the following format:

```
(<field name> = <corresponding filtering value> - )*( <sorting value = ascend/descend> - )? ￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼￼prc_cnt = n – dir = <relative dataset address> 
```

All the input files contents should be like the following sample: 

```
brand model ram hdd price 
apple pro 16 512 30
apple pro 16 246 26
```
