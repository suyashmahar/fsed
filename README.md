# fsed  
:scissors: Change what a process reads from a file without actually changing the contents on disk; sed for files  

## Compiling fsed  
> **NOTE**: fsed is known to work on perfectly fine on WSL (Ubuntu
> 18.04) using Linux 4.4.0-17763-Microsoft and should work on any
> other linux machine out there.

For compiling fsed, GNU Make and a C compiler are required. You can
install them on ubuntu using:

```
sudo apt-get install make gcc
```
  
To compile fsed,  
1. Clone the repository  
   ```  
   git clone git@github.com:suyashmahar/fsed.git  
   ```  
2. Make fsed using your default C compiler  
   ```  
   make  
   ```  
   or, you can choose your favorite one by changing `CC` environment flag.
   ```  
   CC=fancy-compiler-4.8 make  
   ```  
3. Execute fsed using the following template:  
   For changing every occurrence of `old_string` with `new_string` while
   reading file `<filename>` in `<your_program>` use,  
   ```  
   ./fsed -r "/old_string/new_string/" -f <filename> --args <your_program>  
   ```

## Demo
To build a small test program run:
1. Compile the test binary:
   ```
   make testbin
   ```
2. This binary will be placed in the `test` directory, switch to that
   directory:
   ```
   cd test/
   ```
3. Execute the test binary without fsed,  
   ```  
   ./testbin  
   ```  
   The binary will produce following output:  
   ```
   == contents of fd: 3 ==
   aldsafma
   == contents of fd: 4 ==
   aldfma
   == contents of fd: 5 ==
   aldsafma
   ```  
   These are the content of file `sample0.txt`, `sample1.txt` and `sample0.txt` respectively.  
4. Execute fsed to replace every occurrences of `a` with `$` for file `sample1.txt`:  
   ```  
   ../fsed -r '/a/$/' -f "./sample1.txt" --args ./testbin  
   ```  
   On executing testbin with fsed, the apparent content of file
   `sample1.txt` to `testbin` will changed:  
   ```  
   == contents of fd: 3 ==
   aldsafma
   == contents of fd: 4 ==
   $ldfm$
   == contents of fd: 5 ==
   aldsafma
   ```  
   
## Working  
fsed uses `ptrace(2)` system call to intercept every system call made
by the child process. Whenver a system call is made by the child, fsed
checks if it is related to IO operation for the file passed in
arguments. If the intercepted operation is read data fsed replaces the
content of the data in child process's memory using the same
(`ptrace`) system call.  

## Limitations  
#### Performance  
fsed expands a single read system call to a number of different system calls (made to read and modify data). This severely affects the performance of the application, for example for the test program:
```shell
time (
        for (( i=1; i<1000; i++ )); do
                ../fsed -r '/a/$/' -f "./sample1.txt" --args ./testbin > /dev/null
        done
)
```
takes 20.087s to execute:
```
1.64s user 11.70s system 66% cpu 20.087 total
```

while without fsed, the same binary
```shell
time (
        for (( i=1; i<1000; i++ )); do
                ./testbin > /dev/null                                             
        done
)
```
takes 7.329s to execute:
```
 0.34s user 3.72s system 55% cpu 7.329 total
```

#### Scalability
fsed only supports replacing strings of same size, providing a
different sized replacement string may lead to undefined behavior.

## License
This work is licensed under (very) permissive WTFPL, a copy of which can be obtained from [here](LICENSE).  
(c) 2018-19 Suyash Mahar <suyash12mahar@outlook.com> 
