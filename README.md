CPSC 351-02

Group members:

Fernando Mendoza Mendoza96@csu.fullerton.edu

Evan Lam evan218@csu.fullerton.edu

Gerardo Varela gvarela97@csu.fullerton.edu

Angel Soto  a_soto@csu.fullerton.edu

guide urls:
http://beej.us/guide/bgipc/html/multi/shm.html
http://beej.us/guide/bgipc/html/multi/mq.html

language: C++

how to run:
  1.run the make file 
  2.open two terminals
  3.run in terminal ./recv
  4.run in other terminal ./sender keyfile.txt
  
Contributions:
  Fernando Mendoza - worked on sender.cpp, recv.cpp, and testing 
  
  Evan Lam - Started and worked on recv.pp and design report

  Gerardo Varela - testing, review, documantation 
  
  Test Run Outputs:
Terminal 1: ./sender keyfile.txt  
me@cpsc-vm:/Desktop/cpsc351_P1-master$ ./sender keyfile.txt  
start  
	  init  
making keyfile  
shared contents:  
	  sending  
starting send  
reading loop  
sending to receiver  
waiting for receiver  
end send loop  
cleaning up  
me@cpsc-vm:~/Desktop/cpsc351_P1-master$   

me@cpsc-vm:/Desktop/cpsc351_P1-master$ ./recv  
stopper  
init  
shared contents: Hello world  
mainloop  
starting real mainloop  
entering msgSize loop  
we are rdy for next chunk  
got new msg  
Cleaning  
me@cpsc-vm:~/Desktop/cpsc351_P1-master$  
