# Design
Our program consists of six different modules that together build our garbage collector: gc, heap, metadata, mover, page, and stack_scanner.
![IOOPM_PROJ18](https://user-images.githubusercontent.com/90697986/211035376-ea3d5eb9-286f-46f9-b07a-9bae8dea0cf6.png)

# GC 
The GC is designed to be the head of the garbage collector. It is where all the other modules are tied together and where data is allocated and the garbage collection function is defined.

# Heap 
The heap is designed as a long array of consecutively allocated pages.

# Metadata
The metadata is an array of 64 bits, where the first two bits determine if it is to be interpreted as a format vector, a pointer, or a forwarding address. If the first two bits are 0,1, the metadata is the size of some data. If the metadata is a format vector, the first two bits are 1,1, and the third bit is a visiting bit, determining if we have visited this data before or not. The next six bits are an integer representing the size of the next 55 bits. If it is a forwarding address, the first two bits are 0,0, and the rest is a void pointer.

# Mover 
![Screenshot from 2023-01-06 15-52-18](https://user-images.githubusercontent.com/90697986/211036548-2833027e-c08f-4966-8ed5-7befe726baef.png)

The mover is where the actual garbage collection takes place. It takes in all the pointers from the stack_scanner. If the stack is unsafe, we check that the pointer is pointing to a page and save that page in an array called static pages. A static page is a page where data is saved that a pointer from the stack points to. Once we have all of our static pages, we traverse the pointers to see if the data is movable. If the data is saved on a static page, it is not movable; otherwise, it is. The data is then copied to a passive page, which is turned active, and the pointer to the data is repointed. Once all pointers are traversed and all data is copied to new pages, all active pages are set to passive, and the data is removed, thus collecting all the garbage.

# Page 
The page is where we allocate our data. All pages are 2048 bytes in size and can be either active or passive. An active page means that data is currently allocated on it, while a passive page means that the page is empty. In addition to being active or passive, all pages contain an allocation map, which keeps track of where on the page data is allocated.

# Stack_scanner 
![Screenshot from 2023-01-06 15-48-39](https://user-images.githubusercontent.com/90697986/211036193-83289d69-c688-4aa5-8110-af7ee70d9251.png)
The stack_scanner works by receiving a top address, a bottom address, and the address of an integer from GC. This is the span where we expect the stack pointers to point to. We then traverse the stack using the built-in functions builtin_frame_address and environ, which are the top and bottom of the stack. When we have our array of pointers from the stack, we filter out addresses that we are certain are not pointers and pointers that are not pointing to our span. Finally, we return a new array of pointers that we believe point to the heap and write the number of pointers to the integer address.
