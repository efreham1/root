# Design

Our program consists of 6 different moduels that together builds our garbage collector, they are gc, heap, meatadata, mover, page & stack_scanner. 

#GC
The GC is designed to be the head of the garbage colletor, it's here all the other modules are tied together. Here data is were data is allocated as well as were the garbage collection function is defined. 

#Heap
The heap is designed to be a long array of consecutively allocated pages. 

#Metadata
The meta data is a array of 64 bits, where the 2 first bits determin if it is to be interpretate as a format vector, a pointer or a forwarding adress.
If the 2 first bits are  0,1 the metadata is the size of some data. If the metadata is a format vector the first 2 bits are 1,1 bit 3 is a visiting bit, determining if we have visited this data before or not, the next 6 bits is an int that is the size of the next 55 bits. if it is a forwarding adress the 2 first bits are 0,0 and the rest is a void pointer.

#Mover
The mover is where the actual garbage collection takes place, the mover takes in all the pointer from the stack_scanner, if the stack is unsafe we check that the pointer is pointing to a page and saves that page in a array called static pages, a static page is a page where data is save that a pointer from the stack points to. Once we have all of our static pages we traverse the pointers to see if the data is movable, if the data is saved on a static page it is not movable otherwise it is. The data is then copied to a passive page that is turned active and the pointer to the data repointed. Once all pointers are traversed and all data is copied to new pages the all active pages are set to passive and the data removed thus collecting all the garbage

#Page
The page is where we allocate our data all pages are 2048 bytes large, a page can be active or passive, a active page means that data is currently allocated on it while a passive pgae means that the page is empty. In addition to being active or passive all pages contain a allocation map, the jobb of the allocation map is to keep track of where on the page data is allocated.

#Stack_scanner
Stack_scanner works by reciving a top-adress, a bottom-adress & the adress to a int from GC, this is the span where we expect the stackpointers to point to. We then traverse the stack by using the built in functions builtin_frame_adress & environ , which are the top and bottom of the stack. When we have our array of pointers from the stack, we filter out adresses that we are sertain are not pointer and pointers that are not pointers to our span. Lastly returning a new array of pointer we belive point to the heap and writing the numbers of pointers to the int adress. 
